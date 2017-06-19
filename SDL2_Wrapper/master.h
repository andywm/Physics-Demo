#pragma once

#include <memory>
#include <vector>
#include "stage_base.h"
#include "pass_base.h"
#include "render_state.h"
#include "sys_shader.h"
namespace Render
{
	namespace Pass
	{
		class Master final : public Pass::Base
		{
		private:
			std::shared_ptr<Render::State> mData;
		public:
			Master(std::shared_ptr<Render::d3d11::D3DContext> ctx,
				std::shared_ptr<Render::Scene::Scene> sc,
				std::vector<std::shared_ptr<Assets::SysShader>> shdrs);
			~Master() = default;
			virtual void execute() override;
		};
	}
}