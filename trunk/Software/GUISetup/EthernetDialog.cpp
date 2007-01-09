/*
 *   Copyright (C) 2007 by Jonathan Naylor G4KLX
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

#include "EthernetDialog.h"


const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;

BEGIN_EVENT_TABLE(CEthernetDialog, wxDialog)
	EVT_BUTTON(wxID_OK,   CEthernetDialog::onOK)
END_EVENT_TABLE()


CEthernetDialog::CEthernetDialog(wxWindow* parent, const wxString& title, const wxString& address, long control, long data, int id) :
wxDialog(parent, id, title),
m_address(NULL),
m_control(NULL),
m_data(NULL),
m_ipAddress(address),
m_controlPort(control),
m_dataPort(data)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("IP Address:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_address = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_address, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Control Port:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_control = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_control, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Data Port:"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_data = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_data, 0, wxALL, BORDER_SIZE);

	m_address->SetValue(m_ipAddress);

	if (m_controlPort != -1L) {
		wxString text;
		text.Printf(wxT("%ld"), m_controlPort);
		m_control->SetValue(text);
	}

	if (m_dataPort != -1L) {
		wxString text;
		text.Printf(wxT("%ld"), m_dataPort);
		m_data->SetValue(text);
	}

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL, BORDER_SIZE);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);
}

CEthernetDialog::~CEthernetDialog()
{
}

void CEthernetDialog::onOK(wxCommandEvent& event)
{
	m_ipAddress = m_address->GetValue();
	if (m_ipAddress.IsEmpty()) {
		::wxMessageBox(_("The IP Address is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	wxString control = m_control->GetValue();
	if (control.IsEmpty()) {
		::wxMessageBox(_("The Control Port is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	control.ToLong(&m_controlPort);
	if (m_controlPort < 1L || m_controlPort > 65536L) {
		::wxMessageBox(_("The Control Port must be between 1 and 65536"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}
	
	wxString data = m_data->GetValue();
	if (data.IsEmpty()) {
		::wxMessageBox(_("The Data Port is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	data.ToLong(&m_dataPort);
	if (m_dataPort < 1L || m_dataPort > 65536L) {
		::wxMessageBox(_("The Data Port must be between 1 and 65536"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	if (IsModal()) {
		EndModal(wxID_OK);
	} else {
		SetReturnCode(wxID_OK);
		Show(false);
	}
}

wxString CEthernetDialog::getIPAddress() const
{
	return m_ipAddress;
}

long CEthernetDialog::getControlPort() const
{
	return m_controlPort;
}

long CEthernetDialog::getDataPort() const
{
	return m_dataPort;
}
