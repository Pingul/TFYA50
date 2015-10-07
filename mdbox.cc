#include "mdbox.h"
#include "lattice.h"
#include "vector.h"
#include "simulation.h"
#include "random.h"
#include "atom.h"
#include "material.h"

#include <iostream>

MDBox::MDBox(Simulation& sim) : simulation{sim}
{
	createInitialAtoms(*simulation.lattice);
	TEST_setInitialVelocities(simulation.initialTemperature);
	for (auto& atom : atoms)
	{
		std::cout << "atom:\n\t at " << atom->at() << "\n\t v " << atom->velocity() << std::endl;
	}
	updatePositions();
	updateVelocities();

	// Testing purpose // just adding text
	for (auto& atom : atoms)
	{
		std::cout << "atom:\n\t at " << atom->at() << "\n\t v " << atom->velocity() << std::endl; 
	}	
}

void MDBox::createInitialAtoms(const Lattice& lattice)
{
	for (int iii = 0; iii < simulation.dimensions.x; ++iii)
	{
		for (int jjj = 0; jjj < simulation.dimensions.y; ++jjj)
		{
			for (int kkk = 0; kkk < simulation.dimensions.z; ++kkk)
			{
				for (auto& position : lattice.atomPositions)
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
	for (auto& atom : atoms)
	{
		Vector3 oldposition = atom->at();
		Vector3 oldvelocity = atom->velocity();
		Vector3 oldforce = atom->totalForce();
		double  deltatime = simulation.timestepLength;
		double mass = simulation.material->mass;
		Vector3 newposition = oldposition + oldvelocity * deltatime + (oldforce/mass)*(deltatime/2)*deltatime;
		atom->setPosition(newposition);
		atom->setForcePreviousTimestep(oldforce);
	}
}

void MDBox::updateVelocities()
{
	Vector3 sumvelocity = { 0.0, 0.0, 0.0 };
	double sumvelocity2 = 0.0;
	for (auto& atom : atoms)
	{
		Vector3 oldvelocity = atom->velocity();
		Vector3 newforce = atom->totalForce();
		Vector3 oldforce = atom->forcePreviousTimestep();
		double mass = simulation.material->mass;
		double  deltatime = simulation.timestepLength;
		Vector3 newvelocity = oldvelocity + (deltatime/2)*(oldforce + newforce)/mass;
		atom->setVelocity(newvelocity);
		sumvelocity = sumvelocity + newvelocity;
		sumvelocity2 = sumvelocity2 + newvelocity*newvelocity;
	}
}