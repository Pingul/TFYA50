#ifndef _MDBOX_H_
#define _MDBOX_H_

#include "atom.h"
#include <vector>

class Lattice;
class Vector3;
class Simulation;

class MDBox
{
	public:
		MDBox() = delete;
		MDBox(Simulation&);
		~MDBox();

		void updateVerletList();
		void updateForces();
		void updatePositionsAndVelocities();

	private:
		const Simulation& simulation; // We will need to access a lot of Simulation variables
		Vector3 dimensions;
		std::vector<Atom*> atoms;
		std::vector<std::vector<Atom*>> verletList;

		void createInitialAtoms(const Lattice&);
		void setInitialAtomOffsets();
		void setInitialVelocities(double temperature);

		void TEST_createInitialAtoms(const Lattice&);
		void TEST_setInitialVelocities(double temperature);
};

#endif