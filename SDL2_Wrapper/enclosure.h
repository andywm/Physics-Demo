#pragma once
#include <glm\glm.hpp>

namespace Physics
{
	/*
	always centred at 0,0,0
	*/
	class Enclosure
	{
	private:
		float mRadius;
	public:
		Enclosure(const float rad);
		~Enclosure() = default;
		inline const float
		distance(const glm::vec3 & point)
		{
			return -(glm::length(point) - mRadius);
		}
		inline const float 
		radius()
		{
			return mRadius;
		}
	};
}
