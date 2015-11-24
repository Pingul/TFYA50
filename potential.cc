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

	double r2 = (vector2 - vector1)*(vector2 - vector1);
	double r = sqrt(r2);
	double rcut = params.cutoffDistance;
	
	static double rin = 0.95*rcut;
	if (r > rcut)
		return Vector3{0, 0, 0};

	if (r < rin)
	{
		static double sigma6 = pow(sigma, 6);
		double c = ((24 * epsilon*sigma6) / pow(r, 7)) * (2 * sigma6 / pow(r2, 3) - 1.0);
		return c*(vector2 - vector1) / r;
	}
	else
	{
		static double ljin = 4 * epsilon*(pow((sigma / rin), 12) - pow((sigma / rin), 6));
		static double dljrin = -48.0*epsilon / rin*(pow((sigma / rin), 12) - 0.5*pow((sigma / rin), 6));
		double dist = rin - rcut;
		double dd = dljrin*dist;
		double drd = (r - rcut)/dist;
		double drd2 = drd*drd;
		return 1.0 / dist*(3.0*(dd - 2.0*ljin)*drd2*drd +2.0* (-dd + 3.0*ljin)*drd2)*(vector2 - vector1) / r;
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
	{
		double sr6 = pow((sigma/r), 6);
		return 4 * sr6*epsilon*(sr6 - 1);
	}
	else
	{
		static double ljin = 4 * epsilon*(pow((sigma / rin), 12) - pow((sigma / rin), 6));
		static double dljrin = -48.0*epsilon / rin*(pow((sigma / rin), 12) - 0.5*pow((sigma / rin), 6));
		double t = (r - rcut) / (rin - rcut);
		double t2 = t*t;
		double b = dljrin*(rin - rcut);
		return (b-2.0*ljin)*t2*t+(-b+3.0*ljin)*t2;
	}
}