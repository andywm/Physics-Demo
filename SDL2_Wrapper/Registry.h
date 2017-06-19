#pragma once
#include <string>
#include <vector>
#include <map>
#include <vector>
#include "mesh.h"
#include "image.h"
#include "model.h"
#include "sys_shader.h"
#include "archtype.h"
#include "a_world.h"
#include "a_asset_pack.h"

namespace Assets
{
	
	class Registry
	{
	public:
	
	private:
		std::map<std::string, std::shared_ptr<AssetPack>> mAssets;
		std::vector<std::shared_ptr<SysShader>> mShaderInfo;
		WorldConfig mWorldConfig;
		

	private:
		void loadArchtypeAsset(std::shared_ptr<AssetPack> asset,
			const std::string & file);
		void loadWorldAsset(const std::string & resourceID,
			const std::vector<std::vector<std::string>> & resources);
		void loadShaderAsset(
			const std::vector<std::vector<std::string>> & resources);
		void buildWorldConfObject(const std::string & world);
		

	public:
		explicit Registry(const std::string & regdir);
		
		inline const std::vector<std::shared_ptr<SysShader>> &
		shaderPaths() const
		{
			return mShaderInfo;
		}

		std::shared_ptr<Mesh> mesh(const std::string & set, 
			const std::string & name) const;
		const Archtype archtype(const std::string & set);

		inline const WorldConfig &
		worldConfig() const
		{
			return mWorldConfig;
		}

	};
}
