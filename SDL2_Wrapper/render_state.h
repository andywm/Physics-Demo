#pragma once

#include <memory>
#include <vector>
#include "d3d_context.h"
#include "Scene.h"
#include "sys_shader.h"


namespace Render
{
	class State
	{
	private:
		std::shared_ptr<Render::d3d11::D3DContext> mContext;
		std::shared_ptr<Render::Scene::Scene> mScene;
		std::vector<std::shared_ptr<Assets::SysShader>> mShaderPaths;
	
	public:
		State(std::shared_ptr<Render::d3d11::D3DContext> ctx,
			std::shared_ptr<Render::Scene::Scene> sc,
			std::vector<std::shared_ptr<Assets::SysShader>> & shdrs);
		~State() = default;
		const std::shared_ptr<Render::d3d11::D3DContext> context() const;
		const std::shared_ptr<Render::Scene::Scene> scene() const;
		const std::vector<std::shared_ptr<Assets::SysShader>> & shaders()
			const;
	};
}