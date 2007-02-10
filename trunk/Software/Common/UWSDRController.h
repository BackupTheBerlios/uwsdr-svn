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

#ifndef	UWSDRController_H
#define	UWSDRController_H

#include <wx/wx.h>
#include <wx/socket.h>

#include "SDRController.h"
#include "Frequency.h"
#include "ControlInterface.h"

class CUWSDRController : public wxEvtHandler, public ISDRController {

    public:
	CUWSDRController(const wxString& address, int port, unsigned int version);
	virtual ~CUWSDRController();

	virtual void setCallback(IControlInterface* callback, int id);

	virtual bool open();
	virtual void enableTX(bool on);
	virtual void enableRX(bool on);
	virtual void setTXAndFreq(bool transmit, const CFrequency& freq);
	virtual void sendCommand(const char* command);
	virtual void setClockTune(unsigned int clock);
	virtual void close();

	void onSocket(wxSocketEvent& event);

    private:
	wxString           m_address;
	int                m_port;
	int                m_id;
	wxSocketClient*    m_socket;
	IControlInterface* m_callback;
	unsigned int       m_version;
	CFrequency         m_txFreq;
	CFrequency         m_rxFreq;
	bool               m_enableTX;
	bool               m_enableRX;
	bool               m_tx;
	unsigned int       m_clock;

	DECLARE_EVENT_TABLE()
};

#endif
