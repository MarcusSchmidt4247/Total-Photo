// Marcus Schmidt
// Created on 6/20/23

#ifndef CUSTOM_TYPES
#define CUSTOM_TYPES

#include "Filter.h"
#include <string>
#include <vector>
#include <sys/stat.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/checkbox.h>

struct ToggledString
{
	std::string name;
	bool active = false;
};

struct Directory : ToggledString
{
	Directory *parent = nullptr;
	Filter *filter = nullptr;
	wxBoxSizer *subdirectorySizer = nullptr;
	wxCheckBox *activeCheckbox = nullptr;
	wxButton *expandButton = nullptr;
	std::vector<Directory> subdirectories;
	std::string indicesPath;
	bool expanded = false;
};

struct File
{
	std::string name;
	std::string originalName;
	std::string path;
	time_t modifiedTime;
};

enum class SortMethod { NAME = wxID_HIGHEST + 1, DATE = wxID_HIGHEST + 2, RANDOM = wxID_HIGHEST + 3 };

#endif