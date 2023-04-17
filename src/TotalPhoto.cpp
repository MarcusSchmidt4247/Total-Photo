// Marcus Schmidt
// Created on 10/30/22

#include "TotalPhoto.h"
#include "ImageViewer.h"
#include "StaticUtilities.h"
#include <wx/menu.h>
#include <wx/string.h>
#include <wx/gdicmn.h>

bool TotalPhoto::OnInit()
{
	SetExitOnFrameDelete(false);
	wxInitAllImageHandlers();

	// Create an invisible root window so that there's permanently a menubar
	wxMenu *menu = new wxMenu();
	menu->Append(wxID_NEW, "New");
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(menu, "Image Viewer");
	root = new wxFrame(NULL, -1, "", wxPoint(0,0), wxSize(0,0));
	root->SetMenuBar(menuBar);
	root->SetTransparent(1.0);
	root->Show();

	return CreateNew();
}

BEGIN_EVENT_TABLE(TotalPhoto, wxApp)
EVT_MENU(wxID_NEW, TotalPhoto::OnNew)
END_EVENT_TABLE()

// Triggers when the "New" option is selected from the menu
void TotalPhoto::OnNew(wxCommandEvent &event)
{
	CreateNew();
}

// Try to create a new ImageViewer window and return whether or not it succeeded
bool TotalPhoto::CreateNew()
{
	std::string dir = StaticUtilities::GetDirectory(root, "Choose root directory");
	if (dir.compare("") != 0)
	{
		ImageViewer *frame = new ImageViewer(root, -1, "Image Viewer", dir);
		frame->Show();
		return true;
	}
	else
		return false;
}