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

enum class SortMethod { NAME = wxID_HIGHEST + 1, DATE = wxID_HIGHEST + 2, RANDOM = wxID_HIGHEST + 3 };
enum class MediaType { IMAGE, VIDEO, UNKNOWN };

struct ToggledString
{
	std::string name;
	bool active;
};

struct Directory : ToggledString
{
	Directory *parent = nullptr;
	wxBoxSizer *subdirectorySizer = nullptr;
	wxCheckBox *activeCheckbox = nullptr;
	wxButton *expandButton = nullptr;
	std::vector<Filter *> filters;
	std::vector<Directory> subdirectories;
	std::string indicesPath;
	bool expanded = false;
};

struct FileType : ToggledString
{
	MediaType type;
};

struct File
{
	std::string name;
	std::string originalName;
	std::string path;
	time_t modifiedTime;
	MediaType type;
};

#endif