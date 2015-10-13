#include "potential.h"

#include "vector.h"
#include "atom.h"

Vector3 LJPotential::interaction(const Atom&, const Atom&)
{
	// Does nothing atm
	return Vector3::ONES();
}