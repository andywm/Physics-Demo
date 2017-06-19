#pragma once

#include <memory>
#include <vector>
#include "stage_base.h"
#include "pass_base.h"

namespace Render
{
	namespace Pass
	{
		void Base::execute()
		{
			for (const auto & ins : instructions())
			{
				ins->execute();
			}
		}
	}
}