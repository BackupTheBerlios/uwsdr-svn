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

#include "UDPDataWriter.h"


CUDPDataWriter::CUDPDataWriter(const wxString& address, int port) :
m_address(address),
m_port(port),
m_fd(-1),
m_remAddr(),
m_count(0U)
{
}

CUDPDataWriter::~CUDPDataWriter()
{
}

bool CUDPDataWriter::open()
{
	if (m_count >= 1U) {
		m_count++;
		return true;
	}

#if defined(__WINDOWS__)
	WSAData data;

	int ret =  ::WSAStartup(0x101, &data);
	if (ret != 0) {
		::wxLogError(wxT("UDPDataWriter: Error %d when initialising Winsock."), ret);
		return false;
	}
#endif

#if defined(__WINDOWS__)
	unsigned long addr = ::inet_addr(m_address.c_str());
#else
	in_addr_t addr = ::inet_addr(m_address.c_str());
#endif
	unsigned int length = 4U;

	if (addr == INADDR_NONE) {
		struct hostent* host = ::gethostbyname(m_address.c_str());

		if (host == NULL) {
			::wxLogError(wxT("UDPDataWriter: Error %d when resolving host: %s"),
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
		::wxLogError(wxT("UDPDataWriter: Error %d when creating the writing datagram socket"),
#if defined(__WINDOWS__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	::wxLogMessage(wxT("UDPDataWriter: started with address %s and port %d"), m_address.c_str(), m_port);

	m_count++;

	return true;
}

bool CUDPDataWriter::write(const void* buffer, unsigned int len)
{
	wxASSERT(m_fd != -1);

	ssize_t ret = ::sendto(m_fd, (char *)buffer, len, 0, (struct sockaddr *)&m_remAddr, sizeof(struct sockaddr_in));
	if (ret < 0) {
		::wxLogError(wxT("UDPDataWriter: Error %d writing to the datagram socket"),
#if defined(__WINDOWS__)
			::WSAGetLastError());
#else
			errno);
#endif
		return false;
	}

	if (ret != int(len)) {
		::wxLogError(wxT("UDPDataWriter: Error only wrote %ld of %u bytes to the datagram socket"), (unsigned long)ret, len);
		return false;
	}

	return true;
}

void CUDPDataWriter::close()
{
	if (m_count >= 2U) {
		m_count--;
		return;
	}

	m_count--;

#if defined(__WINDOWS__)
	::closesocket(m_fd);
	::WSACleanup();
#else
	::close(m_fd);
#endif

	delete this;
}
