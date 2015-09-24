#include "mdbox.h"
#include "lattice.h"


MDBox::MDBox(const Vector3& dimensions, const Lattice& lattice)
{
	createInitialAtoms(lattice);
}

void MDBox::createInitialAtoms(const Lattice& lattice)
{
	// DAVIDE
}

MDBox::~MDBox()
{
	for (auto& atom : atoms)
	{
		delete atom;
	}
}