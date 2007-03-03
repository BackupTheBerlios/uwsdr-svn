/*
 *   Copyright (C) 2006-2007 by Jonathan Naylor G4KLX
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

#include "SDREmulatorWriter.h"


const unsigned int HEADER_SIZE = 7;
const unsigned int SAMPLE_SIZE = 6;

// const unsigned int MAX_SAMPLES = 210;


CSDREmulatorWriter::CSDREmulatorWriter(const wxString& address, int port, unsigned int version, unsigned int maxSamples, bool delay) :
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
m_delayOn(delay),
m_delayTime(0UL),
m_waiting(),
m_requests(0),
m_overruns(0),
m_packets(0),
m_maxSamples(maxSamples),
m_packetRequests(0),
m_enabled(false)
{
}

CSDREmulatorWriter::~CSDREmulatorWriter()
{
}

bool CSDREmulatorWriter::open(float sampleRate, unsigned int blockSize)
{
	m_enabled = false;

#if defined(__WINDOWS__)
	WSAData data;

	int ret =  ::WSAStartup(0x101, &data);
	if (ret != 0) {
		::wxLogError(wxT("SDREmulatorWriter: Error %d when initialising Winsock."), ret);
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
			::wxLogError(wxT("SDREmulatorWriter: Error %d when resolving host: %s"),
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
		::wxLogError(wxT("SDREmulatorWriter: Error %d when creating the writing datagram socket"),
#if defined(__WINDOWS__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	m_sockBuffer = new unsigned char[HEADER_SIZE + m_maxSamples * SAMPLE_SIZE];
	m_dataBuffer = new float[m_maxSamples * 2];

	m_buffer = new CRingBuffer(blockSize * 10, 2);

	if (m_delayOn) {
		m_delayTime = (500UL * m_maxSamples) / (unsigned long)sampleRate;
		::wxLogMessage(wxT("Delay is %lums"), m_delayTime);
	}

	Create();
	Run();

	return true;
}

/*
 * Put the data into the ring buffer, and use the event to send it out.
 */
void CSDREmulatorWriter::write(const float* buffer, unsigned int nSamples)
{
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);

	if (!m_enabled)
		return;

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
void* CSDREmulatorWriter::Entry()
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

	::wxLogMessage(wxT("SDREmulatorWriter: %u max samples, %u overruns, %u requests, %u packet requests, %u packets"), m_maxSamples, m_overruns, m_requests, m_packetRequests, m_packets);

	return (void*)0;
}

void CSDREmulatorWriter::writePacket()
{
	unsigned int nSamples = m_buffer->getData(m_dataBuffer, m_maxSamples);

	m_packetRequests++;

	while (nSamples > 0 && !TestDestroy()) {
		m_packets++;

		m_sockBuffer[0] = 'D';
		m_sockBuffer[1] = 'R';

		m_sockBuffer[2] = (m_sequence >> 0) & 0xFF;
		m_sockBuffer[3] = (m_sequence >> 8) & 0xFF;

		m_sequence += 2;
		if (m_sequence > 0xFFFF) {
			if ((m_sequence % 2) == 0)
				m_sequence = 1;
			else
				m_sequence = 0;
		}

		m_sockBuffer[4] = 0x00;		// AGC value

		m_sockBuffer[5] = (nSamples >> 0) & 0xFF;
		m_sockBuffer[6] = (nSamples >> 8) & 0xFF;

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
			::wxLogError(wxT("SDREmulatorWriter: Error %d writing to the datagram socket"),
#if defined(__WINDOWS__)
				::WSAGetLastError());
#else
				errno);
#endif
			return;
		}

		if (ret != int(len)) {
			::wxLogError(wxT("SDREmulatorWriter: Error only wrote %d of %lu bytes to the datagram socket"), ret, len);
			return;
		}

		if (m_delayOn)
			::wxMilliSleep(m_delayTime);

		nSamples = m_buffer->getData(m_dataBuffer, m_maxSamples);
	}
}

void CSDREmulatorWriter::close()
{
	m_enabled = false;

	Delete();
}

void CSDREmulatorWriter::enable(bool enable)
{
	m_buffer->clear();

	wxSemaError status = m_waiting.TryWait();
	while (status != wxSEMA_BUSY) {
		m_waiting.Wait();
		status = m_waiting.TryWait();
	}

	m_enabled = enable;
}

void CSDREmulatorWriter::disable()
{
	enable(false);
}
