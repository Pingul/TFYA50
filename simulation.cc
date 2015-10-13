#include "simulation.h"
#include "lattice.h"
#include "material.h"
#include "mdbox.h"
#include "random.h"
#include "fileIO.h"
#include "measure.h"

#include <cmath>
#include <map>
#include <iostream>
#include <string>
#include <exception>


Simulation::Simulation(const char* setFile)
{
	Random::setup();
	initSettings(setFile);
	validateSettings();
	box = new MDBox(*this);
}

void Simulation::initSettings(const char* setFile)
{
	//SETFileReader reader{};
	std::map<std::string, double> nbrSettings;
	std::map<std::string, std::string> strSettings;
	fileIO::SET::read(setFile, nbrSettings, strSettings);
	//reader.readFile(setFile, nbrSettings, strSettings);

	for (auto& strSetting : strSettings)
	{
		std::string variable = strSetting.first;
		std::string value = strSetting.second;
		if (variable.compare("lattice") == 0)
		{
			if (value.compare("FCC") == 0)
				lattice = Lattice::createFCCLattice();
			else
				throw std::runtime_error{ "Lattice '" + value + "' does not exist" };
		}
		else if (variable.compare("material") == 0)
		{
			// We have no special materials atm
			material = Material::TESTMaterial();
		}
	}

	for (auto& nbrSetting : nbrSettings)
	{
		std::string variable = nbrSetting.first;
		double value = nbrSetting.second;
		if (variable.compare("timesteps") == 0)
			timesteps = (int)std::round(value);
		else if (variable.compare("timestepLength") == 0)
			timestepLength = (int)std::round(value);
		else if (variable.compare("saveVisualizationData") == 0)
			saveVisualizationData = (bool)value;
		else if (variable.compare("visualizationLogRate") == 0)
			visualizationLogRate = (int)std::round(value);
		else if (variable.compare("measureDataLogRate") == 0)
			measureDataLogRate = (int)std::round(value);
		else if (variable.compare("cutoffDistance") == 0)
			cutoffDistance = value;
		else if (variable.compare("thermostat") == 0)
			thermostat = (bool)value;
		else if (variable.compare("goalTemperature") == 0)
			goalTemperature = value;
		else if (variable.compare("initialTemperature") == 0)
			initialTemperature = value;
		else if (variable.compare("dimensions.x") == 0)
			dimensions.x = std::round(value);
		else if (variable.compare("dimensions.y") == 0)
			dimensions.y = std::round(value);
		else if (variable.compare("dimensions.z") == 0)
			dimensions.z = std::round(value);
		else if (variable.compare("latticeConstant") == 0)
			lattice->latticeConstant = value; // Dependent on the lattice that we create earlier
		else
			throw std::runtime_error{"Could not find a match for setting '" + variable + "'"};
	}
}

void Simulation::validateSettings()
{
	int errors = 0;
	std::vector<std::string> variables;
	if (timesteps == -1)
	{
		errors++;
		variables.push_back("timestep");
	}
	if (cutoffDistance <= 0.0)
	{
		errors++;
		variables.push_back("cutoffDistance");
	}
	if (initialTemperature < 0.0)
	{
		errors++;
		variables.push_back("initialTemperature");
	}
	if (dimensions.x <= 0.0)
	{
		errors++;
		variables.push_back("dimensions.x");
	}
	if (dimensions.y <= 0.0)
	{
		errors++;
		variables.push_back("dimensions.y");
	}
	if (dimensions.z <= 0.0)
	{
		errors++;
		variables.push_back("dimensions.z");
	}
	if (lattice == nullptr)
	{
		errors++;
		variables.push_back("lattice");	
	}
	else if (lattice->latticeConstant <= 0.0) 
	{
		errors++;
		variables.push_back("latticeConstant");
	}
	if (material == nullptr)
	{
		errors++;
		variables.push_back("material");	
	}

	if (errors > 0)
	{
		std::cout << errors << " errors" << std::endl;
		for (auto& str : variables)
		{
			std::cout << "error: '" << str << "' was not properly initialized" << std::endl;
		}
		throw std::runtime_error{"too many initialization errors"};
	}
	else
	{
		std::cout << "simulation initialization OK" << std::endl;
	}
}

void Simulation::run()
{
	Measure* measure = new KineticEnergy();
	for (int i = 0; i < timesteps; ++i)
	{
		double t = i*timestepLength;
		if (i % verletListUpdateFrequency == 0)
			box->updateVerletList();
		//box->DEBUG_PRINT();
		box->updatePositions();
		box->updateForces(*material);
		box->updateVelocities();
		measure->calculate(t, *box);
		if (i % 10 == 0)
		{
			double percentFinished = ((double)i/(double)timesteps)*100.0;
			std::cout << "completed " << i << " out of " << timesteps << " steps (" << percentFinished << "%)." << std::endl;
		}
	}
	measure->saveToFile("test.mdf");
}

Simulation::~Simulation()
{
	delete lattice;
	delete box;
	delete material;
}