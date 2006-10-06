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

#ifndef	UWSDRDataReader_H
#define	UWSDRDataReader_H

#include <wx/wx.h>
#include <wx/socket.h>

#include "DataReader.h"

class CSDRDataReader : public wxEvtHandler, public IDataReader {

    public:
	CSDRDataReader(const wxString& address, int port, unsigned int version);
	virtual ~CSDRDataReader();

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void close();

	void onSocket(wxSocketEvent& event);

	virtual bool needsClock();
	virtual void clock();

    private:
	wxString          m_address;
	unsigned short    m_port;
	unsigned int      m_version;
	int               m_id;
	IDataCallback*    m_callback;
	wxDatagramSocket* m_socket;
	wxIPV4address     m_ipAddress;
	int               m_sequence;
	float*            m_buffer;
	unsigned char*    m_sockBuffer;

	DECLARE_EVENT_TABLE()
};

#endif
