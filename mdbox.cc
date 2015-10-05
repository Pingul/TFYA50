#include "mdbox.h"
#include "lattice.h"
#include "vector.h"
#include "simulation.h"
#include "random.h"

#include <iostream>

MDBox::MDBox(Simulation& sim) : simulation{sim}
{
	createInitialAtoms(*simulation.lattice);
	TEST_setInitialVelocities(simulation.initialTemperature);

	// Testing purpose // just adding text
	for (auto& atom : atoms)
	{
		std::cout << "atom:\n\t at " << atom->at() << "\n\t v " << atom->velocity() << std::endl; 
	}	
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

void MDBox::TEST_createInitialAtoms(const Lattice& lattice)
{
	for (int i = 0; i < 10; ++i)
	{
		Atom* atom = new Atom();
		atom->setPosition(Vector3::ONES()/(i + 1));
		atoms.push_back(atom);
	}
}

void MDBox::TEST_setInitialVelocities(double temperature)
{
	for (auto& atom : atoms)
	{
		atom->setVelocity(Vector3{Random::next(), Random::next(), Random::next()});
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

void MDBox::updatePositions()
{


}