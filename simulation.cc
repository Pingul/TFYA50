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
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>


const std::string getCurrentDateAndTime()
{
	auto timepoint = std::chrono::system_clock::now();
	const std::time_t t = std::chrono::system_clock::to_time_t(timepoint);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&t), "%Y-%m-%d.%X");
	return ss.str();
}

Simulation::Simulation(const char* setFile)
{
	Random::setup();
	params = new SimulationParams{setFile};
	box = new MDBox{ *params };
	//filePrefix = getCurrentDateAndTime() + " | ";
}

std::string Simulation::filePath()
{
	return params->outputDirectory + "/" + filePrefix;
}

void Simulation::setupMeasures()
{
	measures.push_back(new KineticEnergy());
	measures.push_back(new PotentialEnergy());
}

void Simulation::calculateMeasures(double t)
{
	for (auto& measure : measures)
	{
		measure->calculate(t, *params, *box);
	}
}

void Simulation::saveMeasures()
{
	for (auto& measure : measures)
	{
		measure->saveToFile(filePath() + measure->name() + ".mdf");
	}
}


void Simulation::run()
{
	setupMeasures();
	if (params->saveVisualizationData)
		fileIO::VIS::writeSettings(filePath() + visFile, *params);

	// Checking time
	auto start = std::chrono::system_clock::now();

	for (int i = 0; i < params->timesteps; ++i)
	{
		double t = i*params->timestepLength;
		if (i % params->verletListUpdateFrequency == 0)
			box->updateVerletList();
		box->updatePositions();
		box->updateForces(*params->material);
		box->updateVelocities();

		if (i % params->measureDataLogRate == 0)
			calculateMeasures(t);

		if (params->saveVisualizationData && i % params->visualizationLogRate == 0)
			fileIO::VIS::writeSimulationInstant(filePath() + visFile, t, box->atomSnapshot());

		double percentFinished = ((double)(i + 1)/(double)params->timesteps)*100.0;
		int percentFinished_i = (int)percentFinished;

		if (percentFinished_i % 10 == 0 && (percentFinished - percentFinished_i) == 0)
		{
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed{end - start};
			std::cout << "elapsed time: " << elapsed.count() << std::endl;

			std::cout << "completed " << i + 1 << " out of " << params->timesteps << " steps (" << percentFinished << "%)." << std::endl;
		}
	}
	saveMeasures();
}

Simulation::~Simulation()
{
	delete box;
	delete params;
	
}



SimulationParams::SimulationParams(const char* file)
{
	initSettings(file);
	validateSettings();
}

void SimulationParams::initSettings(const char* setFile)
{
	std::map<std::string, double> nbrSettings;
	std::map<std::string, std::string> strSettings;
	fileIO::SET::read(setFile, nbrSettings, strSettings);

	for (auto& strSetting : strSettings)
	{
		std::string variable = strSetting.first;
		std::string value = strSetting.second;
		if (variable.compare("lattice") == 0)
		{
			if (value.compare("fcc") == 0)
				lattice = Lattice::createFCCLattice();
			else
				throw std::runtime_error{ "Lattice '" + value + "' does not exist" };
		}
		else if (variable.compare("material") == 0)
		{
			if (value.compare("argon") == 0)
				material = Material::Argon();
			else
				throw std::runtime_error{ "Material '" + value + "' does not exist" };
		}
		else if (variable.compare("outputDirectory") == 0)
			outputDirectory = value;
		else
			throw std::runtime_error{ "Could not find a match for setting '" + variable + "'" };
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
			throw std::runtime_error{ "Could not find a match for setting '" + variable + "'" };
	}
}

void SimulationParams::validateSettings()
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
		throw std::runtime_error{ "too many initialization errors" };
	}
	else
	{
		std::cout << "simulation initialization OK" << std::endl;
	}
}

SimulationParams::~SimulationParams()
{
	delete material;
	delete lattice;
}