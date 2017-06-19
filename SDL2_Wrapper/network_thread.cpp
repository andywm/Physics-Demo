#pragma once
#include <iostream>
#include <atomic>
#include "network_thread.h"
#include "udp.h"
#include "tcplistener.h"
#include "address.h"
#include "message_coding.h"
#include "clock.h"

namespace Net
{
	NetworkCore::NetworkCore(int * rate, 
		std::atomic<bool> & isMaster,
		std::atomic<bool> & allowContinue,
		std::shared_ptr<Shared::Peer> netpeer,
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> toDraw,
		int * owned,
		std::pair<std::atomic<bool>, std::atomic<bool>> & cmdPause,
		std::atomic<bool> & phyPause,
		std::shared_ptr<Shared::SharingMechanism> exchg)
		:
		mShutdown(false),
		mAcceptingConnectionsFN(true),
		mNetworkRate(rate),
		mIsMaster(isMaster),
		mAllowContinue(allowContinue),
		mRender(toDraw),
		mOwnLocation(netpeer),
		mOwned(owned),
		mLocalPauseCmd(cmdPause),
		mPhysicsPause(phyPause),
		mContentionSys(exchg)
	{		
		WSADATA impDetails;
		WSAStartup(MAKEWORD(2, 2), &impDetails);
		mUDPSocket = std::unique_ptr<Net::UDP>(new Net::UDP(9171));
		std::lock_guard<std::mutex> lk(mMutex);
		//begin broadcast...
	}

	NetworkCore::~NetworkCore()
	{
		//mShutdown = true;
		WSACleanup();
	}

	void
	NetworkCore::start()
	{
		mThread = 
			std::thread(&Net::NetworkCore::tmain, this);
	}

	void
	NetworkCore::stop()
	{
		std::atomic_thread_fence(std::memory_order_release);
		mShutdown = true;
		mThread.join();
	}

	void
	NetworkCore::tmain()
	{
		mAllowThread = 
			std::thread(&Net::NetworkCore::receiveMessages, this);

		SetThreadAffinityMask(mAllowThread.native_handle(), 0x1);
		SetThreadAffinityMask(mThread.native_handle(), 0x1);

		std::vector<char> pingMsg = udpMakeHello(*mOwned);
		Utilities::Clock helloClock;
		Utilities::Clock hzClock;
		float helloTime = 0;
		double elapsedTime = 0;

		int i = 0;
		while (!mShutdown)
		{			
			elapsedTime += hzClock.tick(); //returns answer in seconds
			helloTime += helloClock.tick();
			double target = 1.0 / *mNetworkRate;

			if (elapsedTime > target)
			{
				//standard messages
				if (helloTime > 1.0)
				{
					mUDPSocket->broadcast(pingMsg);
					helloTime = 0.0f;
				}
				auto tloc = udpMakePeerLocation(mOwnLocation->position);
				mUDPSocket->broadcast(tloc);

				//command messages
				
				//process pause state
				if (mLocalPauseCmd.first)
				{
					mLocalPauseCmd.first = false;
					mPhysicsPause = (bool)mLocalPauseCmd.second;

					std::vector<char> pauseMsg;
					if(mLocalPauseCmd.second == true)
						pauseMsg = udpMakePause();
					else
						pauseMsg = udpMakeUnpause();

					mUDPSocket->broadcast(pauseMsg);
				}

				Shared::transfer_map tmconf;
				Shared::transfer_map tmnoconf;
				mContentionSys->consumeTransfersOut(tmnoconf);
				mContentionSys->consumeContestedOut(tmconf);

				//transfer balls...
				for (auto & peer : mPeers)
				{
					for (auto & ballList : tmnoconf)
					{
						std::vector<std::vector<char>> transfers;						
						if (ballList.second.size() > 0)
						{
							transfers.reserve(ballList.second.size());

							for (auto ball : ballList.second)
							{
								auto encodedBall = encodeBall(ball.first, 
									ball.second->position(), 
									ball.second->velocity(),
									ball.second->orientation(), 
									ball.second->angularVelocity());
								ball.second->own(OwnershipBit::NotOwned);
								transfers.push_back(encodedBall);
							}
							auto msg = 
							udpMakeTransferBalls(transfers, peer.first);
							mUDPSocket->broadcast(msg);
						}
					}			
				}

				manageTimeouts(elapsedTime);
				elapsedTime = 0;
			}
		}	
		//offload here.
		mAcceptingConnectionsFN = false;
		mAllowThread.join();
	}

