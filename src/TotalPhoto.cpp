// Marcus Schmidt
// Created on 10/30/22

#include "TotalPhoto.h"
#include "ImageViewer.h"
#include <wx/dirdlg.h>
#include <wx/menu.h>
#include <wx/string.h>

bool TotalPhoto::OnInit()
{
	SetExitOnFrameDelete(false);

	// Create an invisible root window so that there's permanently a menubar
	wxMenu *menu = new wxMenu();
	menu->Append(wxID_NEW, "New");
	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(menu, "Image Viewer");
	root = new wxFrame(NULL, -1, "");
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
	std::string dir = GetDirectory();

	// If the user chose a directory, open the main window
	if (dir.compare("") != 0)
	{
		ImageViewer *frame = new ImageViewer(root, -1, "Image Viewer", dir);
		frame->Show();

		return true;
	}
	else
		return false;
}

/* Create a dialog that asks the user to choose a directory.
   Return the directory path or an empty string if cancelled. */
std::string TotalPhoto::GetDirectory()
{
	wxDirDialog *dialog = new wxDirDialog(root,
								"Choose root directory",
								"",
								wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dialog->ShowModal() == wxID_OK)
		return dialog->GetPath().utf8_string();
	else
		return "";
}