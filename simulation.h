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
		int timesteps; 
		double timestepLength; // in femtoseconds
		bool saveVisualizationData{false};
		int visualizationLogRate; // every x femtosecond
		int measureDataLogRate; // every x femtosecond
		double cutoffDistance; // in Ångström
		Vector3 dimensions; // in Unit cells

		Lattice* lattice;
		MDBox* box;
		Material* material;
		// List of Measure:s

		void setup();
};

#endif