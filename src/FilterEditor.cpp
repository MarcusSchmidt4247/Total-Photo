// Marcus Schmidt
// Created on 4/16/23

#include "FilterEditor.h"
#include "StaticUtilities.h"
#include <wx/dirdlg.h>
#include <wx/tokenzr.h>
#include <iostream>

FilterEditor::FilterEditor(wxWindow *parent, wxWindowID id, const wxString &title, std::vector<Filter *> *_filters, std::string _defaultPath)
			: wxFrame(parent, id, title, wxDefaultPosition, wxSize(400, 300))
{
	// If no filters exist yet, create one
	filters = _filters;
	if (filters->empty())
		filters->push_back(new Filter());
	
	defaultPath = _defaultPath;

	topSizer = new wxBoxSizer(wxVERTICAL);

	filterSizer = new wxBoxSizer(wxVERTICAL);
	for (Filter *filter : *filters)
	{
		if (!filterContainers.empty())
			filterSizer->Add(new wxStaticText(this, wxID_ANY, "AND"), wxSizerFlags().Border(wxUP, 15).Center());
		filterContainers.push_back(NewFilterContainer(filter));
	}
	topSizer->Add(filterSizer, wxSizerFlags());

	// Add the Cancel and OK buttons at the bottom of the window
	topSizer->AddSpacer(15);
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), wxSizerFlags().Border(wxRIGHT, 5));
	buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), wxSizerFlags());
	topSizer->Add(buttonSizer, wxSizerFlags().Right().Border(wxRIGHT | wxDOWN, 8));

	this->SetSizer(topSizer);
	topSizer->Fit(this);
}

void FilterEditor::OnOk(wxCommandEvent &event)
{
	// Apply the current selections to all filters
	int type;
	for (const auto &filterContainer : filterContainers)
	{
		type = filterContainer.comboBox->GetSelection();
		filterContainer.filter->SetFilter(static_cast<Filter::Type>(type), filterContainer.path);
	}

	// And then close the window
	this->Close();
}

void FilterEditor::OnCancel(wxCommandEvent &event)
{
	// Close the window without applying any of the current selections
	this->Close();
}

void FilterEditor::OnChooseDir(wxCommandEvent &event)
{
	std::string dir = StaticUtilities::ChooseDirectory(this, "Choose a new source folder");
	if (dir.compare("") != 0)
	{
		wxStringTokenizer *tokenizer = (wxStringTokenizer *) event.GetEventUserData();
		int id = std::stoi(tokenizer->GetString().ToStdString());
		for (auto &filterContainer : filterContainers)
		{
			if (filterContainer.id == id)
			{
				filterContainer.path = dir;
				filterContainer.button->SetLabel(GetDisplayPath(dir));
				break;
			}
		}
	}
}

void FilterEditor::OnSelection(wxCommandEvent &event)
{
	// When a combo box item changes one of the filter types, check to see if all of the filter slots are currently being used
	bool full = true;
	int type;
	for (auto filterContainer : filterContainers)
	{
		type = filterContainer.comboBox->GetSelection();
		if (static_cast<Filter::Type>(type) == Filter::Type::NONE)
		{
			full = false;
			break;
		}
	}

	// If all of the slots are being used, add another one
	if (full)
	{
		// Create a divider between filters
		filterSizer->Add(new wxStaticText(this, wxID_ANY, "AND"), wxSizerFlags().Border(wxUP, 15).Center());

		// Create a new filter and its GUI
		Filter *filter = new Filter();
		filters->push_back(filter);
		filterContainers.push_back(NewFilterContainer(filter));

		// Update the window to show this new filter's GUI
		filterSizer->Layout();
		topSizer->Layout();
		topSizer->Fit(this);
	}
}

std::string FilterEditor::GetDisplayPath(std::string path)
{
	if (path.size() <= MAX_DISPLAY_LENGTH)
		return path;
	else
		return "..." + path.substr(path.size() - MAX_DISPLAY_LENGTH, MAX_DISPLAY_LENGTH);
}

FilterEditor::FilterContainer FilterEditor::NewFilterContainer(Filter *filter)
{
	int selection = static_cast<int>(filter->GetType());
	std::string path;
	if (filter->GetPath().compare("") != 0)
		path = filter->GetPath();
	else
		path = defaultPath;

	FilterContainer filterContainer = { StaticUtilities::GetUniqueId(), path, filter, new wxBoxSizer(wxHORIZONTAL) };

	// Create the combo box
	wxString choices[3] = { "None", "Only Include", "Only Exclude" };
	filterContainer.comboBox = new wxComboBox(this, wxID_APPLY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, choices, wxCB_READONLY);
	filterContainer.comboBox->SetSelection(selection);

	// Create the directory button
	int id = StaticUtilities::GetUniqueId();
	filterContainer.button = new wxButton(this, id, GetDisplayPath(path));
	Bind(wxEVT_BUTTON, &FilterEditor::OnChooseDir, this, id, wxID_ANY, new wxStringTokenizer(std::to_string(filterContainer.id), ":"));

	// Create the left side of the sizer with labels for the buttons
	wxBoxSizer *labelSizer = new wxBoxSizer(wxVERTICAL);
	labelSizer->Add(new wxStaticText(this, wxID_ANY, "Filter Type:"), wxSizerFlags().Right());
	labelSizer->AddSpacer(20);
	labelSizer->Add(new wxStaticText(this, wxID_ANY, "Source Folder:"), wxSizerFlags().Right());
	filterContainer.sizer->Add(labelSizer, wxSizerFlags().Border(wxLEFT | wxUP, 13));

	// Create the right side of the sizer with the buttons / controls
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxVERTICAL);
	buttonSizer->AddSpacer(3);
	buttonSizer->Add(filterContainer.comboBox, wxSizerFlags());
	buttonSizer->AddSpacer(15);
	buttonSizer->Add(filterContainer.button, wxSizerFlags());
	filterContainer.sizer->Add(buttonSizer, wxSizerFlags().Border(wxLEFT | wxUP | wxRIGHT, 10));

	// Add this newly created sizer to the window
	filterSizer->Add(filterContainer.sizer, wxSizerFlags());

	return filterContainer;
}

BEGIN_EVENT_TABLE(FilterEditor, wxFrame)
EVT_BUTTON(wxID_CANCEL, FilterEditor::OnCancel)
EVT_BUTTON(wxID_OK, FilterEditor::OnOk)
EVT_COMBOBOX(wxID_APPLY, FilterEditor::OnSelection)
END_EVENT_TABLE()