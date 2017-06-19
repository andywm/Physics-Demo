#include <map>
#include <string>
#include <glm/vec3.hpp>
#include "archtype.h"

namespace Assets
{
	Archtype::Archtype()
		:
		mMaster(1.0f)
	{
	}

	void 
	Archtype::adjustment(const std::string & key, Adjustment & adj)
	{
		if (adj.scale == glm::vec3(0.0f))
			adj.scale = glm::vec3(1.0f);
		if (mAdjustments.find(key) == mAdjustments.end())
			mAdjustments.insert(std::make_pair(key, adj));

	}

	void 
	Archtype::master(const glm::vec3 & scale)
	{
		if(scale.x > 0.01f && scale.y > 0.01f && scale.z > 0.01f)
			mMaster = scale;
	}

	//this must return a value. Consider ref -> value
	const Archtype::Adjustment &
	Archtype::adjustment(const std::string & key) const
	{
		const auto & it = mAdjustments.find(key);
		if (it != mAdjustments.end())
		{
			return it->second;
		}
		throw;
	}

	const glm::vec3 &
	Archtype::master() const
	{
		return mMaster;
	}
}