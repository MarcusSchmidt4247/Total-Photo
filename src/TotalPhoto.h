// Marcus Schmidt
// Created on 10/30/22

#ifndef TOTAL_PHOTO
#define TOTAL_PHOTO

#include <wx/app.h>
#include <wx/frame.h>
#include <wx/event.h>
#include <string>

class TotalPhoto : public wxApp
{
public:
	virtual bool OnInit();
	// virtual int OnExit(); // (OnExit() -> wxWidgets cleans structures -> class destructor)

private:
	DECLARE_EVENT_TABLE();
	void OnNew(wxCommandEvent &event);
	bool CreateNew();
	wxFrame *root;
};

wxIMPLEMENT_APP(TotalPhoto);

#endif