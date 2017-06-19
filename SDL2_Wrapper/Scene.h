#pragma once
#include <memory>
#include <glm/glm.hpp>
//#include "scene_graph.h"
#include "so_camera.h"
#include "so_scenery.h"

namespace Render
{
	namespace Scene
	{
		class Scene
		{
		private:
			bool mSpecialRenderMode;
			bool mShowStatistics;
			glm::vec4 mBackground;
			Graphics::Object::Camera mCamera;
			int mActiveCamera;
		private:
			std::vector<Graphics::Object::Prop> mBalls;
			std::vector<Graphics::Object::Prop> mPeers;
			std::vector<Graphics::Object::Prop> mScenery;
		public:
			Scene();
			~Scene()=default;
			void setAspect(const glm::vec2 & aspect);
			//void activateCamera(const unsigned int index);		

			void cameraOveride(const Assets::WorldConfig::CameraConfig & cf);

			inline void 
			makeScene(std::vector<Graphics::Object::Prop> & v)
			{
				mScenery.swap(v);
			}

			inline void
			makeBallStructure(std::vector<Graphics::Object::Prop> & v)
			{
				mBalls.swap(v);
			}

			inline void
			makePeers(std::vector<Graphics::Object::Prop> & v)
			{
				mPeers.swap(v);
			}

			inline std::vector<Graphics::Object::Prop> &
			balls()
			{
				return mBalls;
			}

			inline std::vector<Graphics::Object::Prop> &
			terrain()
			{
				return mScenery;
			}

			inline std::vector<Graphics::Object::Prop> &
			peers()
			{
				return mPeers;
			}

			inline Graphics::Object::Camera &
			Scene::camera()
			{
				return mCamera;
			}

			inline const bool
			specialRenderMode() const
			{
				return mSpecialRenderMode;
			}

			void
			specialRenderMode(const bool srm)
			{
				mSpecialRenderMode = srm;
			}

			inline const bool
			showStatistics() const
			{
				return mShowStatistics;
			}

			void
			showStatistics(const bool ss)
			{
				mShowStatistics = ss;
			}
		};
	}
}