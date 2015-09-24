#ifndef _MDBOX_H_
#define _MDBOX_H_

#include "atom.h"
#include <vector>

class Lattice;
class Vector3;

class MDBox
{
	public:
		MDBox() = delete;
		MDBox(const Vector3& dimensions, const Lattice& lattice);
		~MDBox();

		void updateVerletList();
		void updateForces();
		void updatePositionsAndVelocities();

	private:
		Vector3 dimensions;
		std::vector<Atom*> atoms;
		std::vector<std::vector<Atom*>> verletList;

		void createInitialAtoms(const Lattice&);
};

#endif