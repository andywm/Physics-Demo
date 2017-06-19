#pragma once
#include <glm/glm.hpp>
#include "Sphere.h"

namespace Physics
{
	struct ManifoldPoint
	{
		enum class Type { CC, CP, CW } type;
		Sphere *contactID1;
		Sphere *contactID2;
		glm::vec3 contactNormal;
		float penetration;
	};

	class ContactManifold
	{
	public:
		ContactManifold();
		~ContactManifold() = default;

		void add(ManifoldPoint & point);
		void clear();
		int size() const;
		ManifoldPoint& point(int index);

	private:
		std::vector<ManifoldPoint> mPoints;
	};
}