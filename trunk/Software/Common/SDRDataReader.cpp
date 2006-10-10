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


const unsigned int HEADER_SIZE = 6;
const unsigned int SAMPLE_SIZE = 6;


CSDRDataReader::CSDRDataReader(const wxString& address, int port) :
m_address(address),
m_port(port),
m_size(0),
m_remAddr(),
m_remAddrLen(0),
m_id(0),
m_callback(NULL),
m_fd(-1),
m_sequence(-1),
m_buffer(NULL),
m_sockBuffer(NULL),
m_requests(0),
m_underruns(0)
{
}

CSDRDataReader::~CSDRDataReader()
{
	delete[] m_remAddr;
}

void CSDRDataReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CSDRDataReader::open(float sampleRate, unsigned int blockSize)
{
	struct hostent* host = ::gethostbyname(m_address.c_str());

	if (host == NULL) {
		::wxLogError(wxT("Cannot resolve host name: %s"), m_address.c_str());
		return false;
	}

	m_remAddrLen = host->h_length;
	m_remAddr = new char[m_remAddrLen];
	::memcpy(m_remAddr, host->h_addr, m_remAddrLen);

	m_fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (m_fd < 0) {
		::wxLogError(wxT("Error %d when creating the reading datagram socket"),
#if defined(__WXMSW__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	struct sockaddr_in sockaddr;
	::memset(&sockaddr, 0x00, sizeof(struct sockaddr_in));
	sockaddr.sin_family      = AF_INET;
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr.sin_port        = htons(m_port);

	int ret = ::bind(m_fd, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		::wxLogError(wxT("Error %d when binding the reading datagram socket"),
#if defined(__WXMSW__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	m_size = HEADER_SIZE + blockSize * SAMPLE_SIZE;

	m_buffer     = new float[blockSize * 2];
	m_sockBuffer = new unsigned char[m_size];

	return true;
}

void CSDRDataReader::close()
{
#if defined(__WXMSW__)
	::closesocket(m_fd);
#else
	::close(m_fd);
#endif
	m_fd = -1;

	delete[] m_buffer;
	delete[] m_sockBuffer;

	m_sequence = -1;

	::wxLogMessage(wxT("SDRDataReader: %u underruns in %u requests"), m_underruns, m_requests);
}

void CSDRDataReader::clock()
{
	// Check that the readfrom() won't block
	fd_set readFds;
	FD_ZERO(&readFds);
	FD_SET(m_fd, &readFds);

	struct timeval tv;
	tv.tv_sec  = 0L;
	tv.tv_usec = 0L;

	int ret = ::select(m_fd + 1, &readFds, NULL, NULL, &tv);
	if (ret < 0) {
		::wxLogError(wxT("Error %d while performing a select"),
#if defined(__WXMSW__)
			::WSAGetLastError());
#else
			errno);
#endif
		return;
	}

	m_requests++;

	// No data?
	if (ret == 0) {
		m_underruns++;
		return;	
	}

	struct sockaddr_in addr;
	int size = sizeof(struct sockaddr_in);

	ssize_t len = ::recvfrom(m_fd, (char *)m_sockBuffer, m_size, 0, (struct sockaddr *)&addr, &len);
	if (len < 0) {
		::wxLogError(wxT("Error %d reading from the datagram socket"),
#if defined(__WXMSW__)
			::WSAGetLastError());
#else
			errno);
#endif
		return;
	}

	if (m_callback == NULL) {
		::wxLogWarning(wxT("No callback set for the SDR data"));
		return;
	}

	// Check if the data is for us
	if (::memcmp(m_remAddr, &addr.sin_addr.s_addr, m_remAddrLen) == 0) {
		unsigned char* p = (unsigned char *)&addr.sin_addr.s_addr;
		::wxLogWarning(wxT("SDR Data received from an invalid IP address: %u.%u.%u.%u"), p[0], p[1], p[2], p[3]);
		return;
	}

	if (len < HEADER_SIZE || m_sockBuffer[0] != 'D' || m_sockBuffer[1] != 'A') {
		::wxLogWarning(wxT("Received a badly formatted data packet"));
		return;
	}

	int seqNo = (m_sockBuffer[2] << 8) + m_sockBuffer[3];

	// FIXME check the sequence no
	if (m_sequence != -1 && seqNo != m_sequence) {
		if (seqNo < m_sequence && 1) {
			::wxLogWarning(wxT("Packet dropped at sequence no: %d"), m_sequence);
			return;
		}
	}

	m_sequence = seqNo + 1;
	if (m_sequence == 0xFFFF)
		m_sequence = 0;

	unsigned int nSamples = (m_sockBuffer[4] << 8) + m_sockBuffer[5];

	int n = HEADER_SIZE;
	unsigned int i = 0;
	for (; i < nSamples && n < len; n += SAMPLE_SIZE, i++) {
		unsigned int iData = (m_sockBuffer[n + 0] << 16) + (m_sockBuffer[n + 1] << 8) + m_sockBuffer[n + 2];
		unsigned int qData = (m_sockBuffer[n + 3] << 16) + (m_sockBuffer[n + 4] << 8) + m_sockBuffer[n + 5];

		m_buffer[i * 2 + 0] = (float(qData) - 8388607.5) / 8388607.5;
		m_buffer[i * 2 + 1] = (float(iData) - 8388607.5) / 8388607.5;
		i++;
	}

	m_callback->callback(m_buffer, i, m_id);
}
