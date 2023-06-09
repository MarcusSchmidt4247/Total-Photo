// Marcus Schmidt
// Created on 10/30/22

#ifndef IMAGE_VIEWER
#define IMAGE_VIEWER

#include "Filter.h"
#include <wx/frame.h>
#include <wx/event.h>
#include <string>
#include <filesystem>
#include <vector>
#include <sys/stat.h>
#include <wx/statbmp.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/splitter.h>
#include <wx/scrolwin.h>
#include <wx/colour.h>

class ImageViewer : public wxFrame
{
public:
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
	std::vector<File> RecurseGetImages(std::filesystem::path path, Directory *directory);
	std::vector<File> RecurseGetImages(std::filesystem::path path, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity);
	int GetId(ListType type, int index);
	int GetIndex(ListType type, int id);

	// Print functions
	void PrintActive(std::vector<ToggledString> vector, std::string name);
	void PrintFile(File file);

	// Sort functions
	template <typename T> void SortAlphabetically(std::vector<T> &vector, T element);
	template <typename T> void SortRandomly(std::vector<T> &vector, T element);
	void SortByTime(std::vector<File> &vector, File element);

	// Miscellaneous functions
	void LoadFile(int index);
	void RecurseActivationState(std::vector<Directory> &subdirectories, bool active);
	void AddSubdirectories(wxBoxSizer *sizer, std::vector<Directory> &subdirectories);
	Directory * FindDirectory(std::string path);
	void MergeVectors(std::vector<File> &a, const std::vector<File> &b);
	bool IsValidExtension(std::string extension);
	void AddImage(const std::filesystem::directory_entry &file, std::vector<File> &vector, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity);

	// Constant variables
	static const int ID_SORT_NAME = wxID_HIGHEST + 1;
	static const int ID_SORT_DATE = wxID_HIGHEST + 2;
	static const int ID_SORT_RANDOM = wxID_HIGHEST + 3;
	const wxColour BACKGROUND_LIGHT = wxColour(236, 236, 236);
	const wxColour BACKGROUND_DARK = wxColour(24, 24, 24);

	// Vector variables
	std::vector<Directory> directories;
	std::vector<ToggledString> fileTypes;
	std::vector<File> files;

	// GUI element variables
	wxSplitterWindow *splitter;
	wxPanel *controlPanel;
	wxPanel *imagePanel;
	wxScrolledWindow *directoryPanel;
	wxStaticBitmap *imageBitmap;
	wxBoxSizer *imageSizer;
	wxBoxSizer *topSizer;

	// Setting variables
	std::filesystem::path rootPath;
	std::string applicationDirectory;
	int sortMethod = ID_SORT_NAME;
	bool showImageName = false;
	int imageIndex = 0;
	int defaultPanelWidth;
};

#endif