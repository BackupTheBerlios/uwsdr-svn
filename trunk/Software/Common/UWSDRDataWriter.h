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

#ifndef	UWSDRDataWriter_H
#define	UWSDRDataWriter_H

#include <wx/wx.h>

#include "UDPDataWriter.h"
#include "RingBuffer.h"
#include "DataWriter.h"


class CUWSDRDataWriter : public wxThread, public IDataWriter {

    public:
	CUWSDRDataWriter(CUDPDataWriter* writer, unsigned int version, unsigned int maxSamples = 2048, bool delay = false);

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void write(const float* buffer, unsigned int nSamples);

	virtual void* Entry();

	virtual void close();

	virtual void enable(bool enable = true);
	virtual void disable();

    protected:
	virtual ~CUWSDRDataWriter();

    private:
	CUDPDataWriter* m_writer;
	unsigned int    m_version;
	int             m_sequence;
	unsigned char*  m_sockBuffer;
	float*          m_dataBuffer;
	CRingBuffer*    m_buffer;
	bool            m_delay;
	unsigned long   m_delayTime;
	wxSemaphore     m_waiting;
	unsigned int    m_requests;
	unsigned int    m_overruns;
	unsigned int    m_packets;
	unsigned int    m_maxSamples;
	unsigned int    m_packetRequests;
	bool            m_enabled;

	void writePacket();
};

#endif