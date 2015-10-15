#include "material.h"

Material* Material::TESTMaterial() 
{ 
	return new Material(39.948, 1, new LJPotential(1, 1)); 
}

Material* Material::Argon()
{
	return new Material(39.948, 1, new LJPotential{ 0.010423316, 3.40 });
}

Material::~Material()
{
	delete potential;
}