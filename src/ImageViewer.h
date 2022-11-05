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
#include <wx/splitter.h>
#include <wx/colour.h>

class ImageViewer : public wxFrame
{
public:
	struct ToggledString
	{
		std::string name;
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
	void OnToggleName(wxCommandEvent &event);
	void OnFirstFile(wxCommandEvent &event);
	void OnToggleSplit(wxCommandEvent &event);
	void OnToggleBackground(wxCommandEvent &event);
	void OnSortChanged(wxCommandEvent &event);
	void OnDirectoryToggled(wxCommandEvent &event);
	void OnFileTypeToggled(wxCommandEvent &event);
	void OnKeyPress(wxKeyEvent &event);

	void LoadFile(int index);

	// Getters
	void GetImages();
	int GetId(int type, int index);
	int GetIndex(int type, int id);

	// Print functions
	void PrintActive(std::vector<ToggledString> vector, std::string name);
	void PrintFile(File file);

	// Sort functions
	template <typename T> void SortAlphabetically(std::vector<T> &vector, T element);
	template <typename T> void SortRandomly(std::vector<T> &vector, T element);
	void SortByTime(std::vector<File> &vector, File element);

	// Constant variables
	static const int ID_SORT_NAME = wxID_HIGHEST + 1;
	static const int ID_SORT_DATE = wxID_HIGHEST + 2;
	static const int ID_SORT_RANDOM = wxID_HIGHEST + 3;
	const wxColour BACKGROUND_LIGHT = wxColour(236, 236, 236);
	const wxColour BACKGROUND_DARK = wxColour(24, 24, 24);

	// Vector variables
	std::vector<ToggledString> directories;
	std::vector<ToggledString> fileTypes;
	std::vector<File> files;

	// GUI element variables
	wxSplitterWindow *splitter;
	wxPanel *controlPanel;
	wxPanel *imagePanel;
	wxStaticBitmap *imageBitmap;
	wxBoxSizer *imageSizer;

	// Setting variables
	std::filesystem::path rootPath;
	int sortMethod = ID_SORT_NAME;
	bool showImageName = false;
	int imageIndex = 0;
	int defaultPanelWidth;
};

#endif