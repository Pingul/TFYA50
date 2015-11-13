#include "thermostat.h"
#include "mdbox.h"
#include <iostream>
#include "measure.h"
#include "simulation.h"

const std::vector<Atom*>& Thermostat::atoms(const MDBox& box)
{
	return box.atoms;
}

void AndersonThermostat::scaleTemperature(double t, MDBox& box)
{
	double currentTemp = Measure::value(params.temperature, t);
	double goalTemp = params.goalTemperature;
	double deltatime = params.timestepLength;
	double scaleFactor = sqrt(1 + (deltatime/(0.5*pow(10,3)))*(goalTemp / currentTemp - 1));
	
	for (auto& atom : atoms(box))
	{
		Vector3 newVelocity = atom->velocity() * scaleFactor;
		atom->setVelocity(newVelocity);
	}
	
	// std::cout << "Should scale velocities" << std::endl;
}