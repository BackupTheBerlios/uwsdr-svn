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

#include "FreqKeypad.h"
#include "UWSDRDefs.h"

const int BUTTON_1     = 15342;
const int BUTTON_2     = 15343;
const int BUTTON_3     = 15344;
const int BUTTON_4     = 15345;
const int BUTTON_5     = 15346;
const int BUTTON_6     = 15347;
const int BUTTON_7     = 15348;
const int BUTTON_8     = 15349;
const int BUTTON_9     = 15350;
const int BUTTON_0     = 15351;
const int BUTTON_POINT = 15352;
const int BUTTON_C     = 15353;

BEGIN_EVENT_TABLE(CFreqKeypad, wxDialog)
	EVT_BUTTON(BUTTON_1,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_2,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_3,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_4,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_5,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_6,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_7,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_8,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_9,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_0,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_C,     CFreqKeypad::onButton)
	EVT_BUTTON(BUTTON_POINT, CFreqKeypad::onButton)
	EVT_BUTTON(wxID_OK,      CFreqKeypad::onOK)
END_EVENT_TABLE()

CFreqKeypad::CFreqKeypad(wxWindow* parent, int id, const CFrequency& minFreq, const CFrequency& maxFreq) :
wxDialog(parent, id, wxString(_("Frequency Keypad"))),
m_minFreq(minFreq),
m_maxFreq(maxFreq),
m_frequency(0, 0),
m_text(NULL)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	m_text = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(FREQPAD_WIDTH * 3, -1));
	m_text->SetMaxLength(12);
	mainSizer->Add(m_text, 0, wxALL, BORDER_SIZE);

	wxGridSizer* buttonSizer = new wxGridSizer(3);

	wxButton* button1 = new wxButton(this, BUTTON_1, wxT("1"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button1);

	wxButton* button2 = new wxButton(this, BUTTON_2, wxT("2"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button2);

	wxButton* button3 = new wxButton(this, BUTTON_3, wxT("3"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button3);

	wxButton* button4 = new wxButton(this, BUTTON_4, wxT("4"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button4);

	wxButton* button5 = new wxButton(this, BUTTON_5, wxT("5"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button5);

	wxButton* button6 = new wxButton(this, BUTTON_6, wxT("6"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button6);

	wxButton* button7 = new wxButton(this, BUTTON_7, wxT("7"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button7);

	wxButton* button8 = new wxButton(this, BUTTON_8, wxT("8"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button8);

	wxButton* button9 = new wxButton(this, BUTTON_9, wxT("9"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button9);

	wxButton* buttonPoint = new wxButton(this, BUTTON_POINT, wxT("."), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(buttonPoint);

	wxButton* button0 = new wxButton(this, BUTTON_0, wxT("0"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(button0);

	wxButton* buttonC = new wxButton(this, BUTTON_C, wxT("C"), wxDefaultPosition, wxSize(FREQPAD_WIDTH, FREQPAD_HEIGHT));
	buttonSizer->Add(buttonC);

	mainSizer->Add(buttonSizer, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL, BORDER_SIZE);

	SetAutoLayout(true);
	Layout();

	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	SetSizer(mainSizer);
}

CFreqKeypad::~CFreqKeypad()
{
}

CFrequency CFreqKeypad::getFrequency() const
{
	return m_frequency;
}


void CFreqKeypad::onButton(wxCommandEvent& event)
{
	switch (event.GetId()) {
		case BUTTON_0:
			m_text->AppendText(wxT("0"));
			return;
		case BUTTON_1:
			m_text->AppendText(wxT("1"));
			return;
		case BUTTON_2:
			m_text->AppendText(wxT("2"));
			return;
		case BUTTON_3:
			m_text->AppendText(wxT("3"));
			return;
		case BUTTON_4:
			m_text->AppendText(wxT("4"));
			return;
		case BUTTON_5:
			m_text->AppendText(wxT("5"));
			return;
		case BUTTON_6:
			m_text->AppendText(wxT("6"));
			return;
		case BUTTON_7:
			m_text->AppendText(wxT("7"));
			return;
		case BUTTON_8:
			m_text->AppendText(wxT("8"));
			return;
		case BUTTON_9:
			m_text->AppendText(wxT("9"));
			return;
		case BUTTON_POINT:
			m_text->AppendText(wxT("."));
			return;
		case BUTTON_C: {
				int len = m_text->GetLastPosition();
				m_text->Remove(len - 1, len);
			}
			return;
	}
}

void CFreqKeypad::onOK(wxCommandEvent& event)
{
	wxString freq = m_text->GetValue();

	if (!m_frequency.setFrequency(freq)) {
		::wxBell();
		return;
	} else if (m_frequency >= m_maxFreq || m_frequency < m_minFreq) {
		::wxBell();
		return;
	}

	wxDialog::OnOK(event);
}
