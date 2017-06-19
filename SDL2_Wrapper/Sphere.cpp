#include "Sphere.h"

namespace Physics
{
	Sphere::Sphere(const long guid,
		const OwnershipBit ob,
		const glm::vec3 & pos,
		const glm::vec3 & velocity,
		const glm::vec3 & orientation,
		const glm::vec3 angularVelocity,
		const float mass,
		const float radius)
		:
		mOwn(ob),
		mPosition(pos),
		mVelocity(velocity),
		mOrientation(orientation),
		mAngularVelocity(angularVelocity),
		mMass(mass),
		mRadius(radius)
	{}

	void
	Sphere::update()
	{
		mPosition = mNewPosition;
		mVelocity = mNewVelocity;
		mOrientation = mNewOrientation;
		mAngularVelocity = mNewAngularVelocity;
	}
}
