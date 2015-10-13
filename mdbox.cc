#include "mdbox.h"
#include "lattice.h"
#include "simulation.h"
#include "random.h"
#include "atom.h"
#include "material.h"
#include "physicalConstants.h"

#include <iostream>
#include <math.h>

void MDBox::DEBUG_PRINT()
{
	// Testing purpose // just adding text
	for (int i = 0; i < 1; i++)
	{
		std::cout << i << " - atom:\n\t at " << atoms[i]->at() << "\n\t v " << atoms[i]->velocity() << std::endl;
	}
}

void MDBox::DEBUG_VERLET_LIST()
{
	int index{ 0 };
	for (auto& interactionList : verletList)
	{
		std::cout << "atom " << index++ << " interacts with " << interactionList.size() << " atoms" << std::endl;
	}

	std::cout << "atom 0: " << atoms[0]->at() << std::endl;
	std::cout << "interacts with: " << std::endl;
	index = 0;
	for (auto& atomTranslationPair : verletList[0])
	{
		std::cout << "\t" << index++ << ": " << atomTranslationPair.first->at() + atomTranslationPair.second << std::endl;
	}
}

MDBox::MDBox(Simulation& sim) : simulation{sim}
{
	// We call init functions here using the simulation as an argument to better separate the 
	// implementation details from the parameters (it is not necessarily good to have a Simulation&
	// as access to all parameters, and might be changed in the future), as well as making each
	// more independent.
	dimensions = simulation.dimensions;
	createInitialAtoms(*simulation.lattice);
	setInitialVelocities(simulation.initialTemperature);
	updateVerletList();
	//DEBUG_PRINT_ATOMS(atoms);
	//updatePositions();
	//updateVelocities();
}

void MDBox::updateVerletList()
{
	verletList.clear();

	for (int i = 0; i < atoms.size(); ++i)
	{
		Atom* atom = atoms[i];
		std::vector<std::pair<Atom*, Vector3>> interactionList;

		for (int j = i + 1; j < atoms.size(); ++j)
		{
			Atom* nextAtom = atoms[j];
			double xTranslation = nextAtom->at().x > atom->at().x ? -1 : 1;
			double yTranslation = nextAtom->at().y > atom->at().y ? -1 : 1;
			double zTranslation = nextAtom->at().z > atom->at().z ? -1 : 1;

			Vector3 translationArr[] =
			{
				Vector3{0, 0, 0},
				Vector3{xTranslation*dimensions.x, 0, 0},
				Vector3{0, xTranslation*dimensions.y, 0},
				Vector3{0, 0, zTranslation*dimensions.z},
				Vector3{xTranslation*dimensions.x, yTranslation*dimensions.y, 0},
				Vector3{xTranslation*dimensions.x, 0, zTranslation*dimensions.z},
				Vector3{0, yTranslation*dimensions.y, zTranslation*dimensions.z},
				Vector3{xTranslation*dimensions.x, yTranslation*dimensions.y, zTranslation*dimensions.z},
			};

			for (int k = 0; k < 8; ++k)
			{
				Vector3 translation = translationArr[k];
				Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
				double dotprod = vectorBetween*vectorBetween;
				double distance = sqrt(dotprod);
				if (distance < simulation.cutoffDistance)
				{
					interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					break; // no need to translate any other as we found one interaction
				}
			}
		}
		verletList.push_back(interactionList);
	}
}

void MDBox::createInitialAtoms(const Lattice& lattice)
{
	for (int iii = 0; iii < dimensions.x; ++iii)
	{
		for (int jjj = 0; jjj < dimensions.y; ++jjj)
		{
			for (int kkk = 0; kkk < dimensions.z; ++kkk)
			{
				for (auto& position : lattice.atomPositions)
				{
					Atom* atom = new Atom();
					atom->setPosition(lattice.latticeConstant*position + lattice.latticeConstant*Vector3{ (double)iii, (double)jjj, (double)kkk });
					//atom->setForce(Vector3{ 1, 1, 1 });
					atoms.push_back(atom);
				}
			}
		}
	}

}

void MDBox::setInitialVelocities(double temperature)
{
	Vector3 sumVelocity = { 0.0, 0.0, 0.0 };
	double sumVelocity2 = 0;
	int nbrAtoms = atoms.size();

	for (auto& atom : atoms)
	{
		atom->setVelocity(Vector3{ Random::next(), Random::next(), Random::next() });
		Vector3 velocity = atom->velocity();
		sumVelocity = sumVelocity + velocity;
		sumVelocity2 = sumVelocity2 + velocity*velocity;
	}

	sumVelocity = sumVelocity / (double)nbrAtoms; 	//center of mass velocity
	sumVelocity2 = sumVelocity2 / (double)nbrAtoms;	//mean squared velocity
	double scaleFactor = 1.0;
	scaleFactor = sqrt(3.0*(temperature / sumVelocity2)*(PHConstants::boltzmann / simulation.material->mass));
	std::cout << "center of mass velocity = " << sumVelocity << std::endl;
	std::cout << "mean squared velocity = " << sumVelocity2 << std::endl;
	std::cout << "temperature = " << temperature << std::endl;

	for (auto& atom : atoms)
	{
		Vector3 velocity = atom->velocity();
		velocity = (velocity - sumVelocity)*scaleFactor;
		atom->setVelocity(velocity);
	}
}

void MDBox::updateForces(const Material& material)
{
}

void MDBox::updatePositions()
{
	for (auto& atom : atoms)
	{
		Vector3 oldPosition = atom->at();
		Vector3 oldVelocity = atom->velocity();
		Vector3 oldForce = atom->totalForce();
		double  deltatime = simulation.timestepLength;
		double mass = simulation.material->mass;
		Vector3 newPosition = oldPosition + oldVelocity * deltatime + (oldForce / mass)*(deltatime / 2)*deltatime;
		atom->setPosition(newPosition);
		atom->setForcePreviousTimestep(oldForce);
	}
}

void MDBox::updateVelocities()
{
	Vector3 sumVelocity = { 0.0, 0.0, 0.0 };
	double sumVelocity2 = 0.0;
	for (auto& atom : atoms)
	{
		Vector3 oldVelocity = atom->velocity();
		Vector3 newForce = atom->totalForce();
		Vector3 oldForce = atom->forcePreviousTimestep();
		double mass = simulation.material->mass;
		double  deltatime = simulation.timestepLength;
		Vector3 newVelocity = oldVelocity + (deltatime / 2)*(oldForce + newForce) / mass;
		atom->setVelocity(newVelocity);
		sumVelocity = sumVelocity + newVelocity;
		sumVelocity2 = sumVelocity2 + newVelocity*newVelocity;
	}
}

MDBox::~MDBox()
{
	for (auto& atom : atoms)
	{
		delete atom;
	}
}