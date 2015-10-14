#ifndef _MDBOX_H_
#define _MDBOX_H_

#include "atom.h"
#include <vector>

typedef std::vector<std::vector<std::pair<Atom*, Vector3>>> VerletList;

class Lattice;
class Simulation;
class Material;
class Measure;
class Vector3;

class MDBox
{
	friend Measure;

	public:
		MDBox() = delete;
		MDBox(Simulation&);
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
		const Simulation& simulation; // We will need to access a lot of Simulation variables
		Vector3 dimensions;
		std::vector<Atom*> atoms;
		VerletList verletList;

		void createInitialAtoms(const Lattice&);
		void setInitialVelocities(double temperature);
};

#endif