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
	namespace Pass
	{
		class EnforceDefault final : public Pass::Base
		{
		public:
			explicit EnforceDefault(std::shared_ptr<Render::State> & d);
			~EnforceDefault() = default;
			void execute() override;
		};
	}
}