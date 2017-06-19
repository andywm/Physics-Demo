#pragma once
#include <map>
#include <string>
#include <glm/vec3.hpp>

namespace Assets
{
	class Archtype
	{
	public:
		struct Adjustment
		{
			glm::vec3 offset;
			glm::vec3 orientation;
			glm::vec3 scale;
		};

		Archtype();
		~Archtype() = default;
		void adjustment(const std::string & key, Adjustment & adj);
		void master(const glm::vec3 & scale);
		const Adjustment & adjustment(const std::string & key) const;
		const glm::vec3 & master() const;
	private:
		glm::vec3 mMaster;
		std::map<std::string, Adjustment> mAdjustments;
	};
}