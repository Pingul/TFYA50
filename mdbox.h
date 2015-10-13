#ifndef _MDBOX_H_
#define _MDBOX_H_

#include "atom.h"
#include <vector>

class Lattice;
class Vector3;
class Simulation;
class Material;
class Measure;

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

	private:
		const Simulation& simulation; // We will need to access a lot of Simulation variables
		Vector3 dimensions;
		std::vector<Atom*> atoms;
		std::vector<std::vector<Atom*>> verletList;

		void createInitialAtoms(const Lattice&);
		void setInitialVelocities(double temperature);
};

#endif