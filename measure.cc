#include "measure.h"
#include "mdbox.h"
#include "fileIO.h"
#include "vector.h"
#include "physicalConstants.h"
#include <iostream>
#include <math.h>

#include "random.h"

const std::vector<Atom*>& Measure::atoms(const MDBox& box)
{
	return box.atoms;
}

const std::vector<std::vector<std::pair<Atom*, Vector3>>>& Measure::verletList(const MDBox& box)
{
	return box.verletList;
}

void Measure::saveToFile(const std::string& file)
{
	fileIO::MDF::write(file, timestamps, values);
}

void KineticEnergy::calculate(double t, const MDBox& box)
{
	double mass = 39.948*PHConstants::amuToefA;
	double energy = 0;	
	for (auto& atom : atoms(box))
	{
		energy += atom->velocity()*atom->velocity();
	}
	energy = 0.5*energy * mass;
	timestamps.push_back(t);
	values.push_back(energy);
}

void PotentialEnergy::calculate(double t, const MDBox& box)
{
	double energy = 0;
	int atomIndex = 0;

	for (auto& interactionList : verletList(box))
	{
		double energyPerAtom = 0;
		Atom* atom{ atoms(box)[atomIndex] };
		for (auto& atomTranslationPair : interactionList)
		{
			Vector3 translatedInteractingAtomPosition = atomTranslationPair.first->at() + atomTranslationPair.second;
			Vector3 currentAtomPosition = atom->at();

			double r = sqrt((currentAtomPosition - translatedInteractingAtomPosition)*(currentAtomPosition - translatedInteractingAtomPosition));
			double epsilon = 1.67e-3;
			double sigma = 3.40;

			energyPerAtom += 4 * epsilon*(pow((sigma / r), 12) - pow((sigma / r), 6));
		}
		atomIndex++;
		//std::cout << "Potential energy per atom: " << energyPerAtom << std::endl;
		energy += energyPerAtom;
	}

	timestamps.push_back(t);
	values.push_back(energy);
}