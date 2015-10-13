#ifndef _FILE_IO_
#define _FILE_IO_

#include <map>

namespace fileIO
{
	namespace SET
	{
		void read(const char* file, std::map<std::string, double>& nbrSettings, std::map<std::string, std::string>& strSettings);
	}

	namespace MDF
	{
		void write(const std::string&, const std::vector<double>&, const std::vector<double>&);
	}
}

#endif