#include "thermostat.h"
#include "mdbox.h"
#include "measure.h"
#include "simulation.h"
#include <iostream>
#include <math.h>

const std::vector<Atom*>& Thermostat::atoms(const MDBox& box)
{
	return box.atoms;
}

void BerendsenThermostat::scaleTemperature(double t, MDBox& box)
{
	double currentTemp = Measure::value(params.temperature, t);
	double goalTemp = params.goalTemperature;
	double deltatime = params.timestepLength;
	static double scaling = deltatime/(params.thermostatScaling*1000);
	double scaleFactor = sqrt(1 + scaling*(goalTemp / currentTemp - 1));
	
	for (auto& atom : atoms(box))
	{
		Vector3 newVelocity = atom->velocity() * scaleFactor;
		atom->setVelocity(newVelocity);
	}
	
	// std::cout << "Should scale velocities" << std::endl;
}