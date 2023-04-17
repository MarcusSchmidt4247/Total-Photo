// Marcus Schmidt
// Created on 4/16/23

#include "FilterEditor.h"
#include "StaticUtilities.h"
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dirdlg.h>
#include <iostream>

FilterEditor::FilterEditor(wxWindow *parent, wxWindowID id, const wxString &title, Filter **_filter, std::string defaultPath)
			: wxFrame(parent, id, title, wxDefaultPosition, wxSize(400, 300))
{
	int selection;

	// If this filter doesn't exist yet, create it
	if (*_filter == nullptr)
	{
		*_filter = new Filter();
		selection = 0;
		path = defaultPath;
	}
	// If the filter exists and was assigned to, change the default values to match it
	else
	{
		selection = static_cast<int>((*_filter)->GetType());
		if ((*_filter)->GetPath().compare("") != 0)
			path = (*_filter)->GetPath();
		else
			path = defaultPath;
	}

	wxBoxSizer *topSizer = new wxBoxSizer(wxVERTICAL);

	// Add a label and dropdown menu to choose the type of filter
	wxBoxSizer *typeSizer = new wxBoxSizer(wxHORIZONTAL);
	typeSizer->Add(new wxStaticText(this, wxID_ANY, "Filter Type:"), wxSizerFlags().Border(wxUP, 2));
	typeSizer->AddSpacer(10);
	wxString choices[3] = { "None", "Only Include", "Only Exclude" };
	comboBox = new wxComboBox(this, wxID_HIGHEST, wxEmptyString, wxDefaultPosition, wxDefaultSize, 3, choices, wxCB_READONLY);
	comboBox->SetSelection(selection);
	typeSizer->Add(comboBox, wxSizerFlags());
	topSizer->Add(typeSizer, wxSizerFlags().Border(wxALL, 15));
	topSizer->AddSpacer(5);
	
	// Add the text for the chosen directory and a button beneath it to choose a new button
	dirText = new wxStaticText(this, wxID_ANY, "Source Folder: " + path);
	topSizer->Add(dirText, wxSizerFlags().Border(wxLEFT | wxRIGHT, 15));
	topSizer->Add(new wxButton(this, wxID_NEW, "Choose Folder"), wxSizerFlags().Border(wxUP, 5).Center());
	topSizer->AddSpacer(20);

	// Add the Cancel and OK buttons at the bottom of the window
	wxBoxSizer *buttonSizer = new wxBoxSizer(wxHORIZONTAL);
	buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), wxSizerFlags().Border(wxRIGHT, 5));
	buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), wxSizerFlags());
	topSizer->Add(buttonSizer, wxSizerFlags().Right().Border(wxRIGHT | wxDOWN, 5));

	this->SetSizer(topSizer);
	topSizer->Fit(this);

	filter = *_filter;
}

void FilterEditor::OnOk(wxCommandEvent &event)
{
	// Apply the current selections to the filter before closing the window
	int type = comboBox->GetSelection();
	filter->SetFilter(static_cast<Filter::Type>(type), path);
	this->Close();
}

void FilterEditor::OnCancel(wxCommandEvent &event)
{
	// Close the window without applying the current selections
	this->Close();
}

void FilterEditor::OnChooseDir(wxCommandEvent &event)
{
	std::string dir = StaticUtilities::GetDirectory(this, "Choose a new source folder");
	if (dir.compare("") != 0)
	{
		path = dir;
		dirText->SetLabelText("Source Folder: " + path);
	}
}

BEGIN_EVENT_TABLE(FilterEditor, wxFrame)
EVT_BUTTON(wxID_CANCEL, FilterEditor::OnCancel)
EVT_BUTTON(wxID_OK, FilterEditor::OnOk)
EVT_BUTTON(wxID_NEW, FilterEditor::OnChooseDir)
END_EVENT_TABLE()