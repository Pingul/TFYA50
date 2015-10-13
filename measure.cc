#include "measure.h"
#include "mdbox.h"

const std::vector<Atom*>& Measure::atoms(const MDBox& box)
{
	return box.atoms;
}

const std::vector<std::vector<Atom*>>& Measure::verletList(const MDBox& box)
{
	return box.verletList;
}

void KineticEnergy::calculate(double t, const MDBox& box)
{
	double energy = 0;
	for (auto& atom : atoms(box))
	{
		// This is just nonsense atm
		energy += atom->velocity()*atom->velocity();
	}
	timestamps.push_back(t);
	values.push_back(energy);
}