#include "atom.h"

Vector3 Atom::at()
{
	return coordinates_;
}

Vector3 Atom::velocity()
{
	return velocity_;
}

Vector3 Atom::totalForce()
{
	return force_;
}

void Atom::setPosition(const Vector3& coordinates)
{
	coordinates_ = coordinates;
}

void Atom::setVelocity(const Vector3& velocity)
{
	velocity_ = velocity;
}

void Atom::setForce(const Vector3& force)
{
	force_ = force;
}
