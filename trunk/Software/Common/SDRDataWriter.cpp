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


const unsigned int HEADER_SIZE = 6;
const unsigned int SAMPLE_SIZE = 6;

// const unsigned int MAX_SAMPLES = 210;


CSDRDataWriter::CSDRDataWriter(const wxString& address, int port, unsigned int version, unsigned int maxSamples, bool delay) :
wxThread(),
m_address(address),
m_port(port),
m_version(version),
m_fd(-1),
m_remAddr(),
m_sequence(0),
m_sockBuffer(NULL),
m_dataBuffer(NULL),
m_buffer(NULL),
m_delay(0UL),
m_waiting(),
m_requests(0),
m_overruns(0),
m_packets(0),
MAX_SAMPLES(maxSamples),
DELAY(delay),
m_packetRequests(0)
{
}

CSDRDataWriter::~CSDRDataWriter()
{
}

bool CSDRDataWriter::open(float sampleRate, unsigned int blockSize)
{
#if defined(__WINDOWS__)
	WSAData data;

	int ret =  ::WSAStartup(0x101, &data);
	if (ret != 0) {
		::wxLogError(wxT("SDRDataWriter: Error %d when initialising Winsock."), ret);
		return false;
	}
#endif

#if defined(__WINDOWS__)
	unsigned long addr = ::inet_addr(m_address.c_str());
#else
	in_addr_t addr = ::inet_addr(m_address.c_str());
#endif
	unsigned int length = 4;

	if (addr == INADDR_NONE) {
		struct hostent* host = ::gethostbyname(m_address.c_str());

		if (host == NULL) {
			::wxLogError(wxT("SDRDataWriter: Error %d when resolving host: %s"),
#if defined(__WINDOWS__)
				::WSAGetLastError(),
#else
				h_errno,
#endif
				m_address.c_str());
			return false;
		}

		::memcpy(&addr, &host->h_addr, host->h_length);
		length = host->h_length;
	}

	::memset(&m_remAddr, 0x00, sizeof(struct sockaddr_in));
	m_remAddr.sin_family = AF_INET;
	m_remAddr.sin_port   = htons(m_port);
	::memcpy(&m_remAddr.sin_addr.s_addr, &addr, length);

	m_fd = ::socket(PF_INET, SOCK_DGRAM, 0);
	if (m_fd < 0) {
		::wxLogError(wxT("SDRDataWriter: Error %d when creating the writing datagram socket"),
#if defined(__WINDOWS__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	m_sockBuffer = new unsigned char[HEADER_SIZE + MAX_SAMPLES * SAMPLE_SIZE];
	m_dataBuffer = new float[MAX_SAMPLES * 2];

	m_buffer = new CRingBuffer(blockSize * 10, 2);

	if (DELAY) {
		m_delay = (500UL * MAX_SAMPLES) / (unsigned long)sampleRate;
		::wxLogMessage(wxT("Delay is %lums"), m_delay);
	}

	Create();
	Run();

	return true;
}

/*
 * Put the data into the ring buffer, and use the event to send it out.
 */
void CSDRDataWriter::write(const float* buffer, unsigned int nSamples)
{
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);

	unsigned int n = m_buffer->addData(buffer, nSamples);

	m_requests++;

	if (n != nSamples)
		m_overruns++;

	if (n > 0)
		m_waiting.Post();
}

/*
 * Put the data into the ring buffer, and use the event to send it out.
 */
void* CSDRDataWriter::Entry()
{
	while (!TestDestroy()) {
		wxSemaError ret = m_waiting.WaitTimeout(500UL);

		if (ret == wxSEMA_NO_ERROR)
			writePacket();
	}

#if defined(__WINDOWS__)
	::closesocket(m_fd);
	::WSACleanup();
#else
	::close(m_fd);
#endif
	m_fd = -1;

	delete[] m_sockBuffer;
	delete[] m_dataBuffer;
	delete   m_buffer;

	::wxLogMessage(wxT("SDRDataWriter: %u max samples, %u overruns, %u requests, %u packet requests, %u packets"), MAX_SAMPLES, m_overruns, m_requests, m_packetRequests, m_packets);

	return (void*)0;
}

void CSDRDataWriter::writePacket()
{
	unsigned int nSamples = m_buffer->getData(m_dataBuffer, MAX_SAMPLES);

	m_packetRequests++;

	while (nSamples > 0 && !TestDestroy()) {
		m_packets++;

		m_sockBuffer[0] = 'D';
		m_sockBuffer[1] = 'A';

		m_sockBuffer[2] = (m_sequence >> 0) & 0xFF;
		m_sockBuffer[3] = (m_sequence >> 8) & 0xFF;

		m_sequence += 2;
		if (m_sequence > 0xFFFF) {
			if ((m_sequence % 2) == 0)
				m_sequence = 1;
			else
				m_sequence = 0;
		}

		m_sockBuffer[4] = (nSamples >> 0) & 0xFF;
		m_sockBuffer[5] = (nSamples >> 8) & 0xFF;

		unsigned int len = HEADER_SIZE;
		for (unsigned int i = 0; i < nSamples; i++) {
			unsigned int iData = (unsigned int)((m_dataBuffer[i * 2 + 0] + 1.0F) * 8388607.0F + 0.5F);
			unsigned int qData = (unsigned int)((m_dataBuffer[i * 2 + 1] + 1.0F) * 8388607.0F + 0.5F);

			m_sockBuffer[len++] = (iData >> 16) & 0xFF;
			m_sockBuffer[len++] = (iData >> 8)  & 0xFF;
			m_sockBuffer[len++] = (iData >> 0)  & 0xFF;

			m_sockBuffer[len++] = (qData >> 16) & 0xFF;
			m_sockBuffer[len++] = (qData >> 8)  & 0xFF;
			m_sockBuffer[len++] = (qData >> 0)  & 0xFF;
		}

		ssize_t ret = ::sendto(m_fd, (char *)m_sockBuffer, len, 0, (struct sockaddr *)&m_remAddr, sizeof(struct sockaddr_in));
		if (ret < 0) {
			::wxLogError(wxT("SDRDataWriter: Error %d writing to the datagram socket"),
#if defined(__WINDOWS__)
				::WSAGetLastError());
#else
				errno);
#endif
			return;
		}

		if (ret != int(len)) {
			::wxLogError(wxT("SDRDataWriter: Error only wrote %d of %u bytes to the datagram socket"), ret, len);
			return;
		}

		if (DELAY)
			::wxMilliSleep(m_delay);

		nSamples = m_buffer->getData(m_dataBuffer, MAX_SAMPLES);
	}
}

void CSDRDataWriter::close()
{
	Delete();
}

void CSDRDataWriter::purge()
{
	m_buffer->clear();

	wxSemaError status = m_waiting.TryWait();
	while (status != wxSEMA_BUSY) {
		m_waiting.Wait();
		status = m_waiting.TryWait();
	}
}
