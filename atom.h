#ifndef _ATOM_H_
#define _ATOM_H_

#include "vector.h"

class Atom
{
	public:
		Atom() = default;

		Vector3 at();
		Vector3 velocity();
		Vector3 totalForce();
		void setPosition(const Vector3&);
		void setVelocity(const Vector3&);
		void setForce(const Vector3&);

	private:
		
};

#endif