#include "mdbox.h"
#include "lattice.h"
#include "vector.h"
#include "simulation.h"

#include <iostream>

MDBox::MDBox(Simulation& sim) : simulation{sim}
{
	TEST_createInitialAtoms(*simulation.lattice);
	for (auto& atom : atoms)
	{
		std::cout << "atom at " << atom->at() << std::endl; 
	}	
}

void MDBox::createInitialAtoms(const Lattice& lattice)
{
	// DAVIDE
}

void MDBox::TEST_createInitialAtoms(const Lattice& lattice)
{
	for (int i = 0; i < 10; ++i)
	{
		Atom* atom = new Atom();
		atom->setPosition(Vector3::ONES()/(i + 1));
		atoms.push_back(atom);
	}
}

void MDBox::setInitialAtomOffsets()
{

}

void MDBox::setInitialVelocities(double temperature)
{

}


MDBox::~MDBox()
{
	for (auto& atom : atoms)
	{
		delete atom;
	}
}