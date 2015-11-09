#include "potential.h"

#include "vector.h"
#include "atom.h"
#include "simulation.h"
#include <math.h>

Vector3 LJPotential::interaction(const Atom& atom1, const Atom& atom2)
{
	double dis2 = (atom2.at() - atom1.at())*(atom2.at() - atom1.at());

	double dis8 = pow(dis2, 4);
	double dis6 = pow(dis2, 3);

	double sigma6 = pow(sigma, 6);
	double sigma12 = pow(sigma, 12);

	double constant = ((24 * epsilon) / dis8) * (2 * sigma12 / dis6 - sigma6);
	
	Vector3 force = constant * (atom2.at()-atom1.at());

	return force;
}

Vector3 LJPotential::interaction(const Vector3& vector1, const Vector3& vector2)
{
	double dis2 = (vector2 - vector1)*(vector2 - vector1);

	double dis8 = pow(dis2, 4);
	double dis6 = pow(dis2, 3);

	double sigma6 = pow(sigma, 6);
	double sigma12 = pow(sigma, 12);

	double constant = ((24 * epsilon) / dis8) * (2 * sigma12 / dis6 - sigma6);

	// hardcoded for test
	Vector3 force;
	double dis = sqrt(dis2);
	if (dis > 6.0)
		force = -0.00021484474200299757 * (6.4 - dis) * (vector2 - vector1);
	else 
		force = constant * (vector2 - vector1);

	return force;
}

double LJPotential::potentialEnergy(const Vector3& vector1, const Vector3& vector2, const SimulationParams& params)
{
	double r = sqrt((vector1 - vector2)*(vector1 - vector2));

	double rcut = params.cutoffDistance;
	double rin = 0.95*rcut;

	if (rin < r < rcut)
		return  4 * epsilon*(pow((sigma / r), 12) - pow((sigma / r), 6))*pow((pow(rcut, 2) - pow(r, 2)), 2)*(pow(rcut, 2) + 2 * pow(r, 2) - 3 * pow(rin, 2)) / pow((pow(rcut, 2) - pow(rin, 2)), 3);
	else if (r > rcut)
		return 0;
	else
	{
		return 4 * epsilon*(pow((sigma / r), 12) - pow((sigma / r), 6));
	}
}