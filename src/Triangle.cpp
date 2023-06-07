#include "Triangle.h"

Triangle::Triangle(Particle* _P1, Particle* _P2, Particle* _P3, float* _FluidDensity, float* _C, glm::vec3* _WindVelocity)
{
	P1 = _P1;
	P2 = _P2;
	P3 = _P3;


	FluidDensity = _FluidDensity;
	C_d = _C;
	WindVelocity = _WindVelocity;
}


void Triangle::ComputeAerodynamicForce()
{
	// v_surface
	glm::vec3 SurfaceVelocity = (
		P1->GetVelocity() 
		+ P2->GetVelocity() 
		+ P2->GetVelocity()) / 3.0f;

	// v
	glm::vec3 v_dir = SurfaceVelocity - *WindVelocity;
	// |v|
	float v_scale = glm::length(v_dir);
	if (v_scale < EPSILON) return;

	v_dir /= v_scale;

	glm::vec3 n = glm::cross(
		P2->GetPosition() - P1->GetPosition(), 
		P3->GetPosition() - P1->GetPosition());
	float area = glm::length(n);
	n /= area;
	area /= 2;

	float crossArea = area * glm::dot(v_dir, n);

	// aerodynamic force
	glm::vec3 f = -0.5f * (*FluidDensity) * v_scale * v_scale * (*C_d) * crossArea * n;

	f /= 3.0f;
	P1->ApplyForce(f);
	P2->ApplyForce(f);
	P3->ApplyForce(f);

}

void Triangle::ComputeNormal()
{
	glm::vec3 n = glm::cross(
		P2->GetPosition() - P1->GetPosition(),
		P3->GetPosition() - P1->GetPosition());

	if (n.length() < EPSILON) return;

	n = glm::normalize(n);

	P1->AddNormal(n);
	P2->AddNormal(n);
	P3->AddNormal(n);
}