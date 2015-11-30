#include "simulation.h"
#include <iostream>
#include <exception>

int main(int argc, char* argv[])
{
	try
	{
		if (argc == 2)
		{
			Simulation sim{argv[1]};
			sim.run();
		}
		else
		{
			Simulation sim;
			sim.run();
		}
		std::cout << "simulation completed" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;
		std::cout << "exiting" << std::endl;
	}

	// press enter to quit
	//std::cin.ignore();

	return 0;
}