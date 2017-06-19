#include <memory>
#include <vector>
#include <AntTweakBar.h>
#include "clear.h"
#include "master.h"
#include "geometry_stage.h"
#include "enforce_default.h"
#include "sys_shader.h"

namespace Render
{
	namespace Pass
	{
		Master::Master(std::shared_ptr<Render::d3d11::D3DContext> ctx,
			std::shared_ptr<Render::Scene::Scene> sc,
			std::vector<std::shared_ptr<Assets::SysShader>> shdrs)
			: 
			Base(mData)
		{
			mData = std::shared_ptr<Render::State>
							(new Render::State(ctx, sc, shdrs));
			//note: mSceneState = Render::State & mData as per the init list.

			instructions().push_back(std::unique_ptr<Pass::EnforceDefault>
							(new Pass::EnforceDefault(sceneState())));

			instructions().push_back(std::unique_ptr<Stage::Clear>
							(new Stage::Clear(sceneState())));

			instructions().push_back(std::unique_ptr<Stage::Geometry>
							(new Stage::Geometry(sceneState())));
		}

		void Master::execute()
		{
			Pass::Base::execute();
			TwDraw();
			//if(sceneState()->scene()->showStatistics()) TwDraw();
			sceneState()->context()->swap();
		}
	}
}