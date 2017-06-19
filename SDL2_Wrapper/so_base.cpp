#pragma once
#include <memory>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include "so_base.h"

namespace Graphics
{
	namespace Object
	{
		Base::Base(const glm::vec3 & pos, const glm::vec3 & rot,
			const glm::vec3 & scl)
			:
			mBaseTranslation(pos),
			mEulerRotation(rot),
			mScale(scl),
			mVisible(true)
		{
			
		}

		const glm::mat4 
		Base::computeTransform() const
		{
			glm::mat4 transform;
			
			transform *= glm::translate(position());
			transform *= glm::rotate(rotation().x, glm::vec3(1, 0, 0));
			transform *= glm::rotate(rotation().y, glm::vec3(0, 1, 0));
			transform *= glm::rotate(rotation().z, glm::vec3(0, 0, 1));
			transform *= glm::scale(scale());

			return transform;
		}
	}
}
