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

#ifndef	SDRDataReader_H
#define	SDRDataReader_H

#include <wx/wx.h>

#include "DataReader.h"
#include "RingBuffer.h"
#include "UDPDataReader.h"
#include "SocketCallback.h"


class CSDRDataReader : public IDataReader, public ISocketCallback {

    public:
	CSDRDataReader(const wxString& address, int port, unsigned int version);

	virtual void callback(unsigned char* buffer, unsigned int len, int id);

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void close();

	virtual void purge();

	virtual bool hasClock();
	virtual void clock();

    protected:
	virtual ~CSDRDataReader();

    private:
	wxString        m_address;
	int             m_port;
	CUDPDataReader* m_reader;
	unsigned int    m_version;
	unsigned int    m_blockSize;
	unsigned int    m_size;
	int             m_id;
	IDataCallback*  m_callback;
	int             m_sequence;
	CRingBuffer*    m_ringBuffer;
	float*          m_sampBuffer;
	unsigned int    m_missed;
	unsigned int    m_requests;
	unsigned int    m_underruns;
};

#endif
