/*
 *   Copyright (C) 2007 by Jonathan Naylor G4KLX
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

#ifndef	EthernetDialog_H
#define	EthernetDialog_H

#include <wx/wx.h>

class CEthernetDialog : public wxDialog {

    public:
	CEthernetDialog(wxWindow* parent, const wxString& title, const wxString& address, long control, int id = -1);
	virtual ~CEthernetDialog();

	void onOK(wxCommandEvent& event);

	virtual wxString getIPAddress() const;
	virtual long     getControlPort() const;

    private:
	wxTextCtrl* m_address;
	wxTextCtrl* m_control;

	wxString    m_ipAddress;
	long        m_controlPort;

	DECLARE_EVENT_TABLE()
};

#endif
