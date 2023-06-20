// Marcus Schmidt
// Created on 10/30/22

#include "ImageViewer.h"
#include "FilterEditor.h"
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/stattext.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <unordered_map>
#include <cstdlib>
#include <cctype>
#include <iostream>

ImageViewer::ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title, std::string path)
			: wxFrame(parent, id, title, wxDefaultPosition, wxSize(700, 500))
{
	rootPath = path;

	// Get the default Documents folder and modify it to lead to the user Application folder
	applicationDirectory = std::string(wxStandardPaths::Get().GetDocumentsDir());
	applicationDirectory.erase(applicationDirectory.size() - 9, 9);
	applicationDirectory = applicationDirectory.append("Applications/Total Photo/");

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

	directoryPanel = new wxScrolledWindow(controlPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize,
										  wxTAB_TRAVERSAL | wxBORDER_SIMPLE);
	directoryPanel->SetScrollRate(0, 15);
	wxBoxSizer *directorySizer = new wxBoxSizer(wxVERTICAL);

	// Put a label of the root directory at the top of the panel
	directorySizer->Add(new wxStaticText(directoryPanel, wxID_ANY, rootPath.filename().string()),
							   wxSizerFlags().Border(wxALL, 5));

	directories = GetSubdirectories(nullptr);
	AddSubdirectories(directorySizer, directories);

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

		int id = GetId(ImageViewer::ListType::FILE_TYPES, (int) fileTypes.size() - 1);
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
	topSizer = new wxBoxSizer(wxHORIZONTAL);
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

	// Also reset focus on the splitter in case it has been given to a child panel, which will lose key presses
	splitter->SetFocusIgnoringChildren();
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

	// Also reset focus on the splitter in case it has been given to a child panel, which will lose key presses
	splitter->SetFocusIgnoringChildren();
}

void ImageViewer::OnDirectoryToggled(wxCommandEvent &event)
{
	// Find the directory
	wxStringTokenizer *tokenizer = (wxStringTokenizer *) event.GetEventUserData();
	std::string userData = tokenizer->GetString().ToStdString();
	ImageViewer::Directory *directory = FindDirectory(userData);

	if (directory != nullptr)
	{
		// Toggle its activation state and recursively set its subdirectories to match
		directory->active = !directory->active;
		RecurseActivationState(directory->subdirectories, directory->active);
	}
	else
		std::cout << "OnDirectoryToggled(): Unable to find directory" << std::endl;
}

void ImageViewer::OnDirectoryExpanded(wxCommandEvent &event)
{
	// Find the directory
	wxStringTokenizer *tokenizer = (wxStringTokenizer *) event.GetEventUserData();
	std::string userData = tokenizer->GetString().ToStdString();
	ImageViewer::Directory *subdirectory = FindDirectory(userData);

	if (subdirectory != nullptr)
	{
		// If this is the first time expanding this directory, create its list of subdirectories in its sizer
		if (!subdirectory->expanded && subdirectory->subdirectories.empty())
		{
			subdirectory->subdirectories = GetSubdirectories(subdirectory);
			AddSubdirectories(subdirectory->subdirectorySizer, subdirectory->subdirectories);
		}
		// Otherwise, simply hide or show the elements in its sizer
		else
			subdirectory->subdirectorySizer->ShowItems(!subdirectory->expanded);

		// Update the relevant GUI containers
		subdirectory->subdirectorySizer->Layout(); // puts subdirectories in correct place
		directoryPanel->Layout(); // moves surrounding directories to new correct place
		controlPanel->Layout(); // gives panel scrollbar if necessary

		// Toggle this directory's expanded state
		subdirectory->expanded = !subdirectory->expanded;

		// And finally toggle the expand button's image icon to match
		std::string imageName = (subdirectory->expanded) ? "assets/collapse.png" : "assets/expand.png";
		subdirectory->expandButton->SetBitmap(wxBitmap(applicationDirectory + imageName, wxBITMAP_TYPE_PNG));
	}
	else
		std::cout << "OnDirectoryExpanded(): Unable to find directory" << std::endl;
}

void ImageViewer::OnDirectoryOverflow(wxCommandEvent &event)
{
	// Find the directory
	wxStringTokenizer *tokenizer = (wxStringTokenizer *) event.GetEventUserData();
	std::string userData = tokenizer->GetString().ToStdString();
	ImageViewer::Directory *directory = FindDirectory(userData);

	if (directory != nullptr)
	{
		FilterEditor *filterEditor = new FilterEditor(this, wxID_ANY, "\"" + directory->name + "\" Filters", &(directory->filter), rootPath);
		filterEditor->Show();
	}
	else
		std::cout << "OnDirectoryOverflow(): Unable to find directory" << std::endl;
}

