#include "atom.h"

Vector3 Atom::at() const
{
	return coordinates_;
}

Vector3 Atom::velocity() const
{
	return velocity_;
}

Vector3 Atom::totalForce() const
{
	return force_;
}

Vector3 Atom::forcePreviousTimestep() const
{
	return forceprevioustimestep_;
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

void Atom::setForcePreviousTimestep(const Vector3& forceprevioustimestep)
{
	forceprevioustimestep_ = forceprevioustimestep;
}
