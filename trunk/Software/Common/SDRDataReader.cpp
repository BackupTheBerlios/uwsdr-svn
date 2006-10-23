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

#if !defined(__WXMSW__)
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

const int HEADER_SIZE = 6;
const int SAMPLE_SIZE = 6;


CSDRDataReader::CSDRDataReader(const wxString& address, int port) :
wxThread(),
m_address(address),
m_port(port),
m_blockSize(0),
m_size(0),
m_remAddr(),
m_remAddrLen(0),
m_id(0),
m_callback(NULL),
m_fd(-1),
m_sequence(-1),
m_buffer(NULL),
m_sockBuffer(NULL),
m_sampBuffer(NULL),
m_missed(0),
m_requests(0),
m_underruns(0)
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
	m_blockSize = blockSize;

	struct hostent* host = ::gethostbyname(m_address.c_str());

	if (host == NULL) {
		::wxLogError(wxT("Cannot resolve host name: %s"), m_address.c_str());
		return false;
	}

	m_remAddrLen = host->h_length;
	m_remAddr = new char[m_remAddrLen];
	::memcpy(m_remAddr, host->h_addr, m_remAddrLen);

	m_fd = ::socket(PF_INET, SOCK_DGRAM, 0);
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

	m_size = HEADER_SIZE + m_blockSize * SAMPLE_SIZE;

	m_buffer     = new CRingBuffer(m_blockSize * 10, 2);
	m_sockBuffer = new unsigned char[m_size];
	m_sampBuffer = new float[m_blockSize * 2];

	Create();
	Run();

	return true;
}

void CSDRDataReader::close()
{
	Delete();
}

void* CSDRDataReader::Entry()
{
	bool ret;

	do {
		ret = readSocket();
	} while (!TestDestroy() && ret);

	// If we died prematurely, wait until killed
	if (!ret) {
		while (!TestDestroy())
			Sleep(500UL);
	}

#if defined(__WXMSW__)
	::closesocket(m_fd);
#else
	::close(m_fd);
#endif
	m_fd = -1;

	delete   m_buffer;
	delete[] m_sockBuffer;
	delete[] m_sampBuffer;
	delete[] m_remAddr;

	m_sequence = -1;

	::wxLogMessage(wxT("SDRDataReader: %u missed frames, %u requests, %u underruns"), m_missed, m_requests, m_underruns);

	return (void *)0;
}

void CSDRDataReader::clock()
{
	wxASSERT(m_callback != NULL);
	wxASSERT(m_buffer != NULL);

	m_requests++;

	unsigned int len = m_buffer->getData(m_sampBuffer, m_blockSize);

	if (len != m_blockSize)
		m_underruns++;

	if (len > 0)
		m_callback->callback(m_sampBuffer, len, m_id);
}

void CSDRDataReader::purge()
{
	wxASSERT(m_buffer != NULL);

	m_buffer->clear();
}

bool CSDRDataReader::hasClock()
{
	return false;
}

bool CSDRDataReader::readSocket()
{
	// Check that the readfrom() won't block
	fd_set readFds;
	FD_ZERO(&readFds);
#if defined(__WXMSW__)
	FD_SET((unsigned int)m_fd, &readFds);
#else
	FD_SET(m_fd, &readFds);
#endif

	// 1/2 sec timeout
	struct timeval tv;
	tv.tv_sec  = 0L;
	tv.tv_usec = 500000L;

	int ret = ::select(m_fd + 1, &readFds, NULL, NULL, &tv);
	if (ret < 0) {
		::wxLogError(wxT("Error %d while performing a select"),
#if defined(__WXMSW__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	// No data?
	if (ret == 0)
		return true;

	struct sockaddr addr;
#if defined(__WXMSW__)
	int size = sizeof(struct sockaddr);
#else
	socklen_t size = sizeof(struct sockaddr);
#endif

	ssize_t len = ::recvfrom(m_fd, (char *)m_sockBuffer, m_size, 0, &addr, &size);
	if (len < 0) {
		::wxLogError(wxT("Error %d reading from the datagram socket"),
#if defined(__WXMSW__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	if (addr.sa_family != AF_INET) {
		::wxLogError(wxT("Received datagram from a non IP address!"));
		return true;
	}

	struct sockaddr_in* inaddr = (struct sockaddr_in *)&addr;

	// Check if the data is for us
	if (::memcmp(m_remAddr, &inaddr->sin_addr.s_addr, m_remAddrLen) != 0) {
		unsigned char* p = (unsigned char *)&inaddr->sin_addr.s_addr;
		unsigned char* q = (unsigned char *)m_remAddr;
		::wxLogWarning(wxT("SDR Data received from an invalid IP address: %u.%u.%u.%u, wanted: %u.%u.%u.%u"),
			p[0], p[1], p[2], p[3], q[0], q[1], q[2], q[3]);
		return true;
	}

	if (len < HEADER_SIZE || m_sockBuffer[0] != 'D' || m_sockBuffer[1] != 'A') {
		::wxLogWarning(wxT("Received a badly formatted data packet"));
		return true;
	}

	int seqNo = (m_sockBuffer[2] << 8) + m_sockBuffer[3];

	if (m_sequence != -1 && seqNo != m_sequence) {
		m_missed++;

		if (seqNo < m_sequence && (seqNo % 2) == (m_sequence % 2)) {
			::wxLogWarning(wxT("Packet dropped at sequence no: %d, expected: %d"), seqNo, m_sequence);
			return true;
		} else {
			::wxLogWarning(wxT("Packet missed at sequence no: %d, expected: %d"), seqNo, m_sequence);
		}
	}

	m_sequence = seqNo + 2;
	if (m_sequence > 0xFFFF) {
		if ((m_sequence % 2) == 0)
			m_sequence = 1;
		else
			m_sequence = 0;
	}

	unsigned int nSamples = (m_sockBuffer[4] << 8) + m_sockBuffer[5];

	int n = HEADER_SIZE;	
	for (unsigned int i = 0; i < nSamples && n < len; n += SAMPLE_SIZE, i++) {
		unsigned int iData = (m_sockBuffer[n + 0] << 16) & 0xFF0000;
		iData += (m_sockBuffer[n + 1] << 8) & 0xFF00;
		iData += (m_sockBuffer[n + 2] << 0) & 0xFF;

		unsigned int qData = (m_sockBuffer[n + 3] << 16) & 0xFF0000;
		qData += (m_sockBuffer[n + 4] << 8) & 0xFF00;
		qData += (m_sockBuffer[n + 5] << 0) & 0xFF;

		float buffer[2];
		buffer[0] = float(iData) / 8388607.0F - 1.0F;
		buffer[1] = float(qData) / 8388607.0F - 1.0F;

		m_buffer->addData(buffer, 1);
	}

	return true;
}
