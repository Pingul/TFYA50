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
	friend void fileIO::SIM::write(const std::string& path, const std::string& file, const Simulation& sim);

	public:
		Simulation(const char* setFile = "../../settings/default.set"); // To help for VS
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
		void saveMetaData();
		std::string filePath(bool withPrefix = true);
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
		double thermostatScaling{1.0};
		Vector3 dimensions{ -1, -1, -1 }; // in Unit cells
		bool pBCx{ true };
		bool pBCy{ true };
		bool pBCz{ true };
		
		// Internal parameters
		//int currentTimestep{-1};
		int verletListUpdateFrequency{ 10 };
		int calculateMeanAfter{0};

		int threads{1};
		std::string outputDirectory{ "" };
		std::string settingsFileName;

		Lattice* lattice{ nullptr };
		Material* material{ nullptr };

		// Measures
		class KineticEnergy* kineticEnergy{nullptr};
		class PotentialEnergy* potentialEnergy{nullptr};
		class TotalEnergy* totalEnergy{nullptr};
		class Temperature* temperature{nullptr};
		class MSD* msd{ nullptr };
		class SurfaceMSD* surfaceMSD{ nullptr };
		class DebyeTemperature* debyeTemperature{ nullptr };
		class SurfaceDebyeTemperature* surfaceDebyeTemperature{ nullptr };
		class Pressure* pressure{ nullptr };
		class SpecificHeat* specificHeat{ nullptr };

		class Thermostat* thermostat{nullptr};
		
	private:
		void initSettings(const char*);
		void validateSettings();
};

#endif