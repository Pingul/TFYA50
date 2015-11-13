#include "potential.h"
#include "simulation.h"

#include "vector.h"
#include "atom.h"
#include <math.h>

Vector3 AtomicPotential::interaction(const Atom& a1, const Atom& a2, const SimulationParams& params)
{
	return interaction(a1.at(), a2.at(), params);
}

Vector3 LJPotential::interaction(const Vector3& vector1, const Vector3& vector2, const SimulationParams& params)
{
	double dis2 = (vector2 - vector1)*(vector2 - vector1);
	double dis = sqrt(dis2);
	double dis7 = pow(dis, 7);
	double dis6 = pow(dis2, 3);

	double sigma6 = pow(sigma, 6);
	double sigma12 = pow(sigma, 12);

	double constant = ((24 * epsilon) / dis7) * (2 * sigma12 / dis6 - sigma6);

	double rcut = params.cutoffDistance;
	double rin = 0.95*rcut;

	double dljrin = -48.0*epsilon / rin*(pow((sigma / rin), 12) - 0.5*pow((sigma / rin), 6));

	Vector3 force; /*= (constant - dljcutoff)* (vector2 - vector1) / dis;*/
	if (dis < rin)
		return force= constant* (vector2 - vector1) / dis;
	else if (dis>rcut)
		return{ 0, 0, 0 };
	else
	{
		static double ljin = 4 * epsilon*(pow((sigma / rin), 12) - pow((sigma / rin), 6));
		double t = (dis - rcut) / (rin - rcut);
		double b = -dljrin*(rin - rcut) + ljin;
		return force = 1.0 / (rin - rcut)*(-3.0*(ljin+b)*pow(t,2)+2.0*(2.0*ljin+b)*t)*(vector2 - vector1) / dis;
	}
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
		double dljrin = -48.0*epsilon / rin*(pow((sigma / rin), 12) - 0.5*pow((sigma / rin), 6));
		double t = (r - rcut) / (rin - rcut);
		double b = -dljrin*(rin - rcut) + ljin;
		return t*ljin+t*(1-t)*(-ljin*(1-t)+b*t);
	}
}