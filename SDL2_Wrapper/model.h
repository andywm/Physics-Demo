#pragma once

#include "tiny_obj_loader.h"
#include <memory>
#include "mesh.h"

namespace Assets
{
	class Model
	{
	private:
		tinyobj::attrib_t mAttrib;
		std::vector<tinyobj::shape_t> mShapes;
		std::vector<tinyobj::material_t> mMaterials;
		std::shared_ptr<Mesh> mMesh;
		void loadIntoMesh();
	public:
		explicit Model(const std::string & file);

		inline const std::shared_ptr<Mesh>
		getMesh() const
		{
			return mMesh;
		}
	};
}
