#pragma once
#include <glm\glm.hpp>

glm::vec3 
rk_eval(const glm::vec3 & initial, float dt, const glm::vec3 & derived)
{
	glm::vec3 dx = initial + (derived * (dt));
	return dx;
}

glm::vec3 
rk4(const glm::vec3 & initial, float dt, const glm::vec3 & evaluateAgainst)
{
	glm::vec3  a, b, c, d;

	a = initial;;
	b = rk_eval(initial,
		dt / 2.0,
		(a / 2.0f));
	c = rk_eval(initial,
		dt / 2.0,
		(b / 2.0f));
	d = rk_eval(initial,
		dt,
		c);

	glm::vec3 dxdt = ((a + (2.0f*(b + c)) + d)) / 6.0f;

	return rk_eval(evaluateAgainst, dt, dxdt);
}