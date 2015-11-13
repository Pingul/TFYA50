#ifndef _FILE_IO_
#define _FILE_IO_

#include <map>
#include <vector>

class Atom;
class SimulationParams;
class Simulation;

namespace fileIO
{
	namespace SET
	{
		void read(const char* file, std::map<std::string, double>& nbrSettings, std::map<std::string, std::string>& strSettings);
	}

	namespace MDF
	{
		// Will overwrite old file
		void write(const std::string& file, const std::vector<double>& timestamps, const std::vector<double>& values);
	}

	namespace VIS
	{
		void writeSettings(const std::string& file, const SimulationParams&);
		void writeSimulationInstant(const std::string& file, double t, const std::vector<Atom*>& atoms);
	}

	namespace SIM
	{
		void write(const std::string& path, const std::string& file, const Simulation& sim);
	}
}

#endif