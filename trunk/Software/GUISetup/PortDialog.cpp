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

enum {
	IN_RTS_CTS,
	IN_RTS_DSR,
	IN_DTR_DSR,
	IN_DTR_CTS
};

enum {
	OUT_RTS,
	OUT_DTR
};

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 100;

const int CHECK_TXIN  = 14286;
const int CHECK_KEYIN = 14287;

BEGIN_EVENT_TABLE(CPortDialog, wxDialog)
	EVT_CHECKBOX(CHECK_TXIN,  CPortDialog::onTXInCheck)
	EVT_CHECKBOX(CHECK_KEYIN, CPortDialog::onKeyInCheck)
	EVT_BUTTON(wxID_OK, CPortDialog::onOK)
END_EVENT_TABLE()


CPortDialog::CPortDialog(wxWindow* parent, const wxString& title, bool setTXOut, int id) :
wxDialog(parent, id, title),
m_txInSelect(NULL),
m_txInDevChoice(NULL),
m_txInPinChoice(NULL),
m_keyInSelect(NULL),
m_keyInDevChoice(NULL),
m_keyInPinChoice(NULL),
m_txOutDevChoice(NULL),
m_txOutPinChoice(NULL),
m_txInEnable(true),
m_txInDev(),
m_txInPin(-1),
m_keyInEnable(true),
m_keyInDev(),
m_keyInPin(-1),
m_txOutEnable(setTXOut),
m_txOutDev(),
m_txOutPin(-1)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(4);

	wxStaticText* dummy1 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy1, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy2 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy2, 0, wxALL, BORDER_SIZE);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Device"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Pin(s)"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Transmit In"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_txInSelect = new wxCheckBox(panel, CHECK_TXIN, wxEmptyString);
	panelSizer->Add(m_txInSelect, 0, wxALL, BORDER_SIZE);

	m_txInDevChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_txInDevChoice, 0, wxALL, BORDER_SIZE);

	m_txInPinChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_txInPinChoice->Append(wxT("RTS + CTS"));
	m_txInPinChoice->Append(wxT("RTS + DSR"));
	m_txInPinChoice->Append(wxT("DTR + DSR"));
	m_txInPinChoice->Append(wxT("DTR + CTS"));
	panelSizer->Add(m_txInPinChoice, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("Key In"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_keyInSelect = new wxCheckBox(panel, CHECK_KEYIN, wxEmptyString);
	panelSizer->Add(m_keyInSelect, 0, wxALL, BORDER_SIZE);

	m_keyInDevChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_keyInDevChoice, 0, wxALL, BORDER_SIZE);

	m_keyInPinChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_keyInPinChoice->Append(wxT("RTS + CTS"));
	m_keyInPinChoice->Append(wxT("RTS + DSR"));
	m_keyInPinChoice->Append(wxT("DTR + DSR"));
	m_keyInPinChoice->Append(wxT("DTR + CTS"));
	panelSizer->Add(m_keyInPinChoice, 0, wxALL, BORDER_SIZE);

	wxStaticText* label5 = new wxStaticText(panel, -1, _("Transmit Out"));
	panelSizer->Add(label5, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy3 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy3, 0, wxALL, BORDER_SIZE);

	m_txOutDevChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_txOutDevChoice, 0, wxALL, BORDER_SIZE);

	m_txOutPinChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_txOutPinChoice->Append(wxT("RTS"));
	m_txOutPinChoice->Append(wxT("DTR"));
	panelSizer->Add(m_txOutPinChoice, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL, BORDER_SIZE);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	m_txOutDevChoice->Enable(setTXOut);
	m_txOutPinChoice->Enable(setTXOut);

	wxArrayString devs = CSerialControl::getDevs();

	for (unsigned int i = 0; i < devs.GetCount(); i++) {
		wxString dev = devs.Item(i);

		m_txInDevChoice->Append(dev);
		m_keyInDevChoice->Append(dev);
		m_txOutDevChoice->Append(dev);
	}
}

CPortDialog::~CPortDialog()
{
}

void CPortDialog::onTXInCheck(wxCommandEvent& event)
{
	bool enable = event.IsChecked();

	m_txInDevChoice->Enable(enable);
	m_txInPinChoice->Enable(enable);
}

