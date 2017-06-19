#include <memory>
#include <vector>
#include "stage_base.h"
#include "pass_base.h"
#include "render_state.h"
#include "set_shader.h"

namespace Render
{
	namespace Stage
	{
		SetShader::SetShader(std::shared_ptr<Render::State> & d)
			:
			Base(d)
		{}

		void 
		SetShader::execute()
		{
			sceneState()->context()->setShaders();
			//...
		}
	}
}