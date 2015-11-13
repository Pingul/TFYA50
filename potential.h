#ifndef _POTENTIAL_H_
#define _POTENTIAL_H_

class Atom;
class Vector3;
class SimulationParams;

/* Base class for potential that will be used in the project

Cannot be instatiated.
*/
class AtomicPotential
{
	public:
		AtomicPotential() = default;
		virtual ~AtomicPotential() = default;

		// shall return a Vector3 that indicates by what force the first Atom affects the second
		virtual Vector3 interaction(const Atom&, const Atom&, const SimulationParams&);
		virtual Vector3 interaction(const Vector3&, const Vector3&, const SimulationParams&) = 0;
		virtual double potentialEnergy(const Vector3&, const Vector3&, const SimulationParams&) = 0;
};

class LJPotential : public AtomicPotential
{
	public:
		LJPotential() = delete;
		LJPotential(double eps, double sig) 
			: epsilon{eps}, sigma{sig} {}
		virtual ~LJPotential() = default;

		virtual Vector3 interaction(const Vector3&, const Vector3&, const SimulationParams&);
		virtual double potentialEnergy(const Vector3&, const Vector3&, const SimulationParams&);

	private:
		double epsilon;	//in eV
		double sigma;	//in Angstrom
};

#endif