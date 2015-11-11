#ifndef _PHYSICAL_CONSTANTS_
#define _PHYSICAL_CONSTANTS_

namespace PHConstants
{
	// position -> Angstrom
	// time -> femtosecond
	// velocity -> Angstrom/femtosecond
	// force -> eV/Angstrom
	// temperature -> Kelvin
	// energy -> eV = 1.6E-19 N*m = 1.6E-9 N*Angstrom
	
	const double amuToefA{ 1.03642e02 };		// mass -> amu = 1.03642E02 eV fs^2 Angstrom^-2
	const double boltzmann_SI{1.38064852e-23};	// in m^2 kg s^-2 K^-1
	const double boltzmann{8.61734e-5};			// ev/K
	const double planckConstant{0.6582119514};	//eV fs
	
};

#endif