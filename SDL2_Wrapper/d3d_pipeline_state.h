#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm\glm.hpp>
#include "mesh.h"
#include "material.h"
#include "image.h"
#include "sys_shader.h"

namespace Render
{
	namespace d3d11
	{
		class Geometry;
		class PipelineState
		{
		private:
			std::shared_ptr<Assets::SysShader> mVertexSlot;
			std::shared_ptr<Assets::SysShader> mFragmentSlot;
			glm::vec4 mClearColour;
			glm::mat4 mModel;
			glm::mat4 mView;
			glm::mat4 mProjection;
			std::shared_ptr<Assets::Mesh> mGeometry;
			std::shared_ptr<Assets::Material> mMaterial;
			std::shared_ptr<Assets::Image> mTexture;
			std::shared_ptr<PipelineState> mDefault;
			void shallowCopy(const PipelineState & rhs);

		public:
			void shader(const std::shared_ptr<Assets::SysShader> shader);
			const std::vector<std::shared_ptr<Assets::SysShader>> shaders()
				const;

			PipelineState() = default;
			PipelineState(const PipelineState & rhs);
			~PipelineState() = default;

			void restoreState(const PipelineState & rhs);
			inline void
			clearColour(const glm::vec4 & col)
			{
				mClearColour = col;
			}

			inline const glm::vec4 &
			clearColour() const
			{
				return mClearColour;
			}

			inline void
			geometry(const std::shared_ptr<Assets::Mesh> mesh) 
			{
				mGeometry = mesh;
			}

			inline void
			material(const std::shared_ptr<Assets::Material> mat) 
			{
				mMaterial = mat;
			}

			inline std::shared_ptr<Assets::Material>
			material() const
			{
				return mMaterial;
			}

			inline void
			texture(const std::shared_ptr<Assets::Image> img) 
			{
				mTexture = img;
			}

			inline std::shared_ptr<Assets::Image>
			texture() const
			{
				return mTexture;
			}

			inline PipelineState
			state() const
			{
				return PipelineState(*this);
			}

			inline std::shared_ptr<Assets::Mesh>
			geometry() const
			{
				return mGeometry;
			}

			inline const glm::mat4 &
			projection() const
			{
				return mProjection;
			}

			inline void
			projection(const glm::mat4 & p)
			{
				mProjection = p;
			}

			inline const glm::mat4 &
			view() const
			{
				return mView;
			}

			inline void
			view(const glm::mat4 & v)
			{
				mView = v;
			}

			inline const glm::mat4 &
			model() const
			{
				return mModel;
			}

			inline void
			model(const glm::mat4 & m)
			{
				mModel = m;
			}

			

			void setSelfAsDefault();
			void resetDefaults();

		};
	}
}