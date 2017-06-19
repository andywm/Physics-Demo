#pragma once
#include <vector>
#include <atomic>
#include <mutex>

namespace Utilities
{
	template<typename T>
	class DoubleBuffer
	{
	private:
		std::vector<T> mBA;
		std::vector<T> mBB;
		std::vector<T> *mFront, *mBack;
		std::mutex mSwapMutex;
		bool mBackIsNewer;
	public:
		DoubleBuffer()
			:
			mFront(&mBA),
			mBack(&mBB),
			mBackIsNewer(true)
		{}

		void
		commitWrite(std::vector<T> * swapIn)
		{
			std::lock_guard<std::mutex> lock(mSwapMutex);
			mBack->swap(*swapIn);
			mBackIsNewer = true;
		}

		std::vector<T> *
		read()
		{
			return mFront;
		}

		void
		swap()
		{
			std::lock_guard<std::mutex> lock(mSwapMutex);
			if (mBackIsNewer)
				mBack->swap(*mFront);
			mBackIsNewer = false;
		}
	};
}