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

#ifndef	FreqDial_H
#define	FreqDial_H

#include <wx/wx.h>

#include "DialInterface.h"

class CFreqDial : public wxPanel {

    public:
	CFreqDial(wxWindow* parent, int id, IDialInterface* callback, const wxPoint& pos, const wxSize& size, long style = 0L, const wxString& name = wxPanelNameStr);
	virtual ~CFreqDial();

	void onPaint(wxPaintEvent& event);
	void onMouse(wxMouseEvent& event);
	void onTimer(wxTimerEvent& event);

    private:
	wxTimer*        m_timer;
	int             m_width;
	int             m_height;
	IDialInterface* m_callback;
	wxBitmap*       m_bitmap;
	int             m_state;
	int             m_angle;
	int             m_mult;

	DECLARE_EVENT_TABLE()

	void drawDial();

	void show(wxDC& dc);
	void moveDial();
};

#endif
