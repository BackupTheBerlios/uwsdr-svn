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

#ifndef	SDRDataWriter_H
#define	SDRDataWriter_H

#include <wx/wx.h>

#include "RingBuffer.h"
#include "DataWriter.h"

#if !defined(__WXMSW__)
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

class CSDRDataWriter : public IDataWriter {

    public:
	CSDRDataWriter(const wxString& address, int port);
	virtual ~CSDRDataWriter();

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void write(const float* buffer, unsigned int nSamples);

	virtual void close();

    private:
	wxString           m_address;
	int                m_port;
	int                m_fd;
	struct sockaddr_in m_remAddr;
	int                m_sequence;
	unsigned char*     m_sockBuffer;
};

#endif
