#include <memory>
#include <vector>
#include "stage_base.h"
#include "pass_base.h"
#include "render_state.h"
#include "set_shader.h"
#include "enforce_default.h"

namespace Render
{
	namespace Pass
	{
		EnforceDefault::EnforceDefault(std::shared_ptr<Render::State> & d)
			:
			Base(d)
		{
			instructions().push_back(std::unique_ptr<Stage::SetShader>
				(new Stage::SetShader(sceneState())));
		}

		void 
		EnforceDefault::execute()
		{
			sceneState()->context()->pipeline().clearColour(
				glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
	
			auto shaders = sceneState()->shaders();

			for(const auto shader : shaders)
			{ 
				if (shader->identity() == Assets::SysShader::IDENTITY::VS)
				{
					if (shader->name() == "default_vs")
					{
						sceneState()->context()->pipeline().shader(shader);
					}
				}
				else if (shader->identity() == Assets::SysShader::IDENTITY::FS)
				{
					if (!sceneState()->scene()->specialRenderMode())
					{
						if (shader->name() == "default_fs")
						{
							sceneState()->context()->pipeline().shader(shader);
						}
					}
					else
					{
						if (shader->name() == "normal_fs")
						{
							sceneState()->context()->pipeline().shader(shader);
						}
					}

				}
			}
			sceneState()->context()->pipeline().setSelfAsDefault();
			Pass::Base::execute();
		}
	}
}