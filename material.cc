#include "material.h"

Material* Material::TESTMaterial() 
{ 
	return new Material(39.948, 1, new LJPotential()); 
}

Material::~Material()
{
	delete potential;
}