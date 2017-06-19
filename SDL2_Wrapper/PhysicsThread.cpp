#pragma once
#include <Windows.h>
#include "PhysicsThread.h"
#include "QuickTransferStruct.hpp"
#include "rk4.h"

#define accelDueToGravity -9.81
#define maxrad 100

namespace Physics
{
	PhysicsCore::PhysicsCore(int ballcount,
		bool master,
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> toDraw,
		std::shared_ptr<Shared::Peer> gfxPeerInfo,
		std::shared_ptr<Shared::Peer> netPeerInfo,
		External external,
		std::atomic<bool> & pause,
		const BufferObjs::Weight & weight,
		std::shared_ptr<Shared::SharingMechanism> exchg)
		:
		mShutdown(false),
		mPhysicsReady(false),
		mToRender(toDraw),
		mPlane(0.0f),
		mEnclosure(48.0f),
		mPeerForGfx(gfxPeerInfo),
		mPeerForNet(netPeerInfo),
		mPlayer(new Shared::Peer()),
		mShadowPlayer(new Shared::Peer()),
		mExternalConfig(external),
		mBallCount(*external.ballCount),
		mPaused(pause),
		mGravityWellMagnitude(0.0f),
		mContentionSys(exchg)
	{
		mBallList.reserve(mBallCount);
		mContested.reserve(mBallCount);
		mOwned.reserve(mBallCount);
		mROI.reserve(mBallCount);
		generateWorld(weight, master);
	}

	void 
	PhysicsCore::generateWorld(const BufferObjs::Weight & weight,
		bool master)
	{
		std::array<float, 3>masses =
		{
			weight.light,
			weight.medium,
			weight.heavy
		};
		int massIdx = 0;
		int currentMax = 0, currentMin = 0;
		int xDir = 1, yDir = 1;
		int x = 0, y = 0;

		for (int i = 0; i < *mExternalConfig.ballCount; i++)
		{
			mBallList.push_back(Sphere(0,
				(master)?OwnershipBit::Owned : OwnershipBit::NotOwned,
				glm::vec3(x * 2, 1, y * 2),
				glm::vec3(0, 0, 0),
				glm::vec3(0),
				glm::vec3(0),
				masses[massIdx++],
				1));

			if (massIdx == masses.size())
				massIdx = 0;

			//generate position in a spiral.

			//we are right
			if (x == currentMax)
			{
				//we are bottom right corner
				if (y == currentMin)
				{
					//go out
					xDir = 1;
					yDir = 0;

					currentMax++;
					currentMin--;
				}
				//we are top right corner
				else if (y == currentMax)
				{
					//go left
					xDir = -1;
					yDir = 0;				
				}
				else
				{
					//go up
					xDir = 0;
					yDir = 1;
				}
			}
			//we are left
			else if (x == currentMin)
			{
				//we are top left corner
				if (y == currentMax)
				{
					//do down
					xDir = 0;
					yDir = -1;
				}
				//we are bottom left corner
				else if(y == currentMin)
				{
					//go right
					xDir = 1;
					yDir = 0;
				}
			}

			x += xDir;
			y += yDir;			
		}

		/*----------------------------------*/
		/*-------Initialise Owned-----------*/
		/*----------------------------------*/
		for (auto &b : mBallList)
		{
			if (b.own() == OwnershipBit::Owned)
			{
				mOwned.push_back(&b);
			}
		}

		//prefill buffer fields.
		std::vector<BufferObjs::PhyToGfx> gfx1;
		std::vector<BufferObjs::PhyToGfx> gfx2;
		for (auto & b : mBallList)
		{
			mPrimarySwapBuffer.reserve(mBallCount);
			gfx1.reserve(mBallCount);
			gfx2.reserve(mBallCount);
		}
		mToRender->commitWrite(&gfx1);
		mToRender->swap();
		mToRender->commitWrite(&gfx2);
	}

	PhysicsCore::~PhysicsCore()
	{}

	void
	PhysicsCore::stop()
	{
		mShutdown = true;
		mPhysicsThread.join();
	}


	void
	PhysicsCore::start()
	{
		mPhysicsThread = 
			std::thread(&PhysicsCore::simulationLoop, this);
	}

