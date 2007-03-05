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

#include "Common.h"

class CPortDialog : public wxDialog {

    public:
	CPortDialog(wxWindow* parent, const wxString& title, bool setTXOut, int id = -1);
	virtual ~CPortDialog();

	void onTXInCheck(wxCommandEvent& event);
	void onKeyInCheck(wxCommandEvent& event);
	void onOK(wxCommandEvent& event);

	virtual void     setTXInEnable(bool enable);
	virtual void     setTXInDev(const wxString& dev);
	virtual void     setTXInPin(INPIN pin);
	virtual bool     getTXInEnable() const;
	virtual wxString getTXInDev() const;
	virtual INPIN    getTXInPin() const;

	virtual void     setKeyInEnable(bool enable);
	virtual void     setKeyInDev(const wxString& dev);
	virtual void     setKeyInPin(INPIN pin);
	virtual bool     getKeyInEnable() const;
	virtual wxString getKeyInDev() const;
	virtual INPIN    getKeyInPin() const;

	virtual void     setTXOutDev(const wxString& dev);
	virtual void     setTXOutPin(OUTPIN pin);
	virtual wxString getTXOutDev() const;
	virtual OUTPIN   getTXOutPin() const;

    private:
	wxCheckBox* m_txInSelect;
	wxChoice*   m_txInDevChoice;
	wxChoice*   m_txInPinChoice;
	wxCheckBox* m_keyInSelect;
	wxChoice*   m_keyInDevChoice;
	wxChoice*   m_keyInPinChoice;
	wxChoice*   m_txOutDevChoice;
	wxChoice*   m_txOutPinChoice;

	bool      m_txInEnable;
	wxString  m_txInDev;
	INPIN     m_txInPin;
	bool      m_keyInEnable;
	wxString  m_keyInDev;
	INPIN     m_keyInPin;
	bool      m_txOutEnable;
	wxString  m_txOutDev;
	OUTPIN    m_txOutPin;

	DECLARE_EVENT_TABLE()
};

#endif
