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
#include "DataControl.h"

class CSDREmulatorFrame : public wxFrame {

    public:
	CSDREmulatorFrame(const wxString& address, unsigned int controlPort, bool muted, unsigned int maxSamples, bool delay);
	virtual ~CSDREmulatorFrame();

	void onClose(wxCloseEvent& event);
	void onExit(wxCommandEvent& event);
	void onInternal1(wxCommandEvent& event);
	void onInternal2(wxCommandEvent& event);
	void onSoundFile(wxCommandEvent& event);
	void onSoundCard(wxCommandEvent& event);

	void onParentSocket(wxSocketEvent& event);
	void onChildSocket(wxSocketEvent& event);

    private:
	CFrequency    m_txFreq;
	CFrequency    m_rxFreq;
	bool          m_txEnable;
	bool          m_rxEnable;
	bool          m_txOn;
	CDataControl* m_data;
	bool          m_started;

	wxSocketServer* m_server;
	wxMenuBar*      m_menuBar;
	wxStaticText*   m_sourceLabel;
	wxStaticText*   m_connectLabel;
	wxStaticText*   m_txFreqLabel;
	wxStaticText*   m_rxFreqLabel;
	wxStaticText*   m_txEnabledLabel;
	wxStaticText*   m_rxEnabledLabel;
	wxStaticText*   m_txOnLabel;
	wxListBox*      m_messages;

	DECLARE_EVENT_TABLE()

	bool createListener(unsigned int port);
	bool createDataThread(const wxString& address, unsigned int port, int inDev, int outDev, bool muted, unsigned int maxSamples, bool delay);

	void processCommand(wxSocketBase& socket, wxChar* buffer);
};

#endif
