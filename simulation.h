#ifndef _SIMULATION_
#define _SIMULATION_

#include "vector.h"
#include "fileIO.h"
#include <string>

class Lattice;
class MDBox;
class Material;
class SimulationParams;

class Simulation
{
	friend MDBox;

	public:
		Simulation(const char* setFile = "../../settings/default.set");
		~Simulation();

		void run();

	private:
		SimulationParams* params;
		MDBox* box;
};

class SimulationParams
{
	public:
		SimulationParams(const char* file = "../../settings/default.set");
		~SimulationParams();

		// Parameters needing initialization
		int timesteps{ -1 };
		double timestepLength; // in femtoseconds
		bool saveVisualizationData{ false };
		int visualizationLogRate; // every x femtosecond
		int measureDataLogRate; // every x femtosecond
		double cutoffDistance{ -1.0 }; // in Ångström
		double initialTemperature{ -1.0 }; // Kelvin
		bool thermostat{ false };
		double goalTemperature; // Kelvin
		Vector3 dimensions{ -1, -1, -1 }; // in Unit cells

		// Internal parameters
		//int currentTimestep{-1};
		int verletListUpdateFrequency{ 50 };

		Lattice* lattice{ nullptr };
		Material* material{ nullptr };
		
	private:
		void initSettings(const char*);
		void validateSettings();
};

#endif