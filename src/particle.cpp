#include "particle.h"


Particle::Particle(glm::vec3* _position, glm::vec3* _normal, float* _mass, float* _gravityAcce, float* _groundPos)
{
	position = _position;
	normal = _normal;
	isFixed = false;

	mass = _mass;
	gravityAcce = _gravityAcce;
	groundPos = _groundPos;

	velocity = glm::vec3(0.0f);
	force = glm::vec3(0.0f);
	prevForce = glm::vec3(0.0f);
}

void Particle::ApplyForce(glm::vec3 f)
{
	force += f;
}

void Particle::ApplyGravity()
{
	force += glm::vec3(0.0f, -(*mass) * (*gravityAcce), 0.0f);
}

void Particle::Integrate(float deltaTime)
{
	if (isFixed) return;

	ApplyGravity();

	// a
	glm::vec3 a = force / (*mass);

	if (a.length() > EPSILON)
	{
		velocity += a * deltaTime;
		if (velocity.length() < EPSILON) velocity = glm::vec3(0.0f);
		*position += velocity * deltaTime;
		prevForce = force;
	}
	GroundCollision();

	
	force = glm::vec3(0.0f);
}

void Particle::ResetForce()
{
	force = glm::vec3(0.0f);
}

glm::vec3 Particle::GetVelocity()
{
	return velocity;
}

glm::vec3 Particle::GetPosition()
{
	return *position;
}


void Particle::ResetNormal()
{
	*normal = glm::vec3(0.0f);
}

void Particle::AddNormal(glm::vec3 n)
{
	*normal += n;
}

void Particle::GroundCollision()
{
	if (position->y < (*groundPos) + EPSILON)
	{
		position->y = (*groundPos) + EPSILON;
		velocity.y = 0.0f;
	}
}