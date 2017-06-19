#pragma once

#include <random>
namespace Utilities
{
	inline float
	random(const float begin, const float end)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		const std::uniform_real_distribution<double> dist(begin, end);
		return static_cast<float>(dist(mt));
	}
}