void ImageViewer::OnFileTypeToggled(wxCommandEvent &event)
{
	int index = GetIndex(ImageViewer::ListType::FILE_TYPES, event.GetId());
	fileTypes[index].active = !fileTypes[index].active;
}

void ImageViewer::OnKeyPress(wxKeyEvent &event)
{
	event.Skip();

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
		else
			std::cout << "OnKeyPress(): Key code not recognized" << std::endl;
	}
	else
		std::cout << "OnKeyPress(): No files" << std::endl;
}

std::vector<ImageViewer::Directory> ImageViewer::GetSubdirectories(Directory *directory)
{
	// Construct a filepath to the provided directory by moving up the chain of parents to the root filepath
	std::string path = "";
	Directory *parent = directory;
	while (parent != nullptr)
	{
		path.insert(0, "/" + parent->name);
		parent = parent->parent;
	}
	path.insert(0, rootPath.string());

	Filter *filter = (directory == nullptr || directory->filter == nullptr) ? nullptr : directory->filter->GetCopy();
	bool active = (directory == nullptr) ? false : directory->active;

	/* Alphabetically sort each discovered directory into a list of subdirectories.
	 * References:
	 * https://stackoverflow.com/questions/612097
	 * https://en.cppreference.com/w/cpp/filesystem */
	std::vector<ImageViewer::Directory> subdirectories;
	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			ImageViewer::Directory subdirectory = { { entry.path().filename().string(), active }, directory, filter };
			SortAlphabetically(subdirectories, subdirectory);
		}
	}

	return subdirectories;
}

void ImageViewer::GetImages()
{
	files.clear();

	// For every directory...
	for (Directory directory : directories)
	{
		std::vector<ImageViewer::File> newFiles = RecurseGetImages(rootPath, &directory);
		MergeVectors(files, newFiles);
	}

	if (files.size() > 0)
	{
		imageIndex = 0;
		LoadFile(imageIndex);
	}
}

std::vector<ImageViewer::File> ImageViewer::RecurseGetImages(std::filesystem::path path, Directory *directory)
{
	std::vector<ImageViewer::File> images;

	// Confirm the provided directory still exists in its expected location
	if (std::filesystem::is_directory((std::filesystem::path) (path.string() + "/" + directory->name)))
	{
		// Gather information about the filter on this directory that will be passed along
		//*************************************************************************************************
		// NOTE: This work is unused in one of the logical paths but avoids repeating code. Improve this. *
		//*************************************************************************************************
		std::unordered_map<std::string, int> filterItems;
		bool defaultValidity = true;
		if (directory->filter != nullptr && directory->filter->GetType() != Filter::NONE)
		{
			filterItems = directory->filter->GetFilterItems();
			if (directory->filter->GetType() == Filter::INCLUDE)
				defaultValidity = false;
		}

		// If it has nested subdirectories in the code
		if (!directory->subdirectories.empty())
		{
			// Recurse to get their images
			for (auto subdirectory : directory->subdirectories)
			{
				std::vector<ImageViewer::File> recursedImages = RecurseGetImages(path.string() + "/" + directory->name, &subdirectory);
				MergeVectors(images, recursedImages);
			}

			// And then get the images from this directory if it's active
			if (directory->active)
			{
				for (const auto &entry : std::filesystem::directory_iterator(path))
				{
					if (entry.is_regular_file() && IsValidExtension(entry.path().extension().string()))
						AddImage(entry, images, filterItems, defaultValidity);
				}
			}
		}
		// If it does not have nested subdirectories in the code but it is activated
		else if (directory->active)
		{
			// And then recurse with a different method
			std::vector<ImageViewer::File> recursedImages = RecurseGetImages(path.string() + "/" + directory->name, filterItems, defaultValidity);
			MergeVectors(images, recursedImages);
		}
	}
	else
		std::cout << "RecurseGetImages(): Directory \"" << (path.string() + "/" + directory->name) << "\" does not exist" << std::endl;
	
	return images;
}

std::vector<ImageViewer::File> ImageViewer::RecurseGetImages(std::filesystem::path path, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity)
{
	std::vector<ImageViewer::File> images;

	for (const auto &entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_directory())
		{
			std::vector<ImageViewer::File> recursedImages = RecurseGetImages(entry.path(), filterItems, defaultValidity);
			MergeVectors(images, recursedImages);
		}
		else if (entry.is_regular_file() && IsValidExtension(entry.path().extension().string()))
			AddImage(entry, images, filterItems, defaultValidity);
	}
	
	return images;
}

