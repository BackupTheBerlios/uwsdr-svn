/*
 *   Copyright (C) 2006,7 by Jonathan Naylor G4KLX
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

class CGUISetupFrame : public wxFrame {

    public:
	CGUISetupFrame();
	virtual ~CGUISetupFrame();

	void onName(wxCommandEvent& event);
	void onBrowse(wxCommandEvent& event);
	void onUserAudio(wxCommandEvent& event);
	void onSDRAudio(wxCommandEvent& event);
	void onEthernet(wxCommandEvent& event);
	void onCreate(wxCommandEvent& event);

    private:
	wxComboBox*    m_name;
	wxTextCtrl*    m_filenameText;
	wxCheckBox*    m_startMenu;
	wxButton*      m_userAudio;
	wxButton*      m_sdrAudio;
	wxButton*      m_ethernet;
#if defined(__WXMSW__)
	wxCheckBox*    m_deskTop;
#endif
	wxString       m_filename;
	int            m_userAudioAPI;
	long           m_userAudioInDev;
	long           m_userAudioOutDev;
	int            m_sdrAudioAPI;
	long           m_sdrAudioInDev;
	long           m_sdrAudioOutDev;
	wxString       m_ipAddress;
	long           m_controlPort;
	long           m_dataPort;

	DECLARE_EVENT_TABLE()

	void enumerateConfigs();
	void readConfig(const wxString& name);
	void writeStartMenu(const wxString& name, const wxString& instDir);
#if defined(__WXMSW__)
	void writeDeskTop(const wxString& name, const wxString& instDir);
#endif
#if defined(__WXGTK__)
	bool getDesktopDir(wxString& dir) const;
#endif
};

#endif
