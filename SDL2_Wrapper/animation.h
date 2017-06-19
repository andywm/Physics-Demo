#pragma once
#include <cmath>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
namespace Utilities
{
	class Animation
	{
	public:
		enum class FN { SIN, COS, LINEAR, NONE };
		struct AxConfig
		{
			FN mode;
			float maxDeflection;
			float arcPerSecond;
			bool invertDirection;
		};
		
		explicit Animation(const AxConfig & xconf, const AxConfig & yconf, 
			const AxConfig & zconf);
		explicit Animation(const Animation &) = default;
		Animation& operator=(const Animation&) = delete;
		~Animation() = default;
		const glm::mat4 frame(const double dTime); 
	private:
		struct RotationalAxis
		{
			float angle;
			float maxDeflecton;
			float arcPerSecond;
			const float(*operation)  (const double, RotationalAxis & lRot);
			void       (*step_angle)(const double, RotationalAxis & lRot);
		} mX, mY, mZ;

		const glm::vec3 cmXAxis, cmYAxis, cmZAxis;
		//static const float oper_none   (const double,   RotationalAxis & lRot);
		//static const float oper_linear (const double t, RotationalAxis & lRot);
		//static const float oper_cosine (const double t, RotationalAxis & lRot);
		//static const float oper_sine   (const double t, RotationalAxis & lRot);
		inline static const float
		oper_none(const double, RotationalAxis & lRot)
		{
			return 0.0f;
		}

		inline static const float
		oper_linear(const double t, RotationalAxis & lRot)
		{
			lRot.step_angle(t, lRot);
			return lRot.angle;
		}

		inline static const float
		oper_cosine(const double t, RotationalAxis & lRot)
		{
			lRot.step_angle(t, lRot);
			return cos(lRot.angle)*lRot.maxDeflecton;
		}

		inline static const float
		oper_sine(const double t, RotationalAxis & lRot)
		{
			lRot.step_angle(t, lRot);
			return sin(lRot.angle)*lRot.maxDeflecton;
		}


		static void negativeProgression(const double t, RotationalAxis & lRot);
		static void positiveProgression(const double t, RotationalAxis & lRot);
	};
}