	void
	PhysicsCore::simulationLoop()
	{
		SetThreadAffinityMask(mPhysicsThread.native_handle(), 0x2);
		double timestep_divisor=0;
		double timestep_sum=0;
		double elapsedTime = 0;

		mPhysicsHz.reset();
		mPhysicsClock.reset();
		mOwnedChanged = true;
		while (!mShutdown)
		{
			if (!mPaused)
			{
				elapsedTime += mPhysicsHz.tick(); //returns answer in seconds
				double target = 1.0 / *mExternalConfig.targetRate;
				if (elapsedTime > target)
				{
					//performance counting stuff
					if (timestep_sum > 1.0)
					{
						*mExternalConfig.timestep =
							timestep_sum / timestep_divisor;

						timestep_divisor = 0;
						timestep_sum = 0;
					}

					elapsedTime = 0;
					//double dt = 2.3e-5;
					double dt = mPhysicsClock.tick() * (*mExternalConfig.timescale);
					//double dt = 0.05f;
					timestep_sum += dt;
					timestep_divisor++;

					//doownroi
					//donetworkroi
					//update owned.
					mContentionSys->ownershipUpdate(mBallList, mContested);
					if (mContentionSys->ownershipChanged())
					{
						mOwned.clear();
						for (auto &b : mBallList)
						{
							if (b.own() == OwnershipBit::Owned)
							{
								mOwned.push_back(&b);
							}
						}
					}

					*mExternalConfig.owned = mOwned.size();
					updateROI();
					collisionDetectionSP(dt);
					collisionDetectionSS(dt);
					collisionDetectionSE(dt);
					collisionResponseSP(dt);
					collisionResponseSS(dt);
					collisionResponseSE(dt);
					applyGravityWellForce();
					forceSumation(dt);

					mFloorManifold.clear();
					mBallsManifold.clear();
					mWallsManifold.clear();

					int i = 0;
					int difference = mPrimarySwapBuffer.size() - mROI.size();

					if (difference < 0)
					{
						//fill it.
						for (int i = 0;
							i < -difference;
							i++)
						{
							mPrimarySwapBuffer.push_back(
								BufferObjs::PhyToGfx());
						}
					}
					else if (difference > 0)
					{
						mPrimarySwapBuffer.erase(
							mPrimarySwapBuffer.end() - difference,
							mPrimarySwapBuffer.end());
					}
					if (mROI.size() > 0)
					{
						for (auto & v : mROI)
						{
							v->update();
							//need to adjust size to fit roi...

							mPrimarySwapBuffer[i].position = v->position();
							mPrimarySwapBuffer[i].orientation = v->orientation();
							mPrimarySwapBuffer[i].mass = v->mass();
							i++;

						}
					}
					else
					{
						mPrimarySwapBuffer.clear();
					}
					mToRender->commitWrite(&mPrimarySwapBuffer);
					mPeerForGfx->position = mPlayer->position;
					mPeerForNet->position = mPlayer->position;
				}
			}
			else
			{
				mPhysicsClock.reset();
				mPhysicsHz.reset();
			}
		}
	}

	void 
	PhysicsCore::updateROI()
	{
		mContentionSys->transferObjects(mBallList, mContested, *mPlayer);
		auto pos = mPlayer->position;
		auto rad = 10;//mPlayer->ROI;

		mROI.clear();
		for (auto & s : mOwned)
		{
			float distance = glm::length((s->position() - pos));
			if (distance < s->radius() + rad)
			{
				mROI.push_back(s);
			}
		}
	}

	void 
	PhysicsCore::collisionDetectionSP(const double dt)
	{
		for (auto & s : mROI)
		{
			if (mPlane.distance(s->position()) < s->radius())
			{
				ManifoldPoint mp;
				mp.contactID1 = s;
				mp.contactID2 = nullptr;
				mp.contactNormal = glm::vec3(0, -1, 0);
				//we hit the floor!
				mFloorManifold.add(mp);
			}
		}
	}

