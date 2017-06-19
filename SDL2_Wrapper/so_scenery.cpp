#include "so_includes.h"
#include "animation.h"

namespace Graphics
{
	namespace Object
	{
		Prop::Prop(const Assets::AssetPack & pack,
			const glm::vec3 & pos, const glm::vec3 & rot,
			const glm::vec3 & scl, const std::string & id)
			:
			Base(pos, rot, scl)
		{
			const auto & potentialMesh = pack.meshes.find(id);
			if (potentialMesh != pack.meshes.end())
				mesh(potentialMesh->second);

			const auto & potentialMaterial = pack.materials.find(id);
			if (potentialMaterial != pack.materials.end())
				material(potentialMaterial->second);

			const auto & potentialTexture = pack.textures.find(id);
			if (potentialTexture != pack.textures.end())
				texture(potentialTexture->second);
		}
	}
}
