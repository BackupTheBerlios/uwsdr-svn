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

#ifndef	SDREmulatorFrame_H
#define	SDREmulatorFrame_H

#include <wx/wx.h>
#include <wx/socket.h>

#include "Frequency.h"

class CSDREmulatorFrame : public wxFrame {

    public:
	CSDREmulatorFrame(unsigned int port);
	virtual ~CSDREmulatorFrame();

	void onClose(wxCloseEvent& event);

	void onParentSocket(wxSocketEvent& event);
	void onChildSocket(wxSocketEvent& event);

    private:
	CFrequency   m_txFreq;
	CFrequency   m_rxFreq;
	bool         m_txEnable;
	bool         m_rxEnable;
	bool         m_txOn;
	unsigned int m_rxGain;

	wxSocketServer* m_server;
	wxStaticText*   m_connectLabel;
	wxStaticText*   m_txFreqLabel;
	wxStaticText*   m_rxFreqLabel;
	wxStaticText*   m_txEnabledLabel;
	wxStaticText*   m_rxEnabledLabel;
	wxStaticText*   m_txOnLabel;
	wxStaticText*   m_rxGainLabel;
	wxListBox*      m_messages;

	DECLARE_EVENT_TABLE()

	bool createListener(unsigned int port);
	void processCommand(wxSocketBase& socket, wxChar* buffer);
};

#endif
