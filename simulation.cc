#include "simulation.h"
#include "lattice.h"
#include "material.h"
#include "mdbox.h"
#include "random.h"
#include "SETFileReader.h"

#include <cmath>
#include <map>
#include <iostream>
#include <string>
#include <exception>


Simulation::Simulation()
{
	Random::setup();
	setup();
}

void Simulation::setup()
{
	initSettings();
	validateSettings();
	box = new MDBox(*this);
}

void Simulation::initSettings()
{
	SETFileReader reader{};
	std::map<std::string, double> nbrSettings;
	std::map<std::string, std::string> strSettings;
	reader.readFile("settingsfile.set", nbrSettings, strSettings);

	for (auto& nbrSetting : nbrSettings)
	{
		std::string variable = nbrSetting.first;
		double value = nbrSetting.second;
		if (variable.compare("timesteps") == 0)
			timesteps = std::round(value);
		else if (variable.compare("timestepLength") == 0)
			timestepLength = std::round(value);
		else if (variable.compare("saveVisualizationData") == 0)
			saveVisualizationData = value;
		else if (variable.compare("visualizationLogRate") == 0)
			visualizationLogRate = std::round(value);
		else if (variable.compare("measureDataLogRate") == 0)
			measureDataLogRate = std::round(value);
		else if (variable.compare("cutoffDistance") == 0)
			cutoffDistance = value;
		else if (variable.compare("thermostat") == 0)
			thermostat = value;
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
		else
			throw std::runtime_error{"Could not find a match for setting '" + variable + "'"};
	}

	for (auto& strSetting : strSettings)
	{
		std::string variable = strSetting.first;
		std::string value = strSetting.second;
		if (variable.compare("lattice") == 0)
		{
			if (value.compare("FCC") == 0)
				lattice = Lattice::createFCCLattice();
			else
				throw std::runtime_error{"Lattice '" + value + "' does not exist"};
		}
		else if (variable.compare("material") == 0)
		{
			material = Material::TESTMaterial();
		}
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

Simulation::~Simulation()
{
	delete lattice;
	delete box;
	delete material;
}