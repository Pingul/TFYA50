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
	virtual Vector3 interaction(const Vector3&, const Vector3&) = 0;
};

class LJPotential : public AtomicPotential
{
public:
	LJPotential() = default;
	virtual ~LJPotential() = default;

	virtual Vector3 interaction(const Atom&, const Atom&);
	virtual Vector3 interaction(const Vector3&, const Vector3&);

private:
	double epsilon{ 0.010423316 };	//in eV
	double sigma{ 3.40 };	//in Angstrom
};

#endif