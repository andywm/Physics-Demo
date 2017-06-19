#include <chrono>
#include <ctime>
#include "clock.h"

namespace Utilities
{
	Clock::Clock()
		:
		mLastTick(0),
		mPace(PACE::REAL),
		mSpeedMultiplier{ 0.25, 0.5, 1.0, 2.0, 4.0 }
	{
		resume();
	}

	void
	Clock::increasePace()
	{
		if (mPace != PACE::QUAD)
			mPace = static_cast<PACE>(static_cast<int>(mPace) + 1);
	}
	void
	Clock::decreasePace()
	{
		if (mPace != PACE::QUATER)
			mPace = static_cast<PACE>(static_cast<int>(mPace) - 1);
	}

	/*
	tick statemachine.
	*/
	void
	Clock::pause()
	{
		mState = STATE::PAUSED;
		mLastTick = elapsed();
	}
	void
	Clock::resume()
	{
		mState = STATE::RUSUMING;
		mLast = std::chrono::system_clock::now();
	}
	void Clock::reset()
	{
		mPace = PACE::REAL;
		mLastTick = 0;
		resume();
	}
	double
	Clock::elapsed()
	{
		const auto now = std::chrono::system_clock::now();
		const auto time = now - mLast;

		const std::chrono::duration<double> 
			elapsedSeconds = now - mLast;
		mLast = std::chrono::system_clock::now();

		return 
			std::chrono::duration_cast<std::chrono::nanoseconds>
			(elapsedSeconds).count() / 1000000000.0;
	}

	double
	Clock::tick()
	{
		if (mState == STATE::RUNNING)
		{
			return elapsed();
		}
		else if (mState == STATE::PAUSED)
		{
			return 0.0;
		}
		else
		{
			mState = STATE::RUNNING;
			return (mLastTick + elapsed());
		}
	}
}