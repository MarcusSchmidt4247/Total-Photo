// Marcus Schmidt
// Created on 10/30/22

#include "ImageViewer.h"
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>

ImageViewer::ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path)
			: wxFrame(parent, id, title)
{
	this->path = path;

	//**************
	// Create menu *
	//**************

	// Sub-menu with radio options to choose one sorting method
	wxMenu *sortMenu = new wxMenu();
	sortMenu->Append(ID_SORT_NAME, "Name", wxEmptyString, wxITEM_RADIO);
	sortMenu->Append(ID_SORT_DATE, "Date Created", wxEmptyString, wxITEM_RADIO);
	sortMenu->Append(ID_SORT_RANDOM, "Random", wxEmptyString, wxITEM_RADIO);

	// Top-level menu with sorting options and to open new viewer
	wxMenu *viewerMenu = new wxMenu();
	viewerMenu->Append(wxID_NEW, "New");
	viewerMenu->AppendSubMenu(sortMenu, "Sort By");

	// Create menu bar
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(viewerMenu, "Image Viewer");
	this->SetMenuBar(menuBar);

	//*********************************
	// Create directory control panel *
	//*********************************

	wxPanel *directoryPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
										  wxTAB_TRAVERSAL | wxBORDER_SIMPLE);
	wxBoxSizer *directorySizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *directoryControlSizer = new wxBoxSizer(wxVERTICAL);

	// Put a label of the root directory at the top of the panel
	std::string label = "Directory: ";
	label.append(this->path.filename().string());
	directoryControlSizer->Add(new wxStaticText(directoryPanel, wxID_ANY, label), wxSizerFlags().Border(wxUP | wxDOWN | wxRIGHT, 5));

	/* Add a checkbox for each subdirectory in the panel.
	 * References:
	 * https://stackoverflow.com/questions/612097
	 * https://en.cppreference.com/w/cpp/filesystem */
	for (const auto &entry : std::filesystem::directory_iterator(this->path))
	{
		if (entry.is_directory())
		{
			wxCheckBox *subdirectory = new wxCheckBox(directoryPanel, wxID_ANY, entry.path().filename().string());
			directoryControlSizer->Add(subdirectory, wxSizerFlags().Border(wxUP | wxDOWN | wxRIGHT, 5));
		}
	}

	directorySizer->AddSpacer(5);
	directorySizer->Add(directoryControlSizer, wxSizerFlags());
	directoryPanel->SetSizer(directorySizer);

	//**********************************
	// Create file types control panel *
	//**********************************

	// There should probably be a better way of generating these (custom values?)
	std::string fileTypes[] = { "PNG", "JPEG", "WEBP", "MP4" };

	wxPanel *typesPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
									  wxTAB_TRAVERSAL | wxBORDER_SIMPLE);
	wxBoxSizer *typesSizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *typesControlSizer = new wxBoxSizer(wxVERTICAL);

	// Put label at the top of the panel
	typesControlSizer->Add(new wxStaticText(typesPanel, wxID_ANY, "File Types:"), wxSizerFlags().Border(wxUP | wxDOWN | wxRIGHT, 5));

	// Add a checkbox for each file type
	for (const auto &entry : fileTypes)
		typesControlSizer->Add(new wxCheckBox(typesPanel, wxID_ANY, entry), wxSizerFlags().Border(wxUP | wxDOWN | wxRIGHT, 5));
	
	typesSizer->AddSpacer(5);
	typesSizer->Add(typesControlSizer, wxSizerFlags());
	typesPanel->SetSizer(typesSizer);

	//************************************************
	// Create the image viewer section of the window *
	//************************************************

	wxBoxSizer *imageSizer = new wxBoxSizer(wxHORIZONTAL);
	imageSizer->Add(new wxStaticText(this, wxID_ANY, "Placeholder", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), wxSizerFlags(1).Center());
	imageSizer->SetMinSize(700, 500);

	//****************************
	// Create the overall window *
	//****************************

	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *controlsSizer = new wxBoxSizer(wxVERTICAL);
	controlsSizer->Add(directoryPanel, wxSizerFlags(1).Expand());
	controlsSizer->Add(typesPanel, wxSizerFlags().Expand());

	topSizer->Add(controlsSizer, wxSizerFlags().Expand());
	topSizer->Add(imageSizer, wxSizerFlags(1).Expand());
	this->SetSizerAndFit(topSizer);
	this->Fit();
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