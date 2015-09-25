#include "simulation.h"
#include "lattice.h"
#include "material.h"
#include "mdbox.h"

#include <iostream>


Simulation::Simulation()
{
	setup();
}

void Simulation::setup()
{
	lattice = Lattice::createFCCLattice();
	material = Material::TESTMaterial();
	dimensions = Vector3{1, 0, 0};
	box = new MDBox(*this);
}

Simulation::~Simulation()
{
	delete lattice;
	delete box;
	delete material;
}