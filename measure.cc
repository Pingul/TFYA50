#include "measure.h"
#include "mdbox.h"
#include "fileIO.h"
#include "vector.h"
#include "physicalConstants.h"
#include "simulation.h"
#include "material.h"
#include "potential.h"
#include <iostream>
#include <math.h>
#include <exception>
#include <algorithm>

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


// void AggregateMeasure::addDependency(std::string name, const Measure& measure)
// {
// 	measures[name] = &measure;
// }

// double AggregateMeasure::dValue(std::string measureName, double t)
// {
// 	auto it = measures.find(measureName);
// 	if (it == measures.end())
// 		return 0.0; 

// 	return 0.0;

// }

// void TotalEnergy::calculate(double t, const SimulationParams& params, const MDBox& box)
// {

// 	double val = dValue()
	
// 	timestamps.push_back(t);
// 	values.push_back(energy);
// }

void KineticEnergy::calculate(double t, const SimulationParams& params, const MDBox& box)
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

void PotentialEnergy::calculate(double t, const SimulationParams& params, const MDBox& box)
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
			energyPerAtom += params.material->potential->potentialEnergy(currentAtomPosition, translatedInteractingAtomPosition);
		}
		atomIndex++;
		energy += energyPerAtom;
	}

	timestamps.push_back(t);
	values.push_back(energy);
}