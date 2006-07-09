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
#include <wx/socket.h>

#include "SDRSetup.xpm"

const int EXECUTE_BUTTON  = 4549;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;

BEGIN_EVENT_TABLE(CSDRSetupFrame, wxFrame)
	EVT_BUTTON(EXECUTE_BUTTON, CSDRSetupFrame::onExecute)
END_EVENT_TABLE()

CSDRSetupFrame::CSDRSetupFrame() :
wxFrame(NULL, -1, wxString(_("uWave SDR Setup")), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
m_oldSDRAddress(NULL),
m_oldSDRControlPort(NULL),
m_sdrAddress(NULL),
m_sdrControlPort(NULL),
m_sdrDataPort(NULL),
m_dspAddress(NULL)
{
	SetIcon(wxIcon(SDRSetup_xpm));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Old SDR IP Address:"));
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

	wxStaticText* label7 = new wxStaticText(panel, -1, _("DSP Address:"));
	panelSizer->Add(label7, 0, wxALL, BORDER_SIZE);

	m_dspAddress = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_dspAddress, 0, wxALL, BORDER_SIZE);

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
   wxIPV4address address;

	wxString oldSDRAddress = m_oldSDRAddress->GetValue();
	if (oldSDRAddress.IsEmpty()) {
		::wxMessageBox(_("The old SDR IP address is not allowed to be empty"));
		return;
	}

   bool valid = address.Hostname(oldSDRAddress);
	if (!valid) {
		::wxMessageBox(_("The old SDR IP address is not valid"));
		return;
	}

   oldSDRAddress = address.IPAddress();

	wxString oldSDRControlPort = m_oldSDRControlPort->GetValue();
	if (oldSDRControlPort.IsEmpty()) {
		::wxMessageBox(_("The old SDR control port is not allowed to be empty"));
		return;
	}

   long port;
   valid = oldSDRControlPort.ToLong(&port);
   if (!valid || port < 1 || port > 65535) {
		::wxMessageBox(_("The old SDR control port is not valid (1-65535)"));
		return;
	}

	wxString sdrAddress = m_sdrAddress->GetValue();
	if (sdrAddress.IsEmpty()) {
		::wxMessageBox(_("The new SDR IP address is not allowed to be empty"));
		return;
	}

   valid = address.Hostname(sdrAddress);
	if (!valid) {
		::wxMessageBox(_("The new SDR IP address is not valid"));
		return;
	}

   sdrAddress = address.IPAddress();

	wxString sdrControlPort = m_sdrControlPort->GetValue();
	if (sdrControlPort.IsEmpty()) {
		::wxMessageBox(_("The new SDR control port is not allowed to be empty"));
		return;
	}

   valid = sdrControlPort.ToLong(&port);
   if (!valid || port < 1 || port > 65535) {
		::wxMessageBox(_("The new SDR control port is not valid (1-65535)"));
		return;
	}

	wxString sdrDataPort = m_sdrDataPort->GetValue();
	if (sdrDataPort.IsEmpty()) {
		::wxMessageBox(_("The new SDR data port is not allowed to be empty"));
		return;
	}

   valid = sdrDataPort.ToLong(&port);
   if (!valid || port < 1 || port > 65535) {
		::wxMessageBox(_("The new SDR data port is not valid (1-65535)"));
		return;
	}

	wxString dspAddress = m_dspAddress->GetValue();
	if (dspAddress.IsEmpty()) {
		::wxMessageBox(_("The DSP IP address is not allowed to be empty"));
		return;
	}

   valid = address.Hostname(dspAddress);
	if (!valid) {
		::wxMessageBox(_("The DSP IP address is not valid"));
		return;
	}

   dspAddress = address.IPAddress();

   // Everything is valid now, now to send the data to the SDR

	Close(true);
}
