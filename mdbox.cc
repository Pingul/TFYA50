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
	for (int i = 0; i < 2; i++)
	{
		std::cout << i << " - atom:\n\t at " << atoms[i]->at() << "\n\t v " << atoms[i]->velocity() << "\n\t F " << atoms[i]->totalForce() << std::endl;
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

MDBox::MDBox(const SimulationParams& params) : simulationParams{params}
{
	// We call init functions here using the simulation as an argument to better separate the 
	// implementation details from the parameters (it is not necessarily good to have a Simulation&
	// as access to all parameters, and might be changed in the future), as well as making each
	// more independent.
	dimensions = simulationParams.dimensions;
	createInitialAtoms(*simulationParams.lattice);
	setInitialVelocities(simulationParams.initialTemperature);
	updateVerletList();
	vCutoff = simulationParams.cutoffDistance*1.05; // We increase this a little to take a little too many atoms in the verlet initially
}

bool MDBox::atEdge(const Atom& atom, bool xEdge, bool yEdge, bool zEdge)
{
	return (!xEdge || (atom.at().x < vCutoff || atom.at().x > simulationParams.lattice->latticeConstant * dimensions.x - vCutoff))
		&& (!yEdge || (atom.at().y < vCutoff || atom.at().y > simulationParams.lattice->latticeConstant * dimensions.y - vCutoff))
		&& (!zEdge || (atom.at().z < vCutoff || atom.at().z > simulationParams.lattice->latticeConstant * dimensions.z - vCutoff));
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

			double xTranslation = nextAtom->at().x > atom->at().x ? -simulationParams.lattice->latticeConstant : simulationParams.lattice->latticeConstant;
			double yTranslation = nextAtom->at().y > atom->at().y ? -simulationParams.lattice->latticeConstant : simulationParams.lattice->latticeConstant;
			double zTranslation = nextAtom->at().z > atom->at().z ? -simulationParams.lattice->latticeConstant : simulationParams.lattice->latticeConstant;

			// Translation of corners:
			if (atEdge(*nextAtom, true, true, true))
			{

				for (int k = 0; k < 8; ++k)
				{

					Vector3 translationArr[] =
					{
						Vector3{ 0, 0, 0 },
						Vector3{ xTranslation*dimensions.x, 0, 0 },
						Vector3{ 0, yTranslation*dimensions.y, 0 },
						Vector3{ 0, 0, zTranslation*dimensions.z },
						Vector3{ xTranslation*dimensions.x, yTranslation*dimensions.y, 0 },
						Vector3{ xTranslation*dimensions.x, 0, zTranslation*dimensions.z },
						Vector3{ 0, yTranslation*dimensions.y, zTranslation*dimensions.z },
						Vector3{ xTranslation*dimensions.x, yTranslation*dimensions.y, zTranslation*dimensions.z },
					};

					Vector3 translation = translationArr[k];
					Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
					double dotprod = vectorBetween*vectorBetween;
					double distance = sqrt(dotprod);
					if (distance < vCutoff)
					{
						interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					}
				}
			}
			//for the edges:
			else if (atEdge(*nextAtom, true, true, false))
			{

				for (int k = 0; k < 4; ++k)
				{

					Vector3 translationArr[] =
					{
						Vector3{ 0, 0, 0 },
						Vector3{ xTranslation*dimensions.x, 0, 0 },
						Vector3{ 0, yTranslation*dimensions.y, 0 },
						Vector3{ xTranslation*dimensions.x, yTranslation*dimensions.y, 0 },
					};

					Vector3 translation = translationArr[k];
					Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
					double dotprod = vectorBetween*vectorBetween;
					double distance = sqrt(dotprod);
					if (distance < vCutoff)
					{
						interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					}
				}
			}
			else if (atEdge(*nextAtom, true, false, true))
			{

				for (int k = 0; k < 4; ++k)
				{

					Vector3 translationArr[] =
					{
						Vector3{ 0, 0, 0 },
						Vector3{ xTranslation*dimensions.x, 0, 0 },
						Vector3{ 0, 0, zTranslation*dimensions.z },
						Vector3{ xTranslation*dimensions.x, 0, zTranslation*dimensions.z },
					};

					Vector3 translation = translationArr[k];
					Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
					double dotprod = vectorBetween*vectorBetween;
					double distance = sqrt(dotprod);
					if (distance < vCutoff)
					{
						interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					}
				}
			}
			else if (atEdge(*nextAtom, false, true, true))
			{

				for (int k = 0; k < 4; ++k)
				{

					Vector3 translationArr[] =
					{
						Vector3{ 0, 0, 0 },
						Vector3{ 0, yTranslation*dimensions.y, 0 },
						Vector3{ 0, 0, zTranslation*dimensions.z },
						Vector3{ 0, yTranslation*dimensions.y, zTranslation*dimensions.z },
					};

					Vector3 translation = translationArr[k];
					Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
					double dotprod = vectorBetween*vectorBetween;
					double distance = sqrt(dotprod);
					if (distance < vCutoff)
					{
						interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					}
				}
			}
			// faces:
			else if (atEdge(*nextAtom, true, false, false))
			{

				for (int k = 0; k < 2; ++k)
				{

					Vector3 translationArr[] =
					{
						Vector3{ 0, 0, 0 },
						Vector3{ xTranslation*dimensions.x, 0, 0 },
					};

					Vector3 translation = translationArr[k];
					Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
					double dotprod = vectorBetween*vectorBetween;
					double distance = sqrt(dotprod);
					if (distance < vCutoff)
					{
						interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					}
				}
			}
			else if (atEdge(*nextAtom, false, true, false))
			{

				for (int k = 0; k < 2; ++k)
				{

					Vector3 translationArr[] =
					{
						Vector3{ 0, 0, 0 },
						Vector3{ 0, yTranslation*dimensions.y, 0 },
					};

					Vector3 translation = translationArr[k];
					Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
					double dotprod = vectorBetween*vectorBetween;
					double distance = sqrt(dotprod);
					if (distance < vCutoff)
					{
						interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					}
				}
			}
			else if (atEdge(*nextAtom, false, false, true))
			{

				for (int k = 0; k < 2; ++k)
				{

					Vector3 translationArr[] =
					{
						Vector3{ 0, 0, 0 },
						Vector3{ 0, 0, zTranslation*dimensions.z },
					};

					Vector3 translation = translationArr[k];
					Vector3 vectorBetween = atom->at() - nextAtom->at() - translation;
					double dotprod = vectorBetween*vectorBetween;
					double distance = sqrt(dotprod);
					if (distance < vCutoff)
					{
						interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, translation });
					}
				}
			}
			else
			{
				Vector3 vectorBetween = atom->at() - nextAtom->at();
				double dotprod = vectorBetween*vectorBetween;
				double distance = sqrt(dotprod);
				if (distance < vCutoff)
				{
					interactionList.push_back(std::pair<Atom*, Vector3>{ nextAtom, Vector3{ 0, 0, 0 } });
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


	double mass = simulationParams.material->mass* PHConstants::amuToefA;
	sumVelocity = sumVelocity / (double)nbrAtoms; 	//center of mass velocity
	sumVelocity2 = sumVelocity2 / (double)nbrAtoms;	//mean squared velocity
	double scaleFactor = 1.0;
	scaleFactor = sqrt(3.0*(temperature / sumVelocity2)*(PHConstants::boltzmann / mass));
	// std::cout << "center of mass velocity = " << sumVelocity << std::endl;
	// std::cout << "mean squared velocity = " << sumVelocity2 << std::endl;
	// std::cout << "temperature = " << temperature << std::endl;

	for (auto& atom : atoms)
	{
		Vector3 velocity = atom->velocity();
		velocity = (velocity - sumVelocity)*scaleFactor;
		atom->setVelocity(velocity);
	}
}

void MDBox::updateForces(const Material& material)
{
	for (auto& atom : atoms)
	{
		atom->setForce({ 0.0, 0.0, 0.0 });
	}

	int atomIndex = 0;
	for (auto& interactionList : verletList)
	{
		Atom* atom{ atoms[atomIndex++] };
		for (auto& atomTranslationPair : interactionList)
		{
			Vector3 translatedInteractingAtomPosition = atomTranslationPair.first->at() + atomTranslationPair.second;
			Vector3 diff{translatedInteractingAtomPosition - atom->at()};
			if (sqrt(diff*diff) > simulationParams.cutoffDistance)
				continue;
			Vector3 interaction = simulationParams.material->potential->interaction(atom->at(), translatedInteractingAtomPosition, simulationParams);
			Vector3 totalForceAtom = atom->totalForce() - interaction;
			Vector3 totalForceInteractingAtom = atomTranslationPair.first->totalForce() + interaction;
			atom->setForce (totalForceAtom);
			atomTranslationPair.first->setForce(totalForceInteractingAtom);
		}
	}
}

void MDBox::updatePositions()
{
	for (auto& atom : atoms)
	{
		Vector3 oldPosition = atom->at();
		Vector3 oldVelocity = atom->velocity();
		Vector3 oldForce = atom->totalForce();
		double  deltatime = simulationParams.timestepLength;
		double mass = simulationParams.material->mass* PHConstants::amuToefA;
		Vector3 newPosition = oldPosition + oldVelocity * deltatime + (oldForce / mass)*(deltatime / 2)*deltatime;

		static const double offset = 1.0; // 0 seems like a bad number, but I do not know why
		if (newPosition.x < -offset)
			newPosition.x = newPosition.x + simulationParams.lattice->latticeConstant*dimensions.x;
		else if (newPosition.x >= simulationParams.lattice->latticeConstant*dimensions.x + offset)
			newPosition.x = newPosition.x - simulationParams.lattice->latticeConstant*dimensions.x;

		if (newPosition.y < -offset)
			newPosition.y = newPosition.y + simulationParams.lattice->latticeConstant*dimensions.y;
		else if (newPosition.y >= simulationParams.lattice->latticeConstant*dimensions.y + offset)
			newPosition.y = newPosition.y - simulationParams.lattice->latticeConstant*dimensions.y;

		if (newPosition.z < -offset)
			newPosition.z = newPosition.z + simulationParams.lattice->latticeConstant*dimensions.z;
		else if (newPosition.z >= simulationParams.lattice->latticeConstant*dimensions.z + offset)
			newPosition.z = newPosition.z - simulationParams.lattice->latticeConstant*dimensions.z;

		atom->setPosition(newPosition);
		atom->setForcePreviousTimestep(oldForce);
	}
}

void MDBox::updateVelocities()
{
	Vector3 sumVelocity = { 0.0, 0.0, 0.0 };
	double sumVelocity2 = 0.0;
	double mass = simulationParams.material->mass* PHConstants::amuToefA;

	for (auto& atom : atoms)
	{
		Vector3 oldVelocity = atom->velocity();
		Vector3 newForce = atom->totalForce();
		Vector3 oldForce = atom->forcePreviousTimestep();
		double  deltatime = simulationParams.timestepLength;
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