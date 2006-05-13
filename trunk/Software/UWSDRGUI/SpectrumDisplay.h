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

#ifndef	SpectrumDisplay_H
#define	SpectrumDisplay_H

#include <wx/wx.h>

class CSpectrumDisplay : public wxPanel {

    public:
	CSpectrumDisplay(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style = 0L, const wxString& name = wxPanelNameStr);
	virtual ~CSpectrumDisplay();

	virtual void setSampleRate(unsigned int sampleRate);

	virtual void showSpectrum(const float* spectrum, double scale = 1.0);

	virtual void setType(int type);
	virtual void setSpeed(int speed);

	virtual int  getType() const;
	virtual int  getSpeed() const;

	void onPaint(wxPaintEvent& event);
	void onMouse(wxMouseEvent& event);
	void onMenu(wxCommandEvent& event);

    private:
	int          m_width;
	int          m_height;
	wxBitmap*    m_background;
	wxBitmap*    m_bitmap;
	unsigned int m_sampleRate;
	wxMenu*      m_menu;
	wxMenu*      m_speedMenu;
	int          m_type;
	int          m_speed;
	int          m_factor;
	unsigned int m_ticks;

	DECLARE_EVENT_TABLE()

	void show(wxDC& dc);

	void createPanadapter();
	void createWaterfall();

	void drawPanadapter(const float* spectrum, double scale);
	void drawWaterfall(const float* spectrum, double scale);
};

#endif
