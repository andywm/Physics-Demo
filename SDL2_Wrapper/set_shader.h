#pragma once

#include <memory>
#include <vector>
#include <glm\glm.hpp>
#include <string>
#include "stage_base.h"
#include "pass_base.h"
#include "d3d_context.h"
#include "render_state.h"

namespace Render
{
	namespace Stage
	{
		class SetShader final : public Stage::Base
		{
		public:
			explicit SetShader(std::shared_ptr<Render::State> & d);
			~SetShader() = default;
			void execute() override;
		};
	}
}