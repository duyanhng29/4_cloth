#pragma once

#include "particle.h"

class Triangle
{
private:
	Particle* P1, * P2, * P3;

	const float* FluidDensity;
	const float* C_d;
	const glm::vec3* WindVelocity;

public:
	Triangle(Particle* _P1, Particle* _P2, Particle* _P3, float* _FluidDensity, float* _C, glm::vec3* _WindVelocity);

	void ComputeAerodynamicForce();
	void ComputeNormal();

};