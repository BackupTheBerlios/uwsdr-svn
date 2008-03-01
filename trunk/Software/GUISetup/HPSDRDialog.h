/*
 *   Copyright (C) 2008 by Jonathan Naylor G4KLX
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

#ifndef	HPSDRDialog_H
#define	HPSDRDialog_H

#include <wx/wx.h>
#include <wx/notebook.h>

class CHPSDRDialog : public wxDialog {

    public:
	CHPSDRDialog(wxWindow* parent, const wxString& title, int c0, int c1, int c2, int c3, int c4, bool rxonly, int id = -1);
	virtual ~CHPSDRDialog();

	void onButton(wxCommandEvent& event);
	void onOK(wxCommandEvent& event);

	virtual int getC0() const;
	virtual int getC1() const;
	virtual int getC2() const;
	virtual int getC3() const;
	virtual int getC4() const;

    private:
	wxCheckBox* m_atlas10mhzRef;

	wxCheckBox* m_janusMic;

	wxCheckBox* m_mercury10mhzRef;
	wxCheckBox* m_mercury1228mhzSource;
	wxChoice*   m_mercuryAtten;
	wxCheckBox* m_mercuryConfig;
	wxCheckBox* m_mercuryDither;
	wxCheckBox* m_mercuryPreamp;

	wxCheckBox* m_penelope10mhzRef;
	wxCheckBox* m_penelope1228mhzSource;
	wxCheckBox* m_penelopeConfig;
	wxCheckBox* m_penelopeMic;

	bool        m_rxonly;
	int         m_c0;
	int         m_c1;
	int         m_c2;
	int         m_c3;
	int         m_c4;

	DECLARE_EVENT_TABLE()

	wxPanel* createAtlasTab(wxNotebook* noteBook);
	wxPanel* createJanusTab(wxNotebook* noteBook);
	wxPanel* createMercuryTab(wxNotebook* noteBook);
	wxPanel* createPenelopeTab(wxNotebook* noteBook);
};

#endif
