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

#include "PortDialog.h"

#include "SerialControl.h"


const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;


BEGIN_EVENT_TABLE(CPortDialog, wxDialog)
	EVT_BUTTON(wxID_OK, CPortDialog::onOK)
END_EVENT_TABLE()


CPortDialog::CPortDialog(wxWindow* parent, const wxString& title, const wxString& device, int pin, int id) :
wxDialog(parent, id, title),
m_devChoice(NULL),
m_pinChoice(NULL),
m_device(device),
m_pin(pin)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Device:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_devChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_devChoice, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Pin:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_pinChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_pinChoice->Append(wxT("RTS"));
	m_pinChoice->Append(wxT("DTR"));
	panelSizer->Add(m_pinChoice, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL, BORDER_SIZE);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	wxArrayString devs = CSerialControl::getDevices();

	for (unsigned int i = 0; i < devs.GetCount(); i++) {
		wxString dev = devs.Item(i);

		m_devChoice->Append(dev);

		if (!m_device.IsEmpty() && dev.IsSameAs(m_device))
			m_devChoice->SetSelection(i);
	}

	if (m_device.IsEmpty()) {
		m_devChoice->SetSelection(0);
		m_device = m_devChoice->GetStringSelection();
	}

	if (m_pin == -1) {
		m_pinChoice->SetSelection(0);
		m_pin = 0;
	} else {
		m_pinChoice->SetSelection(m_pin);
	}
}

CPortDialog::~CPortDialog()
{
}

void CPortDialog::onOK(wxCommandEvent& event)
{
	long dev = m_devChoice->GetSelection();
	if (dev == wxNOT_FOUND) {
		::wxMessageBox(_("The Device is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	long pin = m_pinChoice->GetSelection();
	if (pin == wxNOT_FOUND) {
		::wxMessageBox(_("The Pin is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	m_device = m_devChoice->GetStringSelection();
	m_pin    = m_pinChoice->GetSelection();

	if (IsModal()) {
		EndModal(wxID_OK);
	} else {
		SetReturnCode(wxID_OK);
		Show(false);
	}
}

wxString CPortDialog::getDevice() const
{
	return m_device;
}

int CPortDialog::getPin() const
{
	return m_pin;
}