	void 
	NetworkCore::updateFeeder()
	{
		std::vector<BufferObjs::PhyToGfx> mPrimaryTransferBuffer;
		mPrimaryTransferBuffer.clear();
		for (auto & p : mPeers)
		{
			BufferObjs::PhyToGfx obj;
			obj.position = p.second->pos;
			mPrimaryTransferBuffer.push_back(obj);
			mRender->commitWrite(&mPrimaryTransferBuffer);
		}
	}

	bool 
	NetworkCore::attemptJoin()
	{
		float initialTimeout = 0;
		bool didTimeout = false;

		//seek network state (is there an existing network)
		while (initialTimeout < 2.0f)
		{
			float ptime = mPingTimer.tick();
			initialTimeout += ptime;

			auto message = mUDPSocket->receive();

			auto type = udpWhatMessage(message);
			if (type == MsgTypeBit::Ping)
			{
				return false;
			}
		}
		return true;
	}

	void
	NetworkCore::receiveMessages()
	{	
		mIsMaster = attemptJoin();
		mAllowContinue = true;

		//response reading loop.
		while (mAcceptingConnectionsFN)
		{
			auto message = mUDPSocket->receive();
			auto type = udpWhatMessage(message);
			
			switch(type)
			{ 
				//don't waste time processing a signal from self.
				case MsgTypeBit::Invalid:
					break;
				case MsgTypeBit::Ping:
				{
					auto who = udpWhoSent(message);
					addPeer(who);
					break;
				}
				case MsgTypeBit::DontSend:
				{
					auto who = udpWhoSent(message);
					addToSendBlacklist(who);
					break;
				}
				case MsgTypeBit::TransferBalls:
				{
					std::vector<std::vector<char>> balls;
					unsigned int target;
					udpDecodeTransferBalls(balls, target, message);

					if (target == getOwnIPv4Address())
					{
						for (auto ball : balls)
						{
							glm::vec3 pos;
							glm::vec3 vel;
							glm::vec3 ori;
							glm::vec3 ang;
							OwnershipBit ob = OwnershipBit::NotOwned;
							unsigned int bid;
							decodeBall(bid, pos, vel, ori, ang, ball);
							auto spr = 
							Physics::Sphere(0, ob, pos, vel, ori, ang, 0, 0);

							mContentionSys->transferObject(bid, spr);
						}
					}
					break;
				}
				case MsgTypeBit::ContestBalls:
				{
					//foreach(ball)
					//physics.contest(ball)
					break;
				}
				case MsgTypeBit::ApplyForce:
				{
					//foreach(force)
					//physics.registerForce(ball)
					break;
				}
				case MsgTypeBit::PeerLocation:
				{
					updatePeerLocation(message);
					break;
				}
				case MsgTypeBit::Pause:
				{
					mPhysicsPause = true;
					break;
				}
				case MsgTypeBit::Unpause:
				{
					mPhysicsPause = false;
					break;
				}
				case MsgTypeBit::Timescale:
				{
					updateTimescale(message);
					break;
				}		
			};			
		}
	}

	//------------------------------------------------------------------------------
	//--------------------Sender Actions--------------------------------------------
	//------------------------------------------------------------------------------

	//setPhysicsMonitor
	//remPhysicsMonitor

	//getNonconflictingBall
	//getConflictingBall

