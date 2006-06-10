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

#include "VolumeDial.h"

#include "UWSDRDefs.h"

#include <cmath>

const int VOLUME_TIMER = 56754;

const int STATE_NONE  = 0;
const int STATE_LEFT  = 1;
const int STATE_RIGHT = 2;

BEGIN_EVENT_TABLE(CVolumeDial, wxPanel)
	EVT_PAINT(CVolumeDial::onPaint)
	EVT_LEFT_DOWN(CVolumeDial::onMouse)
	EVT_LEFT_UP(CVolumeDial::onMouse)
	EVT_RIGHT_DOWN(CVolumeDial::onMouse)
	EVT_RIGHT_UP(CVolumeDial::onMouse)
	EVT_LEAVE_WINDOW(CVolumeDial::onMouse)
	EVT_TIMER(VOLUME_TIMER, CVolumeDial::onTimer)
END_EVENT_TABLE()

CVolumeDial::CVolumeDial(wxWindow* parent, int id, int min, int max, int value, IDialInterface* callback, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
wxPanel(parent, id, pos, size, style, name),
m_timer(NULL),
m_width(size.GetWidth()),
m_height(size.GetHeight()),
m_callback(callback),
m_bitmap(NULL),
m_min(min),
m_max(max),
m_value(value),
m_increment(0),
m_state(STATE_NONE)
{
	wxASSERT(m_max > m_min);
	wxASSERT(m_value >= m_min && value < m_max);
	wxASSERT(m_width == m_height);

	m_timer  = new wxTimer(this, VOLUME_TIMER);
	m_bitmap = new wxBitmap(m_width, m_height);

	m_increment = (m_max - m_min) / 200;

	drawDial();
}

CVolumeDial::~CVolumeDial()
{
	delete m_timer;
	delete m_bitmap;
}

void CVolumeDial::setValue(unsigned int value)
{
	m_value = value;

	drawDial();
}

void CVolumeDial::drawDial()
{
	wxMemoryDC dc;
	dc.SelectObject(*m_bitmap);

// This needs fixing XXX
#if defined(__WXMSW__)
	wxColour bgColour(0xD0, 0xD0, 0xD0);
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

	dc.SetBrush(*wxLIGHT_GREY_BRUSH);
	wxPen pen1(*wxBLACK, 2, wxSOLID);
	dc.SetPen(pen1);
	dc.DrawEllipse(1, 1, m_width - 2, m_height - 2);

	double incr = 270.0 / double(m_max - m_min); 

	double angle = (double(m_value - m_min) * incr + 45.0) * (M_PI / 180.0);

	double xFrac = ::sin(angle);
	double yFrac = ::cos(angle);

	int startX = m_width / 2 - int(double(m_width / 2 - 10) * xFrac + 0.5);
	int startY = m_height / 2 + int(double(m_height / 2 - 10) * yFrac + 0.5);

	int endX = m_width / 2 - int(double(m_width / 2 - 20) * xFrac + 0.5);
	int endY = m_height / 2 + int(double(m_height / 2 - 20) * yFrac + 0.5);

	wxPen pen2(*wxBLACK, 5, wxSOLID);
	dc.SetPen(pen2);
	dc.DrawLine(startX, startY, endX, endY);	

	dc.SelectObject(wxNullBitmap);

	wxClientDC clientDC(this);
	show(clientDC);
}

void CVolumeDial::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	show(dc);
}

void CVolumeDial::show(wxDC& dc)
{
	dc.DrawBitmap(*m_bitmap, 0, 0, false);
}

void CVolumeDial::onMouse(wxMouseEvent& event)
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

			if (dist > (m_width - 2) / 2)
				return;

			moveDial();
			m_timer->Start(50);
		}

		m_state = state;
	}
}

void CVolumeDial::onTimer(wxTimerEvent& event)
{
	moveDial();
}

void CVolumeDial::moveDial()
{
	wxASSERT(m_callback != NULL);

	switch (m_state) {
		case STATE_LEFT:
			if (m_value <= m_min)
				return;
			else
				m_value -= m_increment;
			break;
		case STATE_RIGHT:
			if (m_value >= m_max)
				return;
			else
				m_value += m_increment;
			break;
		case STATE_NONE:
			return;
	}

	m_callback->dialMoved(GetId(), m_value);

	drawDial();
}
