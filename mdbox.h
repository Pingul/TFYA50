#ifndef _MDBOX_H_
#define _MDBOX_H_

#include "atom.h"
#include <vector>

typedef std::vector<std::vector<std::pair<Atom*, Vector3>>> VerletList;

class Lattice;
class Simulation;
class SimulationParams;
class Material;
class Measure;
class Vector3;

class MDBox
{
	friend Measure;
	friend class Thermostat;

	public:
		MDBox() = delete;
		MDBox(const SimulationParams& params);
		~MDBox();

		void updateVerletList();
		void updateForces(const Material&);
		void updatePositions();
		void updateVelocities();

		// Gives an unmodifiable reference to atoms, for statistical usage
		const std::vector<Atom*>& atomSnapshot() const { return atoms; }

		// Parallelization
		void forceWork(int, int);
		void zeroForce(int, int);
		void positionWork(int, int );
		void velocityWork(int, int );

	private:
		const SimulationParams& simulationParams;
		Vector3 dimensions;
		std::vector<Atom*> atoms;
		VerletList verletList;
		double vCutoff;

		void createInitialAtoms(const Lattice&);
		void setInitialVelocities(double temperature);

		void createTasks();

		// Helper functions for the verlet list
		bool atEdge(const Atom& atom, bool xEdge, bool yEdge, bool zEdge);
};

#endif