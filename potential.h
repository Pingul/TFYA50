#ifndef _POTENTIAL_H_
#define _POTENTIAL_H_

class Atom;
class Vector3;

/* Base class for potential that will be used in the project

Cannot be instatiated.
*/
class AtomicPotential
{
public:
	AtomicPotential() = default;
	virtual ~AtomicPotential() = default;

	// shall return a Vector3 that indicates by what force the first Atom affects the second
	virtual Vector3 interaction(const Atom&, const Atom&) = 0;
};

class LJPotential : public AtomicPotential
{
public:
	LJPotential() = default;
	virtual ~LJPotential() = default;

	virtual Vector3 interaction(const Atom&, const Atom&);

private:
	double epsilon{ 1.67e-3 };	//in eV
	double sigma{ 3.40 };	//in Angstrom
};

#endif