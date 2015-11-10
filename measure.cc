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

double Measure::value(Measure* measure, double timestamp)
{
	auto lower = std::lower_bound(measure->timestamps.begin(), measure->timestamps.end(), timestamp);
	bool found{lower != measure->timestamps.end() && *lower == timestamp};
	if (!found)
		return 0.0; // lets default to 0 instead of throwing an error
	auto index = std::distance(measure->timestamps.begin(), lower);
	return measure->values.at(index);
}

void TotalEnergy::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double energy{Measure::value(kineticEnergy, t) + Measure::value(potentialEnergy, t)};
	timestamps.push_back(t);
	values.push_back(energy);
}

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
			energyPerAtom += params.material->potential->potentialEnergy(currentAtomPosition, translatedInteractingAtomPosition, params);
		}
		atomIndex++;
		energy += energyPerAtom;
	}

	timestamps.push_back(t);
	values.push_back(energy);
}

void Temperature::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double temperature = { Measure::value(kineticEnergy, t) * 2.0 / (3.0*PHConstants::boltzmann*params.dimensions.x*params.dimensions.y*params.dimensions.z*4.0) };

	timestamps.push_back(t);
	values.push_back(0.0);	
}