// Calculates a unique ID for an item in a list (e.g. the list of directories a user chooses from) based on its index
int ImageViewer::GetId(ImageViewer::ListType type, int index)
{
	//*************************************
	// Check for possibility of overflow! *
	//*************************************
	return wxID_HIGHEST + 100 + (100 * static_cast<int>(type)) + index;
}

// Calculates the index of an item in a list based on its ID
int ImageViewer::GetIndex(ImageViewer::ListType type, int id)
{
	//**************************************
	// Check for possibility of underflow! *
	//**************************************
	return id - wxID_HIGHEST - 100 - (100 * static_cast<int>(type));
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
	std::cout << "\"" << file.originalName << "\" file:" << std::endl;
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

void ImageViewer::LoadFile(int index)
{
	if (index >= files.size())
	{
		std::cout << "LoadFile(): Invalid index" << std::endl;
		return;
	}

	static wxImage image;
	std::string path = files[index].path + files[index].originalName;
	if (image.LoadFile(path))
	{
		// Update the name of the window with the image name if enabled
		std::string name = "Image Viewer";
		if (showImageName)
			name += " - " + files[index].originalName;
		this->SetLabel(name);

		// Update the image shown in the window
		imageBitmap->SetBitmap(wxBitmap(image));
		imageSizer->Layout();
		image.Destroy();
	}
	else
		std::cout << "LoadFile(): Failed to load file \"" << path << "\"" << std::endl;
}

void ImageViewer::RecurseActivationState(std::vector<ImageViewer::Directory> &subdirectories, bool active)
{
	for (ImageViewer::Directory &directory : subdirectories)
	{
		directory.active = active;
		directory.activeCheckbox->SetValue(active);

		RecurseActivationState(directory.subdirectories, active);
	}
}

void ImageViewer::AddSubdirectories(wxBoxSizer *sizer, std::vector<ImageViewer::Directory> &subdirectories)
{
	if (sizer == nullptr)
		std::cout << "AddSubdirectories(): sizer is null" << std::endl;
	else if (subdirectories.empty())
		sizer->Add(new wxStaticText(directoryPanel, wxID_ANY, "No subdirectories"), wxSizerFlags().Border(wxALL, 5));
	else
	{
		// Add the sorted subdirectories to the panel
		std::string indicesPath;
		for (int i = 0; i < subdirectories.size(); i++)
		{
			if (subdirectories[i].parent != nullptr)
				indicesPath = subdirectories[i].parent->indicesPath + ":" + std::to_string(i);
			else
				indicesPath = std::to_string(i);
			subdirectories[i].indicesPath = indicesPath;

			wxBoxSizer *subdirectorySizer = new wxBoxSizer(wxHORIZONTAL);

			// Add a checkbox labelled with the subdirectory's name
			int id = StaticUtilities::GetUniqueId();
			subdirectories[i].activeCheckbox = new wxCheckBox(directoryPanel, id, subdirectories[i].name);
			subdirectories[i].activeCheckbox->SetValue(subdirectories[i].active);
			subdirectorySizer->Add(subdirectories[i].activeCheckbox);
			Bind(wxEVT_CHECKBOX, &ImageViewer::OnDirectoryToggled, this, id, wxID_ANY, new wxStringTokenizer(indicesPath, ":"));

			// Add a button to expand or collapse this subdirectory's own subdirectories
			id = StaticUtilities::GetUniqueId();
			subdirectories[i].expandButton = new wxButton(directoryPanel, id, wxEmptyString, wxDefaultPosition, wxSize(27,12), wxBU_NOTEXT | wxBORDER_NONE);
			subdirectories[i].expandButton->SetBitmap(wxBitmap(applicationDirectory + "assets/expand.png", wxBITMAP_TYPE_PNG));
			subdirectorySizer->Add(subdirectories[i].expandButton, wxSizerFlags().Border(wxLEFT | wxRIGHT | wxUP, 4));
			subdirectorySizer->AddSpacer(4);
			Bind(wxEVT_BUTTON, &ImageViewer::OnDirectoryExpanded, this, id, wxID_ANY, new wxStringTokenizer(indicesPath, ":"));

			// Add a button to open a menu with more options
			id = StaticUtilities::GetUniqueId();
			wxButton *overflowButton = new wxButton(directoryPanel, id, wxEmptyString, wxDefaultPosition, wxSize(15,12), wxBU_NOTEXT | wxBORDER_NONE);
			overflowButton->SetBitmap(wxBitmap(applicationDirectory + "assets/overflow.png", wxBITMAP_TYPE_PNG));
			subdirectorySizer->Add(overflowButton, wxSizerFlags().Border(wxLEFT | wxRIGHT | wxUP, 4));
			Bind(wxEVT_BUTTON, &ImageViewer::OnDirectoryOverflow, this, id, wxID_ANY, new wxStringTokenizer(indicesPath, ":"));

			sizer->Add(subdirectorySizer, wxSizerFlags().Border(wxALL, 5));

			// Add another sizer beneath this subdirectory where its own subdirectories can expand later
			wxBoxSizer *childSizer = new wxBoxSizer(wxVERTICAL);
			subdirectories[i].subdirectorySizer = childSizer;
			sizer->Add(childSizer, wxSizerFlags().Border(wxLEFT, 20));
		}
	}
}

// Navigate the nested lists of directories according to the given path (a string formatted so that subsequent indices to move into are separated by colons).
// Return a null pointer if the path is invalid
ImageViewer::Directory * ImageViewer::FindDirectory(std::string path)
{
	wxStringTokenizer localTokenizer(path, ":");
	ImageViewer::Directory *directory = nullptr;
	while (localTokenizer.HasMoreTokens())
	{
		int index = std::stoi(localTokenizer.GetNextToken().ToStdString());

		if (directory == nullptr && index >= 0 && index < directories.size())
			directory = &directories[index];
		else if (directory != nullptr && index >= 0 && index < directory->subdirectories.size())
			directory = &(directory->subdirectories[index]);
		else
			return nullptr;
	}

	return directory;
}

// Merge vector 'b's elements into vector 'a' using the current sort method (based on merge sort algorithm)
void ImageViewer::MergeVectors(std::vector<ImageViewer::File> &a, const std::vector<ImageViewer::File> &b)
{
	if (b.empty())
		return;

	// Reserve enough space for the final vector so that it won't be reallocated and invalidate the iterator
	a.reserve(a.size() + b.size());

	std::vector<ImageViewer::File>::iterator iterator = a.begin();
	int i = 0;
	while (iterator < a.end() && i < b.size())
	{
		// If the next element in vector 'b' is smaller according to the current sort method, insert it in the next spot in vector 'a'
		if ((sortMethod == ID_SORT_NAME && iterator->name.compare(b[i].name) > 0) ||
		    (sortMethod == ID_SORT_DATE && iterator->modifiedTime > b[i].modifiedTime) ||
			(sortMethod == ID_SORT_RANDOM && std::rand() % 2 == 1))
		{
			a.insert(iterator, b[i]);
			i++;
		}

		// Move to the next element in vector 'a' (either because it was smaller or because the new element from vector 'b' needs to be skipped)
		iterator++;
	}

	// If the end of vector 'a' has been reached but there are still elements in 'b', add them all to the end of 'a'
	while (i < b.size())
	{
		a.push_back(b[i]);
		i++;
	}
}

bool ImageViewer::IsValidExtension(std::string extension)
{
	// Convert the extension to all lowercase characters
	for (int i = 0; i < extension.size(); i++)
		extension[i] = tolower(extension[i]);

	// Compare the file's lowercase extension against the active extensions
	for (auto type : fileTypes)
	{
		if (type.active && extension.compare(type.name) == 0)
			return true;
	}

	return false;
}

void ImageViewer::AddImage(const std::filesystem::directory_entry &file, std::vector<File> &vector, const std::unordered_map<std::string, int> &filterItems, const bool defaultValidity)
{
	std::string name = file.path().filename().string();
	std::string nameStandardized = StaticUtilities::StandardizeImageName(name);
	std::string path = file.path().string().substr(0, file.path().string().length() - name.length());

	// If there is a filter, check whether this image is in the filter and flip its accept state if it's present.
	// Standardize image name before comparing with filter so names like "image.jpg" and "image copy 2.jpg" will evaluate to equal
	bool valid = defaultValidity;
	if (filterItems.find(nameStandardized) != filterItems.end())
		valid = !valid;

	if (valid)
	{
		// Struct documentation: https://man7.org/linux/man-pages/man0/sys_stat.h.0p.html
		struct stat info;
		time_t time;
		// Function documentation: https://pubs.opengroup.org/onlinepubs/007908799/xsh/stat.html
		if (stat(file.path().c_str(), &info) == 0)
		{
			time = info.st_mtime;
		}
		else
		{
			std::cout << "AddImage(): Unable to obtain time for \"" << path << "\"" << std::endl;
			time = 0;
		}

		File image = { nameStandardized, name, path, time };
		if (sortMethod == ID_SORT_NAME)
			SortAlphabetically(vector, image);
		else if (sortMethod == ID_SORT_DATE)
			SortByTime(vector, image);
		else if (sortMethod == ID_SORT_RANDOM)
			SortRandomly(vector, image);
		else
		{
			std::cout << "AddImage(): Unknown sorting method" << std::endl;
			vector.push_back(image);
		}
	}
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