#include "d3d_context.h"

namespace Render
{
	namespace d3d11
	{
		D3DContext::ResourceManager::ResourceManager(D3DContext & parent)
			:
			mContext(parent)
		{}

		std::shared_ptr<Geometry>
		D3DContext::ResourceManager::geometry(std::shared_ptr<Assets::Mesh> m)
		{
			for (const auto & record : mMeshes)
			{
				//address comparision.
				if (record.sysFormat.get() == m.get())
				{
					return record.gpuFormat;
				}

			}
			auto newgeom = 
				std::shared_ptr<Geometry>(new Geometry(mContext, *m));
			const MeshRecord newrec = { m, newgeom };
			mMeshes.push_back(newrec);
			return newgeom;
		}

		std::shared_ptr<Shader> &
		D3DContext::ResourceManager::getShader(
			const std::shared_ptr<Assets::SysShader> sd_set)
		{
			return mShaders[sd_set];
		}

		void
		D3DContext::ResourceManager::enableShaders(
			const std::shared_ptr<Assets::SysShader> sd_set)
		{
			/*code for finding if a shader exists, otherwise making it*/
			const auto s = mShaders.find(sd_set);
			if (s != mShaders.end())
			{
				s->second->activate(mContext.mContext.Get());
				return;
			}
			mShaders.emplace(
				std::make_pair(sd_set,
								std::shared_ptr<Shader>(
										new Shader(sd_set->path(), 
											mContext.mDevice.Get(), 
											sd_set->identity()))
								)
			);

			//auto iter = mShaders.find(sID);
			//if (iter != mShaders.end())
				//iter->second->activate(mContext.mContext.Get());
		}

		/*
		void
		D3DContext::ResourceManager::addShader(
									const std::string & name, Shader * shade)
		{
			mShaders.emplace(
				std::make_pair(name, std::unique_ptr<Shader>(shade)));
		}*/

		void 
		D3DContext::ResourceManager::addShaderPath(
			const std::shared_ptr<Assets::SysShader> shade)
		{
			mShaderPaths.push_back(shade);
		}

		std::shared_ptr<Render::d3d11::Texture>
		D3DContext::ResourceManager::setTexture(std::shared_ptr<Assets::Image> img)
		{
			/*code for finding if a shader exists, otherwise making it*/
			const auto im = mTextures.find(img);
			if (im != mTextures.end())
			{
				return im->second;
			}
			const auto t = std::shared_ptr<Texture>(
				new Texture(&mContext, *img));
			mTextures.emplace(std::make_pair(img, t));
			return t;
		}
	}
}