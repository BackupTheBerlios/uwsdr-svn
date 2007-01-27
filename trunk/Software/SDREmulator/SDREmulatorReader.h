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

#ifndef	SDREmulatorReader_H
#define	SDREmulatorReader_H

#include <wx/wx.h>

#include "DataReader.h"
#include "RingBuffer.h"


class CSDREmulatorReader : public wxThread, public IDataReader {

    public:
	CSDREmulatorReader(const wxString& address, int port, unsigned int version);

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void* Entry();

	virtual void close();

	virtual void purge();

	virtual bool hasClock();
	virtual void clock();

    protected:
	virtual ~CSDREmulatorReader();

    private:
	wxString       m_address;
	unsigned short m_port;
	unsigned int   m_version;
	unsigned int   m_blockSize;
	unsigned int   m_size;
	char*          m_remAddr;
	unsigned int   m_remAddrLen;
	int            m_id;
	IDataCallback* m_callback;
	int            m_fd;
	int            m_sequence;
	CRingBuffer*   m_buffer;
	unsigned char* m_sockBuffer;
	float*         m_sampBuffer;
	unsigned int   m_missed;
	unsigned int   m_requests;
	unsigned int   m_underruns;

	bool readSocket();
};

#endif
