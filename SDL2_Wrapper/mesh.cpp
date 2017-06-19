#include <limits>
#include <array>
#include "mesh.h"

namespace Assets
{
	const int
	Mesh::newMaterial(const glm::vec4 & d, const glm::vec4 & e,
			const glm::vec4 & s, const glm::vec4 & a)
	{
		const Material newmat = { d,e,s,a };
		mMaterials.push_back(newmat);
		return mMaterials.size()-1;
	}

	const int
	Mesh::newVertex(const glm::vec3 & pos,
		const glm::vec3 & norm, const glm::vec2 & uv)
	{
		//this could potentially be a lot of compute time...
		//so don't be quite so thorough...
		const VertexAttribute newva{ pos, norm, uv};

		//for (auto vertex : mVertexData)
		//{
		//	if (vCompare(vertex.position, newva.position)) return cId;
		//	cId++;
		//}
		mVertexData.push_back(newva);
		return mVertexData.size()-1;
	}


	/*ensures correct winding automatically.*/
	const bool
	Mesh::newFace(const int p1, const int p2, const int p3)
	{
		mIndicies.push_back(p1);
		mIndicies.push_back(p2);
		mIndicies.push_back(p3);
		return true;
	}

	//int cId = 0;
	
}