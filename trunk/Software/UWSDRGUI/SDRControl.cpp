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

#include "SDRControl.h"

const int SOCKET_ID = 7895;

BEGIN_EVENT_TABLE(CSDRControl, wxEvtHandler)
	EVT_SOCKET(SOCKET_ID, CSDRControl::onSocket)
END_EVENT_TABLE()

CSDRControl::CSDRControl(IControlInterface* callback, int id) :
wxEvtHandler(),
m_id(id),
m_enabled(false),
m_socket(wxSOCKET_NOWAIT),
m_callback(callback),
m_version(0),
m_txFreq(),
m_rxFreq(),
m_enableTX(false),
m_enableRX(false),
m_tx(false)
{
}

CSDRControl::~CSDRControl()
{
}

bool CSDRControl::open(const wxString& address, int port, unsigned int version, bool enable)
{
	m_enabled = enable;

	if (!m_enabled)
		return true;

	wxIPV4address sockAddress;
	sockAddress.Hostname(address);
	sockAddress.Service(port);

	bool ret = m_socket.Connect(sockAddress);
	if (!ret)
		return false;

	m_version = version;

	m_socket.SetEventHandler(*this, SOCKET_ID);
	m_socket.SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	m_socket.Notify(true);

	return true;
}

void CSDRControl::enableTX(bool on)
{
	if (!m_enabled)
		return;

	if (m_enableTX == on)
		return;

	switch (m_version) {
		case 1:
			sendCommand(on ? "ET1;" : "ET0;");
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_enableTX = on;;
}

void CSDRControl::enableRX(bool on)
{
	if (!m_enabled)
		return;

	if (m_enableRX == on)
		return;

	switch (m_version) {
		case 1:
			sendCommand(on ? "ER1;" : "ER0;");
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_enableRX = on;
}

void CSDRControl::setTXAndFreq(bool transmit, const CFrequency& freq)
{
	if (!m_enabled)
		return;

	if (transmit && m_tx && freq == m_txFreq)
		return;

	if (!transmit && !m_tx && freq == m_rxFreq)
		return;

	if (transmit && freq != m_txFreq) {
		char command[25];

		switch (m_version) {
			case 1:
				::sprintf(command, "FT%s;", freq.getString().c_str());
				sendCommand(command);
				break;
			default:
				wxASSERT(false);
				break;
		}

		m_txFreq = freq;
	} else if (!transmit && freq != m_rxFreq) {
		char command[25];

		switch (m_version) {
			case 1:
				::sprintf(command, "FR%s;", freq.getString().c_str());
				sendCommand(command);
				break;
			default:
				wxASSERT(false);
				break;
		}

		m_rxFreq = freq;
	}

	if (transmit == m_tx)
		return;

	switch (m_version) {
		case 1:
			sendCommand(transmit ? "TX1;" : "TX0;");
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_tx = transmit;
}

void CSDRControl::sendCommand(const char* command)
{
	if (!m_enabled)
		return;

	m_socket.Write(command, ::strlen(command));
}

void CSDRControl::close()
{
	if (!m_enabled)
		return;

	m_socket.Close();
}

void CSDRControl::onSocket(wxSocketEvent& event)
{
	wxASSERT(m_callback != NULL);

	wxSocketBase* sock = event.GetSocket();

	switch(event.GetSocketEvent()) {
		case wxSOCKET_INPUT: {
				char buffer[101];
				sock->Read(buffer, 100);

				wxUint32 n = sock->LastCount();
				buffer[n] = '\0';

				switch (m_version) {
					case 1:
						if (::strstr(buffer, "NK") != NULL)
							m_callback->sdrCommandNAK(m_id);
						break;
					default:
						wxASSERT(false);
						break;
				}
			}
			break;

		case wxSOCKET_LOST:
			m_callback->sdrConnectionLost(m_id);
			sock->Destroy();
			break;
	}
}
