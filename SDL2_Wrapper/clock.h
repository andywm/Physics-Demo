#pragma once
#include <array>
#include <chrono>
#include <ctime>

namespace Utilities
{
	class Clock
	{
	public:
		Clock();
		~Clock() = default;
		Clock& operator=(const Clock&) = delete;
		void increasePace();
		void decreasePace();
		void pause();
		void resume();
		void reset();
		double tick();
	private:
		enum class STATE { RUNNING, PAUSED, RUSUMING };
		enum class PACE {QUATER=0, HALF=1, REAL=2, DOUBLE=3, QUAD=4};
		const std::array<double,5> mSpeedMultiplier;
		STATE mState;
		PACE mPace;
		std::chrono::time_point<std::chrono::system_clock> mLast;
		double mLastTick;
		double elapsed();
	};
}