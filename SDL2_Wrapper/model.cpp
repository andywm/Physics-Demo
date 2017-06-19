#include <tiny_obj_loader.h>
#include <memory>
#include <map>
#include "model.h"
#include "mesh.h"

namespace Assets
{
	namespace
	{
		//quick and dirty hash function
		//will not repeat below a == 69623
		//clashes statistically unlikely above 69623, but possible.
		inline int
		hash(const int a, const int b, const int c)
		{
			const int P1 = 69623; //a prime number...
			const int P2 = 72229; //another prime number.
			return (a*P1 + b)*P2 + c;
		}
	}

	//only takes the first shape from a model file.
	//materials don't work. No idea why, don't really care.
	Model::Model(const std::string & file)
	{
		std::string error;
		if (tinyobj::LoadObj(&mAttrib, &mShapes,
			&mMaterials, &error, file.c_str()))
		{
			mMesh = std::shared_ptr<Mesh>(new Mesh());
			loadIntoMesh();
		}
		//load material here too as tinyobj loader's material loader seems to be
		//broken...


	}

	//compresses all models into one.
	//note: it would be very easy to repurpose this to load .objs into seperate
	//models, and maybe that would be easier - but its probably not worth
	//redesigning how the asset management engine works at this point...
	void
	Model::loadIntoMesh()
	{
		std::map<int, int> uniqIndex; //hash : real index
		std::vector<int> faceIndices;

		size_t index_offset = 0;
		unsigned int maxVert=0, maxInd=0;
		for (size_t shape = 0; shape < mShapes.size(); shape++)
		{
			maxVert += mShapes[shape].mesh.num_face_vertices.size() * 3;
			maxInd += mShapes[shape].mesh.indices.size();
		}
		mMesh->manualSetVertexSize(maxVert);
		mMesh->manualSetIndexSize(maxInd);
		
		for (size_t cs = 0; cs < mShapes.size(); cs++)
		{
			for (size_t f = 0; f < mShapes[cs].mesh.num_face_vertices.size(); f++)
			{
				const int fv = mShapes[cs].mesh.num_face_vertices[f];
				// Loop over vertices in the face.

				for (int v = 0; v < fv; v++)
				{
					// access to vertex
					const tinyobj::index_t idx =
						mShapes[cs].mesh.indices[index_offset];

					const int h =
						hash(idx.vertex_index, 
							idx.normal_index, 
							idx.texcoord_index);
					const auto it = uniqIndex.find(h);

					//if is a unseen index, make it.
					if (it == uniqIndex.end())
					{
						const glm::vec3 pos = glm::vec3(
							mAttrib.vertices[3 * idx.vertex_index + 0],
							mAttrib.vertices[3 * idx.vertex_index + 1],
							mAttrib.vertices[3 * idx.vertex_index + 2]);
						const glm::vec3 norm = glm::vec3(
							mAttrib.normals[3 * idx.normal_index + 0],
							mAttrib.normals[3 * idx.normal_index + 1],
							mAttrib.normals[3 * idx.normal_index + 2]);
						glm::vec2 texc;
						if (idx.texcoord_index != -1)
						{
							texc = glm::vec2(
								mAttrib.texcoords[2*idx.texcoord_index + 0],
								mAttrib.texcoords[2*idx.texcoord_index + 1]);
						}
						//make entry
						const int current = mMesh->newVertex(pos, norm, texc);
						uniqIndex.insert(std::make_pair(h, current));
						faceIndices.push_back(current);
					}
					//otherwise, just encode the real index from the hash.
					else
					{
						//vCache.push_back(it->second);
						faceIndices.push_back(it->second);
					}
					index_offset++;
				}
			}
			//different shapes shouldn't share these.
			uniqIndex.clear();
			index_offset = 0;
		}
		mMesh->setFaces(faceIndices);
	}

	
}