#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

namespace Utilities
{
	class Sphere
	{
	public:
		struct element
		{
			glm::vec3 vertex;
			glm::vec3 normal;
			glm::vec2 uv;
		};

	private:
		float mRad;
		int mRes;
		void generate();
		glm::vec2 niaveUVCoordinate(const glm::vec3 & N) const;
		void makeTriangle(const element & v1, 
			const element & v2, const element & v3);
		element makeVertex(const glm::vec3 & vandn, const glm::vec2 & uv) const;
		std::vector<element> mVerts;

	public:
		Sphere(const float radius, const int resolution);
		inline const std::vector<Sphere::element> &
		getData() const
		{
			return mVerts;
		}
	};
}

