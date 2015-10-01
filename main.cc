#include "simulation.h"
#include <iostream>
#include <exception>

int main()
{
	try
	{
		Simulation sim{"../../settingsfile.set"};
	}
	catch (const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "exiting" << std::endl;
	}

	std::cin.ignore();

	return 0;
}