#ifndef _SIMULATION_
#define _SIMULATION_

class Simulation
{
	public:
		Simulation() = default;
		~Simulation() = default;

		void run();

	private:
		int timesteps;
		int timestepLength;
		// MDBox box;
		int measureDataLogFrequency;
		// List of Measure:s
		bool saveVisualizationData{false);
		int visualizationLogFrequency;
		// Material material
		// Lattice lattice
		double cutoffDistance;
};

#endif