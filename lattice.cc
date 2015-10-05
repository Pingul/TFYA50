#include "lattice.h"

Lattice* Lattice::createFCCLattice()
{
	Lattice* lattice = new Lattice();
	// Modify set the atom positions
	lattice->atomPositions.push_back(Vector3{0.0, 0.0, 0.0});
	lattice->atomPositions.push_back(Vector3{0.5, 0.5, 0.0});
	lattice->atomPositions.push_back(Vector3{ 0.5, 0.0, 0.5});
	lattice->atomPositions.push_back(Vector3{ 0.0, 0.5, 0.5});

	return lattice;
}
