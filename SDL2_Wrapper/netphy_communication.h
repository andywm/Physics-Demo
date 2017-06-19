#pragma once
#include <map>
#include <vector>
#include <mutex>
#include <atomic>
#include "sphere.h"
#include "Peer.h"

namespace Shared
{
	using transfer_map = std::map<unsigned int,
		std::map<unsigned int, Physics::Sphere *>>;
	using force_map = std::map<unsigned int,
		std::map<unsigned int, Physics::Sphere *>>;

	class SharingMechanism
	{
	private:
		std::mutex mMutex;
		bool mOwnershipChanged;
	private:
		std::map<unsigned int, Physics::Sphere> mToContest;
		std::map<unsigned int, Physics::Sphere> mToGainOwn;
		std::map<unsigned int, glm::vec3*> mForeignPeers;
		std::map<unsigned int, glm::vec3> mForcesFromExternal;
		force_map mForcesOnExternal;
		transfer_map mNonConflictTransfer;
		transfer_map mConflictTransfer;
		transfer_map mClearConflicts;
	public:
		inline glm::vec3
		externalForceFor(const unsigned int id)
		{
			std::lock_guard<std::mutex> lk(mMutex);
			auto iter = mForcesFromExternal.find(id);
			if (iter != mForcesFromExternal.end())
				return iter->second;
			return glm::vec3(0.0f);
		}
		inline void
		mkExternalForceFor(const unsigned int id, const glm::vec3 & v)
		{
			std::lock_guard<std::mutex> lk(mMutex);
			auto iter = mForcesFromExternal.find(id);
			if (iter != mForcesFromExternal.end())
				iter->second = iter->second + v;
			else
				mForcesFromExternal.insert(std::make_pair(id, v));
		}
		inline bool
		ownershipChanged()
		{
			mOwnershipChanged = !mOwnershipChanged;
			return !mOwnershipChanged;
		}
		inline void
		contestObject(const unsigned int id, Physics::Sphere s)
		{
			mToContest.insert_or_assign(id, s);
		}
		inline void
		transferObject(const unsigned int id, Physics::Sphere s)
		{
			mToGainOwn.insert_or_assign(id, s);
		}
	public:
		SharingMechanism() = default;
		~SharingMechanism() = default;
		void ownershipUpdate(std::vector<Physics::Sphere> & spheres, 
			std::vector<Physics::Sphere *> & localconflicts);
		void contentionUpdate(std::vector<Physics::Sphere> & spheres,
			std::vector<Physics::Sphere *> & localconflicts, const Peer p);

		void consumeTransfersOut(transfer_map & swap);
		void consumeContestedOut(transfer_map & swap);

		void addMonitor(unsigned int id, glm::vec3 * peerPos);
		void remMonitor(unsigned int id);

		void transferObjects(std::vector<Physics::Sphere> & spheres,
			std::vector<Physics::Sphere *> & localconflicts, const Peer p);
	};
}