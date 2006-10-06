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

#include "SDRDataReader.h"

const int SOCKET_ID = 7896;

const unsigned int SAMPLE_SIZE      = 6;
const unsigned int IN_BUFFER_SIZE   = 500;
const unsigned int SOCK_BUFFER_SIZE = 1500;
const unsigned int OUT_BUFFER_SIZE  = 500;
const unsigned int PACKET_SIZE      = 200;

BEGIN_EVENT_TABLE(CSDRDataReader, wxEvtHandler)
	EVT_SOCKET(SOCKET_ID, CSDRDataReader::onSocket)
END_EVENT_TABLE()


CSDRDataReader::CSDRDataReader(const wxString& address, int port, unsigned int version) :
wxEvtHandler(),
m_address(address),
m_port(port),
m_version(version),
m_id(0),
m_callback(NULL),
m_socket(NULL),
m_ipAddress(),
m_sequence(-1),
m_buffer(NULL),
m_sockBuffer(NULL)
{
}

CSDRDataReader::~CSDRDataReader()
{
}

void CSDRDataReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CSDRDataReader::open(float sampleRate, unsigned int blockSize)
{
	m_buffer     = new float[blockSize];
	m_sockBuffer = new unsigned char[OUT_BUFFER_SIZE];

	m_ipAddress.Hostname(m_address);
	m_ipAddress.Service(m_port);

	m_address = m_ipAddress.IPAddress();
	m_port    = m_ipAddress.Service();

	wxIPV4address myAddress;
	myAddress.AnyAddress();
	myAddress.Service(m_port);

	m_socket = new wxDatagramSocket(myAddress);

	m_socket->SetEventHandler(*this, SOCKET_ID);
	m_socket->SetNotify(wxSOCKET_INPUT_FLAG);
	m_socket->Notify(true);

	return true;
}

void CSDRDataReader::close()
{
	m_socket->Destroy();

	delete[] m_buffer;
	delete[] m_sockBuffer;

	m_sequence = -1;
}

void CSDRDataReader::onSocket(wxSocketEvent& event)
{
	wxIPV4address addr;
	m_socket->RecvFrom(addr, m_sockBuffer, SOCK_BUFFER_SIZE);

	if (m_callback == NULL) {
		::wxLogWarning(wxT("No callback set for the SDR data"));
		return;
	}

	wxString address    = addr.IPAddress();
	unsigned short port = addr.Service();

	// Check if the data is for us
	if (m_address.Cmp(address) != 0 || m_port != port) {
		::wxLogWarning(wxT("SDR Data received from an invalid address or port"));
		return;
	}

	wxUint32 len = m_socket->LastCount();

	if (m_version != 1) {
		::wxLogError(wxT("Invalid version of the protocol = %u"), m_version);
		return;
	}

	if (m_sockBuffer[0] != 'D' || m_sockBuffer[1] != 'A') {
		::wxLogWarning(wxT("Received a badly formatted data packet"));
		return;
	}

	int seqNo = (m_sockBuffer[2] << 8) + m_sockBuffer[3];

	// FIXME check the sequence no
	if (m_sequence != -1) {
	}

	m_sequence = seqNo + 1;
	if (m_sequence == 0xFFFF)
		m_sequence = 0;

	unsigned int nSamples = m_sockBuffer[4];

	unsigned int n = 5;
	for (unsigned int i = 0; i < nSamples && n < len; n += SAMPLE_SIZE) {
		unsigned int iData = (m_sockBuffer[n + 0] << 16) + (m_sockBuffer[n + 1] << 8) + m_sockBuffer[n + 2];
		unsigned int qData = (m_sockBuffer[n + 3] << 16) + (m_sockBuffer[n + 4] << 8) + m_sockBuffer[n + 5];

		m_buffer[i++] = (float(qData) - 8388607.5) / 8388607.5;
		m_buffer[i++] = (float(iData) - 8388607.5) / 8388607.5;
	}

	m_callback->callback(m_buffer, nSamples, m_id);
}

bool CSDRDataReader::needsClock()
{
	return false;
}

void CSDRDataReader::clock()
{
}
