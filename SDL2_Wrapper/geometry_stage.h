#pragma once

#include <memory>
#include <vector>
#include "stage_base.h"
#include "pass_base.h"
#include "render_state.h"

namespace Render
{
	namespace Stage
	{
		class Geometry final : public Stage::Base
		{
		public:
			explicit Geometry(std::shared_ptr<Render::State> & d);
			~Geometry() = default;
			void execute() override;
		};
	}
}