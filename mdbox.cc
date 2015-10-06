#include "mdbox.h"
#include "lattice.h"
#include "vector.h"
#include "simulation.h"
#include "random.h"

#include <iostream>

void DEBUG_PRINT_ATOMS(const std::vector<Atom*>& atoms)
{
	// Testing purpose // just adding text
	int index = 0;
	for (auto& atom : atoms)
	{
		std::cout << index++ << " - atom:\n\t at " << atom->at() << "\n\t v " << atom->velocity() << std::endl;
	}
}

MDBox::MDBox(Simulation& sim) : simulation{sim}
{
	// We call init functions here using the simulation as an argument to better separate the 
	// implementation details from the parameters (it is not necessarily good to have a Simulation&
	// as access to all parameters, and might be changed in the future), as well as making each
	// more independent.
	createInitialAtoms(*simulation.lattice);
	setInitialVelocities(simulation.initialTemperature);
	//DEBUG_PRINT_ATOMS(atoms);
}

void MDBox::createInitialAtoms(const Lattice& lattice)
{
	for (auto& position : lattice.atomPositions)
	{
		for (int iii = 0; iii < simulation.dimensions.x; ++iii)
		{
			for (int jjj = 0; jjj < simulation.dimensions.y; ++jjj)
			{
				for (int kkk = 0; kkk < simulation.dimensions.z; ++kkk)
				{
					Atom* atom = new Atom();
					atom->setPosition(lattice.latticeConstant*position + lattice.latticeConstant*Vector3{ static_cast<double>(iii), static_cast<double>(jjj), static_cast<double>(kkk) });
					atoms.push_back(atom);
				}
			}
		}
	}

}

void MDBox::setInitialVelocities(double temperature)
{
	// Not implemented fully
	for (auto& atom : atoms)
	{
		atom->setVelocity(Vector3{ Random::next(), Random::next(), Random::next() });
	}
}

void MDBox::updateVerletList()
{

}

void MDBox::updateForces(const Material& material)
{

}

void MDBox::updatePositions()
{

}

void MDBox::updateVelocities()
{

}


MDBox::~MDBox()
{
	for (auto& atom : atoms)
	{
		delete atom;
	}
}