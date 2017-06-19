#pragma once

#include "stage_base.h"
#include "render_state.h"

namespace Render
{
	namespace Stage
	{
		class Clear : public Stage::Base
		{
		public:
			explicit Clear(std::shared_ptr<Render::State> & d);
			~Clear() = default;
			void execute() override;
		};


	}
}