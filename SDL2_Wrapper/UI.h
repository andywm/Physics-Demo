#pragma once
#include <AntTweakBar.h>

namespace UI
{
	class Statistics
	{
	private:
		TwBar * mBar;
		int mBallCount;
		int mContestedCount;
		int mOwnedCount;

		int mTargetPhysics;
		int mTargetNetwork;
		int mTargetGraphics;

		float mTimescale;
		float mTimestepAvg;
		float mWellForce;
		float mElasticity;
		float mFriction;
	public:
		Statistics();
		~Statistics()=default;

		inline void
		ballcount(int value)
		{
			mBallCount = value;
		}

		inline void
		contested(int value)
		{
			mContestedCount = value;
		}

		inline void
		owned(int value)
		{
			mOwnedCount = value;
		}

		inline void
		physicsHz(int value)
		{
			mTargetPhysics = value;
		}

		inline void
		networkHz(int value)
		{
			mTargetNetwork = value;
		}

		inline void
		graphicsHz(int value)
		{
			mTargetGraphics = value;
		}

		inline void
		timescale(float value)
		{
			mTimescale = value;
		}

		inline void
		timestep(float value)
		{
			mTimestepAvg = value;
		}

		inline void
		wellForce(float value)
		{
			mWellForce = value;
		}

		inline void
		elasticity(float value)
		{
			mElasticity = value;
		}

		inline void
		friction(float value)
		{
			mFriction = value;
		}
		//---------------------------------------
		//---------------------------------------
		//---------------------------------------
		inline int*
		ballcount()
		{
			return &mBallCount;
		}

		inline int*
		contested()
		{
			return &mContestedCount;
		}

		inline int*
		owned()
		{
			return &mOwnedCount;
		}

		inline int*
		physicsHz()
		{
			return &mTargetPhysics;
		}

		inline int*
		networkHz()
		{
			return &mTargetNetwork;
		}

		inline int*
		graphicsHz()
		{
			return &mTargetGraphics;
		}

		inline float*
		timescale()
		{
			return &mTimescale;
		}

		inline float*
		timestep()
		{
			return &mTimestepAvg;
		}

		inline float*
		wellForce()
		{
			return &mWellForce;
		}

		inline float*
		elasticity()
		{
			return &mElasticity;
		}

		inline float*
		friction()
		{
			return &mFriction;
		}
	};
}