// Marcus Schmidt
// Created on 4/16/23

#ifndef FILTER_EDITOR
#define FILTER_EDITOR

#include "Filter.h"
#include <string>
#include <vector>
#include <wx/frame.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/combobox.h>

class FilterEditor : public wxFrame
{
public:
	FilterEditor(wxWindow *parent, wxWindowID id, const wxString &title, std::vector<Filter *> *_filters, std::string _defaultPath);

private:
	struct FilterContainer
	{
		int id;
		std::string path;
		Filter *filter;
		wxBoxSizer *sizer;
		wxButton *button;
		wxComboBox *comboBox;
	};

	DECLARE_EVENT_TABLE();

	void OnOk(wxCommandEvent &event);
	void OnCancel(wxCommandEvent &event);
	void OnChooseDir(wxCommandEvent &event);
	void OnSelection(wxCommandEvent &event);

	std::string GetDisplayPath(std::string path);
	FilterContainer NewFilterContainer(Filter *filter);

	const static int MAX_DISPLAY_LENGTH = 30;

	std::string defaultPath;
	std::vector<Filter *> *filters;
	std::vector<FilterContainer> filterContainers;
	wxBoxSizer *topSizer;
	wxBoxSizer *filterSizer;
};

#endif