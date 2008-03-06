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

#ifndef	UDPDataReader_H
#define	UDPDataReader_H

#include <wx/wx.h>

#include "RawDataCallback.h"

const unsigned int MAX_CALLBACKS = 5U;

class CUDPDataReader : public wxThread {

    public:
	CUDPDataReader(const wxString& address, int port);

	virtual void setCallback(IRawDataCallback* callback, int id);

	virtual bool open();

	virtual void* Entry();

	virtual void close();

    protected:
	virtual ~CUDPDataReader();

    private:
	wxString          m_address;
	unsigned short    m_port;
	char*             m_remAddr;
	unsigned int      m_remAddrLen;
	int               m_id[MAX_CALLBACKS];
	IRawDataCallback* m_callback[MAX_CALLBACKS];
	int               m_fd;
	char*             m_buffer;
	unsigned int      m_count;
	bool              m_enabled;

	bool readSocket();
};

#endif
