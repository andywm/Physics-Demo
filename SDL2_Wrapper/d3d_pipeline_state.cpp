#include "d3d_pipeline_state.h"
#include "d3d_shaders.h"
#include "sys_shader.h"
namespace Render
{
	namespace d3d11
	{		
		void
		PipelineState::shader(const std::shared_ptr<Assets::SysShader> shader)
		{
			if (shader->identity() == Assets::SysShader::IDENTITY::VS)
			{
				mVertexSlot = shader;
			}
			else if (shader->identity() == Assets::SysShader::IDENTITY::FS)
			{
				mFragmentSlot = shader;
			}
		}

		const std::vector<std::shared_ptr<Assets::SysShader>> 
		PipelineState::shaders() const
		{
			std::vector<std::shared_ptr<Assets::SysShader>> values;
			values.push_back(mVertexSlot);
			values.push_back(mFragmentSlot);

			return values;
		}

		void
		PipelineState::shallowCopy(const PipelineState & rhs)
		{
			mVertexSlot = rhs.mVertexSlot;
			mFragmentSlot = rhs.mFragmentSlot;
			mClearColour = rhs.mClearColour;
			mGeometry = rhs.mGeometry;
			mDefault = rhs.mDefault;
		}

		void
		PipelineState::resetDefaults()
		{
			shallowCopy(*mDefault);
		}

		PipelineState::PipelineState(const PipelineState & rhs)
		{
			shallowCopy(rhs);
		}

		void
		PipelineState::setSelfAsDefault()
		{
			mDefault = std::shared_ptr<PipelineState>(new PipelineState(*this));
			mDefault->mDefault = mDefault;
		}

		void
		PipelineState::restoreState(const PipelineState & rhs)
		{
			shallowCopy(rhs);
		}
	}
}


