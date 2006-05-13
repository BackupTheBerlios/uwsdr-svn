/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "SDRSetupFrame.h"

#include <wx/file.h>

#include "SDRSetup.xpm"

const int EXECUTE_BUTTON  = 47549;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;

BEGIN_EVENT_TABLE(CSDRSetupFrame, wxFrame)
	EVT_BUTTON(EXECUTE_BUTTON, CSDRSetupFrame::onExecute)
END_EVENT_TABLE()

CSDRSetupFrame::CSDRSetupFrame() :
wxFrame(NULL, -1, wxString(_("µWave SDR Setup")), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
m_oldSDRAddress(NULL),
m_oldSDRControlPort(NULL),
m_sdrAddress(NULL),
m_sdrControlPort(NULL),
m_sdrDataPort(NULL),
m_dspAddress(NULL),
m_dspDataPort(NULL)
{
	SetIcon(wxIcon(SDRSetup_xpm));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Old SDR Address:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_oldSDRAddress = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_oldSDRAddress, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Old SDR Control Port:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_oldSDRControlPort = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_oldSDRControlPort, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("New SDR IP Address:"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_sdrAddress = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_sdrAddress, 0, wxALL, BORDER_SIZE);

	wxStaticText* label5 = new wxStaticText(panel, -1, _("New SDR Control Port:"));
	panelSizer->Add(label5, 0, wxALL, BORDER_SIZE);

	m_sdrControlPort = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_sdrControlPort, 0, wxALL, BORDER_SIZE);

	wxStaticText* label6 = new wxStaticText(panel, -1, _("New SDR Data Port:"));
	panelSizer->Add(label6, 0, wxALL, BORDER_SIZE);

	m_sdrDataPort = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_sdrDataPort, 0, wxALL, BORDER_SIZE);

	wxButton* execute = new wxButton(panel, EXECUTE_BUTTON, _("Execute"));
	panelSizer->Add(execute, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);
}

CSDRSetupFrame::~CSDRSetupFrame()
{
}

void CSDRSetupFrame::onExecute(wxCommandEvent& event)
{
/*
	wxString name = m_name->GetValue();
	if (name.IsEmpty()) {
		::wxMessageBox(wxT("The Name is not allowed to be empty"));
		return;
	}

	wxString fileName = m_filename->GetValue();
	if (fileName.IsEmpty()) {
		::wxMessageBox(wxT("The SDR File Name is not allowed to be empty"));
		return;
	}
	if (!wxFile::Exists(fileName.c_str())) {
		::wxMessageBox(wxT("The SDR File does not exist"));
		return;
	}

	wxString audioDev = m_audio->GetValue();
	if (audioDev.IsEmpty()) {
		::wxMessageBox(wxT("The Audio Device is not allowed to be empty"));
		return;
	}

	wxString ipAddress = m_address->GetValue();
	if (ipAddress.IsEmpty()) {
		::wxMessageBox(wxT("The SDR IP Address is not allowed to be empty"));
		return;
	}

	wxString control = m_control->GetValue();
	if (control.IsEmpty()) {
		::wxMessageBox(wxT("The SDR Control Port is not allowed to be empty"));
		return;
	}
	int controlPort = ::atoi(control.c_str());
	if (controlPort < 1 || controlPort > 65536) {
		::wxMessageBox(wxT("The SDR Control Port must be between 1 and 65536"));
		return;
	}
	
	wxString data = m_data->GetValue();
	if (data.IsEmpty()) {
		::wxMessageBox(wxT("The SDR Data Port is not allowed to be empty"));
		return;
	}
	int dataPort = ::atoi(data.c_str());
	if (dataPort < 1 || dataPort > 65536) {
		::wxMessageBox(wxT("The SDR Data Port must be between 1 and 65536"));
		return;
	}

	wxConfig* config = new wxConfig(wxT("UWSDR"));

	wxString fileNameKey    = wxT("/") + name + wxT("/FileName");
	wxString audioDevKey    = wxT("/") + name + wxT("/AudioDev");
	wxString ipAddressKey   = wxT("/") + name + wxT("/IPAddress");
	wxString controlPortKey = wxT("/") + name + wxT("/ControlPort");
	wxString dataPortKey    = wxT("/") + name + wxT("/DataPort");

	wxString test;
	if (config->Read(fileNameKey, &test)) {
		int ret = ::wxMessageBox(wxT("An SDR with the same name already exists. Overwrite?"), wxT("Overwrite confirmation"), wxYES_NO | wxICON_QUESTION);
		if (ret == wxNO)
			return;
	}

	config->Write(fileNameKey,    fileName);
	config->Write(audioDevKey,    audioDev);
	config->Write(ipAddressKey,   ipAddress);
	config->Write(controlPortKey, long(controlPort));
	config->Write(dataPortKey,    long(dataPort));
	config->Flush();

	delete config;

	::wxMessageBox(wxT("µWave SDR configuration written."));
*/
	Close(true);
}
