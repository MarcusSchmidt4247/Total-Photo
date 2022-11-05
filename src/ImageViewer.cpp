// Marcus Schmidt
// Created on 10/30/22

#include "ImageViewer.h"
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/checkbox.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/scrolwin.h>
#include <cstdlib>
#include <cctype>
#include <iostream>

ImageViewer::ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path)
			: wxFrame(parent, id, title, wxDefaultPosition, wxSize(700, 500))
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

	wxMenu *controlsMenu = new wxMenu();
	controlsMenu->Append(wxID_NEW, "Open Another Viewer");
	controlsMenu->Append(wxID_REFRESH, "Refresh Viewer");
	controlsMenu->Append(wxID_FIRST, "Jump to First Image");

	wxMenu *viewMenu = new wxMenu();
	wxMenuItem *nameItem = new wxMenuItem(viewMenu, wxID_PRINT, "Image Name", wxEmptyString, wxITEM_CHECK);
	nameItem->Check(showImageName);
	viewMenu->Append(nameItem);
	wxMenuItem *panelItem = new wxMenuItem(viewMenu, wxID_SETUP, "Control Panels", wxEmptyString, wxITEM_CHECK);
	panelItem->Check();
	viewMenu->Append(panelItem);
	wxMenuItem *backgroundItem = new wxMenuItem(viewMenu, wxID_SELECT_COLOR, "Dark Background", wxEmptyString, wxITEM_CHECK);
	//backgroundItem->Check();
	viewMenu->Append(backgroundItem);

	// Top-level menu with sorting options and to open new viewer
	wxMenu *viewerMenu = new wxMenu();
	viewerMenu->AppendSubMenu(controlsMenu, "Actions");
	viewerMenu->AppendSubMenu(sortMenu, "Sort By");
	viewerMenu->AppendSubMenu(viewMenu, "View");

	// Create menu bar
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(viewerMenu, "Image Viewer");
	this->SetMenuBar(menuBar);

	//*************************************
	// Set up main split window component *
	//*************************************

	splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_THIN_SASH);
	splitter->SetSashGravity(0);
	splitter->SetMinimumPaneSize(50);
	splitter->Bind(wxEVT_KEY_DOWN, &ImageViewer::OnKeyPress, this);

	//***************************************************
	// Create control panel and its directory sub-panel *
	//***************************************************

	controlPanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	wxBoxSizer *controlSizer = new wxBoxSizer(wxVERTICAL);

	wxScrolledWindow *directoryPanel = new wxScrolledWindow(controlPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
										  wxTAB_TRAVERSAL | wxBORDER_SIMPLE);
	directoryPanel->SetScrollRate(0, 15);
	wxBoxSizer *directorySizer = new wxBoxSizer(wxVERTICAL);

	// Put a label of the root directory at the top of the panel
	directorySizer->Add(new wxStaticText(directoryPanel, wxID_ANY, rootPath.filename().string()),
							   wxSizerFlags().Border(wxALL, 5));

	/* Alphabetically sort each discovered directory into the list.
	 * References:
	 * https://stackoverflow.com/questions/612097
	 * https://en.cppreference.com/w/cpp/filesystem */
	for (const auto &entry : std::filesystem::directory_iterator(rootPath))
	{
		if (entry.is_directory())
		{
			ToggledString directory = { entry.path().filename().string() };
			SortAlphabetically(directories, directory);
		}
	}

	// Add a checkbox for each sorted subdirectory in the panel (start at 1 to skip the invisible root directory)
	for (int i = 1; i < directories.size(); i++)
	{
		int id = GetId(0, i);
		wxCheckBox *subdirectory = new wxCheckBox(directoryPanel, id, directories[i].name);
		directorySizer->Add(subdirectory, wxSizerFlags().Border(wxALL, 5));
		Bind(wxEVT_CHECKBOX, &ImageViewer::OnDirectoryToggled, this, id);
	}

	directoryPanel->SetSizer(directorySizer);
	controlSizer->Add(directoryPanel, wxSizerFlags(1).Expand());

	//**********************************
	// Create the file types sub-panel *
	//**********************************

	// This should be reworked to be more customizable and programatic
	ToggledString types[4];
	types[0] = { ".png", true };
	types[1] = { ".jpg", true };
	types[2] = { ".jpeg", true };
	types[3] = { ".mp4", false };

	wxPanel *typesPanel = new wxPanel(controlPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
									  wxTAB_TRAVERSAL | wxBORDER_SIMPLE);
	wxBoxSizer *typesSizer = new wxBoxSizer(wxVERTICAL);

	// Put label at the top of the panel
	typesSizer->Add(new wxStaticText(typesPanel, wxID_ANY, "File Types:"), wxSizerFlags().Border(wxALL, 5));

	// Add a checkbox for each file type
	for (const auto &entry : types)
	{
		fileTypes.push_back(entry);

		int id = GetId(1, (int) fileTypes.size() - 1);
		wxCheckBox *checkbox = new wxCheckBox(typesPanel, id, entry.name);
		if (entry.active)
			checkbox->SetValue(true);

		typesSizer->Add(checkbox, wxSizerFlags().Border(wxALL, 5));
		Bind(wxEVT_CHECKBOX, &ImageViewer::OnFileTypeToggled, this, id);
	}
	
	typesPanel->SetSizer(typesSizer);
	controlSizer->Add(typesPanel, wxSizerFlags().Expand());
	controlPanel->SetSizer(controlSizer);

	//*********************
	// Create image panel *
	//*********************

	imagePanel = new wxPanel(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	imageSizer = new wxBoxSizer(wxHORIZONTAL);
	imageBitmap = new wxStaticBitmap(imagePanel, wxID_ANY, wxBitmap(1,1));
	imageBitmap->SetScaleMode(wxStaticBitmap::Scale_AspectFit);
	imageSizer->Add(imageBitmap, wxSizerFlags(1).Expand());
	imageSizer->SetMinSize(700, 500);
	imagePanel->SetSizer(imageSizer);

	//**************************
	// Configure entire window *
	//**************************

	// Split the window into the two panels
	splitter->SplitVertically(controlPanel, imagePanel);

	// Insert this split window into the frame
	wxBoxSizer *topSizer = new wxBoxSizer(wxHORIZONTAL);
	topSizer->Add(splitter, wxSizerFlags(1).Expand());

	// Record the current window size and set it as the minimum, then resize the window to fit the sizer
	this->SetMinSize(this->GetSize());
	this->SetSizer(topSizer);
	topSizer->Fit(this);

	// Set the window split position to match the sizer's preference
	defaultPanelWidth = controlSizer->GetMinSize().GetWidth();
	splitter->SetSashPosition(defaultPanelWidth);
}

