#include "plane.h"

namespace Physics
{
	Plane::Plane(const float h)
		:
		mHeight(h),
		mNormal(glm::vec3(0,1,0))
	{}

	float 
	Plane::distance(const glm::vec3 & point)
	{
		auto projPointOnPlane = point - (glm::dot(point, normal()) * normal());
		auto path = projPointOnPlane - point;
		float length = glm::length(path);//.length();
		return length;
	}
}