#include "fileIO.h"
#include "simulation.h"
#include "measure.h"
#include "lattice.h"
#include "atom.h"
#include "mdbox.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <exception>
#include <sstream>
#include <cctype>

namespace fileIO
{
	std::string trim(const std::string& str, const std::string& whitespace = " \t\n\v\f\r")
	{
		size_t strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return "";

		size_t strEnd = str.find_last_not_of(whitespace);
		if (strEnd == std::string::npos)
			return "";

		return str.substr(strBegin, strEnd - strBegin + 1);
	}

	std::string strip(const std::string& str, const std::string& comment = "#")
	{
		size_t pos = str.find_first_of(comment);
		return str.substr(0, pos);
	}

	// A statement is on the form 'variable = value'
	bool isStatement(const std::string& str)
	{
		std::regex pattern{ "[A-Za-z0-9_]+?([ \t]*)=" };
		return std::regex_search(str, pattern);
	}

	std::string statementVariable(const std::string& statement)
	{
		if (!isStatement(statement))
			throw std::runtime_error{ "Expected statement" };

		std::string variable{ statement.substr(0, statement.find('=')) };
		return trim(variable);
	}

	void extractStatement(const std::string& statement, std::map<std::string, double>& nbrSettings, std::map<std::string, std::string>& strSettings)
	{
		if (!isStatement(statement))
			throw std::runtime_error{ "Expected statement" };

		std::string variable{ statementVariable(statement) };
		std::string strValue{ statement.substr(statement.find('=') + 1, std::string::npos) };
		double value;
		try
		{
			value = std::stod(strValue);
			nbrSettings.insert(std::pair<std::string, double>{variable, value});
		}
		catch (...)
		{
			strSettings.insert(std::pair<std::string, std::string>{variable, trim(strValue)});
		}
	}

	//
	// SET namespace
	// Settings file
	//
	namespace SET
	{
		void read(const char* file, std::map<std::string, double>& nbrSettings, std::map<std::string, std::string>& strSettings)
		{
			std::string line;
			std::ifstream simFile{ file };
			if (simFile.is_open())
			{
				while (getline(simFile, line))
				{
					std::string trimmedLine{ trim(strip(line)) };

					if (trimmedLine.empty())
						continue;

					if (isStatement(trimmedLine))
						extractStatement(trimmedLine, nbrSettings, strSettings);
					else
						throw std::runtime_error{ "Could not parse '" + line + "'" };
				}
				simFile.close();
			}
			else
				throw std::runtime_error{ "Could not open settings file" };
		}
	}


	// 
	// MDF
	// Measure Data File
	//
	namespace MDF
	{
		void write(const std::string& file, const std::vector<double>& timestamps, const std::vector<double>& values)
		{
			if (timestamps.size() != values.size())
				throw std::runtime_error{ "'timestamps' and 'values' not of same size" };

			std::ofstream mdfFile{ file };
			if (mdfFile.is_open())
			{
				for (int i = 0; i < timestamps.size(); i++)
				{
					std::string timestamp{ std::to_string(timestamps[i]) + "\t"};
					mdfFile.write(timestamp.c_str(), timestamp.size());
					std::string stringValue{ std::to_string(values[i]) + "\n"};
					mdfFile.write(stringValue.c_str(), stringValue.size());
				}
			}
			else
				throw std::runtime_error{ "Could not create mdf-file '" + file + "'" };
		}
	}

	//
	// VIS
	// Visualization file
	//
	namespace VIS
	{
		void writeSettings(const std::string& file, const SimulationParams& params)
		{
			std::ofstream visFile{ file }; // will overwrite
			if (visFile.is_open())
			{
				std::string timeDelta{ "timeDelta = " + std::to_string(params.timestepLength) + "\n" };
				visFile.write(timeDelta.c_str(), timeDelta.size());

				std::string xDim{ "dimensions.x = " + std::to_string(params.dimensions.x*params.lattice->latticeConstant) + "\n" };
				std::string yDim{ "dimensions.y = " + std::to_string(params.dimensions.y*params.lattice->latticeConstant) + "\n" };
				std::string zDim{ "dimensions.z = " + std::to_string(params.dimensions.z*params.lattice->latticeConstant) + "\n" };
				visFile.write(xDim.c_str(), xDim.size());
				visFile.write(yDim.c_str(), yDim.size());
				visFile.write(zDim.c_str(), zDim.size());
			}
			else
				throw std::runtime_error{"Could not write settings to '" + file + "'"};
		}

		void writeSimulationInstant(const std::string& file, double t, const std::vector<Atom*>& atoms)
		{
			std::ofstream visFile{ file, std::ofstream::app };
			if (visFile.is_open())
			{
				std::string tString{ "t = " + std::to_string(t) + "\n" };
				visFile.write(tString.c_str(), tString.size());
				for (auto& atom : atoms)
				{
					Vector3&& pos{ atom->at() };
					std::string posString{ std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z) + "\n" };
					visFile.write(posString.c_str(), posString.size());
				}
			}
			else
				throw std::runtime_error{ "Could not write the simulation instant to '" + file + "'" };
		}
	}

	//
	// SIM
	// Simulation meta data
	namespace SIM
	{
		void write(const std::string& path, const std::string& file, const Simulation& sim)
		{
			std::ofstream simFile{path + "/" + file + ".sim"};
			if (simFile.is_open())
			{
				// Measures
				std::string m{"AVERAGES"};
				simFile.write(m.c_str(), m.size());
				int start = sim.params->calculateMeanAfter;
				int end = sim.params->timesteps;
				for (auto& measure : sim.measures)
				{	
					std::string s{"\n" + measure->name() + " : " + std::to_string(measure->average(start, end))};
					simFile.write(s.c_str(), s.size());
				}
				m = {"\n\nMEASURE FILES"};
				simFile.write(m.c_str(), m.size());
				for (auto& measure : sim.measures)
				{
					std::string s{"\n" + sim.filePrefix + measure->name() + ".mdf"};
					simFile.write(s.c_str(), s.size());
				}
			}
			else 
				throw std::runtime_error{ "Could not write meta data to '" + file + "'" };
		}
	}
}
