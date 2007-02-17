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

#include "CWKeyboard.h"
#include "UWSDRApp.h"

#include <wx/statline.h>

/*
 * This is the lookup table to convert characters to morse code.
 */
static struct {
	wxChar  character;
	wxChar* cwString;
} cwLookup[] = {
	{wxT('A'), wxT(".-")},
	{wxT('B'), wxT("-...")},
	{wxT('C'), wxT("-.-.")},
	{wxT('D'), wxT("-..")},
	{wxT('E'), wxT(".")},
	{wxT('F'), wxT("..-.")},
	{wxT('G'), wxT("--.")},
	{wxT('H'), wxT("....")},
	{wxT('I'), wxT("..")},
	{wxT('J'), wxT(".---")},
	{wxT('K'), wxT("-.-")},
	{wxT('L'), wxT(".-..")},
	{wxT('M'), wxT("--")},
	{wxT('N'), wxT("-.")},
	{wxT('O'), wxT("---")},
	{wxT('P'), wxT(".--.")},
	{wxT('Q'), wxT("--.-")},
	{wxT('R'), wxT(".-.")},
	{wxT('S'), wxT("...")},
	{wxT('T'), wxT("-")},
	{wxT('U'), wxT("..-")},
	{wxT('V'), wxT("...-")},
	{wxT('W'), wxT(".--")},
	{wxT('X'), wxT("-..-")},
	{wxT('Y'), wxT("-.--")},
	{wxT('Z'), wxT("--..")},

	{wxT('1'), wxT(".----")},
	{wxT('2'), wxT("..---")},
	{wxT('3'), wxT("...--")},
	{wxT('4'), wxT("....-")},
	{wxT('5'), wxT(".....")},
	{wxT('6'), wxT("-....")},
	{wxT('7'), wxT("--...")},
	{wxT('8'), wxT("---..")},
	{wxT('9'), wxT("----.")},
	{wxT('0'), wxT("-----")},

	{wxT('/'), wxT("-..-.")},
	{wxT('?'), wxT("..--..")},
	{wxT(','), wxT("--..--")},
	{wxT('='), wxT("-...-")},
	{wxT('*'), wxT("...-.-")},
	{wxT('.'), wxT(".-.-.")},

	{wxT(' '), wxT(" ")}
};

const int cwLookupLen = sizeof(cwLookup) / sizeof(cwLookup[0]);

enum {
	BUTTON_0 = 18344,
	BUTTON_1,
	BUTTON_2,
	BUTTON_3,
	BUTTON_4,
	BUTTON_5,
	BUTTON_6,
	BUTTON_7,
	BUTTON_8,
	BUTTON_9,
	BUTTON_10,
	BUTTON_TRANSMIT,
	BUTTON_ABORT
};

BEGIN_EVENT_TABLE(CCWKeyboard, wxDialog)
	EVT_BUTTON(BUTTON_TRANSMIT, CCWKeyboard::onTransmit)
	EVT_BUTTON(BUTTON_ABORT,    CCWKeyboard::onAbort)
	EVT_BUTTON(wxID_HELP,       CCWKeyboard::onHelp)
END_EVENT_TABLE()

