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

#include "SDREmulatorFrame.h"

#include <wx/statline.h>

const int SOCKET_PARENT = 37549;
const int SOCKET_CHILD  = 37550;
const int EXIT_BUTTON   = 37551;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 100;

const int MESSAGES_WIDTH  = 400;
const int MESSAGES_HEIGHT = 200;

const CFrequency maxFreq = CFrequency(2450, 0);
const CFrequency minFreq = CFrequency(2300, 0);

BEGIN_EVENT_TABLE(CSDREmulatorFrame, wxFrame)
	EVT_SOCKET(SOCKET_PARENT, CSDREmulatorFrame::onParentSocket)
	EVT_SOCKET(SOCKET_CHILD,  CSDREmulatorFrame::onChildSocket)
	EVT_CLOSE(CSDREmulatorFrame::onClose)
END_EVENT_TABLE()

CSDREmulatorFrame::CSDREmulatorFrame(unsigned int port) :
wxFrame(NULL, -1, wxString(wxT("uWave SDR Emulator")), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
m_txFreq(),
m_rxFreq(),
m_txEnable(false),
m_rxEnable(false),
m_txOn(false),
m_rxGain(0),
m_server(NULL),
m_messages(NULL)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxBoxSizer* vertSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, wxT("Status:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_connectLabel = new wxStaticText(panel, -1, wxT("Not connected"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_connectLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, wxT("TX Freq:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_txFreqLabel = new wxStaticText(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_txFreqLabel, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, wxT("RX Freq:"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_rxFreqLabel = new wxStaticText(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
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

	wxStaticText* label7 = new wxStaticText(panel, -1, wxT("RX Gain:"));
	panelSizer->Add(label7, 0, wxALL, BORDER_SIZE);

	m_rxGainLabel = new wxStaticText(panel, -1, wxT("0"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_rxGainLabel, 0, wxALL, BORDER_SIZE);

	vertSizer->Add(panelSizer);

	m_messages = new wxListBox(panel, -1, wxDefaultPosition, wxSize(MESSAGES_WIDTH, MESSAGES_HEIGHT));
	vertSizer->Add(m_messages, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(vertSizer);

	mainSizer->Add(panel);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	createListener(port);
}

CSDREmulatorFrame::~CSDREmulatorFrame()
{
}

bool CSDREmulatorFrame::createListener(unsigned int port)
{
	wxIPV4address address;
	address.Service(port);

	m_server = new wxSocketServer(address);
	if (!m_server->Ok()) {
		::wxMessageBox(wxT("problem creating the server socket"));
		return false;
	}

	m_server->SetEventHandler(*this, SOCKET_PARENT);
	m_server->SetNotify(wxSOCKET_CONNECTION_FLAG);
	m_server->Notify(true);

	return true;
}

void CSDREmulatorFrame::onParentSocket(wxSocketEvent& event)
{
	wxSocketBase* child = m_server->Accept();
	if (child == NULL) {
		m_server->Close();
		::wxMessageBox(wxT("Error on accepting the socket"));
		return;
	}

	child->SetEventHandler(*this, SOCKET_CHILD);
	child->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	child->Notify(true);

	wxIPV4address peer;
	child->GetPeer(peer);

	wxString text;
	text.Printf(wxT("Connected to %s:%u"), peer.Hostname().c_str(), peer.Service());

	m_connectLabel->SetLabel(text);
}

void CSDREmulatorFrame::onChildSocket(wxSocketEvent& event)
{
	wxSocketBase* socket = event.GetSocket();

	switch (event.GetSocketEvent()) {
		case wxSOCKET_INPUT: {
				char buffer[201];
				socket->Read(buffer, 200);
				if (socket->Error()) {
					::wxMessageBox(wxT("Error when reading data"));
					socket->Close();
					return;
				}

				int n = socket->LastCount();
				buffer[n] = wxT('\0');

				processCommand(*socket, buffer);
			}
			break;
		case wxSOCKET_LOST:
			m_connectLabel->SetLabel(wxT("Not connected"));
			socket->Destroy();
			break;
		default:
			wxASSERT(false);
			break;
	}
}

void CSDREmulatorFrame::processCommand(wxSocketBase& socket, wxChar* buffer)
{
	wxString messages = buffer;

	int pos = messages.Find(wxT(';'));
	while (pos != -1) {
		wxString message = messages.Left(pos);
		messages = messages.Mid(pos + 1);

		if (message.Length() < 3) {
			::sprintf(buffer, wxT("NK%s;"), message.c_str());
			socket.Write(buffer, ::strlen(buffer));

			wxString text;

			text.Printf(wxT("==> %s"), message.c_str());
			m_messages->Append(text);

			text.Printf(wxT("<== %s"), buffer);
			m_messages->Append(text);

			pos = messages.Find(wxT(";"));

			continue;
		}

		bool ack = false;
		wxString command = message.Left(2);

		if (command.Cmp(wxT("FA")) == 0) {
			wxString freqText = message.Mid(2);
			CFrequency freq = CFrequency(freqText);

			if (freq >= minFreq && freq < maxFreq) {
				m_txFreq = freq;
				m_rxFreq = freq;
				ack = true;
			}
		} else if (command.Cmp(wxT("FR")) == 0) {
			wxString freqText = message.Mid(2);
			CFrequency freq = CFrequency(freqText);

			if (freq >= minFreq && freq < maxFreq) {
				m_rxFreq = freq;
				ack = true;
			}
		} else if (command.Cmp(wxT("FT")) == 0) {
			wxString freqText = message.Mid(2);
			CFrequency freq = CFrequency(freqText);

			if (freq >= minFreq && freq < maxFreq) {
				m_txFreq = freq;
				ack = true;
			}	
		} else if (command.Cmp(wxT("ET")) == 0) {
			int n = ::atoi(message.Mid(2).c_str());

			if (n == 0 || n == 1) {
				m_txEnable = (n == 1);
				ack = true;
			}
		} else if (command.Cmp(wxT("ER")) == 0) {
			int n = ::atoi(message.Mid(2).c_str());

			if (n == 0 || n == 1) {
				m_rxEnable = (n == 1);
				ack = true;
			}
		} else if (command.Cmp(wxT("TX")) == 0) {
			int n = ::atoi(message.Mid(2).c_str());

			if (m_txEnable && (n == 0 || n == 1)) {
				m_txOn = (n == 1);
				ack = true;
			}
		} else if (command.Cmp(wxT("RG")) == 0) {
			int n = ::atoi(message.Mid(2).c_str());

			if (n >= 0) {
				m_rxGain = n;
				ack = true;
			}
		}

		if (ack) {
			::sprintf(buffer, "AK%s;", command.c_str());
			socket.Write(buffer, ::strlen(buffer));

			wxString text;

			text.Printf(wxT("%s MHz"), m_txFreq.getString().c_str());
			m_txFreqLabel->SetLabel(text);

			text.Printf(wxT("%s MHz"), m_rxFreq.getString().c_str());
			m_rxFreqLabel->SetLabel(text);

			m_txEnabledLabel->SetLabel(m_txEnable ? wxT("Yes") : wxT("No"));

			m_rxEnabledLabel->SetLabel(m_rxEnable ? wxT("Yes") : wxT("No"));

			m_txOnLabel->SetLabel(m_txOn ? wxT("Yes") : wxT("No"));

			text.Printf(wxT("%u"), m_rxGain);
			m_rxGainLabel->SetLabel(text);
		} else {
			::sprintf(buffer, "NK%s;", command.c_str());
			socket.Write(buffer, ::strlen(buffer));

			wxString text;

			text.Printf(wxT("==> %s"), message.c_str());
			m_messages->Append(text);

			text.Printf(wxT("<== %s"), buffer);
			m_messages->Append(text);
		}

		pos = messages.Find(wxT(';'));
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
		wxT("Exit uWSDR"),
		wxOK | wxCANCEL | wxICON_QUESTION);

	if (reply == wxOK)
		Destroy();
	else
		event.Veto();
}
