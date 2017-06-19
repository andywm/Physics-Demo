#pragma once
#include <string>
#include "so_includes.h"
#include "animation.h"
//#include "scene_graph.h"

namespace Graphics
{
	namespace Object
	{
		class Prop : public Base
		{
		public:
			explicit Prop(const Assets::AssetPack & assets,
				const glm::vec3 & pos, const glm::vec3 & rot,
				const glm::vec3 & scl, const std::string & id);
			~Prop() = default;
		};
	}
}
