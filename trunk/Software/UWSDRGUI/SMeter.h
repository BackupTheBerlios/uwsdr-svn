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

#ifndef	SMeter_H
#define	SMeter_H

#include <wx/wx.h>

class CSMeter : public wxPanel {

    public:
	CSMeter(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style = 0L, const wxString& name = wxPanelNameStr);
	virtual ~CSMeter();

	virtual void setTXMenu(bool set);

	virtual void setLevel(float level);

	virtual void setRXMeter(int meter);
	virtual void setTXMeter(int meter);

	virtual int  getRXMeter() const;
	virtual int  getTXMeter() const;

	void onPaint(wxPaintEvent& event);
	void onMouse(wxMouseEvent& event);
	void onMenu(wxCommandEvent& event);

    private:
	int          m_width;
	int          m_height;
	wxBitmap*    m_background;
	wxBitmap*    m_bitmap;
	wxMenu*      m_menu;
	wxMenu*      m_rxMenu;
	wxMenu*      m_txMenu;
	int          m_rxMeter;
	int          m_txMeter;
	float        m_lastLevel;

	DECLARE_EVENT_TABLE()

	void show(wxDC& dc);
	void createBackground();
};

#endif
