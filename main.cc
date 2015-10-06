#include "simulation.h"
#include <iostream>
#include <exception>

int main()
{
	try
	{
		Simulation sim{};
		sim.run();
		std::cout << "simulation completed" << std::endl;
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