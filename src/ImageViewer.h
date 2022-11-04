// Marcus Schmidt
// Created on 10/30/22

#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include <wx/frame.h>
#include <wx/event.h>
#include <string>

class ImageViewer : public wxFrame
{
public:
	ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path);

private:
	DECLARE_EVENT_TABLE();
	void OnSortChanged(wxCommandEvent &event);

	static const int ID_SORT_NAME = wxID_HIGHEST + 1;
	static const int ID_SORT_DATE = wxID_HIGHEST + 2;
	static const int ID_SORT_RANDOM = wxID_HIGHEST + 3;

	std::string path;
	int sortMethod = ID_SORT_NAME;
};

#endif