// Marcus Schmidt
// Created on 10/30/22

#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include <wx/frame.h>
#include <wx/event.h>
#include <string>
#include <filesystem>
#include <vector>

class ImageViewer : public wxFrame
{
public:
	struct ToggledString
	{
		std::string value;
		bool active = false;
	};

	ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path);

private:
	DECLARE_EVENT_TABLE();

	// Event functions
	void OnRefresh(wxCommandEvent &event);
	void OnSortChanged(wxCommandEvent &event);
	void OnDirectoryToggled(wxCommandEvent &event);
	void OnFileTypeToggled(wxCommandEvent &event);

	void GetImages();
	int GetId(int type, int index);
	int GetIndex(int type, int id);
	void PrintActive(std::vector<ToggledString> vector, std::string name);

	// Constant variables
	static const int ID_SORT_NAME = wxID_HIGHEST + 1;
	static const int ID_SORT_DATE = wxID_HIGHEST + 2;
	static const int ID_SORT_RANDOM = wxID_HIGHEST + 3;

	// Settings variables
	std::filesystem::path rootPath;
	int sortMethod = ID_SORT_NAME;

	// Lists of active directories and file types, and sorted files
	std::vector<ToggledString> directories;
	std::vector<ToggledString> fileTypes;
	std::vector<std::string> files;
};

#endif