#include "springdamper.h"

SpringDamper::SpringDamper(Particle* _P1, Particle* _P2, float* _SpringConst, float* _DampingConst, float* _RestLength)
{
	P1 = _P1;
	P2 = _P2;

	SpringConst = _SpringConst;
	DampingConst = _DampingConst;
	RestLength = _RestLength;

	prevDir = P2->GetPosition() - P1->GetPosition();
}


void SpringDamper::ComputeForce()
{
	glm::vec3 dir = P2->GetPosition() - P1->GetPosition(); // e*
	float currLength = glm::length(dir);  // l
	if (currLength > EPSILON)
	{
		dir = glm::normalize(dir); // e
		prevDir = dir;
	}
	else
	{

		dir = glm::vec3(0.0f);
		currLength = 0.0f;
	}

	glm::vec3 f_spring = -(*SpringConst) * ((*RestLength) - currLength) * dir;
	if (f_spring.length() < EPSILON) f_spring = glm::vec3(0.0f);

	// closing velocity
	float v = glm::dot(
		P1->GetVelocity() - P2->GetVelocity(),
		dir);

	// f
	glm::vec3 f_damper = -(*DampingConst) * v * dir;
	if (f_damper.length() < EPSILON) f_damper = glm::vec3(0.0f);

	
	P1->ApplyForce(f_spring + f_damper);
	P2->ApplyForce(-f_spring - f_damper);
}