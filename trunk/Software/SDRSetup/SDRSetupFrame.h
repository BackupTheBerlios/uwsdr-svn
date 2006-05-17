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

#ifndef	SDRSetupFrame_H
#define	SDRSetupFrame_H

#include <wx/wx.h>

class CSDRSetupFrame : public wxFrame {

    public:
	CSDRSetupFrame();
	virtual ~CSDRSetupFrame();

	void onExecute(wxCommandEvent& event);

    private:
	wxTextCtrl* m_oldSDRAddress;
	wxTextCtrl* m_oldSDRControlPort;
	wxTextCtrl* m_sdrAddress;
	wxTextCtrl* m_sdrControlPort;
	wxTextCtrl* m_sdrDataPort;
	wxTextCtrl* m_dspAddress;

	DECLARE_EVENT_TABLE()
};

#endif
