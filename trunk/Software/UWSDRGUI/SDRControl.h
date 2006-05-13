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

#ifndef	SDRControl_H
#define	SDRControl_H

#include <wx/wx.h>
#include <wx/socket.h>

#include "Frequency.h"
#include "ControlInterface.h"

class CSDRControl : public wxEvtHandler {

    public:
	CSDRControl(IControlInterface* callback, int id);
	virtual ~CSDRControl();

	virtual bool open(const wxString& address, int port, unsigned int version, bool enable);
	virtual void enableTX(bool on);
	virtual void enableRX(bool on);
	virtual void setTXAndFreq(bool transmit, const CFrequency& freq);
	virtual void sendCommand(const char* command);
	virtual void close();

	virtual void onSocket(wxSocketEvent& event);

    private:
	int                m_id;
	bool               m_enabled;
	wxSocketClient     m_socket;
	IControlInterface* m_callback;
	unsigned int       m_version;
	CFrequency         m_txFreq;
	CFrequency         m_rxFreq;
	bool               m_enableTX;
	bool               m_enableRX;
	bool               m_tx;

	DECLARE_EVENT_TABLE()
};

#endif
