#include <memory>
#include "render_state.h"

namespace Render
{
	State::State(std::shared_ptr<Render::d3d11::D3DContext> ctx,
		std::shared_ptr<Render::Scene::Scene> sc,
		std::vector<std::shared_ptr<Assets::SysShader>> & shdrs)
		:
		mScene(sc),
		mContext(ctx),
		mShaderPaths(shdrs)
	{}

	const std::shared_ptr<Render::d3d11::D3DContext>
	State::context() const
	{
		return mContext;
	}

	const std::shared_ptr<Render::Scene::Scene> 
	State::scene() const
	{
		return mScene;
	}

	const std::vector<std::shared_ptr<Assets::SysShader>> & 
	State::shaders() const
	{
		return mShaderPaths;
	}

}