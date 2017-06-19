#pragma once

#include <memory>
#include <vector>
#include "stage_base.h"

namespace Render
{ 
	namespace Pass
	{
		class Base : public Stage::Base
		{
		private:
			std::vector<std::unique_ptr<Stage::Base>> mInstructions;
		protected:
			inline std::vector<std::unique_ptr<Stage::Base>> & 
				instructions()
			{
				return mInstructions;
			}

		public:
			explicit Base(std::shared_ptr<Render::State> & d) : Stage::Base(d) {}
			virtual void execute() override;
			virtual ~Base() = default;
		};
	}
}