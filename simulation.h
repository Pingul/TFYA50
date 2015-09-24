#ifndef _SIMULATION_
#define _SIMULATION_

class Lattice;

class Simulation
{
	public:
		Simulation();
		~Simulation() = default;

		void run();

	private:
		int timesteps;
		int timestepLength;
		// MDBox box;
		int measureDataLogFrequency;
		// List of Measure:s
		bool saveVisualizationData{false};
		int visualizationLogFrequency;
		// Material material
		double cutoffDistance;

		Lattice* lattice;

		void setup();
};

#endif