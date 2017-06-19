#pragma once
#include <cmath>
#include <array>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include "animation.h"
namespace Utilities
{
	Animation::Animation(const AxConfig & xconf, const AxConfig & yconf,
		const AxConfig & zconf)
		:
		cmXAxis(1, 0, 0), cmYAxis(0, 0, 1), cmZAxis(0, 1, 0),
		mX{ 0, xconf.maxDeflection, xconf.arcPerSecond, nullptr, nullptr },
		mY{ 0, yconf.maxDeflection, yconf.arcPerSecond, nullptr, nullptr },
		mZ{ 0, zconf.maxDeflection, zconf.arcPerSecond, nullptr, nullptr }

	{
		const std::array<const AxConfig *, 3> configs = 
		{ &xconf, &yconf, &zconf };
		std::array<RotationalAxis *, 3> axes = { &mX, &mY, &mZ };
		for (int i = 0; i < 3; i++)
		{
			const AxConfig * const conf = configs[i];
			RotationalAxis * const axis = axes[i];

			if (conf->invertDirection)
				axis->step_angle = &negativeProgression;
			else
				axis->step_angle = &positiveProgression;

			switch (conf->mode)
			{
			case FN::COS:
				axis->operation = &oper_cosine;
				break;
			case FN::SIN:
				axis->operation = &oper_sine;
				break;
			case FN::LINEAR:
				axis->operation = &oper_linear;
				break;
			default:
				axis->operation = &oper_none;
				break;
			}
		}
	}

	const glm::mat4
	Animation::frame(const double dTime)
	{
		glm::mat4 rot = glm::rotate(mX.operation(dTime, mX), cmXAxis);
		rot *= glm::rotate(mY.operation(dTime, mY), cmYAxis);
		rot *= glm::rotate(mZ.operation(dTime, mZ), cmZAxis);
		
		return rot;
	}

	void
	Animation::negativeProgression(const double t, RotationalAxis & lRot)
	{
		lRot.angle -= (lRot.arcPerSecond*static_cast<float>(t));
	}

	void
	Animation::positiveProgression(const double t, RotationalAxis & lRot)
	{
		lRot.angle += (lRot.arcPerSecond*static_cast<float>(t));
	}
}