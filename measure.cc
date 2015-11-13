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
			Atom* otherAtom = atomTranslationPair.first;
			Vector3 distBetween = otherAtom->at() - atom->at();
			Vector3 forceBetween = params.material->potential->interaction(*atom, *otherAtom);
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
	double temperature = { Measure::value(kineticEnergy, t) * 2.0 / (double(atoms(box).size())*3.0*PHConstants::boltzmann) };  //(3.0*PHConstants::boltzmann*params.dimensions.x*params.dimensions.y*params.dimensions.z*4.0) };

	timestamps.push_back(t);
	values.push_back(temperature);	
}

void MSD::calculate(double t, const SimulationParams& params, const MDBox& box)
{
	double msd = 0;
	int atomIndex = 0;
	
	for (int iii = 0; iii < params.dimensions.x; ++iii)
	{
		for (int jjj = 0; jjj < params.dimensions.y; ++jjj)
		{
			for (int kkk = 0; kkk < params.dimensions.z; ++kkk)
			{
				for (auto& position : params.lattice->atomPositions)
				{

					Atom* atom{ atoms(box)[atomIndex] };
					Vector3 fccPosition=params.lattice->latticeConstant*position + params.lattice->latticeConstant*Vector3{ (double)iii, (double)jjj, (double)kkk };
					msd += (atom->at() - fccPosition)*(atom->at() - fccPosition);

					atomIndex++;
				}
			}
		}
	}

	msd = msd / double(atoms(box).size());

	timestamps.push_back(t);
	values.push_back(msd);
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

	double pressure = (npart*PHConstants::boltzmann*instantT)/pow(4000,3);
	int atomIndex = 0;

	for (auto& interactionList : verletList(box))
	{
		double energyPerAtom = 0;
		Atom* atom{ atoms(box)[atomIndex] };

		for (auto& atomTranslationPair : interactionList)
		{
			//Vector3 translatedInteractingAtomPosition = atomTranslationPair.first->at() + atomTranslationPair.second;
			Vector3 currentAtomPosition = atom->at();

			Atom* otherAtom = atomTranslationPair.first;

			Vector3 distBetween = otherAtom->at() - atom->at();
			Vector3 forceBetween = params.material->potential->interaction(atom->at(), otherAtom->at(), params);

			pressure += pressure;
		}
		atomIndex++;
		pressure += pressure;
	}
	std::cout << "Pressure = " << pressure << std::endl;
	//std::cout << " " << params.material->potential->potentialEnergy({ 0,0,0 }, { 6.1,0,0 }, params);

	timestamps.push_back(t);
	values.push_back(pressure);

}