/*
 *   Copyright (C) 2006,7 by Jonathan Naylor G4KLX
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

#ifndef	SDREmulatorWriter_H
#define	SDREmulatorWriter_H

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
#include <errno.h>
#endif

class CSDREmulatorWriter : public wxThread, public IDataWriter {

    public:
	CSDREmulatorWriter(const wxString& address, int port, unsigned int version, unsigned int maxSamples = 2048, bool delay = false);
	virtual ~CSDREmulatorWriter();

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void write(const float* buffer, unsigned int nSamples);

	virtual void* Entry();

	virtual void close();

	virtual void purge();

    private:
	wxString           m_address;
	unsigned short     m_port;
	unsigned int       m_version;
	int                m_fd;
	struct sockaddr_in m_remAddr;
	int                m_sequence;
	unsigned char*     m_sockBuffer;
	float*             m_dataBuffer;
	CRingBuffer*       m_buffer;
	unsigned long      m_delay;
	wxSemaphore        m_waiting;
	unsigned int       m_requests;
	unsigned int       m_overruns;
	unsigned int       m_packets;
	unsigned int       MAX_SAMPLES;
	bool               DELAY;
	unsigned int       m_packetRequests;

	void writePacket();
};

#endif
