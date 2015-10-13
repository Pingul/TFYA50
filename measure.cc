#include "measure.h"
#include "mdbox.h"
#include "fileIO.h"
#include "vector.h"

#include "random.h"

const std::vector<Atom*>& Measure::atoms(const MDBox& box)
{
	return box.atoms;
}

const std::vector<std::vector<std::pair<Atom*, Vector3>>>& Measure::verletList(const MDBox& box)
{
	return box.verletList;
}

void Measure::saveToFile(const std::string& file)
{
	fileIO::MDF::write(file, timestamps, values);
}

void KineticEnergy::calculate(double t, const MDBox& box)
{
	double energy = 0;
	for (auto& atom : atoms(box))
	{
		// This is just nonsense atm
		energy += atom->velocity()*atom->velocity() + Random::next();
	}
	timestamps.push_back(t);
	values.push_back(energy);
}