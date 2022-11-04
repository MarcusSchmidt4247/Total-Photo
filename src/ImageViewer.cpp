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
	// Set menu bar
	wxMenu *menu = new wxMenu();
	menu->Append(wxID_NEW, "New");
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(menu, "Image Viewer");
	this->SetMenuBar(menuBar);

	this->path = path;

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