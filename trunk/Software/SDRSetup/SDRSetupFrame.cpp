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

#include "SDRSetupXpm.h"

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

	wxStaticText* label7 = new wxStaticText(panel, -1, _("DSP IP Address:"));
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
	wxIPV4address oldControl;
	wxIPV4address newControl;
	wxIPV4address newData;
	wxIPV4address dsp;

	wxString addressString = m_oldSDRAddress->GetValue();
	if (addressString.IsEmpty()) {
		::wxMessageBox(_("The old SDR IP address is not allowed to be empty"));
		return;
	}

	bool valid = oldControl.Hostname(addressString);
	if (!valid) {
		::wxMessageBox(_("The old SDR IP address is not valid"));
		return;
	}

	wxString portString = m_oldSDRControlPort->GetValue();
	if (portString.IsEmpty()) {
		::wxMessageBox(_("The old SDR control port is not allowed to be empty"));
		return;
	}

	long port;
	valid = portString.ToLong(&port);
	if (!valid || port < 1L || port > 65535L) {
		::wxMessageBox(_("The old SDR control port is not valid (1-65535)"));
		return;
	}

	oldControl.Service(port);

	addressString = m_sdrAddress->GetValue();
	if (addressString.IsEmpty()) {
		::wxMessageBox(_("The new SDR IP address is not allowed to be empty"));
		return;
	}

	valid = newControl.Hostname(addressString);
	if (!valid) {
		::wxMessageBox(_("The new SDR IP address is not valid"));
		return;
	}

	newData.Hostname(addressString);

	portString = m_sdrControlPort->GetValue();
	if (portString.IsEmpty()) {
		::wxMessageBox(_("The new SDR control port is not allowed to be empty"));
		return;
	}

	valid = portString.ToLong(&port);
	if (!valid || port < 1L || port > 65535L) {
		::wxMessageBox(_("The new SDR control port is not valid (1-65535)"));
		return;
	}

	newControl.Service(port);

	portString = m_sdrDataPort->GetValue();
	if (portString.IsEmpty()) {
		::wxMessageBox(_("The new SDR data port is not allowed to be empty"));
		return;
	}

	valid = portString.ToLong(&port);
	if (!valid || port < 1L || port > 65535L) {
		::wxMessageBox(_("The new SDR data port is not valid (1-65535)"));
		return;
	}

	newData.Service(port);
	dsp.Service(port);

	addressString = m_dspAddress->GetValue();
	if (addressString.IsEmpty()) {
		::wxMessageBox(_("The DSP IP address is not allowed to be empty"));
		return;
	}

	valid = dsp.Hostname(addressString);
	if (!valid) {
		::wxMessageBox(_("The DSP IP address is not valid"));
		return;
	}

	// Everything is valid now, now to send the data to the SDR
	wxSocketClient* socket = new wxSocketClient();

	bool ret = socket->Connect(oldControl);
	if (!ret) {
		::wxMessageBox(_("Cannot connect to the SDR"));
		socket->Destroy();
		return;
	}

	socket->SetTimeout(10);

	ret = setNewSDR(socket, newControl, newData);
	if (!ret) {
		socket->Destroy();
		return;
	}

	ret = setNewDSP(socket, dsp);
	if (!ret) {
		socket->Destroy();
		return;
	}

	socket->Destroy();

	::wxLogMessage(_("SDR updated succesfully"));

	Close(true);
}

bool CSDRSetupFrame::setNewSDR(wxSocketClient* socket, const wxIPV4address& control, const wxIPV4address& data) const
{
	wxString command;

	command.Printf(wxT("SI%s,%u,%u;"), control.IPAddress().c_str(), control.Service(), data.Service());

	return sendCommand(socket, command);
}

bool CSDRSetupFrame::setNewDSP(wxSocketClient* socket, const wxIPV4address& dsp) const
{
	wxString command;

	command.Printf(wxT("SD%s;"), dsp.IPAddress().c_str());

	return sendCommand(socket, command);
}

bool CSDRSetupFrame::sendCommand(wxSocketClient* socket, const wxString& command) const
{
	socket->Write(command.c_str(), command.Length());

	bool ret = socket->Error();
	if (ret) {
		wxString message;
		message.Printf(_("Error writing to the SDR\n%s"), command.c_str());
		::wxMessageBox(message);
		return false;
	}

	ret = socket->WaitForRead();
	if (!ret) {
		::wxMessageBox(_("Error when waiting for the SDR"));
		return false;
	}

	char buffer[65];
	socket->Read(buffer, 64);

	ret = socket->Error();
	if (ret) {
		::wxMessageBox(_("Error reading from the SDR"));
		return false;
	}

	int n = socket->LastCount();
	buffer[n] = '\0';

	if (::strncmp(buffer, "NK", 2) == 0) {
		wxString message;
		message.Printf(_("Received an error from the SDR\n%s"), buffer);
		::wxMessageBox(message);
		return false;
	}

	if (::strncmp(buffer, "AK", 2) != 0) {
		wxString message;
		message.Printf(_("Received an unknown reply from the SDR\n%s"), buffer);
		::wxMessageBox(message);
		return false;
	}

	return true;
}
