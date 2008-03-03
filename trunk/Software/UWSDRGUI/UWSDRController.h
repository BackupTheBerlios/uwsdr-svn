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

#ifndef	UWSDRController_H
#define	UWSDRController_H

#include <deque>
using namespace std;

#include <wx/wx.h>

#include "SDRController.h"
#include "Frequency.h"
#include "ControlInterface.h"
#include "UDPDataReader.h"
#include "UDPDataWriter.h"
#include "SocketCallback.h"


class CUWSDRController : public wxThread, public ISDRController, public ISocketCallback {

    public:
	CUWSDRController(CUDPDataReader* reader, CUDPDataWriter* writer, unsigned int version);

	virtual void setCallback(IControlInterface* callback);

	virtual void* Entry();

	virtual bool open();
	virtual void enableTX(bool on);
	virtual void enableRX(bool on);
	virtual void setTXAndFreq(bool transmit, const CFrequency& freq);
	virtual bool sendCommand(const wxString& command);
	virtual void setClockTune(unsigned int clock);
	virtual void close();

	virtual bool callback(char* buffer, unsigned int len, int id);

    protected:
	virtual ~CUWSDRController();

    private:
	CUDPDataReader*    m_reader;
	CUDPDataWriter*    m_writer;
	int                m_port;
	IControlInterface* m_callback;
	unsigned int       m_version;
	CFrequency         m_txFreq;
	CFrequency         m_rxFreq;
	bool               m_enableTX;
	bool               m_enableRX;
	bool               m_tx;
	unsigned int       m_clock;
	bool               m_replies;
	unsigned int       m_tries;
	deque<wxString>    m_commands;
	wxSemaphore        m_flag;
};

#endif
