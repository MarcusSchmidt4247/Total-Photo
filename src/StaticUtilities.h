// Marcus Schmidt
// Created on 4/16/23

#ifndef STATIC_UTILITIES
#define STATIC_UTILITIES

#include <string>
#include <wx/frame.h>

class StaticUtilities
{
public:
	static std::string GetDirectory(wxFrame *parent, std::string message);
	static std::string StandardizeImageName(std::string name);
};

#endif