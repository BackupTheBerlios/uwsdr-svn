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

#ifndef	CWKeyboard_H
#define	CWKeyboard_H

#include <wx/wx.h>
#include <wx/spinctrl.h>

#include "UWSDRDefs.h"

class CCWKeyboard : public wxDialog {

    public:
	CCWKeyboard(wxWindow* parent, int id);
	~CCWKeyboard();

	void onTransmit(wxCommandEvent& event);
	void onHelp(wxCommandEvent& event);

	void     setLocal(const wxString& text);
	wxString getLocal() const;

	void     setRemote(const wxString& text);
	wxString getRemote() const;

	void     setLocator(const wxString& text);
	wxString getLocator() const;

	void     setReport(const wxString& text);
	wxString getReport() const;

	void     setSerial(const wxString& text);
	wxString getSerial() const;

	void     setMessage(unsigned int n, const wxString& text);
	wxString getMessage(unsigned int n) const;

	void         setSpeed(unsigned int speed);
	unsigned int getSpeed() const;

    private:
	wxTextCtrl*    m_local;
	wxTextCtrl*    m_remote;
	wxTextCtrl*    m_locator;
	wxTextCtrl*    m_report;
	wxTextCtrl*    m_serial;
	wxTextCtrl*    m_text[CWKEYBOARD_COUNT];
	wxRadioButton* m_button[CWKEYBOARD_COUNT];
	wxSpinCtrl*    m_speed;

	DECLARE_EVENT_TABLE()
};

#endif
