#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "so_camera.h"
#include "perspective_view.h"

/*
note, there is now a lot of redundancy in this class and sg_camera. Think about
a refactor or rethink.

also; Camera::MODE was meant to define either a free camera, or an orbital one,
I never did figure out the orbital one.
*/

namespace Graphics
{
	namespace Object
	{
		Camera::Camera(const glm::vec3 & pos, const glm::vec3 & orient, 
			const int linr, const int rotr)
			:
			Base(pos, orient, glm::vec3(1)),
			linRate(1.4f),
			rotRate(0.014f),
			rotMultiplier(rotr),
			linMultiplier(linr),
			pitch((float)orient.x),
			yaw((float)orient.y),
			mAspectRatio(1.0f),
			mObjectFocus(0)
		{
			//mRotation = glm::rotate(r.y, glm::vec3(0, 1, 0));
			//mRotation *= glm::rotate(r.x, glm::vec3(1, 0, 0));
			mTranslation *= glm::translate(glm::vec3(pos.x, pos.y, pos.z));
			//buildSceneEntry();
		}

		/*
		pans the camera on the local Y axis.
		*recomputes entire rotation matrix so operation
		ordering is perserved and correct.
		*/
		void
		Camera::pan(const int by)
		{
			//rotate around the Y before X
			yaw += static_cast<float>(by*rotRate*rotMultiplier);

			mRotation = glm::rotate(pitch, glm::vec3(1, 0, 0));
			mRotation *= glm::rotate(yaw, glm::vec3(0, 1, 0));

			rotation(glm::vec3(rotation().x + pitch, rotation().y + yaw, 0));
		}

		/*
		tilts the camera on the local X axis.
		*recomputes entire rotation matrix so operation
		ordering is perserved and correct.
		*/
		void
		Camera::tilt(const int by)
		{
			//rotate around the Y before X
			pitch += static_cast<float>(by*rotRate*rotMultiplier);

			mRotation = glm::rotate(pitch, glm::vec3(1, 0, 0));
			mRotation *= glm::rotate(yaw, glm::vec3(0, 1, 0));

			rotation(glm::vec3(rotation().x+pitch, rotation().y+yaw, 0));
		}

		/*
		moves the camera through the world on according to its own axis.
		this needs fixing!
		*/
		void
		Camera::move(const glm::vec3 & by)
		{
			auto offset = by * (linRate * linMultiplier) * -1.0f;
			position(offset);
			mTranslation *= glm::translate(offset);
			//glm::vec4 offset;
			//const glm::vec3 pre = by * (linRate * linMultiplier) * -1.0f;

			//offset = (glm::vec4(pre, 1.0) * mRotation);
			//mTranslation *= glm::translate((glm::vec3)offset);

			//position(position() + (glm::vec3)offset);
		}

		/*
		sets the aspect ratio based on screen parameters.
		*/
		void
		Camera::aspectRatio(const float aspect)
		{
			mAspectRatio = aspect;
		}
	}

}