CCWKeyboard::CCWKeyboard(wxWindow* parent, int id) :
wxDialog(parent, id, wxString(_("CW Keyboard"))),
m_local(NULL),
m_remote(NULL),
m_locator(NULL),
m_report(NULL),
m_serial(NULL),
m_text(),
m_button(),
m_speed(NULL)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* entrySizer = new wxFlexGridSizer(6);

	wxStaticText* labelLocal = new wxStaticText(this, -1, _("My callsign (%M):"));
	entrySizer->Add(labelLocal, 0, wxALL, BORDER_SIZE);

	m_local = new wxTextCtrl(this, -1);
	m_local->SetMaxLength(12);
	entrySizer->Add(m_local, 0, wxALL, BORDER_SIZE);

	wxStaticText* labelRemote = new wxStaticText(this, -1, _("Remote callsign (%R):"));
	entrySizer->Add(labelRemote, 0, wxALL, BORDER_SIZE);

	m_remote = new wxTextCtrl(this, -1);
	m_remote->SetMaxLength(12);
	entrySizer->Add(m_remote, 0, wxALL, BORDER_SIZE);

	wxStaticText* labelLocator = new wxStaticText(this, -1, _("Locator (%Q):"));
	entrySizer->Add(labelLocator, 0, wxALL, BORDER_SIZE);

	m_locator = new wxTextCtrl(this, -1);
	m_locator->SetMaxLength(12);
	entrySizer->Add(m_locator, 0, wxALL, BORDER_SIZE);

	wxStaticText* labelReport = new wxStaticText(this, -1, _("Report (%S):"));
	entrySizer->Add(labelReport, 0, wxALL, BORDER_SIZE);

	m_report = new wxTextCtrl(this, -1);
	m_report->SetMaxLength(8);
	entrySizer->Add(m_report, 0, wxALL, BORDER_SIZE);

	wxStaticText* labelSerial = new wxStaticText(this, -1, _("Serial number (%N):"));
	entrySizer->Add(labelSerial, 0, wxALL, BORDER_SIZE);

	m_serial = new wxTextCtrl(this, -1);
	m_serial->SetMaxLength(8);
	entrySizer->Add(m_serial, 0, wxALL, BORDER_SIZE);

	wxStaticText* speedLabel = new wxStaticText(this, -1, _("Speed (WPM):"));
	entrySizer->Add(speedLabel, 0, wxALL, BORDER_SIZE);

	m_speed = new wxSpinCtrl(this, -1);
	m_speed->SetRange(5, 30);
	entrySizer->Add(m_speed, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(entrySizer, 0, wxALL, BORDER_SIZE);

	wxStaticLine* line1 = new wxStaticLine(this, -1, wxDefaultPosition, wxSize(CWKEYB_WIDTH, -1), wxLI_HORIZONTAL);
	mainSizer->Add(line1, 0, wxALL, BORDER_SIZE);

	wxFlexGridSizer* textSizer = new wxFlexGridSizer(3);

	for (int i = 0; i < CWKEYBOARD_COUNT; i++) {
		wxString text;
		text.Printf(_("Message %d:"), i + 1);

		wxStaticText* label = new wxStaticText(this, -1, text);
		textSizer->Add(label, 0, wxALL, BORDER_SIZE);

		m_text[i] = new wxTextCtrl(this, -1, wxEmptyString, wxDefaultPosition, wxSize(CWTEXT_WIDTH, -1));
		m_text[i]->SetMaxLength(200);
		textSizer->Add(m_text[i], 0, wxALL, BORDER_SIZE);

		m_button[i] = new wxRadioButton(this, BUTTON_0 + i, wxEmptyString, wxDefaultPosition, wxDefaultSize, (i == 0) ? wxRB_GROUP : 0);
		m_button[i]->SetValue(i == 0);
		textSizer->Add(m_button[i], 0, wxALL, BORDER_SIZE);
	}

	mainSizer->Add(textSizer, 0, wxALL, BORDER_SIZE);

	wxStaticLine* line2 = new wxStaticLine(this, -1, wxDefaultPosition, wxSize(CWKEYB_WIDTH, -1), wxLI_HORIZONTAL);
	mainSizer->Add(line2, 0, wxALL, BORDER_SIZE);

	wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

	wxButton* transmitButton = new wxButton(this, BUTTON_TRANSMIT, _("Transmit"));
	buttonSizer->Add(transmitButton, 0, wxALL, BORDER_SIZE);

	wxButton* abortButton = new wxButton(this, BUTTON_ABORT, _("Abort"));
	buttonSizer->Add(abortButton, 0, wxALL, BORDER_SIZE);

	wxButton* closeButton = new wxButton(this, wxID_CANCEL, _("Close"));
	buttonSizer->Add(closeButton, 0, wxALL, BORDER_SIZE);

	wxButton* helpButton = new wxButton(this, wxID_HELP, _("Help"));
	buttonSizer->Add(helpButton, 0, wxALL, BORDER_SIZE);

	mainSizer->Add(buttonSizer, 0, wxALL, BORDER_SIZE);

	SetAutoLayout(true);
	Layout();

	mainSizer->Fit(this);
	mainSizer->SetSizeHints(this);

	SetSizer(mainSizer);
}

