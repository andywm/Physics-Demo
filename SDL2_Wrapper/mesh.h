#pragma once
#include <glm\glm.hpp>
#include <vector>
#include <memory>

namespace Assets
{
	class Mesh
	{
	private:
		struct VertexAttribute
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texcoords;
			//int materialID;
		};
		struct Material
		{
			glm::vec4 diffuse;
			glm::vec4 emissive;
			glm::vec4 specular;
			glm::vec4 ambient;
		};
		std::vector<VertexAttribute> mVertexData;
		std::vector<Material> mMaterials;
		std::vector<int> mIndicies;
	public:

		Mesh() = default;
		const int newVertex(const glm::vec3 & pos,
			const glm::vec3 & norm, const glm::vec2 & uv);
		const int newMaterial(const glm::vec4 & d, const glm::vec4 & e, 
			const glm::vec4 & s, const glm::vec4 & a);
		const bool newFace(const int p1, const int p2, const int p3);
		~Mesh() = default;

		//void setFaces(std::vector<int> & faces);
		//void manualSetVertexSize(int set);
		//void manualSetIndexSize(int set);

		//enum class Winding {ACW, CW} winding;
		void
		Mesh::setFaces(const std::vector<int> & faces)
		{
			mIndicies = faces;
		}

		//indicies
		inline const float *
		Mesh::vertices() const
		{
			return (float*)&mVertexData[0];
		}
		inline const int *
		Mesh::indicies() const
		{
			return &mIndicies[0];
		}

		inline const int
		Mesh::verticesCount() const
		{
			return static_cast<int>(mVertexData.size());
		}
		inline const int
		Mesh::indiciesCount() const
		{
			return static_cast<int>(mIndicies.size());
		}
		inline void
		Mesh::manualSetVertexSize(const int set)
		{
			mVertexData.reserve(set);
		}
		inline void
		Mesh::manualSetIndexSize(const int set)
		{
			mIndicies.reserve(set);
		}
		static inline const int
		Mesh::attributeByteSize() 
		{
			return sizeof(VertexAttribute);
		}
		static inline const int
		Mesh::indexByteSize() 
		{
			return sizeof(int);
		}
	};
}