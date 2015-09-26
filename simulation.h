#ifndef _SIMULATION_
#define _SIMULATION_

#include "vector.h"

class Lattice;
class MDBox;
class Material;

class Simulation
{
	friend MDBox;

	public:
		Simulation();
		~Simulation();

		void run();

	private:
		int timesteps{-1}; 
		double timestepLength; // in femtoseconds
		bool saveVisualizationData{false};
		int visualizationLogRate; // every x femtosecond
		int measureDataLogRate; // every x femtosecond
		double cutoffDistance{-1.0}; // in Ångström
		double initialTemperature{-1.0}; // Kelvin
		bool thermostat{false};
		double goalTemperature; // Kelvin
		Vector3 dimensions{-1, -1, -1}; // in Unit cells

		Lattice* lattice{nullptr};
		MDBox* box;
		Material* material{nullptr};
		// List of Measure:s

		void setup();
		void initSettings();
		void validateSettings();
};

#endif