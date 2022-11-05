// Marcus Schmidt
// Created on 10/30/22

#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include <wx/frame.h>
#include <wx/event.h>
#include <string>
#include <filesystem>
#include <vector>
#include <sys/stat.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>

class ImageViewer : public wxFrame
{
public:
	struct ToggledString
	{
		std::string value;
		bool active = false;
	};

	struct File
	{
		std::string name;
		std::string path;
		time_t modifiedTime;
	};

	ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path);

private:
	DECLARE_EVENT_TABLE();

	// Event functions
	void OnRefresh(wxCommandEvent &event);
	void OnSortChanged(wxCommandEvent &event);
	void OnDirectoryToggled(wxCommandEvent &event);
	void OnFileTypeToggled(wxCommandEvent &event);
	void OnKeyPress(wxKeyEvent &event);

	void LoadFile(std::string path);

	// Getters
	void GetImages();
	int GetId(int type, int index);
	int GetIndex(int type, int id);

	// Print functions
	void PrintActive(std::vector<ToggledString> vector, std::string name);
	void PrintFile(File file);

	// Constant variables
	static const int ID_SORT_NAME = wxID_HIGHEST + 1;
	static const int ID_SORT_DATE = wxID_HIGHEST + 2;
	static const int ID_SORT_RANDOM = wxID_HIGHEST + 3;

	// Vector variables
	std::vector<ToggledString> directories;
	std::vector<ToggledString> fileTypes;
	std::vector<File> files;

	// Miscellaneous variables
	wxStaticBitmap *imageBitmap;
	wxBoxSizer *imageSizer;
	std::filesystem::path rootPath;
	int sortMethod = ID_SORT_NAME;
	int imageIndex = 0;
};

#endif