#pragma once
#include <vector>
#include <map>
#include <glm\glm.hpp>
#include "OwnershipEnum.h"

namespace Physics
{
	class Sphere
	{
	private:
		long mGUID;
		glm::vec3 mFrameForce;

		glm::vec3 mPosition;
		glm::vec3 mVelocity;
		glm::vec3 mOrientation;
		glm::vec3 mAngularVelocity;
		//sim step
		glm::vec3 mNewPosition;
		glm::vec3 mNewVelocity;
		glm::vec3 mNewOrientation;
		glm::vec3 mNewAngularVelocity;
		float mMass;
		float mRadius;
		OwnershipBit mOwn;
	public:
		Sphere(const long guid,
			const OwnershipBit ob,
			const glm::vec3 & pos,
			const glm::vec3 & velocity,
			const glm::vec3 & orientation,
			const glm::vec3 angularVelocity,
			const float mass,
			const float radius);
		~Sphere() = default;
		void update();

		/*------------------------------------------------------------------------*/
		/*---------Just Inlined Accessors and Mutators Beyond this point----------*/
		/*------------------------------------------------------------------------*/

		inline const void
		force(const glm::vec3 & v3)
		{
			mFrameForce = v3;
		}

		inline const void
		forcePlus(const glm::vec3 & v3)
		{
			mFrameForce += v3;
		}

		inline const void
		forceReset(const glm::vec3 & v3)
		{
			mFrameForce = glm::vec3(0, 0, 0);
		}

		//getters
		inline const void
		guid(long guid)
		{
			mGUID = guid;
		}

		inline const void
		mass(float m)
		{
			mMass = m;
		}

		inline const void
		radius(float m)
		{
			mRadius = m;
		}

		inline const void
		own(OwnershipBit own)
		{
			mOwn = own;
			std::vector<int> vint;
			vint.reserve(10);
			vint.capacity();
		}

		inline const glm::vec3 &
		position()
		{
			return mPosition;
		}

		inline const glm::vec3 &
		velocity()
		{
			return mVelocity;
		}

		inline const glm::vec3 &
		orientation()
		{
			return mOrientation;
		}

		inline const glm::vec3 &
		angularVelocity()
		{
			return mAngularVelocity;
		}

		//setters//
		inline const void
		position(const glm::vec3 & pos)
		{
			mPosition = pos;
		}

		inline const void
		velocity(const glm::vec3 & vel)
		{
			mVelocity = vel;
		}

		inline const void
		orientation(const glm::vec3 & ori)
		{
			mOrientation = ori;
		}

		inline const void
		angularVelocity(const glm::vec3 & vel)
		{
			mAngularVelocity = vel;
		}

		inline const void
		newPosition(const glm::vec3 & pos)
		{
			mNewPosition = pos;
		}

		inline const void
		newVelocity(const glm::vec3 & vel)
		{
			mNewVelocity = vel;
		}

		inline const void
		newOrientation(const glm::vec3 & ori)
		{
			mNewOrientation = ori;
		}

		inline const void
		newAngularVelocity(const glm::vec3 & vel)
		{
			mNewAngularVelocity = vel;
		}

		inline float
		mass()
		{
			return mMass;
		}

		inline float
		radius()
		{
			return mRadius;
		}

		inline OwnershipBit
		own()
		{
			return mOwn;
		}

		inline const long
		guid()
		{
			return mGUID;
		}

		inline const glm::vec3
		newPosition()
		{
			return mNewPosition;
		}

		inline const glm::vec3
		newAngularVelocity()
		{
			return mNewAngularVelocity;
		}


		inline const glm::vec3
		newVelocity()
		{
			return mNewVelocity;
		}

		inline const glm::vec3
		newOrientation()
		{
			return mNewOrientation;
		}

		inline const glm::vec3
		force()
		{
			return mFrameForce;
		}
	};
}