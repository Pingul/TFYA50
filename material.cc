#include "material.h"

Material* Material::TESTMaterial() 
{ 
	return new Material(39.948, 1, new LJPotential(1, 1)); 
}

Material* Material::Argon()
{
	return new Material(39.948, 1, new LJPotential{ 0.010423316, 3.40 });	//lattice param = 5.260 Ang
}

Material* Material::Silver()
{
	return new Material(107.8682, 1, new LJPotential{ 0.34 , 2.65 });	//lattice param = 4.090 Ang
}

Material* Material::Helium()
{
	return new Material(4.0026, 1, new LJPotential{ 0.0008738 , 2.56 });	//lattice param = 4.242 Ang
}

Material* Material::Neon()
{
	return new Material(20.1797, 1, new LJPotential{ 0.003121 , 2.74 });	//lattice param = 4.429 Ang
}

Material* Material::Kripton()
{
	return new Material(83.7980, 1, new LJPotential{ 0.01404 , 3.65 });	//lattice param = 5.706 Ang
}

Material* Material::Xenon()
{
	return new Material(131.293, 1, new LJPotential{ 0.01997 , 3.98 });	//lattice param = 6.202 Ang
}

Material* Material::Nickel()
{
	return new Material(58.6934, 1, new LJPotential{ 0.66 , 2.24 });	//lattice param = 3.524 Ang
}

Material* Material::Rhodium()
{
	return new Material(102.9055, 1, new LJPotential{ 0.67 , 2.46 });	//lattice param = 3.803 Ang
}


Material::~Material()
{
	delete potential;
}