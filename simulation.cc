#include "simulation.h"
#include "lattice.h"

#include <iostream>


void Simulation::setup()
{
	lattice = Lattice::createFCCLattice();
}

int main()
{
	std::cout << "running" << std::endl;
}