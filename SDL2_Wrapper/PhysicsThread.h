#pragma once
#include <vector>
#include <mutex>
#include "DoubleBuffer.h"
#include "QuickTransferStruct.hpp"
#include "sphere.h"
#include "plane.h"
#include "enclosure.h"
#include "clock.h"
#include "ContactManifold.h"
#include "Peer.h"
#include "Weight.hpp"
#include "netphy_communication.h"

namespace Physics
{
	class PhysicsCore
	{
	public:
		struct External
		{
			int   *targetRate;
			int   *ballCount;
			int   *contested;
			int   *owned;
			float *friction;
			float *elasticity;
			float *wellforce;
			float *timescale;
			float *timestep;
		};
	private:
		External mExternalConfig;
		std::mutex mMutex;
		std::atomic<bool> mOwnedChanged;
		std::atomic<bool> & mPaused;
	private:
		std::shared_ptr<Shared::SharingMechanism> mContentionSys;
		std::unique_ptr<Shared::Peer> mShadowPlayer;
		std::unique_ptr<Shared::Peer> mPlayer;
		float mGravityWellMagnitude;
		Plane mPlane;
		Enclosure mEnclosure;
		ContactManifold mFloorManifold;
		ContactManifold mBallsManifold;
		ContactManifold mWallsManifold;
	private:
		int mBallCount;
		bool mShutdown;
		bool mPhysicsReady;
		std::vector<Sphere> mBallList;
		std::vector<Sphere *> mOwned;
		std::vector<Sphere *> mContested;
		std::vector<Sphere *> mROI;
		float mRestitution;
		float mFriction;

		Utilities::Clock mPhysicsClock;
		Utilities::Clock mPhysicsHz;
		std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> mToRender;
		std::thread mPhysicsThread;
		std::vector<BufferObjs::PhyToGfx> mPrimarySwapBuffer;
		std::shared_ptr<Shared::Peer> mPeerForGfx;
		std::shared_ptr<Shared::Peer> mPeerForNet;
	public:
		PhysicsCore(int ballcount,
			bool master,
			std::shared_ptr<Utilities::DoubleBuffer<BufferObjs::PhyToGfx>> toDraw,
			std::shared_ptr<Shared::Peer> gfxPeerInfo,
			std::shared_ptr<Shared::Peer> netPeerInfo,
			External external,
			std::atomic<bool> & pause,
			const BufferObjs::Weight & weight,
			std::shared_ptr<Shared::SharingMechanism> exchg);
		~PhysicsCore();
		void start();
		void stop();
		void movePeer(const glm::vec3 & by);
		void zeroGravityWell();
		void gravityWellMagnitude(const float value);
		void setFriction(const float fric);
		void setRestitution(const float rest);
	private:
		//void collisionResponseSpherePlane(const double dt);
		void simulationLoop();
		void generateWorld(const BufferObjs::Weight & weight, bool master);
		/*--------------------------------------------------------------*/
		/*--------------------------------------------------------------*/
		/*------------Actual Simulation Stuff---------------------------*/
		/*--------------------------------------------------------------*/
		/*--------------------------------------------------------------*/
		void updateROI();
		void collisionDetectionSP(const double dt);
		void collisionDetectionSS(const double dt);
		void collisionDetectionSE(const double dt);
		void applyGravityWellForce();
		void collisionResponseSP(const double dt);
		void collisionResponseSS(const double dt);
		void collisionResponseSE(const double dt);
		void forceSumation(const double dt);

		//---

		Sphere loseOwn(int id);

		void gainOwn(int id, glm::vec3 & pos,
			glm::vec3 & vel, glm::vec3 & ori, glm::vec3 & ang);
	
	};
}