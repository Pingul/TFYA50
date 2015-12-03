#include "measure.h"
#include "mdbox.h"
#include "fileIO.h"
#include "vector.h"
#include "physicalConstants.h"
#include "simulation.h"
#include "material.h"
#include "potential.h"
#include "lattice.h"

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

double Measure::average(int start, int end)
{
	if (start >= end)
		return 0.0;

	double val{0.0};
	int used = 0;
	for (int i = 0; i < values.size(); i++)
	{
		if (timestamps[i] >= start && timestamps[i] <= end)
		{
			val += values[i];
			used++;
		}
	}
	return used == 0 ? 0.0 : val/used;
}

void TotalEnergy::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double energy{Measure::value(kineticEnergy, t) + Measure::value(potentialEnergy, t)};

	timestamps.push_back(t);
	values.push_back(energy);
}

void KineticEnergy::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double mass = params.material->mass*PHConstants::amuToefA;
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
	double temperature = { Measure::value(kineticEnergy, t) * 2.0 / (double(atoms(box).size())*3.0*PHConstants::boltzmann) }; 

	timestamps.push_back(t);
	values.push_back(temperature);	
}

void MSD::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double msd = 0;
	int atomIndex = 0;

	for (auto& atom : atoms(box))
	{
		msd += (atom->at() - atom->initialPosition())*(atom->at() - atom->initialPosition());
	}

	msd = msd / double(atoms(box).size());

	timestamps.push_back(t);
	values.push_back(msd);
}


void SurfaceMSD::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double smsd = 0;

	if (!params.pBCx)
	{
		int used = 0;
		for (int atomIndex = 0; atomIndex < atoms(box).size(); ++atomIndex)
		{
			Atom* atom = atoms(box).at(atomIndex);
			if (atom->at().x < 2 * params.lattice->latticeConstant || atom->at().x > params.dimensions.x*params.lattice->latticeConstant - 2 * params.lattice->latticeConstant)
			{
				smsd += (atom->at().x - atom->initialPosition().x)*(atom->at().x - atom->initialPosition().x);
				used++;
			}
		}

		smsd = smsd / used;
	}

	if (!params.pBCy)
	{
		int used = 0;
		for (int atomIndex = 0; atomIndex < atoms(box).size(); ++atomIndex)
		{
			Atom* atom = atoms(box).at(atomIndex);
			if (atom->at().y < 2 * params.lattice->latticeConstant || atom->at().y > params.dimensions.y*params.lattice->latticeConstant - 2 * params.lattice->latticeConstant)
			{
				smsd += (atom->at().y - atom->initialPosition().y)*(atom->at().y - atom->initialPosition().y);
				used++;
			}
		}

		smsd = smsd / used;
	}

	if (!params.pBCz)
	{
		int used = 0;
		for (int atomIndex = 0; atomIndex < atoms(box).size(); ++atomIndex)
		{
			Atom* atom = atoms(box).at(atomIndex);
			if (atom->at().z < 2 * params.lattice->latticeConstant || atom->at().z > params.dimensions.z*params.lattice->latticeConstant - 2 * params.lattice->latticeConstant)
			{
				smsd += (atom->at().z - atom->initialPosition().z)*(atom->at().z - atom->initialPosition().z);
				used++;
			}
		}

		smsd = smsd / used;
	}


	timestamps.push_back(t);
	values.push_back(smsd);
}

void DebyeTemperature::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double mass = params.material->mass*PHConstants::amuToefA;
	double instantT = { Measure::value(temperature,t) };
	double meanSquareDisplacement = { Measure::value(mSD,t) };
	double hBar2 = pow(PHConstants::planckConstant, 2);

	double debyeTemperature = sqrt(3.0*hBar2*instantT / (mass*PHConstants::boltzmann*meanSquareDisplacement));

	timestamps.push_back(t);
	values.push_back(debyeTemperature);
}


 void Pressure::calculate(double t, const SimulationParams& params, const MDBox& box)
 {
 	int npart = atoms(box).size();
 	double instantT = { Measure::value(temperature,t) };

	double pressure = 0;
	int atomIndex = 0;
	double volume = params.dimensions.x * params.dimensions.y * params.dimensions.z * pow(params.lattice->latticeConstant, 3);
 	for (auto& interactionList : verletList(box))
 	{
 		double energyPerAtom = 0;
 		Atom* atom{ atoms(box)[atomIndex] };

 		for (auto& atomTranslationPair : interactionList)
 		{
 			Vector3 currentAtomPosition = atom->at();

			Atom* otherAtom = atomTranslationPair.first;
 			Vector3 distBetween = otherAtom->at() - atom->at();
 			Vector3 forceBetween = params.material->potential->interaction(atom->at(), otherAtom->at(), params);
			pressure += distBetween * forceBetween;
		}
		atomIndex++;
	}
	pressure = (npart*PHConstants::boltzmann*instantT) / volume + pressure/(6.0*volume*atoms(box).size());

 	timestamps.push_back(t);
 	values.push_back(pressure);
 }
