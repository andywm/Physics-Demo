#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "perspective_view.h"

namespace Utilities
{
	const glm::mat4
	perspective(const float fov, const float aspect)
	{
		const glm::mat4 GLtoD3D(glm::translate(glm::vec3(0.0, 0.0, 0.5)) *
								glm::scale(glm::vec3(1.0, 1.0, 0.5)));

		return GLtoD3D * glm::perspectiveLH(glm::radians(fov),
											aspect,
											0.01f,
											10000.0f);
	}
}