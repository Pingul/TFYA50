#include "material.h"

Material* Material::TESTMaterial() 
{ 
	return new Material(1, 1, new LJPotential()); 
}

Material::~Material()
{
	delete potential;
}