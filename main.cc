#include "simulation.h"
#include <iostream>
#include <exception>

int main()
{
	try
	{
		Simulation sim{};
	}
	catch (const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "exiting" << std::endl;
	}

	// press enter to quit
	std::cin.ignore();

	return 0;
}