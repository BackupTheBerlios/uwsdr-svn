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

#ifndef	UWSDRData_H
#define	UWSDRData_H

#include <wx/wx.h>
#include <wx/socket.h>

#include "RingBuffer.h"
#include "DataReader.h"
#include "DataWriter.h"

class CUWSDRData : public wxEvtHandler, public IDataReader, public IDataWriter {

    public:
	CUWSDRData(const wxString& address, int port, unsigned int version, bool enable);
	virtual ~CUWSDRData();

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void write(const float* buffer, unsigned int nSamples);

	virtual void close();

	void onSocket(wxSocketEvent& event);

    private:
	wxString          m_address;
	unsigned short    m_port;
	unsigned int      m_version;
	unsigned int      m_count;
	bool              m_enabled;
	int               m_id;
	IDataCallback*    m_callback;
	wxDatagramSocket* m_socket;
	wxIPV4address     m_ipAddress;
	int               m_sequenceIn;
	int               m_sequenceOut;
	float*            m_inBuffer;
	unsigned char*    m_sockBuffer;
	unsigned char*    m_outBuffer;
	CRingBuffer       m_rxBuffer;
	CRingBuffer       m_txBuffer;

	DECLARE_EVENT_TABLE()

	void writePacket(const float* buffer, unsigned int nSamples);
};

#endif
