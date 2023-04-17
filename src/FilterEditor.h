// Marcus Schmidt
// Created on 4/16/23

#ifndef FILTER_EDITOR
#define FILTER_EDITOR

#include "Filter.h"
#include <wx/frame.h>
#include <wx/event.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <string>

class FilterEditor : public wxFrame
{
public:
	FilterEditor(wxWindow *parent, wxWindowID id, const wxString &title, Filter **_filter, std::string defaultPath);

private:
	DECLARE_EVENT_TABLE();

	void OnOk(wxCommandEvent &event);
	void OnCancel(wxCommandEvent &event);
	void OnChooseDir(wxCommandEvent &event);

	Filter *filter;
	std::string path;
	wxStaticText *dirText;
	wxComboBox *comboBox;
};

#endif