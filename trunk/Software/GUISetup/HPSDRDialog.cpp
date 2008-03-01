/*
 *   Copyright (C) 2008 by Jonathan Naylor G4KLX
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

#include "HPSDRDialog.h"

const int BORDER_SIZE = 5;
const int CHOICE_WIDTH = 100;

const int C1_10MHZREF_MASK          = 0x0C;
const int C1_10MHZREF_ATLAS         = 0x00;
const int C1_10MHZREF_PENELOPE      = 0x04;
const int C1_10MHZREF_MERCURY       = 0x08;

const int C1_1228MHZSOURCE_MASK     = 0x10;
const int C1_1228MHZSOURCE_PENELOPE = 0x00;
const int C1_1228MHZSOURCE_MERCURY  = 0x10;

const int C1_CONFIG_MASK            = 0x60;
const int C1_CONFIG_PENELOPE        = 0x20;
const int C1_CONFIG_MERCURY         = 0x40;

const int C1_MIC_MASK               = 0x80;
const int C1_MIC_JANUS              = 0x00;
const int C1_MIC_PENELOPE           = 0x80;

const int C3_ATTEN_MASK             = 0x03;
const int C3_ATTEN_0DB              = 0x00;
const int C3_ATTEN_10DB             = 0x01;
const int C3_ATTEN_20DB             = 0x02;
const int C3_ATTEN_30DB             = 0x03;

const int C3_PREAMP_MASK            = 0x04;
const int C3_PREAMP_ON              = 0x04;

const int C3_DITHER_MASK            = 0x08;
const int C3_DITHER_ON              = 0x08;


const int CHECK_ATLAS_REF       = 8574;
const int CHECK_MERCURY_REF     = 8575;
const int CHECK_PENELOPE_REF    = 8576;

const int CHECK_MERCURY_SOURCE  = 8577;
const int CHECK_PENELOPE_SOURCE = 8578;

const int CHECK_JANUS_MIC       = 8579;
const int CHECK_PENELOPE_MIC    = 8580;

BEGIN_EVENT_TABLE(CHPSDRDialog, wxDialog)
	EVT_CHECKBOX(CHECK_ATLAS_REF,    CHPSDRDialog::onButton)
	EVT_CHECKBOX(CHECK_MERCURY_REF,  CHPSDRDialog::onButton)
	EVT_CHECKBOX(CHECK_PENELOPE_REF, CHPSDRDialog::onButton)

	EVT_CHECKBOX(CHECK_MERCURY_SOURCE,  CHPSDRDialog::onButton)
	EVT_CHECKBOX(CHECK_PENELOPE_SOURCE, CHPSDRDialog::onButton)

	EVT_CHECKBOX(CHECK_JANUS_MIC,    CHPSDRDialog::onButton)
	EVT_CHECKBOX(CHECK_PENELOPE_MIC, CHPSDRDialog::onButton)

	EVT_BUTTON(wxID_OK, CHPSDRDialog::onOK)
END_EVENT_TABLE()


CHPSDRDialog::CHPSDRDialog(wxWindow* parent, const wxString& title, int c0, int c1, int c2, int c3, int c4, bool rxonly, int id) :
wxDialog(parent, id, title),
m_atlas10mhzRef(NULL),
m_janusMic(NULL),
m_mercury10mhzRef(NULL),
m_mercury1228mhzSource(NULL),
m_mercuryAtten(NULL),
m_mercuryConfig(NULL),
m_mercuryDither(NULL),
m_mercuryPreamp(NULL),
m_penelope10mhzRef(NULL),
m_penelope1228mhzSource(NULL),
m_penelopeConfig(NULL),
m_penelopeMic(NULL),
m_rxonly(rxonly),
m_c0(c0),
m_c1(c1),
m_c2(c2),
m_c3(c3),
m_c4(c4)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxNotebook* noteBook = new wxNotebook(this, -1);

	noteBook->AddPage(createAtlasTab(noteBook), _("Atlas"), false);
	noteBook->AddPage(createJanusTab(noteBook), _("Janus"), false);
	noteBook->AddPage(createMercuryTab(noteBook), _("Mercury"), true);
	noteBook->AddPage(createPenelopeTab(noteBook), _("Penelope"), false);

	mainSizer->Add(noteBook, 1, wxALL | wxGROW, BORDER_SIZE);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL | wxALIGN_RIGHT, BORDER_SIZE);

	SetAutoLayout(true);
	Layout();

	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	SetSizer(mainSizer);

	switch (c1 & C1_10MHZREF_MASK) {
		case C1_10MHZREF_ATLAS:
			m_atlas10mhzRef->SetValue(true);
			break;
		case C1_10MHZREF_MERCURY:
			m_mercury10mhzRef->SetValue(true);
			break;
		case C1_10MHZREF_PENELOPE:
			m_penelope10mhzRef->SetValue(true);
			break;
	}

	switch (c1 & C1_1228MHZSOURCE_MASK) {
		case C1_1228MHZSOURCE_MERCURY:
			m_mercury1228mhzSource->SetValue(true);
			break;
		case C1_1228MHZSOURCE_PENELOPE:
			m_penelope1228mhzSource->SetValue(true);
			break;
	}

	switch (c1 & C1_CONFIG_MASK) {
		case C1_CONFIG_MERCURY:
			m_mercuryConfig->SetValue(true);
			break;
		case C1_CONFIG_PENELOPE:
			m_penelopeConfig->SetValue(true);
			break;
		case C1_CONFIG_MERCURY | C1_CONFIG_PENELOPE:
			m_mercuryConfig->SetValue(true);
			m_penelopeConfig->SetValue(true);
			break;
	}

	if (!m_rxonly) {
		switch (c1 & C1_MIC_MASK) {
			case C1_MIC_JANUS:
				m_janusMic->SetValue(true);
				break;
			case C1_MIC_PENELOPE:
				m_penelopeMic->SetValue(true);
				break;
		}
	}

	switch (c3 & C3_ATTEN_MASK) {
		case C3_ATTEN_0DB:
			m_mercuryAtten->SetSelection(0);
			break;
		case C3_ATTEN_10DB:
			m_mercuryAtten->SetSelection(1);
			break;
		case C3_ATTEN_20DB:
			m_mercuryAtten->SetSelection(2);
			break;
		case C3_ATTEN_30DB:
			m_mercuryAtten->SetSelection(3);
			break;
	}

	if (c3 & C3_PREAMP_ON)
		m_mercuryPreamp->SetValue(true);

	if (c3 & C3_DITHER_ON)
		m_mercuryDither->SetValue(true);

	if (m_rxonly) {
		m_janusMic->Enable(false);
		m_penelope10mhzRef->Enable(false);
		m_penelope1228mhzSource->Enable(false);
		m_penelopeConfig->Enable(false);
		m_penelopeMic->Enable(false);
	}
}

CHPSDRDialog::~CHPSDRDialog()
{
}

wxPanel* CHPSDRDialog::createAtlasTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label = new wxStaticText(panel, -1, _("10MHz Reference"));
	panelSizer->Add(label, 0, wxALL, BORDER_SIZE);

	m_atlas10mhzRef = new wxCheckBox(panel, CHECK_ATLAS_REF, wxEmptyString);
	panelSizer->Add(m_atlas10mhzRef, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(panelSizer, 0, wxTOP, BORDER_SIZE);

	panel->SetAutoLayout(true);

	panelSizer->Fit(panel);
	panelSizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CHPSDRDialog::createJanusTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label = new wxStaticText(panel, -1, _("Microphone"));
	panelSizer->Add(label, 0, wxALL, BORDER_SIZE);

	m_janusMic = new wxCheckBox(panel, CHECK_JANUS_MIC, wxEmptyString);
	panelSizer->Add(m_janusMic, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(panelSizer, 0, wxTOP, BORDER_SIZE);

	panel->SetAutoLayout(true);

	panelSizer->Fit(panel);
	panelSizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CHPSDRDialog::createMercuryTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("10MHz Reference"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_mercury10mhzRef = new wxCheckBox(panel, CHECK_MERCURY_REF, wxEmptyString);
	panelSizer->Add(m_mercury10mhzRef, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("122.8MHz Source"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_mercury1228mhzSource = new wxCheckBox(panel, CHECK_MERCURY_SOURCE, wxEmptyString);
	panelSizer->Add(m_mercury1228mhzSource, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Configuration"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_mercuryConfig = new wxCheckBox(panel, -1, wxEmptyString);
	panelSizer->Add(m_mercuryConfig, 0, wxALL, BORDER_SIZE);

	wxStaticText* label5 = new wxStaticText(panel, -1, _("LT2208 Preamp"));
	panelSizer->Add(label5, 0, wxALL, BORDER_SIZE);

	m_mercuryPreamp = new wxCheckBox(panel, -1, wxEmptyString);
	panelSizer->Add(m_mercuryPreamp, 0, wxALL, BORDER_SIZE);

	wxStaticText* label6 = new wxStaticText(panel, -1, _("LT2208 Dither"));
	panelSizer->Add(label6, 0, wxALL, BORDER_SIZE);

	m_mercuryDither = new wxCheckBox(panel, -1, wxEmptyString);
	panelSizer->Add(m_mercuryDither, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("Attenuator"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_mercuryAtten = new wxChoice(panel, -1, wxDefaultPosition, wxSize(CHOICE_WIDTH, -1));
	m_mercuryAtten->Append(_("0dB"));
	m_mercuryAtten->Append(_("10dB"));
	m_mercuryAtten->Append(_("20dB"));
	m_mercuryAtten->Append(_("30dB"));
	panelSizer->Add(m_mercuryAtten, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(panelSizer, 0, wxTOP, BORDER_SIZE);

	panel->SetAutoLayout(true);

	panelSizer->Fit(panel);
	panelSizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

wxPanel* CHPSDRDialog::createPenelopeTab(wxNotebook* noteBook)
{
	wxPanel* panel = new wxPanel(noteBook, -1);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("10MHz Reference"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_penelope10mhzRef = new wxCheckBox(panel, CHECK_PENELOPE_REF, wxEmptyString);
	panelSizer->Add(m_penelope10mhzRef, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("122.8MHz Source"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_penelope1228mhzSource = new wxCheckBox(panel, CHECK_PENELOPE_SOURCE, wxEmptyString);
	panelSizer->Add(m_penelope1228mhzSource, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Configuration"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_penelopeConfig = new wxCheckBox(panel, -1, wxEmptyString);
	panelSizer->Add(m_penelopeConfig, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("Microphone"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_penelopeMic = new wxCheckBox(panel, CHECK_PENELOPE_MIC, wxEmptyString);
	panelSizer->Add(m_penelopeMic, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(panelSizer, 0, wxTOP, BORDER_SIZE);

	panel->SetAutoLayout(true);

	panelSizer->Fit(panel);
	panelSizer->SetSizeHints(panel);

	panel->SetSizer(mainSizer);

	return panel;
}

void CHPSDRDialog::onButton(wxCommandEvent& event)
{
	bool checked = event.IsChecked();
	int id = event.GetId();

	switch (id) {
		case CHECK_ATLAS_REF:
			m_mercury10mhzRef->SetValue(!checked);
			m_penelope10mhzRef->SetValue(false);
			break;
		case CHECK_MERCURY_REF:
			m_atlas10mhzRef->SetValue(!checked);
			m_penelope10mhzRef->SetValue(false);
			break;
		case CHECK_PENELOPE_REF:
			m_atlas10mhzRef->SetValue(!checked);
			m_mercury10mhzRef->SetValue(false);
			break;

		case CHECK_MERCURY_SOURCE:
			m_penelope1228mhzSource->SetValue(!checked);
			break;
		case CHECK_PENELOPE_SOURCE:
			m_mercury1228mhzSource->SetValue(!checked);
			break;

		case CHECK_JANUS_MIC:
			m_penelopeMic->SetValue(!checked);
			break;
		case CHECK_PENELOPE_MIC:
			m_janusMic->SetValue(!checked);
			break;
	}
}

void CHPSDRDialog::onOK(wxCommandEvent& WXUNUSED(event))
{
	m_c0 = 0x00;
	m_c1 = 0x00;
	m_c2 = 0x00;
	m_c3 = 0x00;
	m_c4 = 0x00;

	if (m_atlas10mhzRef->IsChecked())
		m_c1 |= C1_10MHZREF_ATLAS;

	if (m_mercury10mhzRef->IsChecked())
		m_c1 |= C1_10MHZREF_MERCURY;

	if (m_penelope10mhzRef->IsChecked() && !m_rxonly)
		m_c1 |= C1_10MHZREF_PENELOPE;

	if (m_mercury1228mhzSource->IsChecked())
		m_c1 |= C1_1228MHZSOURCE_MERCURY;

	if (m_penelope1228mhzSource->IsChecked() && !m_rxonly)
		m_c1 |= C1_1228MHZSOURCE_PENELOPE;

	if (m_mercuryConfig->IsChecked())
		m_c1 |= C1_CONFIG_MERCURY;

	if (m_penelopeConfig->IsChecked() && !m_rxonly)
		m_c1 |= C1_CONFIG_PENELOPE;

	if (m_janusMic->IsChecked() && !m_rxonly)
		m_c1 |= C1_MIC_JANUS;

	if (m_penelopeMic->IsChecked() && !m_rxonly)
		m_c1 |= C1_MIC_PENELOPE;

	long atten = m_mercuryAtten->GetSelection();
	switch (atten) {
		case 0L: m_c3 |= C3_ATTEN_0DB;  break;
		case 1L: m_c3 |= C3_ATTEN_10DB; break;
		case 2L: m_c3 |= C3_ATTEN_20DB; break;
		case 3L: m_c3 |= C3_ATTEN_30DB; break;
	}

	if (m_mercuryPreamp->IsChecked())
		m_c3 |= C3_PREAMP_ON;

	if (m_mercuryDither->IsChecked())
		m_c3 |= C3_DITHER_ON;

	if (IsModal()) {
		EndModal(wxID_OK);
	} else {
		SetReturnCode(wxID_OK);
		Show(false);
	}
}

int CHPSDRDialog::getC0() const
{
	return m_c0;
}

int CHPSDRDialog::getC1() const
{
	return m_c1;
}

int CHPSDRDialog::getC2() const
{
	return m_c2;
}

int CHPSDRDialog::getC3() const
{
	return m_c3;
}

int CHPSDRDialog::getC4() const
{
	return m_c4;
}
