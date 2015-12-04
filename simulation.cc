#include "simulation.h"
#include "lattice.h"
#include "material.h"
#include "mdbox.h"
#include "random.h"
#include "fileIO.h"
#include "measure.h"
#include "thermostat.h"
#include "threadpool.h"

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

std::vector<std::string> split(const std::string& str, std::vector<std::string>& items, const char delimiter = ' ')
{
	std::stringstream stream{str};
	std::string item;
	while (std::getline(stream, item, delimiter))
	{
		items.push_back(item);
	}
	return items;
}

std::string fileName(std::string filePath)
{
	// Expecting file path of type 'path/to/file.ending'
	std::vector<std::string> items;
	split(filePath, items, '/');
	std::string file{items.back()};
	items.clear();
	split(file, items, '.');
	return items.front();
}

Simulation::Simulation(const char* setFile)
{
	Random::setup();
	params = new SimulationParams{setFile};
	// Threadpool::dispatchOnce(params->threads);
	filePrefix = fileName(setFile) + "_";
	//filePrefix = getCurrentDateAndTime() + " | ";
}

std::string Simulation::filePath(bool withPrefix)
{
	return params->outputDirectory + "/" + (withPrefix ? filePrefix : "");
}

void Simulation::setupMeasures()
{
	params->kineticEnergy = new KineticEnergy();
	params->potentialEnergy = new PotentialEnergy();
	params->totalEnergy = new TotalEnergy(params->kineticEnergy, params->potentialEnergy);
	params->temperature = new Temperature(params->kineticEnergy);
	params->msd = new MSD();
	params->surfaceMSD = new SurfaceMSD();
	params->debyeTemperature = new DebyeTemperature(params->temperature, params->msd);
	params->surfaceDebyeTemperature = new SurfaceDebyeTemperature(params->temperature, params->surfaceMSD);
	params->pressure = new Pressure(params->temperature);
	params->specificHeat = new SpecificHeat(params->temperature);

	// This makes administration somewhat easier
	// Only add the measures you want to calculate independently
	measures.push_back(params->kineticEnergy);
	measures.push_back(params->potentialEnergy);
	measures.push_back(params->totalEnergy);
	measures.push_back(params->temperature);
	measures.push_back(params->msd);
	measures.push_back(params->surfaceMSD); 
	measures.push_back(params->debyeTemperature);
	measures.push_back(params->surfaceDebyeTemperature);
	measures.push_back(params->pressure);
	measures.push_back(params->specificHeat);
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
		try
		{
			measure->saveToFile(filePath() + measure->name() + ".mdf");
		}
		catch (std::exception& e)
		{
			std::cout << "SAVE ERROR: " << e.what() << std::endl;
		}
	}
}

void Simulation::saveMetaData()
{
	try
	{
		std::string fName{ fileName(params->settingsFileName) };
		fileIO::SIM::write(filePath(false), fName, *this);
	}
	catch (std::exception& e)
	{
		std::cout << "SAVE ERROR: " << e.what() << std::endl;
	}
}

void Simulation::run()
{
	// Checking time
	auto start = std::chrono::system_clock::now();
	box = new MDBox{ *params };

	std::cout << "Running simulation with " << box->atomSnapshot().size() << " atoms for " << params->timesteps << " timesteps." << std::endl;//<< params->threads << " threads" << std::endl;
	std::cout << "Start temperature is " << params->initialTemperature << " K";
	if (params->thermostat != nullptr)
		std::cout << " and using a thermostat with " << params->goalTemperature << " K";
	std::cout << std::endl << "Go!" << std::endl;

	setupMeasures();
	if (params->saveVisualizationData)
		fileIO::VIS::writeSettings(filePath() + visFile, *params);

	for (int i = 0; i < params->timesteps; ++i)
	{
		double t = i*params->timestepLength;
		if (i % params->verletListUpdateFrequency == 0)
			box->updateVerletList();
		box->updatePositions();
		box->updateForces(*params->material);
		box->updateVelocities();

		if (i % params->measureDataLogRate == 0)
		{
			calculateMeasures(t);
			// We can only apply the thermostat if we've calculated the temperature, so we do it in conjunction with the measures
			if (params->thermostat != nullptr)
			{
				params->thermostat->scaleTemperature(t, *box);
			}
		}


		if (params->saveVisualizationData && i % params->visualizationLogRate == 0)
			fileIO::VIS::writeSimulationInstant(filePath() + visFile, t, box->atomSnapshot());

		double percentFinished = ((double)(i + 1)/(double)params->timesteps)*100.0;
		int percentFinished_i = (int)percentFinished;

		if (percentFinished_i % 10 == 0 && (percentFinished - percentFinished_i) == 0)
		{
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed{end - start};
			int seconds = (int)elapsed.count();
			double decimals = elapsed.count() - seconds;
			int hours = seconds/3600;
			int minutes = seconds/60;
			seconds -= (3600*hours + 60*minutes);
			std::ios::fmtflags fl(std::cout.flags());
			std::cout << "elapsed time: " 
				<< std::setfill('0') << std::setw(1) << hours << ":" 
				<< std::setfill('0') << std::setw(1) << minutes << ":" 
				<< std::fixed << std::setprecision(4) << (seconds + decimals) << std::endl;
			std::cout.flags(fl); // Restore the flags
			std::cout << "completed " << i + 1 << " out of " << params->timesteps << " steps (" << percentFinished_i << "%)." << std::endl;
		}
	}
	saveMeasures();
	saveMetaData();
}