void ImageViewer::OnRefresh(wxCommandEvent &event)
{
	GetImages();
}

void ImageViewer::OnToggleName(wxCommandEvent &event)
{
	showImageName = !showImageName;
}

void ImageViewer::OnFirstFile(wxCommandEvent &event)
{
	imageIndex = 0;
	LoadFile(imageIndex);
}

void ImageViewer::OnToggleSplit(wxCommandEvent &event)
{
	if (splitter->IsSplit())
		splitter->Unsplit(controlPanel);
	else
	{
		splitter->SplitVertically(controlPanel, imagePanel);
		splitter->SetSashPosition(defaultPanelWidth);
	}
}

void ImageViewer::OnToggleBackground(wxCommandEvent &event)
{
	if (this->GetBackgroundColour() == BACKGROUND_LIGHT)
		this->SetBackgroundColour(BACKGROUND_DARK);
	else
		this->SetBackgroundColour(BACKGROUND_LIGHT);
}

void ImageViewer::OnSortChanged(wxCommandEvent &event)
{
	// Update the sorting method and recollect the images
	sortMethod = event.GetId();
	GetImages();
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

void ImageViewer::OnKeyPress(wxKeyEvent &event)
{
	if (files.size() > 0)
	{
		int keyCode = event.GetKeyCode();
		if (keyCode == WXK_RIGHT)
		{
			imageIndex++;
			if (imageIndex >= files.size())
				imageIndex = 0;

			LoadFile(imageIndex);
		}
		else if (keyCode == WXK_LEFT)
		{
			imageIndex--;
			if (imageIndex < 0)
				imageIndex = files.size() - 1;

			LoadFile(imageIndex);
		}
	}

	event.Skip();
}

void ImageViewer::LoadFile(int index)
{
	if (index >= files.size())
	{
		std::cout << "LoadFile(): Invalid index" << std::endl;
		return;
	}

	static bool first = true;
	static wxImage image;
	if (first)
	{
		image.AddHandler(new wxPNGHandler);
		image.AddHandler(new wxJPEGHandler);
		first = false;
	}

	std::string path = files[index].path + files[index].name;
	if (image.LoadFile(path))
	{
		// Update the name of the window with the image name if enabled
		std::string name = "Image Viewer";
		if (showImageName)
			name += " - " + files[index].name;
		this->SetLabel(name);

		// Update the image shown in the window
		imageBitmap->SetBitmap(wxBitmap(image));
		imageSizer->Layout();
		image.Destroy();
	}
	else
		std::cout << "Failed to load file \"" << path << "\"" << std::endl;
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
			std::filesystem::path path = rootPath.string() + "/" + directory.name;
			if (std::filesystem::is_directory(path))
			{
				// Then iterate through every file in the directory
				for (const auto &entry : std::filesystem::directory_iterator(path))
				{
					if (entry.is_regular_file())
					{
						// Make the file's extension lowercase
						const char *c_extension = entry.path().extension().c_str();
						std::string extension = "";
						for (int i = 0; i < entry.path().extension().string().length(); i++)
							extension += tolower(c_extension[i]);

						// Compare the file's lowercase extension against the active extensions
						for (auto type : fileTypes)
						{
							if (type.active && extension.compare(type.name) == 0)
							{
								std::string name = entry.path().filename().string();
								std::string path = entry.path().string().substr(0, entry.path().string().length() - name.length());

								// Struct documentation: https://man7.org/linux/man-pages/man0/sys_stat.h.0p.html
								struct stat info;
								time_t time;
								// Function documentation: https://pubs.opengroup.org/onlinepubs/007908799/xsh/stat.html
								if (stat(entry.path().c_str(), &info) == 0)
								{
									time = info.st_mtime;
								}
								else
								{
									std::cout << "Unable to obtain time for \"" << path << "\"" << std::endl;
									time = 0;
								}

								File file = { name, path, time };

								if (sortMethod == ID_SORT_NAME)
									SortAlphabetically(files, file);
								else if (sortMethod == ID_SORT_DATE)
									SortByTime(files, file);
								else if (sortMethod == ID_SORT_RANDOM)
									SortRandomly(files, file);
								else
								{
									std::cout << "ERROR: Unknown sorting method" << std::endl;
									files.push_back(file);
								}
							}
						}
					}
				}
			}
			else
				std::cout << "Directory \"" << path.string() << "\" does not exist" << std::endl;
		}
	}

	if (files.size() > 0)
	{
		imageIndex = 0;
		LoadFile(imageIndex);
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
			std::cout << "-- " << element.name << std::endl;
		}
	}
	std::cout << std::endl;
}