void CPortDialog::onKeyInCheck(wxCommandEvent& event)
{
	bool enable = event.IsChecked();

	m_keyInDevChoice->Enable(enable);
	m_keyInPinChoice->Enable(enable);
}

void CPortDialog::onOK(wxCommandEvent& event)
{
	m_txInEnable = m_txInSelect->GetValue();

	if (m_txInEnable) {
		long dev = m_txInDevChoice->GetSelection();
		if (dev == wxNOT_FOUND) {
			::wxMessageBox(_("The Transmit In Device is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		long pin = m_txInPinChoice->GetSelection();
		if (pin == wxNOT_FOUND) {
			::wxMessageBox(_("The Transmit In Pin is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	m_keyInEnable = m_keyInSelect->GetValue();

	if (m_keyInEnable) {
		long dev = m_keyInDevChoice->GetSelection();
		if (dev == wxNOT_FOUND) {
			::wxMessageBox(_("The Key In Device is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		long pin = m_keyInPinChoice->GetSelection();
		if (pin == wxNOT_FOUND) {
			::wxMessageBox(_("The Key In Pin is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (m_txOutEnable) {
		long dev = m_txOutDevChoice->GetSelection();
		if (dev == wxNOT_FOUND) {
			::wxMessageBox(_("The Transmit Out Device is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		long pin = m_txOutPinChoice->GetSelection();
		if (pin == wxNOT_FOUND) {
			::wxMessageBox(_("The Transmit Out Pin is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (m_txInEnable && m_keyInEnable) {
		wxString  txInDev = m_txInDevChoice->GetStringSelection();
		int       txInPin = m_txInPinChoice->GetSelection();
		wxString keyInDev = m_keyInDevChoice->GetStringSelection();
		int      keyInPin = m_keyInPinChoice->GetSelection();

		if (txInDev.IsSameAs(keyInDev)) {
			if (txInPin == IN_RTS_CTS && keyInPin == IN_RTS_CTS ||
				txInPin == IN_RTS_CTS && keyInPin == IN_DTR_CTS ||
				txInPin == IN_RTS_DSR && keyInPin == IN_RTS_DSR ||
				txInPin == IN_RTS_DSR && keyInPin == IN_DTR_DSR ||
			    txInPin == IN_DTR_DSR && keyInPin == IN_RTS_DSR ||
				txInPin == IN_DTR_DSR && keyInPin == IN_DTR_DSR ||
				txInPin == IN_DTR_CTS && keyInPin == IN_RTS_CTS ||
				txInPin == IN_DTR_CTS && keyInPin == IN_DTR_CTS) {
				::wxMessageBox(_("The Transmit In and Key In may not conflict"), _("GUISetup Error"), wxICON_ERROR);
				return;
			}
		}
	}

	if (m_txInEnable && m_txOutEnable) {
		wxString  txInDev = m_txInDevChoice->GetStringSelection();
		int       txInPin = m_txInPinChoice->GetSelection();
		wxString txOutDev = m_txOutDevChoice->GetStringSelection();
		int      txOutPin = m_txOutPinChoice->GetSelection();

		if (txInDev.IsSameAs(txOutDev)) {
			if ((txInPin == IN_RTS_CTS || txInPin == IN_RTS_DSR) && txOutPin == OUT_RTS ||
				(txInPin == IN_DTR_CTS || txInPin == IN_DTR_DSR) && txOutPin == OUT_DTR) {
				::wxMessageBox(_("The Transmit In and Transmit Out may not conflict"), _("GUISetup Error"), wxICON_ERROR);
				return;
			}
		}
	}

	if (m_keyInEnable && m_txOutEnable) {
		wxString keyInDev = m_keyInDevChoice->GetStringSelection();
		int      keyInPin = m_keyInPinChoice->GetSelection();
		wxString txOutDev = m_txOutDevChoice->GetStringSelection();
		int      txOutPin = m_txOutPinChoice->GetSelection();

		if (keyInDev.IsSameAs(txOutDev)) {
			if ((keyInPin == IN_RTS_CTS || keyInPin == IN_RTS_DSR) && txOutPin == OUT_RTS ||
				(keyInPin == IN_DTR_CTS || keyInPin == IN_DTR_DSR) && txOutPin == OUT_DTR) {
				::wxMessageBox(_("The Key In and Transmit Out may not conflict"), _("GUISetup Error"), wxICON_ERROR);
				return;
			}
		}
	}

	if (m_txInEnable) {
		m_txInDev = m_txInDevChoice->GetStringSelection();
		m_txInPin = m_txInPinChoice->GetSelection();
	}

	if (m_keyInEnable) {
		m_keyInDev = m_keyInDevChoice->GetStringSelection();
		m_keyInPin = m_keyInPinChoice->GetSelection();
	}

	if (m_txOutEnable) {
		m_txOutDev = m_txOutDevChoice->GetStringSelection();
		m_txOutPin = m_txOutPinChoice->GetSelection();
	}

	if (IsModal()) {
		EndModal(wxID_OK);
	} else {
		SetReturnCode(wxID_OK);
		Show(false);
	}
}

void CPortDialog::setTXInEnable(bool enable)
{
	m_txInEnable = enable;

	m_txInSelect->SetValue(enable);
	m_txInDevChoice->Enable(enable);
	m_txInPinChoice->Enable(enable);
}

void CPortDialog::setTXInDev(const wxString& dev)
{
	m_txInDev = dev;

	if (m_txInDev.IsEmpty()) {
		m_txInDevChoice->SetSelection(0);
		m_txInDev = m_txInDevChoice->GetStringSelection();
	} else {
		m_txInDevChoice->SetStringSelection(dev);
	}
}

void CPortDialog::setTXInPin(int pin)
{
	m_txInPin = pin;

	if (m_txInPin == -1) {
		m_txInPinChoice->SetSelection(0);
		m_txInPin = 0;
	} else {
		m_txInPinChoice->SetSelection(m_txInPin);
	}
}

bool CPortDialog::getTXInEnable() const
{
	return m_txInEnable;
}

wxString CPortDialog::getTXInDev() const
{
	return m_txInDev;
}

int CPortDialog::getTXInPin() const
{
	return m_txInPin;
}

void CPortDialog::setKeyInEnable(bool enable)
{
	m_keyInEnable = enable;

	m_keyInSelect->SetValue(enable);
	m_keyInDevChoice->Enable(enable);
	m_keyInPinChoice->Enable(enable);
}

void CPortDialog::setKeyInDev(const wxString& dev)
{
	m_keyInDev = dev;

	if (m_keyInDev.IsEmpty()) {
		m_keyInDevChoice->SetSelection(0);
		m_keyInDev = m_keyInDevChoice->GetStringSelection();
	} else {
		m_keyInDevChoice->SetStringSelection(dev);
	}
}

void CPortDialog::setKeyInPin(int pin)
{
	m_keyInPin = pin;

	if (m_keyInPin == -1) {
		m_keyInPinChoice->SetSelection(0);
		m_keyInPin = 0;
	} else {
		m_keyInPinChoice->SetSelection(m_keyInPin);
	}
}

bool CPortDialog::getKeyInEnable() const
{
	return m_keyInEnable;
}

wxString CPortDialog::getKeyInDev() const
{
	return m_keyInDev;
}

int CPortDialog::getKeyInPin() const
{
	return m_keyInPin;
}

void CPortDialog::setTXOutDev(const wxString& dev)
{
	m_txOutDev = dev;

	if (m_txOutDev.IsEmpty()) {
		m_txOutDevChoice->SetSelection(0);
		m_txOutDev = m_txOutDevChoice->GetStringSelection();
	} else {
		m_txOutDevChoice->SetStringSelection(dev);
	}
}

void CPortDialog::setTXOutPin(int pin)
{
	m_txOutPin = pin;

	if (m_txOutPin == -1) {
		m_txOutPinChoice->SetSelection(0);
		m_txOutPin = 0;
	} else {
		m_txOutPinChoice->SetSelection(m_txOutPin);
	}
}

wxString CPortDialog::getTXOutDev() const
{
	return m_txOutDev;
}

int CPortDialog::getTXOutPin() const
{
	return m_txOutPin;
}
