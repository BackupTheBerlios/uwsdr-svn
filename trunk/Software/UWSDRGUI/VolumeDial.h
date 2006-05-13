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

#ifndef	VolumeDial_H
#define	VolumeDial_H

#include <wx/wx.h>

#include "DialInterface.h"

class CVolumeDial : public wxPanel {

    public:
	CVolumeDial(wxWindow* parent, int id, int min, int max, int value, IDialInterface* callback, const wxPoint& pos, const wxSize& size, long style = 0L, const wxString& name = wxPanelNameStr);
	virtual ~CVolumeDial();

	virtual void setValue(unsigned int value);

	void onPaint(wxPaintEvent& event);
	void onMouse(wxMouseEvent& event);
	void onTimer(wxTimerEvent& event);

    private:
	wxTimer*        m_timer;
	int             m_width;
	int             m_height;
	IDialInterface* m_callback;
	wxBitmap*       m_bitmap;
	int             m_min;
	int             m_max;
	int             m_value;
	int             m_increment;
	int             m_state;

	DECLARE_EVENT_TABLE()

	void drawDial();

	void show(wxDC& dc);
	void moveDial();
};

#endif
