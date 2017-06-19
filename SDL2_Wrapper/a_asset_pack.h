#pragma once
#include "mesh.h"
#include "image.h"
#include "material.h"
#include "archtype.h"

namespace Assets
{
	struct AssetPack
	{
		std::map<std::string, std::shared_ptr<Mesh>> meshes;
		std::map<std::string, std::shared_ptr<Image>> textures;
		std::map<std::string, std::shared_ptr<Material>> materials;
		Archtype archtype;
	};
}