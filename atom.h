#ifndef _ATOM_H_
#define _ATOM_H_

#include "vector.h"

class Atom
{
	public:
		Atom() = default;
		// Atom(Vector3 coordinates, Vector3 velocity, Vector3 force) 
		// 	: coordinates_{coordinates}, velocity_{velocity}, force_{force} {}
		~Atom() = default;

		Vector3 at();
		Vector3 velocity();
		Vector3 totalForce();
		void setPosition(const Vector3&);
		void setVelocity(const Vector3&);
		void setForce(const Vector3&);

	private:
		Vector3 coordinates_{0, 0, 0}; // Ångström
		Vector3 velocity_{0, 0, 0}; // m/s
		Vector3 force_{0, 0, 0}; // N
		
};

#endif