#include "mdbox.h"
#include "lattice.h"
#include "vector.h"
#include "simulation.h"
#include "random.h"
#include "atom.h"

#include <iostream>

MDBox::MDBox(Simulation& sim) : simulation{sim}
{
	TEST_createInitialAtoms(*simulation.lattice);
	TEST_setInitialVelocities(simulation.initialTemperature);
	updateVerletList();

	// Testing purpose // just adding text
	for (auto& atom : atoms)
	{
		std::cout << "atom:\n\t at " << atom->at() << "\n\t v " << atom->velocity() << std::endl; 
	}	
}

void MDBox::createInitialAtoms(const Lattice& lattice)
{
	// DAVIDE
	// Atom* atom1 = new Atom();
	// atom1->setPosition(Vector3{1, 2, 3});
	// for (int x = 0; x < dimensions.x; ++x)
	// {
			// for (auto& position : lattice.atomPositons)
			// {
			// 		position == Vector3
			// }
	// }

	// Pseudoalgorithm
	// for all the dimensions:
	// 	for all atoms in a unit cell in the lattice:
	// 		create a new atom
	// 		push is back the container 'atoms'
	// atoms.push_back(atom1));
}

void MDBox::updateVerletList()
{
	verletList.clear();

	for (int i = 0; i < atoms.size(); ++i)
	{
		Atom* atom = atoms[i];
		std::vector<Atom*> interactionList;

		for (int j = i; j < atoms.size(); ++j)
		{
			Atom* nextAtom = atoms[j];
			Vector3 vectorBetween = atom->at() - nextAtom->at();
			double dotprod = vectorBetween*vectorBetween;

			if (close enough)
				interactionList.push_back(nextAtom);
		}
		verletList.push_back(interactionList);

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