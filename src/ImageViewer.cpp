// Marcus Schmidt
// Created on 10/30/22

#include "ImageViewer.h"
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <filesystem>

ImageViewer::ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path)
			: wxFrame(parent, id, title)
{
	this->path = path;

	// Sub-menu with radio options to choose one sorting method
	wxMenu *sortMenu = new wxMenu();
	sortMenu->Append(ID_SORT_NAME, "Name", wxEmptyString, wxITEM_RADIO);
	sortMenu->Append(ID_SORT_DATE, "Date Created", wxEmptyString, wxITEM_RADIO);
	sortMenu->Append(ID_SORT_RANDOM, "Random", wxEmptyString, wxITEM_RADIO);

	// Top-level menu with sorting options and to open new viewer
	wxMenu *viewerMenu = new wxMenu();
	viewerMenu->AppendSubMenu(sortMenu, "Sort By");
	viewerMenu->Append(wxID_NEW, "New");

	// Create menu bar
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(viewerMenu, "Image Viewer");
	this->SetMenuBar(menuBar);

	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);

	/* References:
	 * https://stackoverflow.com/questions/612097
	 * https://en.cppreference.com/w/cpp/filesystem */
	wxPanel *directoryPanel = new wxPanel(this);
	wxBoxSizer *directorySizer = new wxBoxSizer(wxVERTICAL);
	directorySizer->Add(new wxStaticText(directoryPanel, wxID_ANY, path), wxSizerFlags());
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			wxCheckBox *subdirectory = new wxCheckBox(directoryPanel, wxID_ANY, entry.path().filename().string());
			directorySizer->Add(subdirectory, wxSizerFlags());
		}
	}
	directoryPanel->SetSizer(directorySizer);

	topSizer->Add(directoryPanel, wxSizerFlags());
	SetSizer(topSizer);
}

void ImageViewer::OnSortChanged(wxCommandEvent &event)
{
	sortMethod = event.GetId();

	//********************************************************************
	// To-do: Resort the list of images according to the new sort method *
	//********************************************************************
}

// Don't catch wxID_NEW because the event will rise to TotalPhoto.cpp
BEGIN_EVENT_TABLE(ImageViewer, wxFrame)
EVT_MENU(ID_SORT_NAME, ImageViewer::OnSortChanged)
EVT_MENU(ID_SORT_DATE, ImageViewer::OnSortChanged)
EVT_MENU(ID_SORT_RANDOM, ImageViewer::OnSortChanged)
END_EVENT_TABLE()