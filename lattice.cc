#include "lattice.h"

Lattice* Lattice::createFCCLattice()
{
	Lattice* lattice = new Lattice();
	// Modify set the atom positions
	lattice->atomPositions.push_back(Vector3{1.0, 0.0, 1.0});
	lattice->atomPositions.push_back(Vector3{1.0, 0.0, 0.0});

	return lattice;
}