	void
	PhysicsCore::collisionDetectionSS(const double dt)
	{
		int v = 0;
		for (int i=0; i<mROI.size(); i++)
		{
			v = i+1; //remove posibility of repeat checks.
			for (v; v<mROI.size(); v++)
			{
				if (i != v)
				{
					auto & b1 = mROI[i];
					auto & b2 = mROI[v];

					auto posVec = b1->position() - b2->position();
					auto dist = glm::length(posVec) - (b1->radius());
					if (dist < 0.0f)
					{
						ManifoldPoint mp;
						mp.contactID1 = b1;
						mp.contactID2 = b2;
						mp.contactNormal = glm::normalize(posVec);
						mp.penetration = dist;
						mBallsManifold.add(mp);
					}
				}
			}
		}
	}

	void
	PhysicsCore::collisionDetectionSE(const double dt)
	{
		for (auto & s : mROI)
		{
			if (mEnclosure.distance(s->position()) + s->radius() < 0.0f)
			{
				ManifoldPoint mp;
				mp.contactID1 = s;
				mp.contactID2 = nullptr;
				mp.contactNormal = glm::normalize(s->position());
				//we hit the wall!
				mWallsManifold.add(mp);
			}
		}
	}

	void 
	PhysicsCore::applyGravityWellForce()
	{
		glm::vec3 well = mPlayer->position;
		float wellArea = 10;// mPlayer->gwROI;
		for (auto b : mROI)
		{
			glm::vec3 travelVec = b->position() - well;
			if (glm::length(travelVec) < (wellArea - b->radius()))
			{
				travelVec = glm::normalize(travelVec);
				travelVec *= mGravityWellMagnitude;

				auto force = travelVec * b->mass();
				b->forcePlus(force);
			}
		}
	}

	void 
	PhysicsCore::collisionResponseSP(const double dt)
	{
		for (int i = 0; i < mFloorManifold.size(); i++)
		{
			auto point = mFloorManifold.point(i);
			auto pos = point.contactID1->position();

			point.contactID1->newPosition(glm::vec3(pos.x, 1.0f, pos.z));
			point.contactID1->position(point.contactID1->newPosition());
			//point.contactID1->newPosition(point.contactID1->position()
			//	+ glm::vec3(0, mPlane.distance(point.contactID1->position() 
			//	- point.contactID1->radius()), 0));		
			if (std::fabs(point.contactID1->velocity().length() > 0.05f))
			{
				auto newVel = point.contactID1->velocity();
				newVel.y = -newVel.y * 0.5f;

				float u1dotL = glm::dot(point.contactID1->velocity(), 
					point.contactNormal);
				auto friction = -(point.contactID1->velocity() -
					point.contactID1->velocity()*u1dotL)*mFriction;

				auto dvdt = (newVel - point.contactID1->velocity()) / (float)dt;
				auto force = dvdt * point.contactID1->mass();
				point.contactID1->forcePlus(force);

				dvdt = (friction) / (float)dt;
				force = dvdt * point.contactID1->mass();
				point.contactID1->forcePlus(force);
			}
		}
	}

	void
	PhysicsCore::collisionResponseSS(const double dt)
	{
		for (int i = 0; i < mBallsManifold.size(); i++)
		{
			auto point = mBallsManifold.point(i);

			glm::vec3 contactNormal = point.contactNormal;
			float m1 = point.contactID1->mass();
			float m2 = point.contactID2->mass();
			float u1dotL = glm::dot(point.contactID1->velocity(), contactNormal);
			float u2dotL = glm::dot(point.contactID2->velocity(), contactNormal);

			auto check =
				glm::dot(point.contactID2->velocity() - point.contactID1->velocity(), 
					point.contactNormal);

			if (check > 0.0f)
			{
				auto friction1 = -(point.contactID1->velocity() -
					point.contactID1->velocity()*u1dotL)*mFriction;

				auto friction2 = -(point.contactID2->velocity() -
					point.contactID2->velocity()*u2dotL)*mFriction;

				glm::vec3 vL1 =
					((m1 - (mRestitution * m2)) * (u1dotL)*contactNormal +
					(m2 + (mRestitution * m2)) * (u2dotL)*contactNormal)
					/ (m1 + m2);


				glm::vec3 vL2 =
					((m1 + (mRestitution * m1)) * (u1dotL)*contactNormal +
					(m2 - (mRestitution * m1)) * (u2dotL)*contactNormal)
					/ (m1 + m2);

				auto newVel_s1 = point.contactID1->velocity()
					- (u1dotL * contactNormal) + vL1;

				auto newVel_s2 = point.contactID2->velocity()
					- (u2dotL * contactNormal) + vL2;

				auto dvdt = (newVel_s1 - point.contactID1->velocity()) / (float)dt;
				auto force = dvdt * point.contactID1->mass();
				point.contactID1->forcePlus(force);

				dvdt = (newVel_s2 - point.contactID2->velocity()) / (float)dt;
				force = dvdt * point.contactID2->mass();
				point.contactID2->forcePlus(force);

				dvdt = (friction1) / (float)dt;
				force = dvdt * point.contactID1->mass();
				point.contactID1->forcePlus(force);

				dvdt = (friction2) / (float)dt;
				force = dvdt * point.contactID2->mass();
				point.contactID2->forcePlus(force);
			}
		}
	}

