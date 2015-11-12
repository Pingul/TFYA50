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
		void DEBUG_PRINT();
		void DEBUG_VERLET_LIST();

		// Gives an unmodifiable reference to atoms, for statistical usage
		const std::vector<Atom*>& atomSnapshot() const { return atoms; }

	private:
		const SimulationParams& simulationParams;
		Vector3 dimensions;
		std::vector<Atom*> atoms;
		VerletList verletList;

		void createInitialAtoms(const Lattice&);
		void setInitialVelocities(double temperature);

		// Helper functions for the verlet list
		bool atCorner(const Atom&);
		bool atEdge(const Atom&);
		bool atFace(const Atom&);
};

#endif