// Marcus Schmidt
// Created on 10/30/22

#include "ImageViewer.h"
#include <wx/menu.h>

ImageViewer::ImageViewer(wxWindow *parent, wxWindowID id, const wxString &title)
			: wxFrame(parent, id, title)
{
	wxMenu *menu = new wxMenu();
	menu->Append(wxID_NEW, "New");

	wxMenuBar *menuBar = new wxMenuBar();
	menuBar->Append(menu, "Image Viewer");

	this->SetMenuBar(menuBar);
}