/*
 *   Copyright (C) 2006-2008,2013 by Jonathan Naylor G4KLX
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
#include "UDPDataReader.h"
#include "UDPDataWriter.h"

#include <wx/file.h>

#if defined(__WXGTK__) || defined(__WXMAC__)
#include "SDRSetup.xpm"
#endif

const int EXECUTE_BUTTON  = 4549;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;

BEGIN_EVENT_TABLE(CSDRSetupFrame, wxFrame)
	EVT_BUTTON(EXECUTE_BUTTON, CSDRSetupFrame::onExecute)
END_EVENT_TABLE()

CSDRSetupFrame::CSDRSetupFrame() :
wxFrame(NULL, -1, wxString(_("UWSDR Setup")), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
m_oldSDRAddress(NULL),
m_oldSDRControlPort(NULL),
m_sdrAddress(NULL),
m_sdrPort(NULL),
m_reply(REPLY_NONE)
{
	SetIcon(wxICON(SDRSetup));

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

	m_sdrPort = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_sdrPort, 0, wxALL, BORDER_SIZE);

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

void CSDRSetupFrame::onExecute(wxCommandEvent& WXUNUSED(event))
{
	wxIPV4address oldControl;
	wxIPV4address newControl;

	wxString addressString = m_oldSDRAddress->GetValue();
	if (addressString.IsEmpty()) {
		::wxMessageBox(_("The old SDR IP address is not allowed to be empty"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	bool valid = oldControl.Hostname(addressString);
	if (!valid) {
		::wxMessageBox(_("The old SDR IP address is not valid"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	wxString portString = m_oldSDRControlPort->GetValue();
	if (portString.IsEmpty()) {
		::wxMessageBox(_("The old SDR control port is not allowed to be empty"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	long port;
	valid = portString.ToLong(&port);
	if (!valid || port < 1L || port > 65535L) {
		::wxMessageBox(_("The old SDR control port is not valid (1-65535)"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	oldControl.Service(port);

	addressString = m_sdrAddress->GetValue();
	if (addressString.IsEmpty()) {
		::wxMessageBox(_("The new SDR IP address is not allowed to be empty"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	valid = newControl.Hostname(addressString);
	if (!valid) {
		::wxMessageBox(_("The new SDR IP address is not valid"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	portString = m_sdrPort->GetValue();
	if (portString.IsEmpty()) {
		::wxMessageBox(_("The new SDR port is not allowed to be empty"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	valid = portString.ToLong(&port);
	if (!valid || port < 1L || port > 65535L) {
		::wxMessageBox(_("The new SDR port is not valid (1-65535)"), _("SDRSetup Error"), wxICON_ERROR);
		return;
	}

	newControl.Service(port);

	bool ret = setNew(oldControl, newControl);
	if (ret)
		Close(true);
}

bool CSDRSetupFrame::callback(char* buffer, unsigned int WXUNUSED(len), int WXUNUSED(id))
{
	if (::strncmp(buffer, "AK", 2) == 0)
		m_reply = REPLY_ACK;
	else if (::strncmp(buffer, "NK", 2) == 0)
		m_reply = REPLY_NAK;
	else if (::strncmp(buffer, "DR", 2) == 0)
		{ }

	return true;
}

bool CSDRSetupFrame::setNew(const wxIPV4address& oldControl, const wxIPV4address& newControl)
{
	wxString command;

	command.Printf(wxT("SI%s,%u;"), newControl.IPAddress().c_str(), newControl.Service());

	CUDPDataWriter* writer = new CUDPDataWriter(oldControl.IPAddress(), oldControl.Service());
	CUDPDataReader* reader = new CUDPDataReader(oldControl.IPAddress(), oldControl.Service());
	reader->setCallback(this, 0);

	bool ret = reader->open();
	if (!ret)
		return false;

	ret = writer->open();
	if (!ret) {
		reader->close();
		return false;
	}

	for (unsigned int i = 0U; i < 20U && m_reply == REPLY_NONE; i++) {
		if ((i % 4U) == 0U) {
			ret = writer->write(command.c_str(), command.Length());
			if (!ret) {
				reader->close();
				writer->close();
				::wxMessageBox(_("Error writing to the SDR"), _("SDRSetup Error"), wxICON_ERROR);
				return false;
			}
		}

		::wxMilliSleep(500UL);
	}

	reader->close();
	writer->close();

	switch (m_reply) {
		case REPLY_ACK:
			::wxLogMessage(_("SDR updated succesfully"));
			return true;

		case REPLY_NAK:
			::wxMessageBox(_("Received an error from the SDR"), _("SDRSetup Error"), wxICON_ERROR);
			return false;

		case REPLY_NONE:
			::wxMessageBox(_("Timed out when waiting for the SDR"), _("SDRSetup Error"), wxICON_ERROR);
			return false;

		default:
			::wxMessageBox(_("Received an unknown reply from the SDR"), _("SDRSetup Error"), wxICON_ERROR);
			return false;
	}
}
