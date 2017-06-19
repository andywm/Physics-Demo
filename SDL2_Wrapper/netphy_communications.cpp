#include "netphy_communication.h"

namespace Shared
{
	void 
	SharingMechanism::consumeTransfersOut(transfer_map & swap)
	{
		mNonConflictTransfer.swap(swap);
		mNonConflictTransfer.clear();
	}

	void 
	SharingMechanism::consumeContestedOut(transfer_map & swap)
	{
		mConflictTransfer.swap(swap);
		mConflictTransfer.clear();
	}

	void 
	SharingMechanism::addMonitor(unsigned int id, glm::vec3 * peerPos)
	{
		std::lock_guard<std::mutex> lk(mMutex);
		mForeignPeers.insert(std::make_pair(id, peerPos));
	}

	void 
	SharingMechanism::remMonitor(unsigned int id)
	{
		std::lock_guard<std::mutex> lk(mMutex);
		auto iter = mForeignPeers.find(id);
		if (iter != mForeignPeers.end())
			mForeignPeers.erase(iter);
	}

	void 
	SharingMechanism::ownershipUpdate(std::vector<Physics::Sphere> & spheres,
			std::vector<Physics::Sphere *> & localconflicts)
	{
		for (auto iter = mToContest.begin(); iter != mToContest.end(); iter++)
		{
			iter->second.mass(spheres[iter->first].mass());
			iter->second.radius(spheres[iter->first].radius());

			spheres[iter->first] = iter->second;
			spheres[iter->first].own(OwnershipBit::NotOwned);
			if (std::find(localconflicts.begin(),
				localconflicts.end(), 
				&spheres[iter->first]) == localconflicts.end())
			{
				localconflicts.push_back(&spheres[iter->first]);
			}
			mOwnershipChanged = true;
			mToContest.clear();
		}

		for (auto iter = mToGainOwn.begin(); iter != mToGainOwn.end(); iter++)
		{
			iter->second.mass(spheres[iter->first].mass());
			iter->second.radius(spheres[iter->first].radius());

			spheres[iter->first] = iter->second;
			spheres[iter->first].own(OwnershipBit::Owned);

			mOwnershipChanged = true;
			mToGainOwn.clear();
		}
	}

	void
	SharingMechanism::transferObjects(std::vector<Physics::Sphere> & spheres,
		std::vector<Physics::Sphere *> & localconflicts, const Peer p)
	{
		for (auto iter = mForeignPeers.begin();
			iter != mForeignPeers.end();
			iter++)
		{
			auto target = iter->first;
			for (unsigned int ballID = 0; ballID < spheres.size(); ballID++)
			{
				if (spheres[ballID].own() == OwnershipBit::NotOwned) continue;

				glm::vec3 ownDistVec = spheres[ballID].position() - p.position;
				float ownDist = glm::length(ownDistVec);

				if (ownDist > 10 + spheres[ballID].radius())
				{
					glm::vec3 distVec = spheres[ballID].position() - *(iter->second);
					float dist = glm::length(distVec);

					if (dist < 10 + spheres[ballID].radius())
					{
						//transfer without conflict
						//make internal map if not present.
						if (mNonConflictTransfer.find(target) == mNonConflictTransfer.end())
						{
							mNonConflictTransfer.insert(std::make_pair(
								target,
								std::map<unsigned int, Physics::Sphere*>()
							));
						}
						//add object.
						if (mNonConflictTransfer[target].find(ballID)
							== mNonConflictTransfer[target].end())
						{
							mNonConflictTransfer[target].insert(
								std::make_pair(
									ballID,
									&spheres[ballID])
							);
							spheres[ballID].own(OwnershipBit::NotOwned);
						}
					}
				}
			}
		}
	}

	void
	SharingMechanism::contentionUpdate(std::vector<Physics::Sphere> & spheres,
		std::vector<Physics::Sphere *> & localconflicts, const Peer p)
	{
		transfer_map conflict;
		transfer_map noconflict;

		for (auto iter = mForeignPeers.begin();
			iter != mForeignPeers.end();
			iter++)
		{
			//for external transfers
			for (unsigned int ballID = 0; ballID < spheres.size(); ballID++)
			{
				if (spheres[ballID].own() == OwnershipBit::NotOwned) continue;
				glm::vec3 ownDistVec = spheres[ballID].position() - p.position;
				float ownDist = glm::length(ownDistVec);

				glm::vec3 distVec = spheres[ballID].position() - *(iter->second);
				float dist = glm::length(distVec);

				auto target = iter->first;
				if (ownDist < 10 + spheres[ballID].radius())
				{
					if (dist < 10 + spheres[ballID].radius())
					{
						//transfer with conflict
						//make internal map if not present.
						if (conflict.find(target) == conflict.end())
						{
							conflict.insert(std::make_pair(
								target,
								std::map<unsigned int, Physics::Sphere*>()
							));
						}
						//add object.
						if (conflict[target].find(ballID)
							== conflict[target].end())
						{
							conflict[target].insert(
								std::make_pair(
									ballID,
									&spheres[ballID])
							);
						}
					}
					else
					{
						//remove outdated conflicts.
						auto ans = std::find(localconflicts.begin(),
							localconflicts.end(), &spheres[ballID]);

						if (ans != localconflicts.end())
						{
							localconflicts.erase(ans);
							mOwnershipChanged = true;
						}
					}
				}
				else
				{
					if (dist < 10 + spheres[ballID].radius())
					{
						//transfer without conflict
						//make internal map if not present.
						if (noconflict.find(target) == noconflict.end())
						{
							noconflict.insert(std::make_pair(
								target,
								std::map<unsigned int, Physics::Sphere*>()
							));
						}
						//add object.
						if (noconflict[target].find(ballID)
							== noconflict[target].end())
						{
							noconflict[target].insert(
								std::make_pair(
									ballID,
									&spheres[ballID])
							);
						}
					}
				}

				if (noconflict.size() > 0 || conflict.size() > 0)
				{
					std::lock_guard<std::mutex> lk(mMutex);
					mOwnershipChanged = true;
					mConflictTransfer.swap(conflict);
					mNonConflictTransfer.swap(noconflict);
				}
			}
		}
	}
}