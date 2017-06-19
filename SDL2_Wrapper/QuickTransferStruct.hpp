#pragma once
#include <glm\glm.hpp>
#include "OwnershipEnum.h"
namespace BufferObjs
{
	struct PhyToGfx
	{
		OwnershipBit ob;
		float mass;
		glm::vec3 position;
		glm::vec3 orientation;
	};
}