	void 
	PhysicsCore::collisionResponseSE(const double dt)
	{
		for (int i = 0; i < mWallsManifold.size(); i++)
		{
			auto point = mWallsManifold.point(i);
			auto pos = point.contactID1->position();
		
			auto penetration = mEnclosure.distance(point.contactID1->position());

			auto newPos = -point.contactNormal*-penetration 
				+ point.contactID1->position();

			point.contactID1->newPosition(newPos);
			point.contactID1->position(newPos);

			if (std::fabs(point.contactID1->velocity().length() > 0.05f))
			{
				float u1dotL = glm::dot(point.contactID1->velocity(),
					point.contactNormal);
				auto friction = -(point.contactID1->velocity() -
					point.contactID1->velocity()*u1dotL)*mFriction;

				auto newVel = -point.contactID1->velocity() * 0.5f;

				auto dvdt = (newVel - point.contactID1->velocity()) / (float)dt;
				auto force = dvdt * point.contactID1->mass();
				point.contactID1->forcePlus(force);

				dvdt = (friction) / (float)dt;
				force = dvdt * point.contactID1->mass();
				point.contactID1->forcePlus(force);
			}
		}
	}

	void
	PhysicsCore::forceSumation(const double dt)
	{
		glm::vec3 force;
		glm::vec3 accel;

		for (auto & s : mROI)
		{
			//force = glm::vec3(0, accelDueToGravity, 0)*s.mass();
			accel = s->force() / s->mass();
			s->force(glm::vec3(0, accelDueToGravity, 0)*s->mass());

			s->newVelocity(rk4(accel, dt, s->velocity()));
			s->newPosition(rk4(s->newVelocity(), dt, s->position()));
		}
	}

	void 
	PhysicsCore::zeroGravityWell()
	{
		mGravityWellMagnitude = 0.0f;
		*mExternalConfig.wellforce = 0.0f;
	}

	void
	PhysicsCore::gravityWellMagnitude(const float value)
	{
		mGravityWellMagnitude += value;
		*mExternalConfig.wellforce = mGravityWellMagnitude;
	}

	void 
	PhysicsCore::setFriction(const float fric)
	{
		mFriction = fric/10.0f;
		*mExternalConfig.friction = fric;
		// div 10? no idea. just go with it.
	}

	void
	PhysicsCore::setRestitution(const float rest)
	{
		mRestitution = rest;
		*mExternalConfig.elasticity = rest;
	}

	void
	PhysicsCore::movePeer(const glm::vec3 & by)
	{
		mShadowPlayer->position = mPlayer->position;
		mShadowPlayer->position += (by/7.0f);
		std::swap(mPlayer, mShadowPlayer);
	}

	Sphere
	PhysicsCore::loseOwn(int id)
	{
		std::lock_guard<std::mutex> lk(mMutex);
		mBallList[id].own(OwnershipBit::NotOwned);
		mOwnedChanged = true;
		return mBallList[id];
	}

	void
	PhysicsCore::gainOwn(int id, glm::vec3 & pos, 
		glm::vec3 & vel, glm::vec3 & ori, glm::vec3 & ang)
	{
		std::lock_guard<std::mutex> lk(mMutex);

		mBallList[id].own(OwnershipBit::Owned);
		mBallList[id].position(pos);
		mBallList[id].velocity(vel);
		mBallList[id].orientation(ori);
		mBallList[id].angularVelocity(ang);
		mOwnedChanged = true;
	}
}

	/*----------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------*/
