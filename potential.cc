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

Vector3 LJPotential::interaction(const Vector3& vector1, const Vector3& vector2, const SimulationParams& params)
{
	double dis2 = (vector2 - vector1)*(vector2 - vector1);
	double dis8 = pow(dis2, 4);
	double dis6 = pow(dis2, 3);

	double sigma6 = pow(sigma, 6);
	double sigma12 = pow(sigma, 12);

	double constant = ((24 * epsilon) / dis8) * (2 * sigma12 / dis6 - sigma6);

	double rcut = params.cutoffDistance;
	double rin = 0.95*rcut;

	double rin6 = pow(rin, 6);
	double rin14 = pow(rin, 14);

	double beta = (-120.0*sigma6 + 60.0*sigma*rin6) / ((1.0 / (152.0*sigma6*epsilon))*rin14 - 380.0*sigma6 + 380.0*rin6);
	double gamma = 4.0*sigma6*(epsilon / rin6)*((sigma6 / rin6) - 1.0);

	Vector3 force;

	double dis = sqrt(dis2);

	if (rin < dis < rcut)
		force = (beta/dis)*(dis-rcut)*(3.0*dis-2.0*rin-rcut+(2.0/beta)*(gamma/pow(rin-rcut, 2)))*(vector2 - vector1);
	else if (dis < rin)
		force = constant * (vector2 - vector1);
	else
		force = 0* (vector2 - vector1);

	return force;
}

double LJPotential::potentialEnergy(const Vector3& vector1, const Vector3& vector2, const SimulationParams& params)
{
	double r = sqrt((vector1 - vector2)*(vector1 - vector2));

	double sigma6 = pow(sigma, 6);

	double rcut = params.cutoffDistance;
	double rin = 0.95*rcut;

	double rin6 = pow(rin, 6);
	double rin14 = pow(rin, 14);

	double beta = (-120.0*sigma6+60.0*sigma*rin6) / ((1.0/(152.0*sigma6*epsilon))*rin14-380.0*sigma6+380.0*rin6);
	double gamma = 4.0*sigma6*(epsilon / rin6)*((sigma6 / rin6) - 1.0);



	if (rin < r < rcut)
		return  beta*pow(r - rcut, 2)*(r - rin + (gamma / (beta*pow(rin - rcut, 2))));
	else if (r > rcut)
		return 0;
	else
	{
		return 4 * epsilon*(pow((sigma / r), 12) - pow((sigma / r), 6));
	}
}