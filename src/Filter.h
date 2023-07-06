// Marcus Schmidt
// Created on 4/16/23

#ifndef FILTER
#define FILTER

#include <wx/string.h>
#include <unordered_map>
#include <filesystem>
#include <string>

class Filter
{
public:
	enum Type { NONE, INCLUDE, EXCLUDE };

	Filter();
	void SetFilter(Type _type, std::string _path);
	Filter * GetCopy();

	Type GetType() { return type; }
	std::string GetPath() { return path; }
	std::unordered_map<std::string, int> GetFilterItems();

private:
	Type type;
	std::string path;
};

#endif