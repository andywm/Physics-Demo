/*------------------------------------------------------------------------------
author: andywm, 2016
description:
3D camera defintion, provides basic camera
functionallity including panning, tilting, and space
traversal.
------------------------------------------------------------------------------*/

#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>
//#include "scene_graph.h"
#include "so_includes.h"
#include "perspective_view.h"

namespace Graphics
{
	namespace Object
	{
		/*
		camera class, provides 2 axis rotation and
		3D space navigation
		*/
		class Camera : public Base
		{
		public:
			//members//
		private:
			const float rotRate;
			const float linRate;
			const int linMultiplier;
			const int rotMultiplier;


			float mAspectRatio;

			//glm::mat4 mProjection; 
			glm::mat4 mRotation;
			glm::mat4 mTranslation;
			float pitch, yaw;
			int mObjectFocus;
			//methods//
		public:
			Camera(const glm::vec3 & pos, const glm::vec3 & orient, 
				const int linr, const int rotr);
			Camera& operator=(const Camera&) = delete;
			~Camera() = default;
			void pan(const int by);
			void tilt(const int by);
			void move(const glm::vec3 & by);
			/*return the camera's projection matrix*/
			inline const glm::mat4
			Camera::projection() const
			{
				return Utilities::perspective(50.0f, mAspectRatio);
			}
			inline const glm::mat4
			Camera::view() const
			{
				return glm::inverse(mTranslation *  mRotation);
			}
		
			void aspectRatio(const float aspect);
		};
	}
}
#endif 