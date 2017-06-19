#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <glm/vec3.hpp>
#include <map>
#include "mesh.h"
#include "image.h"
#include "model.h"
#include "Registry.h"
#include "associative_language_parser.h"

namespace APL = ::AssociativeLanguageParser;

namespace Assets
{
	namespace
	{
		Assets::Archtype::Adjustment
		analyseForVectors(const std::vector<std::string> &line, 
			const int beginAt)				
		{
			glm::vec3 offset, orientation, scale;
			scale = glm::vec3(1);
			for (unsigned int i = beginAt; i < line.size(); i++)
			{
				if (line[i] == "offset" || line[i] == "position")
				{
					offset = APL::Extended::vector3FromString(line[i + 1]);
				}
				else if (line[i] == "orientation")
				{
					orientation = APL::Extended::vector3FromString(line[i + 1]);
				}
				else if (line[i] == "scale")
				{
					scale = APL::Extended::vector3FromString(line[i + 1]);
				}
			}
			return Archtype::Adjustment{ offset, orientation, scale };
		}
	}

	/*
		retrieves all assets and packs them,
		retrieves shadders

		loads in world file and builds world config struct.
	*/
	Registry::Registry(const std::string & regdir)
	{
		std::vector<std::vector<std::string>> dirs;
		std::string worldcfg = "";

		//read the top level assets.txt file.
		if (APL::readFile(".\\" + regdir + "\\assets.txt", dirs))
		{
			//go through all the directories listed in that file.
			for (auto subdir : dirs)
			{
				//what resource, what path to that resource.
				const std::string resourceID = subdir[0];
				const std::string resourcePath = subdir[1];

				//make the world config object last.
				if (resourceID == "worldconf")
				{
					worldcfg = regdir + "\\" + resourcePath;
					continue;
				}
				const std::string relPath(regdir + "\\" + resourcePath + "\\");
				std::vector<std::vector<std::string>> assetPaths;

				//read in the relationship file found at that location.
				APL::readFile(relPath + "relationships.txt", assetPaths);

				//relative path correction.
				for (auto & p : assetPaths)
				{
					p[2] = relPath + p[2];
				}
				if (resourceID == "simobj")
					loadWorldAsset(resourcePath, assetPaths);
				else if (resourceID == "shaders")
					loadShaderAsset(assetPaths);
			}
		}
		buildWorldConfObject(worldcfg);
	}

	void 
	Registry::buildWorldConfObject(const std::string & world)
	{
		std::vector<std::vector<std::string>> worldElements;
		APL::readFile(world, worldElements);

		WorldConfig::EnvSettings settings;
		//foreach line
		for (auto & element : worldElements)
		{
			//get general world settings...
			if(element[0] == "env_setting")
			{
				if (element[1] == "balls")
				{
					std::istringstream iss(element[2], std::ios::in);
					int value;
					iss >> value;
					//cap
					value = (value < 256) ? 256 : (value>64000)? 64000 : value;
					settings.balls = value;
				}
				else if (element[1] == "phyHz")
				{
					std::istringstream iss(element[2], std::ios::in);
					int value;
					iss >> value;
					value = (value < 1) ? 1 : value;
					settings.phyHz = value;
				}
				else if (element[1] == "netHz")
				{
					std::istringstream iss(element[2], std::ios::in);
					int value;
					iss >> value;
					value = (value < 1) ? 1 : value;
					settings.netHz = value;
				}
				else if (element[1] == "gfxHz")
				{
					std::istringstream iss(element[2], std::ios::in);
					int value;
					iss >> value;
					value = (value < 1) ? 1 : value;
					settings.gfxHz = value;
				}
				else if (element[1] == "boundary")
				{
					std::istringstream iss(element[2], std::ios::in);
					int value;
					iss >> value;
					settings.boundary = value;
				}
				else if (element[1] == "timescale")
				{
					std::istringstream iss(element[2], std::ios::in);
					float value;
					iss >> value;
					value = (value < 0.01f) ? 0.01f : (value > 1.0f)? 1.0f : value;
					settings.timescale = value;;
				}
				else if (element[1] == "elasticity")
				{
					std::istringstream iss(element[2], std::ios::in);
					float value;
					iss >> value;
					value = (value < 0.0f) ? 0.0f : (value > 1.0f) ? 1.0f : value;
					settings.elasticity = value;
				}
				else if (element[1] == "friction")
				{
					std::istringstream iss(element[2], std::ios::in);
					float value;
					iss >> value;
					value = (value < 0.0f) ? 0.0f : (value > 1.0f) ? 1.0f : value;
					settings.friction = value;
				}
				else if (element[1] == "light")
				{
					std::istringstream iss(element[2], std::ios::in);
					float value;
					iss >> value;
					value = (value <1) ? 1.0f : value;
					settings.light = value;
				}
				else if (element[1] == "medium")
				{
					std::istringstream iss(element[2], std::ios::in);
					float value;
					iss >> value;
					value = (value <1) ? 1.0f : value;
					settings.medium = value;
				}
				else if (element[1] == "heavy")
				{
					std::istringstream iss(element[2], std::ios::in);
					float value;
					iss >> value;
					value = (value <1) ? 1.0f : value;
					settings.heavy = value;
				}
			}
			//set camera positions and orientations
			else if (element[0] == "camera")
			{
				const auto adj = analyseForVectors(element, 2);
				const WorldConfig::CameraConfig cfg{ adj.offset, adj.orientation };

				if (element[1] == "c1")	
					mWorldConfig.cameraConfig(cfg, 0);					
				else if (element[1] == "c2")
					mWorldConfig.cameraConfig(cfg, 1);
			}
			//make an entry for a simobject
			else if (element[0] == "simobj")
			{
				const auto adj = analyseForVectors(element, 2);
				const auto wa = WorldConfig::WorldAsset
				{
					element[1], //makeA
					mAssets[element[1]], //using these assets
					adj.offset, //and these transformation values.
					adj.orientation,
					adj.scale
				};
				mWorldConfig.worldAsset(wa);
			}
		}
		mWorldConfig.settings(settings);
	}

