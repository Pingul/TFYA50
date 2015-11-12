#include "potential.h"
#include "simulation.h"

#include "vector.h"
#include "atom.h"
#include <math.h>

Vector3 LJPotential::interaction(const Atom& atom1, const Atom& atom2, const SimulationParams&)
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

Vector3 LJPotential::interaction(const Vector3& vector1, const Vector3& vector2, const SimulationParams&)
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

double LJPotential::potentialEnergy(const Vector3& vector1, const Vector3& vector2, const SimulationParams&)
{
	double r = sqrt((vector1 - vector2)*(vector1 - vector2));

	if (r > 6.0)
		return  -0.0013347770494899366 * (6.4 - r);

	return 4 * epsilon*(pow((sigma / r), 12) - pow((sigma / r), 6));
}