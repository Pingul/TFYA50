#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "potential.h"

class Material
{
	public:
		Material() = delete;
		Material(double m, double r, AtomicPotential* p)
			: mass{m}, radius{r}, potential{p} {}
		~Material();

		static Material* TESTMaterial();
		static Material* Argon();
		static Material* Silver();
		static Material* Helium();
		static Material* Neon();
		static Material* Krypton();
		static Material* Xenon();
		static Material* Nickel();
		static Material* Rhodium();

		double mass; // atomic mass units
		double radius; // Ångström
		AtomicPotential* potential{nullptr};
};

#endif