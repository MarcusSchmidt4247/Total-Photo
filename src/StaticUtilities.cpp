// Marcus Schmidt
// Created on 4/16/23

#include "StaticUtilities.h"
#include <wx/dirdlg.h>
#include <iostream>

std::string StaticUtilities::GetDirectory(wxFrame *parent, std::string message)
{
	wxDirDialog *dialog = new wxDirDialog(parent,
								message,
								"",
								wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dialog->ShowModal() == wxID_OK)
		return dialog->GetPath().utf8_string();
	else
		return "";
}

/* If this filename ends with ' copy' or ' copy X' before the extension, remove it.
 * The benefit is to be able to recognize copied images in different folders so that
 * the filter will work properly without having to manually rename copied images to match. */
std::string StaticUtilities::StandardizeImageName(std::string name)
{
	std::size_t pos = name.rfind(".");
	if (pos == std::string::npos)
		pos = name.size();

	if (pos > 5 && name.substr(pos - 5, 5).compare(" copy") == 0)
		name.erase(pos - 5, 5);
	else if (pos > 7 && name.substr(pos - 7, 6).compare(" copy ") == 0)
		name.erase(pos - 7, 7);

	return name;
}