	//------------------------------------------------------------------------------
	//--------------------Receive Actions-------------------------------------------
	//------------------------------------------------------------------------------

	void 
	NetworkCore::netRevcdHello(const std::vector<char> & message, 
		const float timeout)
	{
		auto ipAddress = udpWhoSent(message);
		addPeer(ipAddress);
	}

	void
	NetworkCore::netRevcdDisallow(const std::vector<char> & message)
	{
		//move to leaving queue.
		auto leaving = udpWhoSent(message);
		//do not send.
	}

	void
	NetworkCore::netRevcdContest(const std::vector<char> & message)
	{
		//phy.Contest
	}

	void
	NetworkCore::netRevcdTransferIn(const std::vector<char> & message)
	{

	}

	void
	NetworkCore::updatePeerLocation(const std::vector<char> & message)
	{
		unsigned int who;
		glm::vec3 where;
		udpDecodePeerLocation(who, where, message);
		auto iter = mPeers.find(who);
		if (iter != mPeers.end())
			iter->second->pos = where;
	}

	void
	NetworkCore::netRevcdPause()
	{
		//mPhyPauseState.second = true;
	//	mPhyPauseState.first = true;
	}

	void
	NetworkCore::netRevcdUnpause()
	{
		//mPhyPauseState.second = false;
		//mPhyPauseState.first = true;
	}

	void
	NetworkCore::updateTimescale(const std::vector<char> & message)
	{
		float ts;
		udpDecodeTimescale(ts, message);
		//mPhyTimescaleUpdate.second = ts;
		//mPhyTimescaleUpdate.first = true;
	}

	void
	NetworkCore::addPeer(const unsigned int identity)
	{
		std::lock_guard<std::mutex> lk(mMutex);

		auto iter = mPeers.find(identity);
		if (iter == mPeers.end())
		{
			std::shared_ptr<Peer> np(new Peer
			{
				glm::vec3(10000),
				0.0f,
				true
			});
			mPeers.insert(std::make_pair(identity, np));
			mContentionSys->addMonitor(identity, &np->pos);
		}
		else
		{
			iter->second->lastPingWas = 0.0f;
		}
	}

	void
	NetworkCore::remPeer(const unsigned int identity)
	{
		std::lock_guard<std::mutex> lk(mMutex);
		auto iter = mPeers.find(identity);
		if (iter != mPeers.end())
		{
			mPeers.erase(iter);
		}
	}

	void
	NetworkCore::addToSendBlacklist(const unsigned int identity)
	{
		std::lock_guard<std::mutex> lk(mMutex);
		auto iter = mPeers.find(identity);
		if (iter != mPeers.end())
		{
			auto iter2 = mBlacklist.find(identity);
			if (iter2 == mBlacklist.end())
			{
				mBlacklist.insert(std::make_pair(identity, iter->second));
			}
		}
	}

	void
	NetworkCore::remSendBlacklist(const unsigned int identity)
	{
		std::lock_guard<std::mutex> lk(mMutex);
		auto iter = mBlacklist.find(identity);
		if (iter == mBlacklist.end())
		{
			mBlacklist.erase(iter);
		}
	}

	void
	NetworkCore::manageTimeouts(const float dTimeout)
	{
		std::lock_guard<std::mutex> lk(mMutex);
		for (auto & peer : mPeers)
		{
			peer.second->lastPingWas += dTimeout;
		}
		for (auto & peer : mBlacklist)
		{
			peer.second->lastPingWas += dTimeout;
		}
		//if expired...
		for (auto iterator = mPeers.begin(); 
			iterator != mPeers.end(); iterator++)
		{
			if (iterator->second->lastPingWas > 5.0f)
			{
				mPeers.erase(iterator);
			}
		}
		for (auto iterator = mBlacklist.begin();
			iterator != mBlacklist.end(); iterator++)
		{
			if (iterator->second->lastPingWas > 5.0f)
			{
				mBlacklist.erase(iterator);
			}
		}
		updateFeeder();		
	}
}