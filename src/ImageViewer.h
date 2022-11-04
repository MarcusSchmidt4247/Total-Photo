// Marcus Schmidt
// Created on 10/30/22

#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include <wx/frame.h>
#include <string>

class ImageViewer : public wxFrame
{
public:
	ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path);

private:
	std::string path;
};

#endif