	void
	Registry::loadShaderAsset(
			const std::vector<std::vector<std::string>> & resources)
	{
		for (auto & s_pair : resources)
		{
			SysShader::IDENTITY id;
			if (s_pair[0] == "vs")
				id = SysShader::IDENTITY::VS;
			else
				id = SysShader::IDENTITY::FS;

			const std::shared_ptr<SysShader> ptr(
				new SysShader(s_pair[2], s_pair[1], id));
			mShaderInfo.push_back(ptr);
		}
	}

	void
	Registry::loadArchtypeAsset(std::shared_ptr<AssetPack> asset,
		const std::string & file)
	{
		std::vector<std::vector<std::string>> fileContents;
		APL::readFile(file, fileContents);

		//pack into structure
		//first item is always the main orientation
		//look for scale, offset and orientation + name

		for (const auto & line : fileContents)
		{
			const std::string uniqID = line[0];

			if (uniqID == "master")
			{
				const auto vecs = analyseForVectors(line, 1);
				asset->archtype.master(vecs.scale);
			}
			else
			{			
				asset->archtype.adjustment(uniqID, analyseForVectors(line, 1));
			}
		}
	}

	void
	Registry::loadWorldAsset(const std::string & resourceID,
			const std::vector<std::vector<std::string>> & resources)
	{
		std::map<std::string, std::shared_ptr<Model>> tempModelStore;

		//apl syntax: isA name path [optional]
		for (auto resource : resources)
		{
			const std::string isA = resource[0];
			const std::string name = resource[1];
			const std::string file = resource[2];

			//ensure asset exists
			{
				const auto & temp = mAssets.find(resourceID);

				if(temp == mAssets.end())
					mAssets.insert(std::make_pair(resourceID, 
						std::shared_ptr<AssetPack> (new AssetPack())));
			}
			const auto & asset = mAssets.find(resourceID)->second;

			if (isA == "model")
			{
				if (tempModelStore.find(name) == tempModelStore.end())
				{
					const std::shared_ptr<Model> mod(new Model(file));
					tempModelStore.insert(std::make_pair(name, mod));
				}

				asset->meshes.emplace(std::make_pair(
					name,
					tempModelStore[name]->getMesh()));
			}
			else if (isA == "image")
			{
				asset->textures.emplace(std::make_pair(
					name,
					std::shared_ptr<Image>(new Image(file))));
			}
			else if (isA == "material")
			{
				asset->materials.emplace(std::make_pair(
					name,
					std::shared_ptr<Material>(new Material(file))));
			}
			else if (isA == "archtype")
			{
				loadArchtypeAsset(asset, file);
			}
		}
	}

	std::shared_ptr<Mesh>
	Registry::mesh(const std::string & set, const std::string & name) const
	{
		const auto & asset = mAssets.find(set)->second;
		const auto it = asset->meshes.find(name);
		return it->second;
	}

	const Archtype 
	Registry::archtype(const std::string & set)
	{
		const auto & asset = mAssets.find(set)->second;
		return asset->archtype;
	}

	
}