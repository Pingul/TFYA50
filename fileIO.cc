#include "fileIO.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <exception>
#include <sstream>

namespace fileIO
{
	std::string trim(const std::string& str, const std::string& whitespace = " \t")
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
					std::string timestamp{ "t = " + std::to_string(timestamps[i]) + "\n"};
					mdfFile.write(timestamp.c_str(), timestamp.size());
					std::string stringValue{ std::to_string(values[i]) + "\n"};
					mdfFile.write(stringValue.c_str(), stringValue.size());
				}
			}
			else
				throw std::runtime_error{ "Could not create mdf-file '" + file + "'" };
		}
	}
}
