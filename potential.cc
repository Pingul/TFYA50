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

Vector3 LJPotential::interaction(const Vector3& vector1, const Vector3& vector2, const SimulationParams& params)
{
	double dis = sqrt((vector2 - vector1)*(vector2 - vector1));
	static double rcut = params.cutoffDistance;
	static double rin = 0.95*rcut;
	if (dis>rcut)
		return Vector3{ 0, 0, 0 };


	Vector3 force; /*= (constant - dljcutoff)* (vector2 - vector1) / dis;*/
	if (dis < rin)
	{
		static double sigma6 = pow(sigma, 6);
		double deriv = ((24 * epsilon) / pow(dis, 7)) * sigma6*(2 * sigma6 / pow(dis, 6) - 1);
		return deriv*(vector2 - vector1) / dis;
	}
	static double dljrin = -48.0*epsilon / rin*(pow((sigma / rin), 12) - 0.5*pow((sigma / rin), 6));
	return dljrin / (rin - rcut)*(dis - rcut)*(vector2 - vector1)/dis;	
}

double LJPotential::potentialEnergy(const Vector3& vector1, const Vector3& vector2, const SimulationParams& params)
{
	double r = sqrt((vector1 - vector2)*(vector1 - vector2));
	static double rcut = params.cutoffDistance;
	if (r > rcut)
		return 0;

	static double rin = 0.95*rcut;

	if (r < rin)
		return 4 * epsilon*(pow((sigma / r), 12) - pow((sigma / r), 6));
	else
	{
		static double ljin = 4 * epsilon*(pow((sigma / rin), 12) - pow((sigma / rin), 6));
		return ljin / (rin - rcut)*(r - rcut);
	}
}