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

#ifndef	PortDialog_H
#define	PortDialog_H

#include <wx/wx.h>


class CPortDialog : public wxDialog {

    public:
	CPortDialog(wxWindow* parent, const wxString& title, const wxString& device, int pin, int id = -1);
	virtual ~CPortDialog();

	void onDevice(wxCommandEvent& event);
	void onOK(wxCommandEvent& event);

	virtual wxString getDevice() const;
	virtual int      getPin() const;

    private:
	wxChoice* m_devChoice;
	wxChoice* m_pinChoice;

	wxString  m_device;
	int       m_pin;

	DECLARE_EVENT_TABLE()
};

#endif
