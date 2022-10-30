// Marcus Schmidt
// Created on 10/30/22

#include "TotalPhoto.h"
#include <wx/dirdlg.h>
#include <wx/string.h>
#include <iostream>

bool TotalPhoto::OnInit()
{
	//SetExitOnFrameDelete(false);

	std::string dir = GetDirectory();
	if (dir.compare("") != 0)
	{
		// If the user chose a directory, open the main window
		return true;
	}
	else
		return false;
}

/* Create a dialog that asks the user to choose a directory.
   Return the directory path or an empty string if cancelled. */
std::string TotalPhoto::GetDirectory()
{
	wxDirDialog *dialog = new wxDirDialog(NULL,
								"Choose root directory",
								"",
								wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dialog->ShowModal() == wxID_OK)
		return dialog->GetPath().utf8_string();
	else
		return "";
}