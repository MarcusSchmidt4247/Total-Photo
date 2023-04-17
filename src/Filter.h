// Marcus Schmidt
// Created on 4/16/23

#ifndef FILTER
#define FILTER

#include "StaticUtilities.h"
#include <wx/string.h>
#include <unordered_map>
#include <filesystem>
#include <string>
#include <iostream>

class Filter
{
public:
	enum Type { NONE, INCLUDE, EXCLUDE };

	Filter()
	{
		type = NONE;
		path = "";
	}

	void SetFilter(Type _type, std::string _path)
	{
		type = _type;
		path = _path;
	}

	Type GetType() { return type; }
	std::string GetPath() { return path; }
	std::unordered_map<std::string, int> GetFilterItems()
	{
		std::unordered_map<std::string, int> items;

		// Go to path and collect image names
		if (std::filesystem::is_directory(path))
		{
			// Then iterate through every file in the directory
			for (const auto &entry : std::filesystem::directory_iterator(path))
			{
				if (entry.is_regular_file())
					items[StaticUtilities::StandardizeImageName(entry.path().filename().string())] = 1;
			}
		}
		else
			std::cout << "GetFilterItems(): Directory \"" << path << "\" does not exist" << std::endl;

		return items;
	}

private:
	Type type;
	std::string path;
};

#endif