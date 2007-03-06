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

#ifndef	GUISetupFrame_H
#define	GUISetupFrame_H

#include <wx/wx.h>

#include "SDRDescrFile.h"
#include "PortDialog.h"

class CGUISetupFrame : public wxFrame {

    public:
	CGUISetupFrame();
	virtual ~CGUISetupFrame();

	void onName(wxCommandEvent& event);
	void onBrowse(wxCommandEvent& event);
	void onUserAudio(wxCommandEvent& event);
	void onSDRAudio(wxCommandEvent& event);
	void onEthernet(wxCommandEvent& event);
	void onPort(wxCommandEvent& event);
	void onCreate(wxCommandEvent& event);

    private:
	wxComboBox*    m_name;
	wxTextCtrl*    m_filenameText;
	wxCheckBox*    m_startMenu;
	wxCheckBox*    m_deskTop;
	wxButton*      m_userAudio;
	wxButton*      m_sdrAudio;
	wxButton*      m_ethernet;
	wxButton*      m_port;
	wxString       m_filename;
	SDRTYPE        m_sdrType;
	long           m_userAudioInDev;
	long           m_userAudioOutDev;
	long           m_sdrAudioInDev;
	long           m_sdrAudioOutDev;
	wxString       m_ipAddress;
	long           m_controlPort;
	long           m_dataPort;
	bool           m_txInEnable;
	wxString       m_txInDev;
	INPIN          m_txInPin;
	bool           m_keyInEnable;
	wxString       m_keyInDev;
	INPIN          m_keyInPin;
	wxString       m_txOutDev;
	OUTPIN         m_txOutPin;

	DECLARE_EVENT_TABLE()

	void enumerateConfigs();
	void readConfig(const wxString& name);
#if defined(__WXMSW__)
	void writeStartMenu(const wxString& name, const wxString& instDir);
	void writeDeskTop(const wxString& name, const wxString& instDir);
#elif defined(__WXGTK__)
	bool getMenuDir(wxString& dir) const;
	bool getDesktopDir(wxString& dir) const;
	void writeDeskTop(const wxString& name, const wxString& instDir);
#endif
};

#endif
