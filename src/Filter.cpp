// Marcus Schmidt
// Created on 6/20/23

#include "Filter.h"
#include "StaticUtilities.h"

Filter::Filter()
{
	type = NONE;
	path = "";
}

void Filter::SetFilter(Type _type, std::string _path)
{
	type = _type;
	path = _path;
}

Filter * Filter::GetCopy()
{
	Filter *copy = new Filter();
	copy->SetFilter(type, path);
	return copy;
}

std::unordered_map<std::string, int> Filter::GetFilterItems()
{
	std::unordered_map<std::string, int> items;

	std::vector<std::string> images = StaticUtilities::RecurseGetImages((std::filesystem::path) path);
	for (const auto &entry : images)
		items[StaticUtilities::StandardizeImageName(entry)] = 1;

	return items;
}