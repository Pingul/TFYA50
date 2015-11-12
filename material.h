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

		double mass; // atomic mass units
		double radius; // Ångström
		AtomicPotential* potential{nullptr};
};

#endif