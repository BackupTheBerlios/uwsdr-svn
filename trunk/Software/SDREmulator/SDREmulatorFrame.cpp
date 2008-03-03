/*
 *   Copyright (C) 2006-2008 by Jonathan Naylor G4KLX
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

#include "SDREmulatorFrame.h"
#include "SoundCardDialog.h"

#include <wx/statline.h>

#include "SignalReader.h"
#include "NullWriter.h"
#include "SoundFileReader.h"

const int MENU_INTERNAL_1 = 17551;
const int MENU_INTERNAL_2 = 17552;
const int MENU_FILE       = 17553;
const int MENU_CARD       = 17554;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 100;

const int MESSAGES_WIDTH  = 400;
const int MESSAGES_HEIGHT = 200;

const CFrequency maxFreq = CFrequency(2451, 0.0);
const CFrequency minFreq = CFrequency(2299, 0.0);

DEFINE_EVENT_TYPE(COMMAND_EVENT)

BEGIN_EVENT_TABLE(CSDREmulatorFrame, wxFrame)
	EVT_MENU(wxID_EXIT,       CSDREmulatorFrame::onExit)
	EVT_MENU(MENU_INTERNAL_1, CSDREmulatorFrame::onInternal1)
	EVT_MENU(MENU_INTERNAL_2, CSDREmulatorFrame::onInternal2)
	EVT_MENU(MENU_FILE,       CSDREmulatorFrame::onSoundFile)
	EVT_MENU(MENU_CARD,       CSDREmulatorFrame::onSoundCard)
	EVT_CLOSE(CSDREmulatorFrame::onClose)
	EVT_CUSTOM(COMMAND_EVENT, wxID_ANY, CSDREmulatorFrame::onCommand)
END_EVENT_TABLE()

CSDREmulatorFrame::CSDREmulatorFrame(const wxString& address, unsigned int port) :
wxFrame(NULL, -1, wxString(wxT("uWave SDR Emulator")), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
m_txFreq(),
m_rxFreq(),
m_txEnable(false),
m_rxEnable(false),
m_txOn(false),
m_data(NULL),
m_started(false),
m_message(),
m_messages(NULL)
{
	wxMenu* fileMenu = new wxMenu();
	fileMenu->Append(wxID_EXIT, wxT("Exit\tAlt-F4"));

	wxMenu* sourceMenu = new wxMenu();
	sourceMenu->AppendCheckItem(MENU_INTERNAL_1, wxT("Noisy Carrier"));
	sourceMenu->Check(MENU_INTERNAL_1,  true);
	sourceMenu->AppendCheckItem(MENU_INTERNAL_2, wxT("Clean Carrier"));
	sourceMenu->Check(MENU_INTERNAL_2,  false);
	sourceMenu->AppendCheckItem(MENU_CARD,       wxT("Sound Card"));
	sourceMenu->Check(MENU_CARD,        false);
	sourceMenu->AppendCheckItem(MENU_FILE,       wxT("Sound File..."));
	sourceMenu->Check(MENU_FILE,        false);

	m_menuBar = new wxMenuBar();
	m_menuBar->Append(fileMenu,   wxT("File"));
	m_menuBar->Append(sourceMenu, wxT("Source"));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label0 = new wxStaticText(panel, -1, wxT("Source:"));
	panelSizer->Add(label0, 0, wxALL, BORDER_SIZE);

	m_sourceLabel = new wxStaticText(panel, -1, wxT("Noisy Carrier"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_sourceLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label1 = new wxStaticText(panel, -1, wxT("Status:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_connectLabel = new wxStaticText(panel, -1, wxT("Not connected"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_connectLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, wxT("TX Freq:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_txFreqLabel = new wxStaticText(panel, -1, wxT("None"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_txFreqLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, wxT("RX Freq:"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_rxFreqLabel = new wxStaticText(panel, -1, wxT("None"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_rxFreqLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, wxT("TX Enabled:"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_txEnabledLabel = new wxStaticText(panel, -1, wxT("No"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_txEnabledLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label5 = new wxStaticText(panel, -1, wxT("RX Enabled:"));
	panelSizer->Add(label5, 0, wxALL, BORDER_SIZE);

	m_rxEnabledLabel = new wxStaticText(panel, -1, wxT("No"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_rxEnabledLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label6 = new wxStaticText(panel, -1, wxT("Transmitting:"));
	panelSizer->Add(label6, 0, wxALL, BORDER_SIZE);

	m_txOnLabel = new wxStaticText(panel, -1, wxT("No"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_txOnLabel, 0, wxALL, BORDER_SIZE);

	vertSizer->Add(panelSizer);

	m_messages = new wxListBox(panel, -1, wxDefaultPosition, wxSize(MESSAGES_WIDTH, MESSAGES_HEIGHT));
	vertSizer->Add(m_messages, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(vertSizer);

	mainSizer->Add(panel);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	SetMenuBar(m_menuBar);

	CSoundCardDialog soundCard(this);
	int ret = soundCard.ShowModal();
	if (ret != wxID_OK) {
		Close(true);
		return;
	}

	int inDev  = soundCard.getInDev();
	int outDev = soundCard.getOutDev();

	// Start the data reading and writing thread
	ret = createDataThread(address, port, inDev, outDev);
	if (!ret) {
		::wxMessageBox(wxT("Cannot open the control port.\nSee Emulator.log for details"));
		Close(true);
	}
}

CSDREmulatorFrame::~CSDREmulatorFrame()
{
}

bool CSDREmulatorFrame::createDataThread(const wxString& address, unsigned int port, int inDev, int outDev)
{
	m_data = new CDataControl(48000.0F, address, port, inDev, outDev);

	m_data->setCallback(this, 0);

	bool ret = m_data->open();
	if (!ret)
		return false;

	m_data->setMute(true);

	m_started = true;

	return true;
}

bool CSDREmulatorFrame::callback(char* buffer, unsigned int len, int WXUNUSED(id))
{
	buffer[len] = '\0';
	m_message = buffer;

	wxCommandEvent event(COMMAND_EVENT);
	AddPendingEvent(event);

	return true;
}

void CSDREmulatorFrame::onCommand(wxEvent& WXUNUSED(event))
{
	int pos = m_message.Find(wxT(';'));

	if (pos == -1 || m_message.length() < 3) {
		m_data->sendNAK(m_message);

		wxString text;

		text.Printf(wxT("==> %s;"), m_message.c_str());
		m_messages->Append(text);

		text.Printf(wxT("<== NK%s;"), m_message.c_str());
		m_messages->Append(text);

		return;
	}

	bool ack  = false;
	bool echo = true;
	wxString command = m_message.Left(2);

	if (command.Cmp(wxT("CF")) == 0) {
		long n;
		m_message.Mid(2).ToLong(&n);

		if (n >= 0L && n <= 65536L)
			ack = true;

		echo = true;
	} else if (command.Cmp(wxT("FR")) == 0) {
		wxString freqText = m_message.Mid(2);
		CFrequency freq = CFrequency(freqText);

		if (freq >= minFreq && freq < maxFreq) {
			m_rxFreq = freq;
			ack = true;
		}

		echo = false;
	} else if (command.Cmp(wxT("FT")) == 0) {
		wxString freqText = m_message.Mid(2);
		CFrequency freq = CFrequency(freqText);

		if (freq >= minFreq && freq < maxFreq) {
			m_txFreq = freq;
			ack = true;
		}	

		echo = false;
	} else if (command.Cmp(wxT("ET")) == 0) {
		long n;
		m_message.Mid(2).ToLong(&n);

		if (n == 0L || n == 1L) {
			m_txEnable = (n == 1L);
			ack = true;
		}
	} else if (command.Cmp(wxT("ER")) == 0) {
		long n;
		m_message.Mid(2).ToLong(&n);

		if (n == 0L || n == 1L) {
			m_rxEnable = (n == 1L);
			ack = true;

			m_data->setMute(!m_rxEnable);
		}
	} else if (command.Cmp(wxT("TX")) == 0) {
		long n;
		m_message.Mid(2).ToLong(&n);

		if (m_txEnable && (n == 0L || n == 1L)) {
			m_txOn = (n == 1L);
			ack = true;

			m_data->setTX(m_txOn);
		}
	} else if (command.Cmp(wxT("SI")) == 0) {
		ack = true;
	}

	if (ack) {
		m_data->sendACK(command.c_str());

		wxString text;

		if (echo) {
			text.Printf(wxT("==> %s;"), m_message.c_str());
			m_messages->Append(text);

			text.Printf(wxT("<== AK%s;"), command.c_str());
			m_messages->Append(text);
		}

		text.Printf(wxT("%s MHz"), m_txFreq.getString().c_str());
		m_txFreqLabel->SetLabel(text);

		text.Printf(wxT("%s MHz"), m_rxFreq.getString().c_str());
		m_rxFreqLabel->SetLabel(text);

		m_txEnabledLabel->SetLabel(m_txEnable ? wxT("Yes") : wxT("No"));
		m_rxEnabledLabel->SetLabel(m_rxEnable ? wxT("Yes") : wxT("No"));

		m_txOnLabel->SetLabel(m_txOn ? wxT("Yes") : wxT("No"));

		m_data->setTX(m_txOn);
	} else {
		m_data->sendNAK(command.c_str());

		wxString text;

		text.Printf(wxT("==> %s;"), m_message.c_str());
		m_messages->Append(text);

		text.Printf(wxT("<== NK%s;"), command.c_str());
		m_messages->Append(text);
	}
}

void CSDREmulatorFrame::onClose(wxCloseEvent& event)
{
	if (!event.CanVeto()) {
		Destroy();
		return;
	}

	if (m_txOn) {
		event.Veto();
		return;
	}

	int reply = ::wxMessageBox(wxT("Do you want to exit the uWave SDR Emulator"),
		wxT("Exit"),
		wxOK | wxCANCEL | wxICON_QUESTION);

	if (reply == wxOK) {
		if (m_started)
			m_data->close();
		Destroy();
	} else {
		event.Veto();
	}
}

void CSDREmulatorFrame::onExit(wxCommandEvent& WXUNUSED(event))
{
	if (m_txOn)
		return;

	int reply = ::wxMessageBox(wxT("Do you want to exit the uWave SDR Emulator"),
		wxT("Exit uWSDR"),
		wxOK | wxCANCEL | wxICON_QUESTION);

	if (reply == wxOK) {
		if (m_started)
			m_data->close();
		Destroy();
	}
}

void CSDREmulatorFrame::onInternal1(wxCommandEvent& WXUNUSED(event))
{
	m_menuBar->Check(MENU_INTERNAL_1, true);
	m_menuBar->Check(MENU_INTERNAL_2, false);
	m_menuBar->Check(MENU_FILE,       false);
	m_menuBar->Check(MENU_CARD,       false);

	m_sourceLabel->SetLabel(wxT("Noisy Carrier"));

	m_data->setSource(SOURCE_INTERNAL_1);
}

void CSDREmulatorFrame::onInternal2(wxCommandEvent& WXUNUSED(event))
{
	m_menuBar->Check(MENU_INTERNAL_1, false);
	m_menuBar->Check(MENU_INTERNAL_2, true);
	m_menuBar->Check(MENU_FILE,       false);
	m_menuBar->Check(MENU_CARD,       false);

	m_sourceLabel->SetLabel(wxT("Clean carrier"));

	m_data->setSource(SOURCE_INTERNAL_2);
}

void CSDREmulatorFrame::onSoundFile(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog fileDialog(this, wxT("Select a Wave File"), wxEmptyString, wxEmptyString, wxT("WAV files (*.wav)|*.WAV;*.wav"), wxFD_OPEN);
	int ret1 = fileDialog.ShowModal();

	if (ret1 == wxID_CANCEL)
		return;

	wxString fileName = fileDialog.GetPath();

	bool ret2 = m_data->setSoundFileReader(fileName);
	if (!ret2) {
		::wxMessageBox(wxT("Problem opening the sound file"));
		return;
	}

	m_menuBar->Check(MENU_INTERNAL_1, false);
	m_menuBar->Check(MENU_INTERNAL_2, false);
	m_menuBar->Check(MENU_FILE,       true);
	m_menuBar->Check(MENU_CARD,       false);

	m_sourceLabel->SetLabel(wxT("Sound File"));
}

void CSDREmulatorFrame::onSoundCard(wxCommandEvent& WXUNUSED(event))
{
	m_menuBar->Check(MENU_INTERNAL_1, false);
	m_menuBar->Check(MENU_INTERNAL_2, false);
	m_menuBar->Check(MENU_FILE,       false);
	m_menuBar->Check(MENU_CARD,       true);

	m_sourceLabel->SetLabel(wxT("Sound Card"));

	m_data->setSource(SOURCE_SOUNDCARD);
}