Simulation::~Simulation()
{
	delete box;
	delete params;	
}



SimulationParams::SimulationParams(const char* file)
{
	settingsFileName = file;
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
			{
				throw std::runtime_error{ "Lattice '" + value + "' does not exist" };
			}
		}
		else if (variable.compare("material") == 0)
		{
			if (value.compare("argon") == 0)
				material = Material::Argon();
			else if (value.compare("silver") == 0)
				material = Material::Silver();
			else if (value.compare("helium") == 0)
				material = Material::Helium();
			else if (value.compare("neon") == 0)
				material = Material::Neon();
			else if (value.compare("krypton") == 0)
				material = Material::Krypton();
			else if (value.compare("xenon") == 0)
				material = Material::Xenon();
			else if (value.compare("nickel") == 0)
				material = Material::Nickel();
			else if (value.compare("rhodium") == 0)
				material = Material::Rhodium();
			else
				throw std::runtime_error{ "Material '" + value + "' does not exist" };
		}
		else if (variable.compare("thermostat") == 0)
		{
			if (value.compare("berendsen") == 0)
				thermostat = new BerendsenThermostat{*this}; // Note that 'this' is not fully populated at this state. If the initialization does something funky here (as accessing certain SimulationParams values), it might break
			else
				throw std::runtime_error{ "Thermostat of type '" + value + "' does not exist"};
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
		else if (variable.compare("verletListUpdateFrequency") == 0)
			verletListUpdateFrequency = (int)std::round(value);
		else if (variable.compare("saveVisualizationData") == 0)
			saveVisualizationData = (bool)value;
		else if (variable.compare("visualizationLogRate") == 0)
			visualizationLogRate = (int)std::round(value);
		else if (variable.compare("measureDataLogRate") == 0)
			measureDataLogRate = (int)std::round(value);
		else if (variable.compare("cutoffDistance") == 0)
			cutoffDistance = value;
		else if (variable.compare("goalTemperature") == 0)
			goalTemperature = value;
		else if (variable.compare("initialTemperature") == 0)
			initialTemperature = value;
		else if (variable.compare("pBCx") == 0)
			pBCx = (bool)value;
		else if (variable.compare("pBCy") == 0)
			pBCy = (bool)value;
		else if (variable.compare("pBCz") == 0)
			pBCz = (bool)value;
		else if (variable.compare("dimensions.x") == 0)
			dimensions.x = std::round(value);
		else if (variable.compare("dimensions.y") == 0)
			dimensions.y = std::round(value);
		else if (variable.compare("dimensions.z") == 0)
			dimensions.z = std::round(value);
		else if (variable.compare("latticeConstant") == 0)
			lattice->latticeConstant = value; // Dependent on the lattice that we created earlier
		else if (variable.compare("thermostatScaling") == 0)
			thermostatScaling = value;
		else if (variable.compare("epsilon") == 0)
		{
			// Special case for LJ potential
			if (material == nullptr || material->potential == nullptr)
				throw std::runtime_error{"No material specified for 'epislon'"};

			LJPotential* pot = dynamic_cast<LJPotential*>(material->potential);
			if (pot != nullptr)
				pot->epsilon = value;
		}
		else if (variable.compare("sigma") == 0)
		{
			if (material == nullptr || material->potential == nullptr)
				throw std::runtime_error{"No material specified for 'sigma'"};

			LJPotential* pot = dynamic_cast<LJPotential*>(material->potential);
			if (pot != nullptr)
				pot->sigma = value;
		}
		else if (variable.compare("threads") == 0)
			threads = (int)std::round(value);
		else if (variable.compare("calculateMeanAfter") == 0)
			calculateMeanAfter = (int)std::round(value);
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
	if (calculateMeanAfter > timesteps || calculateMeanAfter < 0)
	{
		errors++;
		variables.push_back("calculateMeanAfter");	
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
	delete kineticEnergy;
	delete potentialEnergy;
	delete totalEnergy;
	delete temperature;
	delete msd;
	delete debyeTemperature;
	delete thermostat;
}