#ifndef	_THERMOSTAT_
#define _THERMOSTAT_

#include "atom.h"
#include <vector>

class SimulationParams;
class MDBox;

class Thermostat 
{
	public:
		virtual ~Thermostat() = default;
		virtual void scaleTemperature(double t, MDBox& box) = 0;

	protected:
		Thermostat(const SimulationParams& p) 
			: params{p} {}
		const SimulationParams& params;
		const std::vector<Atom*>& atoms(const MDBox&);
};

class BerendsenThermostat : public Thermostat
{
	public:
		BerendsenThermostat(const SimulationParams& p) 
			: Thermostat{p} {}

		void scaleTemperature(double t, MDBox& box);
};

#endif