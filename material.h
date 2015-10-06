#ifndef _MATERIAL_H_
#define _MATERIAL_H_

class AtomicPotential;

class Material
{
	public:
		Material() = delete;
		Material(double m, double r)
			: mass{m}, radius{r} {}
		~Material() = default;

		static Material* TESTMaterial() { return new Material(1, 1); }

		double mass; // kg
		double radius; // Ångström
		AtomicPotential* potential{nullptr};
};

#endif