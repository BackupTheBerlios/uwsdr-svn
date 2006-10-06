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

#include "SDRDataWriter.h"

const int SOCKET_ID = 7897;

const unsigned int SAMPLE_SIZE      = 6;
const unsigned int IN_BUFFER_SIZE   = 500;
const unsigned int SOCK_BUFFER_SIZE = 1500;
const unsigned int OUT_BUFFER_SIZE  = 500;
const unsigned int PACKET_SIZE      = 200;

BEGIN_EVENT_TABLE(CSDRDataWriter, wxEvtHandler)
	EVT_SOCKET(SOCKET_ID, CSDRDataWriter::onSocket)
END_EVENT_TABLE()


CSDRDataWriter::CSDRDataWriter(const wxString& address, int port, unsigned int version) :
wxEvtHandler(),
m_address(address),
m_port(port),
m_version(version),
m_socket(NULL),
m_ipAddress(),
m_sequence(0),
m_event(true),
m_buffer(NULL),
m_sockBuffer(NULL),
m_txBuffer(10000, 2)
{
}

CSDRDataWriter::~CSDRDataWriter()
{
}

bool CSDRDataWriter::open(float sampleRate, unsigned int blockSize)
{
	m_buffer     = new float[PACKET_SIZE * 2];
	m_sockBuffer = new unsigned char[SOCK_BUFFER_SIZE];

	m_ipAddress.Hostname(m_address);
	m_ipAddress.Service(m_port);

	m_address = m_ipAddress.IPAddress();
	m_port    = m_ipAddress.Service();

	wxIPV4address myAddress;
	myAddress.AnyAddress();
	myAddress.Service(m_port + 1);

	m_socket = new wxDatagramSocket(myAddress);

	m_socket->SetEventHandler(*this, SOCKET_ID);
	m_socket->SetNotify(wxSOCKET_OUTPUT_FLAG);
	m_socket->Notify(true);

	return true;
}

/*
 * Put the data into the ring buffer, and use the event to send it out.
 */
void CSDRDataWriter::write(const float* buffer, unsigned int nSamples)
{
	wxASSERT(nSamples > 0);
	wxASSERT(buffer != NULL);

	if (m_version != 1) {
		::wxLogError(wxT("Invalid version of the protocol = %u"), m_version);
		return;
	}

	m_txBuffer.addData(buffer, nSamples);

	if (m_event) {
		wxSocketEvent event;
		onSocket(event);
	}
}

void CSDRDataWriter::close()
{
	m_socket->Destroy();

	delete[] m_buffer;
	delete[] m_sockBuffer;
}

void CSDRDataWriter::onSocket(wxSocketEvent& event)
{
	if (m_txBuffer.dataSpace() == 0) {
		m_event = true;
		return;
	}

	m_event = false;

	unsigned int nSamples = m_txBuffer.getData(m_buffer, PACKET_SIZE);

	m_sockBuffer[0] = 'D';
	m_sockBuffer[1] = 'A';

	m_sockBuffer[2] = (m_sequence >> 8) & 0xFF;
	m_sockBuffer[3] =  m_sequence & 0xFF;

	m_sequence++;
	if (m_sequence == 0xFFFF)
		m_sequence = 0;

	m_sockBuffer[4] = nSamples;

	unsigned int len = 5;
	for (unsigned int i = 0; i < nSamples && len < OUT_BUFFER_SIZE; i++) {
		unsigned int qData = (unsigned int)(m_buffer[i * 2 + 0] * 8388607.5 + 8388607.5);
		unsigned int iData = (unsigned int)(m_buffer[i * 2 + 1] * 8388607.5 + 8388607.5);

		m_sockBuffer[len++] = (iData >> 16) & 0xFF;
		m_sockBuffer[len++] = (iData >> 8)  & 0xFF;
		m_sockBuffer[len++] = (iData >> 0)  & 0xFF;

		m_sockBuffer[len++] = (qData >> 16) & 0xFF;
		m_sockBuffer[len++] = (qData >> 8)  & 0xFF;
		m_sockBuffer[len++] = (qData >> 0)  & 0xFF;
	}

	m_socket->SendTo(m_ipAddress, m_sockBuffer, len);
}
