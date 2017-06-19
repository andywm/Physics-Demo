#include <memory>
#include <glm\glm.hpp>
#include "Scene.h"

namespace Render
{
	namespace Scene
	{
		Scene::Scene()
			:
			mBackground(glm::vec4(0.5f,0.5f,0.5f, 1.0f)),
			mActiveCamera(0),
			mSpecialRenderMode(false),
			mShowStatistics(true),
			mCamera(glm::vec3(0),glm::vec3(0),1,1)
		{}

		void 
		Scene::cameraOveride(const Assets::WorldConfig::CameraConfig & cf)
		{
			mCamera.move(cf.position);
			mCamera.tilt(cf.orientation.y);
		}

		void 
		Scene::setAspect(const glm::vec2 & aspect)
		{
			mCamera.aspectRatio(static_cast<float>(aspect.x) /
				static_cast<float>(aspect.y));
		}
	}
}