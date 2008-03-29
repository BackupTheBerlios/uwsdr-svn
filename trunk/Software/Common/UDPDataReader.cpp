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

#include "UDPDataReader.h"

const unsigned int MAX_SOCK_SIZE = 16384U;

#if !defined(__WINDOWS__)
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#endif


CUDPDataReader::CUDPDataReader(const wxString& address, int port) :
wxThread(),
m_address(address),
m_port(port),
m_remAddr(),
m_remAddrLen(0U),
m_id(),
m_callback(),
m_fd(-1),
m_buffer(NULL),
m_count(0U),
m_enabled(false)
{
	for (int i = 0U; i < MAX_CALLBACKS; i++)
		m_callback[i] = NULL;
}

CUDPDataReader::~CUDPDataReader()
{
}

void CUDPDataReader::setCallback(IRawDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	if (id < 0 || id >= MAX_CALLBACKS) {
		m_callback[0] = callback;
		m_id[0]       = id;
	} else {
		m_callback[id] = callback;
		m_id[id]       = id;
	}
}

bool CUDPDataReader::open()
{
	if (m_count >= 1U) {
		m_count++;
		return true;
	}

#if defined(__WINDOWS__)
	WSAData data;

	int wsaRet =  ::WSAStartup(0x101, &data);
	if (wsaRet != 0) {
		::wxLogError(wxT("UDPDataReader: Error %d when initialising Winsock."), wsaRet);
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
			::wxLogError(wxT("UDPDataReader: Error %d when resolving host: %s"),
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

	m_remAddrLen = length;
	m_remAddr = new char[m_remAddrLen];
	::memcpy(m_remAddr, &addr, m_remAddrLen);

	m_fd = ::socket(PF_INET, SOCK_DGRAM, 0);
	if (m_fd < 0) {
		::wxLogError(wxT("UDPDataReader: Error %d when creating the reading datagram socket"),
#if defined(__WINDOWS__)
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
		::wxLogError(wxT("UDPDataReader: Error %d when binding the reading datagram socket"),
#if defined(__WINDOWS__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	m_buffer = new char[MAX_SOCK_SIZE];

	m_enabled = true;
	m_count++;

	::wxLogMessage(wxT("UDPDataReader: started with address %s and port %d"), m_address.c_str(), m_port);

	Create();
	Run();

	return true;
}

void CUDPDataReader::close()
{
	m_enabled = false;

	if (m_count >= 2U) {
		m_count--;
		return;
	}

	m_count--;

	Delete();
}

void* CUDPDataReader::Entry()
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

#if defined(__WINDOWS__)
	::closesocket(m_fd);
	::WSACleanup();
#else
	::close(m_fd);
#endif
	m_fd = -1;

	delete[] m_remAddr;
	delete[] m_buffer;

	return (void *)0;
}

bool CUDPDataReader::readSocket()
{
	wxASSERT(m_fd != -1);

	// Check that the readfrom() won't block
	fd_set readFds;
	FD_ZERO(&readFds);
#if defined(__WINDOWS__)
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
		::wxLogError(wxT("UDPDataReader: Error %d while performing a select"),
#if defined(__WINDOWS__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	// No data?
	if (ret == 0)
		return true;

	if (!m_enabled)
		return true;

	struct sockaddr addr;
#if defined(__WINDOWS__)
	int size = sizeof(struct sockaddr);
#else
	socklen_t size = sizeof(struct sockaddr);
#endif

	ssize_t len = ::recvfrom(m_fd, m_buffer, MAX_SOCK_SIZE, 0, &addr, &size);
	if (len < 0) {
		::wxLogError(wxT("UDPDataReader: Error %d reading from the datagram socket"),
#if defined(__WINDOWS__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	if (addr.sa_family != AF_INET) {
		::wxLogError(wxT("UDPDataReader: Received datagram from a non IP address!"));
		return true;
	}

	struct sockaddr_in* inaddr = (struct sockaddr_in *)&addr;

	// Check if the data is for us
	if (::memcmp(m_remAddr, &inaddr->sin_addr.s_addr, m_remAddrLen) != 0) {
		unsigned char* p = (unsigned char *)&inaddr->sin_addr.s_addr;
		unsigned char* q = (unsigned char *)m_remAddr;
		::wxLogWarning(wxT("UDPDataReader: UDP Data received from an invalid IP address: %u.%u.%u.%u, wanted: %u.%u.%u.%u"), p[0], p[1], p[2], p[3], q[0], q[1], q[2], q[3]);
		return true;
	}

	if (len == 0)
		return true;

	for (int i = 0U; i < MAX_CALLBACKS; i++) {
		if (m_callback[i] != NULL) {
			bool ret = m_callback[i]->callback(m_buffer, len, m_id[i]);
			if (ret)
				break;
		}
	}

	return true;
}
