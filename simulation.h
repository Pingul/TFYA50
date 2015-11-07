#ifndef _SIMULATION_
#define _SIMULATION_

#include "vector.h"
#include "fileIO.h"
#include <string>

class Lattice;
class MDBox;
class Material;
class Measure;
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
		std::vector<Measure*> measures;

		std::string filePrefix{ "" };
		std::string visFile{ "md.vis" };

		void setupMeasures();
		void calculateMeasures(double t);
		void saveMeasures();
		std::string filePath();
};

class SimulationParams
{
	public:
		SimulationParams(const char* file);
		~SimulationParams();

		// Parameters needing initialization
		int timesteps{ -1 };
		double timestepLength; // in femtoseconds
		bool saveVisualizationData{ false };
		int visualizationLogRate; // every x femtosecond
		int measureDataLogRate; // every x femtosecond
		double cutoffDistance{ -1.0 }; // in Ångström
		double initialTemperature{ -1.0 }; // Kelvin
		double goalTemperature; // Kelvin
		Vector3 dimensions{ -1, -1, -1 }; // in Unit cells

		// Internal parameters
		//int currentTimestep{-1};
		int verletListUpdateFrequency{ 50 };

		std::string outputDirectory{ "" };

		Lattice* lattice{ nullptr };
		Material* material{ nullptr };

		// Measures
		class KineticEnergy* kineticEnergy{nullptr};
		class PotentialEnergy* potentialEnergy{nullptr};
		class TotalEnergy* totalEnergy{nullptr};
		class Temperature* temperature{nullptr};

		class Thermostat* thermostat{nullptr};
		
	private:
		void initSettings(const char*);
		void validateSettings();
};

#endif