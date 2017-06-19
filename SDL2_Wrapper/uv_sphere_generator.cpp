#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "uv_sphere_generator.h"

namespace Utilities
{
	Sphere::Sphere(const float radius, const int resolution)
		:
		mRad(radius),
		mRes(resolution)
	{
		generate();
	}

	glm::vec2
	Sphere::niaveUVCoordinate(const glm::vec3 & N) const
	{
		const float tu = static_cast<float>(asin(N.x) / M_PI + 0.5f);
		const float tv = static_cast<float>(asin(N.y) / M_PI + 0.5f);
		return glm::vec2(tu, tv);
	}

	Sphere::element
	Sphere::makeVertex(const glm::vec3 & vandn, const glm::vec2 & uv) const
	{
		return Sphere::element{ vandn, vandn, uv };
	}

	void
	Sphere::makeTriangle(const element & v1,
			const element & v2, const element & v3)
	{
		mVerts.push_back(v1);
		mVerts.push_back(v2);
		mVerts.push_back(v3);
	}

	void
	Sphere::generate()
	{
		const int stacks = mRes;
		const int slices = mRes;

		// stacks are ELEVATION so they count theta
		for (int t = 0; t < stacks; t++)
		{
			const float theta1 = (static_cast<float>(t) / stacks) *
				static_cast<float>(M_PI);
			const float theta2 = (static_cast<float>(t + 1) / stacks)*
				static_cast<float>(M_PI);

			// slices are ORANGE SLICES so the count azimuth
			for (int p = 0; p < slices; p++)
			{
				// azimuth goes around 0 .. 2*PI
				const float phi1 = (static_cast<float>(p) / slices) 
					* 2.0f * static_cast<float>(M_PI);
				const float phi2 = (static_cast<float>(p + 1) / slices) 
					* 2.0f * static_cast<float>(M_PI);

				//phi2   phi1
				// |      |
				// 2------1 -- theta1
				// |\ _   |
				// |    \ |
				// 3------4 -- theta2
				//

				//there are both positional and normal vectors...
				//will use
				const glm::vec3 v1 = glm::vec3(
					/*x*/mRad * sin(theta1) * cos(phi1),
					/*y*/mRad * sin(theta1) * sin(phi1),
					/*z*/mRad * cos(theta1));

				const glm::vec3 v2 = glm::vec3(
					/*x*/mRad * sin(theta1) * cos(phi2),
					/*y*/mRad * sin(theta1) * sin(phi2),
					/*z*/mRad * cos(theta1));

				const glm::vec3 v3 = glm::vec3(
					/*x*/mRad * sin(theta2) * cos(phi2),
					/*y*/mRad * sin(theta2) * sin(phi2),
					/*z*/mRad * cos(theta2));

				const glm::vec3 v4 = glm::vec3(
					/*x*/mRad * sin(theta2) * cos(phi1),
					/*y*/mRad * sin(theta2) * sin(phi1),
					/*z*/mRad * cos(theta2));

				// facing in
				if (t == 0) // top cap
				{
					//t1p1,  t2p1, t2p2,
					makeTriangle(
						makeVertex(v4, niaveUVCoordinate(v1)),
						makeVertex(v3, niaveUVCoordinate(v4)),
						makeVertex(v1, niaveUVCoordinate(v3)));
				}
				else if (t + 1 == stacks) //end cap
				{
					//t2p2, t1p2, t1p1
					makeTriangle(
						makeVertex(v2, niaveUVCoordinate(v3)),
						makeVertex(v1, niaveUVCoordinate(v2)),
						makeVertex(v3, niaveUVCoordinate(v1)));
				}
				else
				{
					//eh, this shouldn't work with my understanding of this
					//algorith, but it seems to.. So..
					// body, facing IN:
					makeTriangle(
						makeVertex(v1, niaveUVCoordinate(v1)),
						makeVertex(v4, niaveUVCoordinate(v4)),
						makeVertex(v2, niaveUVCoordinate(v3)));
					makeTriangle(
						makeVertex(v4, niaveUVCoordinate(v2)),
						makeVertex(v3, niaveUVCoordinate(v1)),
						makeVertex(v2, niaveUVCoordinate(v4)));
				}
			}
		}
	}
}
