#ifndef _SET_FILE_READER_
#define _SET_FILE_READER_

#include <string>
#include <map>

/*
	This settings file reader has some flaws atm. We generally have 3 data types that we read:
	int, double and string. To separate the read from the file that has the settings somewhat
	dynamically, we group together the settings in two parts, numberSettings and stringSettings.
	This means that not the exact values might be read in, and in turn some values won't be read
	properly. I do hope this error is small and can be neglected.

*/
class SETFileReader
{
	public:
		SETFileReader() = default;
		~SETFileReader() = default;

		void readFile(const char* file, std::map<std::string, double>& nbrSettings, std::map<std::string, std::string>& strSettings);
};

#endif