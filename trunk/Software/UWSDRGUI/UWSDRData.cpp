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

#include "UWSDRData.h"

const int SOCKET_ID = 7896;

const int SAMPLE_SIZE      = 6;
const int IN_BUFFER_SIZE   = 500;
const int SOCK_BUFFER_SIZE = 1500;
const int OUT_BUFFER_SIZE  = 500;
const int PACKET_SIZE      = 200;

BEGIN_EVENT_TABLE(CUWSDRData, wxEvtHandler)
	EVT_SOCKET(SOCKET_ID, CUWSDRData::onSocket)
END_EVENT_TABLE()

// RingBuffer on receive for buffering XXX

CUWSDRData::CUWSDRData(const wxString& address, int port, unsigned int version, bool enable) :
wxEvtHandler(),
m_address(address),
m_port(port),
m_version(version),
m_count(0),
m_enabled(enable),
m_id(0),
m_callback(NULL),
m_socket(NULL),
m_ipAddress(),
m_sequenceIn(-1),
m_sequenceOut(0),
m_inBuffer(NULL),
m_sockBuffer(NULL),
m_outBuffer(NULL),
m_rxBuffer(1000),
m_txBuffer(1000)
{
}

CUWSDRData::~CUWSDRData()
{
}

void CUWSDRData::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CUWSDRData::open(unsigned int sampleRate, unsigned int blockSize)
{
	if (!m_enabled)
		return true;

	m_count++;

	if (m_count > 1)
		return true;

	m_inBuffer    = new float[blockSize];
	m_outBuffer   = new unsigned char[SOCK_BUFFER_SIZE];
	m_sockBuffer  = new unsigned char[OUT_BUFFER_SIZE];

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

void CUWSDRData::write(const float* buffer, unsigned int nSamples)
{
	if (!m_enabled)
		return;

	wxASSERT(nSamples > 0);
	wxASSERT(buffer != NULL);

	if (m_version != 1) {
		::wxLogError(_("Invalid version of the protocol = %u"), m_version);
		return;
	}

	unsigned int n = 0;
	while (nSamples >= PACKET_SIZE) {
		unsigned int size = (nSamples >= PACKET_SIZE) ? PACKET_SIZE : nSamples;

		if (size > 0)
			writePacket(buffer + n, size);

		nSamples -= size;
		n        += size;
	}	
}

void CUWSDRData::writePacket(const float* buffer, unsigned int nSamples)
{
	wxASSERT(m_socket != NULL);
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0 && nSamples <= PACKET_SIZE);
	wxASSERT(m_outBuffer != NULL);

	m_outBuffer[0] = 'D';
	m_outBuffer[1] = 'A';

	m_outBuffer[2] = (m_sequenceOut >> 8) & 0xFF;
	m_outBuffer[3] =  m_sequenceOut & 0xFF;

	m_sequenceOut++;
	if (m_sequenceOut == 0xFFFF)
		m_sequenceOut = 0;

	m_outBuffer[4] = nSamples;

	unsigned int len = 5;
	for (unsigned int i = 0; i < nSamples && len < OUT_BUFFER_SIZE; i++) {
		unsigned int qData = (unsigned int)(buffer[i * 2 + 0] * 8388607.5 + 8388607.5);
		unsigned int iData = (unsigned int)(buffer[i * 2 + 1] * 8388607.5 + 8388607.5);

		m_outBuffer[len++] = (iData >> 16) & 0xFF;
		m_outBuffer[len++] = (iData >> 8)  & 0xFF;
		m_outBuffer[len++] = (iData >> 0)  & 0xFF;

		m_outBuffer[len++] = (qData >> 16) & 0xFF;
		m_outBuffer[len++] = (qData >> 8)  & 0xFF;
		m_outBuffer[len++] = (qData >> 0)  & 0xFF;
	}

	m_socket->SendTo(m_ipAddress, m_outBuffer, len);
}

void CUWSDRData::close()
{
	if (!m_enabled)
		return;

	m_count--;

	if (m_count > 0)
		return;

	delete[] m_inBuffer;
	delete[] m_outBuffer;
	delete[] m_sockBuffer;

	delete m_socket;

	m_sequenceIn = -1;
}

void CUWSDRData::onSocket(wxSocketEvent& event)
{
	wxASSERT(m_socket != NULL);
	wxASSERT(m_inBuffer != NULL);

	if (event.GetSocketEvent() == wxSOCKET_INPUT) {
		wxIPV4address addr;
		m_socket->RecvFrom(addr, m_sockBuffer, SOCK_BUFFER_SIZE);

		if (m_callback == NULL) {
			::wxLogWarning(_("No callback set for the SDR data"));
			return;
		}

		wxString address    = addr.IPAddress();
		unsigned short port = addr.Service();

		// Check if the data is for us
		if (m_address.Cmp(address) != 0 || m_port != port) {
			::wxLogWarning(_("SDR Data received from an invalid address or port"));
			return;
		}

		wxUint32 len = m_socket->LastCount();

		if (m_version != 1) {
			::wxLogError(_("Invalid version of the protocol = %u"), m_version);
			return;
		}

		if (m_sockBuffer[0] != 'D' || m_sockBuffer[1] != 'A') {
			::wxLogWarning(_("Received a badly formatted data packet"));
			return;
		}

		int seqNo = (m_sockBuffer[2] << 8) + m_sockBuffer[3];

		// FIXME check the sequence no
		if (m_sequenceIn != -1) {
		}

		m_sequenceIn = seqNo + 1;
		if (m_sequenceIn == 0xFFFF)
			m_sequenceIn = 0;

		unsigned int nSamples = m_sockBuffer[4];

		unsigned int n = 5;
		for (unsigned int i = 0; i < nSamples && n < len; n += SAMPLE_SIZE) {
			unsigned int iData = (m_sockBuffer[n + 0] << 16) + (m_sockBuffer[n + 1] << 8) + m_sockBuffer[n + 2];
			unsigned int qData = (m_sockBuffer[n + 3] << 16) + (m_sockBuffer[n + 4] << 8) + m_sockBuffer[n + 5];

			m_inBuffer[i++] = (float(qData) - 8388607.5) / 8388607.5;
			m_inBuffer[i++] = (float(iData) - 8388607.5) / 8388607.5;
		}

		m_callback->callback(m_inBuffer, nSamples, m_id);
	}
}
