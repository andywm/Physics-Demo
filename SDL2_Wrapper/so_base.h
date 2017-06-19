#pragma once
#include <memory>
#include <glm\glm.hpp>
#include "Registry.h"

namespace Graphics
{
	namespace Object
	{
		class Base
		{
		private:
			glm::vec3 mBaseTranslation, mEulerRotation, mScale;
			std::shared_ptr<Assets::Mesh> mBodyMesh;
			std::shared_ptr<Assets::Material> mMaterial;
			std::shared_ptr<Assets::Image> mTexture;
			bool mVisible;

		public:
			Base(const glm::vec3 & pos, const glm::vec3 & rot, 
				const glm::vec3 & scl);
			virtual ~Base() = default;

			const glm::mat4 computeTransform() const;

			inline const glm::vec3 &
			position() const
			{
				return mBaseTranslation;
			}

		public:
			inline void
			position(const glm::vec3 & pos)
			{
				mBaseTranslation = pos;
			}

			inline const glm::vec3 &
			rotation() const
			{
				return mEulerRotation;
			}

			inline void
			rotation(const glm::vec3 & rot)
			{
				mEulerRotation = rot;
			}

			inline const glm::vec3 &
			scale() const
			{
				return mScale;
			}

			inline void
			scale(const glm::vec3 & scl)
			{
				mScale = scl;
			}

			inline const std::shared_ptr<Assets::Mesh>
			mesh() const
			{
				return mBodyMesh;
			}

			inline void
			mesh(const std::shared_ptr<Assets::Mesh> m)
			{
				mBodyMesh = m;
			}

			inline const std::shared_ptr<Assets::Material>
			material() const
			{
				return mMaterial;
			}

			inline void
			material(const std::shared_ptr<Assets::Material> m)
			{
				mMaterial = m;
			}

			inline const std::shared_ptr<Assets::Image>
			texture() const
			{
				return mTexture;
			}

			inline void
			texture(const std::shared_ptr<Assets::Image> img)
			{
				mTexture = img;
			}

			inline const bool
			visible() const
			{
				return mVisible;
			}

			inline void
			visible(const bool vis)
			{
				mVisible = vis;
			}
		};
	}
}
