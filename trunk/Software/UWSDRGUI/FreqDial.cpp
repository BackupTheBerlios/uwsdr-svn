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

#include "FreqDial.h"

#include "UWSDRDefs.h"

#include <cmath>

enum {
	STATE_NONE,
	STATE_LEFT,
	STATE_RIGHT
};

const int FREQ_TIMER = 67523;

BEGIN_EVENT_TABLE(CFreqDial, wxPanel)
	EVT_PAINT(CFreqDial::onPaint)
	EVT_LEFT_DOWN(CFreqDial::onMouse)
	EVT_LEFT_UP(CFreqDial::onMouse)
	EVT_RIGHT_DOWN(CFreqDial::onMouse)
	EVT_RIGHT_UP(CFreqDial::onMouse)
	EVT_LEAVE_WINDOW(CFreqDial::onMouse)
	EVT_TIMER(FREQ_TIMER, CFreqDial::onTimer)
END_EVENT_TABLE()

CFreqDial::CFreqDial(wxWindow* parent, int id, IDialInterface* callback, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
wxPanel(parent, id, pos, size, style, name),
m_timer(NULL),
m_width(size.GetWidth()),
m_height(size.GetHeight()),
m_callback(callback),
m_bitmap(NULL),
m_state(STATE_NONE),
m_angle(0),
m_mult(0)
{
	wxASSERT(m_height == m_width);

	m_timer  = new wxTimer(this, FREQ_TIMER),
	m_bitmap = new wxBitmap(m_width, m_height);

	drawDial();
}

CFreqDial::~CFreqDial()
{
	delete m_timer;
	delete m_bitmap;
}

void CFreqDial::drawDial()
{
	wxMemoryDC dc;
	dc.SelectObject(*m_bitmap);

// This needs fixing XXX
#if defined(__WXMSW__)
	wxColour bgColour(0xD4, 0xD0, 0xC8);
#elif defined(__WXGTK__)
	wxColour bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_BACKGROUND);
#else
#error "Unknown platform"
#endif

	wxBrush brush0(bgColour);
	wxPen pen0(bgColour);

	dc.SetPen(pen0);
	dc.SetBrush(brush0);
	dc.DrawRectangle(0, 0, m_width, m_height);

	int middleX = m_width / 2;
	int middleY = m_height / 2;

	dc.SetBrush(*wxLIGHT_GREY_BRUSH);
	wxPen pen1(*wxBLACK, 2, wxSOLID);
	dc.SetPen(pen1);
	dc.DrawCircle(middleX, middleY, (m_width - 2) / 2);

	wxPen pen3(*wxWHITE, 1, wxSOLID);
	dc.SetPen(pen3);
	dc.DrawCircle(middleX, middleY, (m_width - 2) / 3);
	dc.DrawCircle(middleX, middleY, (m_width - 2) / 6);

	int x = m_width / 2 - int(double(m_width / 2 - 25) * ::sin(m_angle * (M_PI / 180.0)) + 0.5);
	int y = m_height / 2 + int(double(m_height / 2 - 25) * ::cos(m_angle * (M_PI / 180.0)) + 0.5);

	dc.SetBrush(*wxBLACK_BRUSH);
	wxPen pen2(*wxWHITE, 2, wxSOLID);
	dc.SetPen(pen2);
	dc.DrawCircle(x, y, 20);

	dc.SelectObject(wxNullBitmap);

	wxClientDC clientDC(this);
	show(clientDC);
}

void CFreqDial::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	show(dc);
}

void CFreqDial::show(wxDC& dc)
{
	dc.DrawBitmap(*m_bitmap, 0, 0, false);
}

void CFreqDial::onMouse(wxMouseEvent& event)
{
	wxASSERT(m_timer != NULL);

	int state;
	if (event.LeftDown())
		state = STATE_LEFT;
	else if (event.LeftUp())
		state = STATE_NONE;
	else if (event.RightDown())
		state = STATE_RIGHT;
	else if (event.RightUp())
		state = STATE_NONE;
	else if (event.Leaving())
		state = STATE_NONE;
	else
		return;

	if (state != m_state) {
		if (state == STATE_NONE) {
			m_timer->Stop();
		} else {
			long diffX = event.GetX() - m_width / 2;
			long diffY = event.GetY() - m_height / 2;
			int   dist = int(::sqrt(double(diffX * diffX + diffY * diffY)) + 0.5);

			if (dist <= (m_width - 2) / 6)
				m_mult = 1;
			else if (dist <= (m_width - 2) / 3)
				m_mult = 2;
			else if (dist <= (m_width - 2) / 2)
				m_mult = 3;
			else
				return;

			moveDial();
			m_timer->Start(30);
		}

		m_state = state;
	}
}

void CFreqDial::onTimer(wxTimerEvent& event)
{
	moveDial();
}

void CFreqDial::moveDial()
{
	wxASSERT(m_callback != NULL);

	switch (m_state) {
		case STATE_LEFT:
			m_angle -= 3 * m_mult;
			m_callback->dialMoved(GetId(), -m_mult);
			break;
		case STATE_RIGHT:
			m_angle += 3 * m_mult;
			m_callback->dialMoved(GetId(), m_mult);
			break;
		case STATE_NONE:
			return;
	}

	if (m_angle > 360)
		m_angle -= 360;
	if (m_angle < -360)
		m_angle += 360;

	drawDial();
}
