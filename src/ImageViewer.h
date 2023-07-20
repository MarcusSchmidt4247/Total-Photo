// Marcus Schmidt
// Created on 10/30/22

#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include "Filter.h"
#include "CustomTypes.h"
#include <string>
#include <vector>
#include <filesystem>
#include <wx/frame.h>
#include <wx/event.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/colour.h>
#include <wx/mediactrl.h>

class ImageViewer : public wxFrame
{
public:
	ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path);

private:
	DECLARE_EVENT_TABLE();

	enum class ListType { DIRECTORIES = 0, FILE_TYPES = 1, BUTTONS = 2 };

	// Event functions
	void OnRefresh(wxCommandEvent &event);
	void OnToggleName(wxCommandEvent &event);
	void OnFirstFile(wxCommandEvent &event);
	void OnToggleSplit(wxCommandEvent &event);
	void OnToggleBackground(wxCommandEvent &event);
	void OnSortChanged(wxCommandEvent &event);
	void OnDirectoryToggled(wxCommandEvent &event);
	void OnDirectoryExpanded(wxCommandEvent &event);
	void OnDirectoryOverflow(wxCommandEvent &event);
	void OnFileTypeToggled(wxCommandEvent &event);
	void OnKeyPress(wxKeyEvent &event);

	// Getters
	std::vector<Directory> GetSubdirectories(Directory *directory);
	void GetImages();
	int GetId(ListType type, int index);
	int GetIndex(ListType type, int id);

	// Print functions
	void PrintActive(std::vector<ToggledString> vector, std::string name);
	void PrintFile(File file);

	// Miscellaneous functions
	void LoadFile(int index);
	void RecurseActivationState(std::vector<Directory> &subdirectories, bool active);
	void AddSubdirectories(wxBoxSizer *sizer, std::vector<Directory> &subdirectories);
	Directory * FindDirectory(std::string path);

	// Constant variables
	const wxColour BACKGROUND_LIGHT = wxColour(236, 236, 236);
	const wxColour BACKGROUND_DARK = wxColour(24, 24, 24);

	// Vector variables
	std::vector<Directory> directories;
	std::vector<FileType> fileTypes;
	std::vector<File> files;

	// GUI element variables
	wxSplitterWindow *splitter;
	wxPanel *controlPanel;
	wxPanel *imagePanel;
	wxScrolledWindow *directoryPanel;
	wxStaticBitmap *imageBitmap;
	wxMediaCtrl *mediaCtrl;
	wxBoxSizer *imageSizer;
	wxBoxSizer *topSizer;

	// Setting variables
	std::filesystem::path rootPath;
	std::string applicationDirectory;
	SortMethod sortMethod = SortMethod::NAME;
	bool showImageName = false;
	int imageIndex = 0;
	int defaultPanelWidth;
};

#endif