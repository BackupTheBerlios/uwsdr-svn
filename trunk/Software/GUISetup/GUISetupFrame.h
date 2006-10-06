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

#ifndef	GUISetupFrame_H
#define	GUISetupFrame_H

#include <wx/wx.h>

#include "SoundCardInfo.h"

class CGUISetupFrame : public wxFrame {

    public:
	CGUISetupFrame();
	virtual ~CGUISetupFrame();

	void onAPI(wxCommandEvent& event);
	void onName(wxCommandEvent& event);
	void onBrowse(wxCommandEvent& event);
	void onCreate(wxCommandEvent& event);

    private:
	wxComboBox*    m_name;
	wxTextCtrl*    m_filename;
	wxChoice*      m_apiChoice;
	wxChoice*      m_devChoice;
	wxTextCtrl*    m_address;
	wxTextCtrl*    m_control;
	wxTextCtrl*    m_data;
	wxCheckBox*    m_startMenu;
	CSoundCardInfo m_info;
#ifdef __WXMSW__
	wxCheckBox* m_deskTop;
#endif

	DECLARE_EVENT_TABLE()

	void enumerateConfigs();
	void readConfig(const wxString& name);
	void enumerateAPI();
	void enumerateAudio(const CSoundCardAPI& api);
	void writeStartMenu(const wxString& name, const wxString& instDir);
#ifdef __WXMSW__
	void writeDeskTop(const wxString& name, const wxString& instDir);
#endif
};

#endif
