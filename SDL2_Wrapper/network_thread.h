#pragma once
#include <memory>
#include <thread>
#include <map>
#include <mutex>
#include <vector>
#include "clock.h"
#include "udp.h"
#include "tcpconnection.h"
#include "Peer.h"
#include "DoubleBuffer.h"
#include "QuickTransferStruct.hpp"
#include "netphy_communication.h"

namespace Net
{
	class NetworkCore
	{
	private:
		struct Peer
		{
			glm::vec3 pos;
			float lastPingWas;
			std::atomic<bool> isAcceptingUpdates;
		};
	private:
		void addPeer(const unsigned int identity);
		void remPeer(const unsigned int identity);
		void addToSendBlacklist(const unsigned int identity);
		void remSendBlacklist(const unsigned int identity);
		void manageTimeouts(const float dTimeout);

		void netRevcdHello(const std::vector<char> & message,
			const float timeout);
		void netRevcdDisallow(const std::vector<char> & message);
		void netRevcdContest(const std::vector<char> & message);
		void netRevcdTransferIn(const std::vector<char> & message);
		void updatePeerLocation(const std::vector<char> & message);
		void netRevcdPause();
		void netRevcdUnpause();
		void updateTimescale(const std::vector<char> & message);

	private:
		std::thread mSeekThread;
		std::thread mAllowThread;
		Utilities::Clock mPingTimer;
		int * mNetworkRate;
		int * mOwned;
		std::atomic<bool> mShutdown;
		std::atomic<bool> mAcceptingConnectionsFN;
		std::atomic<bool> & mIsMaster;
		std::atomic<bool> & mAllowContinue;
		std::atomic<bool> & mPhysicsPause;
		std::pair<std::atomic<bool>, std::atomic<bool>> & mLocalPauseCmd;
		std::atomic<bool> mIsInitialPing;

		bool mReady;
		std::unique_ptr<Net::UDP> mUDPSocket;
		std::thread mThread;
		std::mutex mMutex;
		std::vector<std::shared_ptr<TCPConnection>> mConnections;
		std::map<unsigned int, std::shared_ptr<Peer>> mPeers;
		std::map<unsigned int, std::shared_ptr<Peer>> mBlacklist;
		std::shared_ptr<Shared::Peer> mOwnLocation;
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> mRender;
		std::shared_ptr<Shared::SharingMechanism> mContentionSys;
		void updateFeeder();
	public:
		NetworkCore(int * rate, std::atomic<bool> & isMaster, 
			std::atomic<bool> & allowContinue,
			std::shared_ptr<Shared::Peer> netpeer,
			std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> toDraw,
			int * owned,
			std::pair<std::atomic<bool>, std::atomic<bool>> & cmdPause,
			std::atomic<bool> & phyPause,
			std::shared_ptr<Shared::SharingMechanism> exchg);
		~NetworkCore();
		void tmain();
		bool attemptJoin();
		void receiveMessages();
		void stop();
		void start();
	};
}
