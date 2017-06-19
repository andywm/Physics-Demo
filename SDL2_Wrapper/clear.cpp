#include <memory>
#include "clear.h"
#include "master.h"
#include "d3d_pipeline_state.h"
#include "d3d_context.h"
#include "Scene.h"

namespace Render
{
	namespace Stage
	{
		Clear::Clear(std::shared_ptr<Render::State> & d) : Base(d) {}

		void Clear::execute()
		{						
			sceneState()->context()->execute(Render::ACTION::CLEAR);
		}
		
	}
}