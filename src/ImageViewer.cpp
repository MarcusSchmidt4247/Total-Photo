// Marcus Schmidt
// Created on 10/30/22

#include "ImageViewer.h"
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <sys/stat.h>
#include <time.h>
#include <iostream>

ImageViewer::ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path)
			: wxFrame(parent, id, title)
{
	rootPath = path;

	// Add the root directory to the list as an always-active source for files
	ToggledString rootDir = { "", true };
	directories.push_back(rootDir);

	//**************
	// Create menu *
	//**************

	// Sub-menu with radio options to choose one sorting method
	wxMenu *sortMenu = new wxMenu();
	sortMenu->Append(ID_SORT_NAME, "Name", wxEmptyString, wxITEM_RADIO);
	sortMenu->Append(ID_SORT_DATE, "Date Modified", wxEmptyString, wxITEM_RADIO);
	sortMenu->Append(ID_SORT_RANDOM, "Random", wxEmptyString, wxITEM_RADIO);

	// Top-level menu with sorting options and to open new viewer
	wxMenu *viewerMenu = new wxMenu();
	viewerMenu->Append(wxID_NEW, "New");
	viewerMenu->Append(wxID_REFRESH, "Refresh");
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
	wxBoxSizer *directorySizer = new wxBoxSizer(wxVERTICAL);

	// Put a label of the root directory at the top of the panel
	directorySizer->Add(new wxStaticText(directoryPanel, wxID_ANY, rootPath.filename().string()),
							   wxSizerFlags().Border(wxALL, 5));

	/* Add a checkbox for each subdirectory in the panel.
	 * References:
	 * https://stackoverflow.com/questions/612097
	 * https://en.cppreference.com/w/cpp/filesystem */
	for (const auto &entry : std::filesystem::directory_iterator(rootPath))
	{
		if (entry.is_directory())
		{
			ToggledString directory = { entry.path().filename().string() };
			directories.push_back(directory);

			int id = GetId(0, (int) directories.size() - 1);
			wxCheckBox *subdirectory = new wxCheckBox(directoryPanel, id, entry.path().filename().string());
			directorySizer->Add(subdirectory, wxSizerFlags().Border(wxALL, 5));
			Bind(wxEVT_CHECKBOX, &ImageViewer::OnDirectoryToggled, this, id);
		}
	}

	directoryPanel->SetSizer(directorySizer);

	//**********************************
	// Create file types control panel *
	//**********************************
	wxPanel *typesPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
									  wxTAB_TRAVERSAL | wxBORDER_SIMPLE);
	wxBoxSizer *typesSizer = new wxBoxSizer(wxVERTICAL);

	// Put label at the top of the panel
	typesSizer->Add(new wxStaticText(typesPanel, wxID_ANY, "File Types:"), wxSizerFlags().Border(wxALL, 5));

	//******************************************************************
	// This should be reworked to be more customizable and programatic *
	//******************************************************************
	std::string temp[] = { ".png", ".jpg", ".webp", ".mp4" };

	// Add a checkbox for each file type
	for (const auto &entry : temp)
	{
		ToggledString type = { entry };
		fileTypes.push_back(type);

		int id = GetId(1, (int) fileTypes.size() - 1);
		typesSizer->Add(new wxCheckBox(typesPanel, id, entry), wxSizerFlags().Border(wxALL, 5));
		Bind(wxEVT_CHECKBOX, &ImageViewer::OnFileTypeToggled, this, id);
	}
	
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

void ImageViewer::OnRefresh(wxCommandEvent &event)
{
	GetImages();
}

void ImageViewer::OnSortChanged(wxCommandEvent &event)
{
	sortMethod = event.GetId();

	//*********************************************************************
	// To-do: Re-sort the list of images according to the new sort method *
	//*********************************************************************
}

void ImageViewer::OnDirectoryToggled(wxCommandEvent &event)
{
	int index = GetIndex(0, event.GetId());
	directories[index].active = !directories[index].active;
}

void ImageViewer::OnFileTypeToggled(wxCommandEvent &event)
{
	int index = GetIndex(1, event.GetId());
	fileTypes[index].active = !fileTypes[index].active;
}

void ImageViewer::GetImages()
{
	files.clear();

	// For every directory...
	for (ToggledString directory : directories)
	{
		// Check if it's active...
		if (directory.active)
		{
			// And that it still exists on the computer...
			std::filesystem::path path = rootPath.string() + "/" + directory.value;
			if (std::filesystem::is_directory(path))
			{
				// Then iterate through every file in the directory
				for (const auto &entry : std::filesystem::directory_iterator(path))
				{
					if (entry.is_regular_file())
					{
						// Compare the file's extension against the active extensions
						std::string extension = entry.path().extension().string();
						for (auto type : fileTypes)
						{
							if (type.active && extension.compare(type.value) == 0)
							{
								// Obtain the relative path to this file from the root directory
								int rootLength = rootPath.string().length() + 1; // +1 accounts for the /
								std::string relativePath = entry.path().string().substr(rootLength, entry.path().string().length() - rootLength);

								files.push_back(relativePath);
							}
						}
					}
				}

				/*
				// Evaluate each file according to the sort method
				if (sortMethod == ID_SORT_DATE)
				{
					// Struct documentation: https://man7.org/linux/man-pages/man0/sys_stat.h.0p.html
					struct stat info;
					// Function documentation: https://pubs.opengroup.org/onlinepubs/007908799/xsh/stat.html
					if (stat("name.extension", &info) == 0)
					{
						struct tm *timeInfo = localtime(&info.st_mtime);
						// asctime(timeInfo)
					}
					else
					{
						// Error!
					}
				}
				*/
			}
			else
				std::cout << "Directory \"" << path.string() << "\" does not exist" << std::endl;
		}
	}
}

int ImageViewer::GetId(int type, int index)
{
	//*************************************
	// Check for possibility of overflow! *
	//*************************************
	return wxID_HIGHEST + 100 + (100 * type) + index;
}

int ImageViewer::GetIndex(int type, int id)
{
	//**************************************
	// Check for possibility of underflow! *
	//**************************************
	return id - wxID_HIGHEST - 100 - (100 * type);
}

void ImageViewer::PrintActive(std::vector<ImageViewer::ToggledString> vector, std::string name)
{
	std::cout << "\"" << name << "\" active elements:" << std::endl;
	for (auto &element : vector)
	{
		if (element.active)
		{
			std::cout << "-- " << element.value << std::endl;
		}
	}
	std::cout << std::endl;
}

// To draw images (official sample, relevant classes):
// https://github.com/wxWidgets/wxWidgets/blob/master/samples/image/canvas.cpp
// and
// https://github.com/wxWidgets/wxWidgets/blob/master/samples/image/image.cpp

// Don't catch wxID_NEW because the event will rise to TotalPhoto.cpp
BEGIN_EVENT_TABLE(ImageViewer, wxFrame)
EVT_MENU(wxID_REFRESH, ImageViewer::OnRefresh)
EVT_MENU(ID_SORT_NAME, ImageViewer::OnSortChanged)
EVT_MENU(ID_SORT_DATE, ImageViewer::OnSortChanged)
EVT_MENU(ID_SORT_RANDOM, ImageViewer::OnSortChanged)
END_EVENT_TABLE()