CCWKeyboard::~CCWKeyboard()
{
}

void CCWKeyboard::setLocal(const wxString& text)
{
	m_local->SetValue(text);
}

wxString CCWKeyboard::getLocal() const
{
	return m_local->GetValue();
}

void CCWKeyboard::setRemote(const wxString& text)
{
	m_remote->SetValue(text);
}

wxString CCWKeyboard::getRemote() const
{
	return m_remote->GetValue();
}

void CCWKeyboard::setLocator(const wxString& text)
{
	m_locator->SetValue(text);
}

wxString CCWKeyboard::getLocator() const
{
	return m_locator->GetValue();
}

void CCWKeyboard::setReport(const wxString& text)
{
	m_report->SetValue(text);
}

wxString CCWKeyboard::getReport() const
{
	return m_report->GetValue();
}

void CCWKeyboard::setSerial(const wxString& text)
{
	m_serial->SetValue(text);
}

wxString CCWKeyboard::getSerial() const
{
	return m_serial->GetValue();
}

void CCWKeyboard::setMessage(unsigned int n, const wxString& text)
{
	wxASSERT(n < CWKEYBOARD_COUNT);

	m_text[n]->SetValue(text);
}

wxString CCWKeyboard::getMessage(unsigned int n) const
{
	wxASSERT(n < CWKEYBOARD_COUNT);

	return m_text[n]->GetValue();
}

void CCWKeyboard::setSpeed(unsigned int speed)
{
	wxASSERT(speed >= 5 && speed <= 30);

	m_speed->SetValue(speed);
}

unsigned int CCWKeyboard::getSpeed() const
{
	return m_speed->GetValue();
}

void CCWKeyboard::onHelp(wxCommandEvent& WXUNUSED(event))
{
	::wxGetApp().showHelp(500);
}

/*
 * Convert the input string to the required dashes, dots, and spaces. This is where
 * substitutions are also done.
 */
void CCWKeyboard::onTransmit(wxCommandEvent& WXUNUSED(event))
{
	wxString text;

	for (int i = 0; i < CWKEYBOARD_COUNT; i++) {
		if (m_button[i]->GetValue()) {
			text = m_text[i]->GetValue();
			break;
		}
	}

	text.UpperCase();

	text.Replace(wxT("%M"), m_local->GetValue(), true);
	text.Replace(wxT("%R"), m_remote->GetValue(), true);
	text.Replace(wxT("%Q"), m_locator->GetValue(), true);
	text.Replace(wxT("%S"), m_report->GetValue(), true);
	text.Replace(wxT("%N"), m_serial->GetValue(), true);

	text.UpperCase();

	wxString cwData = wxT(" ");

	for (unsigned int n = 0; n < text.length(); n++) {
		wxChar c = text.GetChar(n);

		for (int m = 0; m < cwLookupLen; m++)	{
			if (cwLookup[m].character == c) {
				cwData.Append(cwLookup[m].cwString);
				cwData.Append(wxT(" "));
				break;
			}
		}
	}

	cwData.Append(wxT(" "));

	::wxGetApp().sendCW(getSpeed(), cwData);
}

/*
 * Tell the system to stop transmitting.
 */
void CCWKeyboard::onAbort(wxCommandEvent& WXUNUSED(event))
{
	::wxGetApp().sendCW(0, wxEmptyString);
}
