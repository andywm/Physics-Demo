#pragma once
#include <glm\glm.hpp>

namespace Physics
{
	/*
		axis aligned in xz plane only. 
		y height.
	*/
	class Plane
	{
	private:
		float mHeight;
		glm::vec3 mNormal;
	public:
		Plane(const float h);
		~Plane() = default;
		inline const glm::vec3 & 
		normal()
		{
			return mNormal;
		}

		float distance(const glm::vec3 & point);
	};
}
