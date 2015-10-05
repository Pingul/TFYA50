#ifndef _LATTICE_H_
#define _LATTICE_H_ 

#include <vector>
#include "vector.h"

class Lattice
{
	public:
		static Lattice* createFCCLattice();

		std::vector<Vector3> atomPositions; // For a unit cell in the pattern
		double latticeConstant{ 1.0 };
};

#endif