void ImageViewer::PrintFile(ImageViewer::File file)
{
	std::cout << "\"" << file.name << "\" file:" << std::endl;
	std::cout << "-- Path: " << file.path << std::endl;
	std::cout << "-- Modified: " << asctime(localtime(&file.modifiedTime)) << std::endl << std::endl;
}

template <typename T>
void ImageViewer::SortAlphabetically(std::vector<T> &vector, T element)
{
	typename std::vector<T>::iterator it;
	for (it = vector.begin(); it < vector.end(); it++)
	{
		// If the current element in the list is alphabetically greater, break so the new element is inserted here
		if ((*it).name.compare(element.name) > 0)
			break;
	}

	vector.insert(it, element);
}

template <typename T>
void ImageViewer::SortRandomly(std::vector<T> &vector, T element)
{
	if (vector.size() > 0)
	{
		typename std::vector<T>::iterator it = vector.begin();

		// Reference: https://en.cppreference.com/w/cpp/numeric/random/rand
		std::srand(std::time(nullptr));
		int index = std::rand() % vector.size();

		std::advance(it, index);
		vector.insert(it, element);
	}
	else
		vector.push_back(element);
}

void ImageViewer::SortByTime(std::vector<File> &vector, File element)
{
	std::vector<File>::iterator it;
	for (it = vector.begin(); it < vector.end(); it++)
	{
		// If the current element in the list was modified later, break so the new element is inserted here
		if ((*it).modifiedTime > element.modifiedTime)
			break;
	}

	vector.insert(it, element);
}

// Don't catch wxID_NEW because the event will rise to TotalPhoto.cpp
BEGIN_EVENT_TABLE(ImageViewer, wxFrame)
EVT_MENU(wxID_REFRESH, ImageViewer::OnRefresh)
EVT_MENU(wxID_PRINT, ImageViewer::OnToggleName)
EVT_MENU(wxID_FIRST, ImageViewer::OnFirstFile)
EVT_MENU(wxID_SETUP, ImageViewer::OnToggleSplit)
EVT_MENU(wxID_SELECT_COLOR, ImageViewer::OnToggleBackground)
EVT_MENU(ID_SORT_NAME, ImageViewer::OnSortChanged)
EVT_MENU(ID_SORT_DATE, ImageViewer::OnSortChanged)
EVT_MENU(ID_SORT_RANDOM, ImageViewer::OnSortChanged)
END_EVENT_TABLE()