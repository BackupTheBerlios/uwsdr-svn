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

#include "SMeter.h"

#include "UWSDRDefs.h"

#include <cmath>

enum {
	MENU_I_INPUT = 7863,
	MENU_Q_INPUT,
	MENU_SIGNAL,
	MENU_AVG_SIGNAL,
	MENU_AGC,
	MENU_MICROPHONE,
	MENU_POWER,
	MENU_ALC
};

BEGIN_EVENT_TABLE(CSMeter, wxPanel)
	EVT_RIGHT_DOWN(CSMeter::onMouse)
	EVT_MENU(MENU_I_INPUT,    CSMeter::onMenu)
	EVT_MENU(MENU_Q_INPUT,    CSMeter::onMenu)
	EVT_MENU(MENU_SIGNAL,     CSMeter::onMenu)
	EVT_MENU(MENU_AVG_SIGNAL, CSMeter::onMenu)
	EVT_MENU(MENU_AGC,        CSMeter::onMenu)
	EVT_MENU(MENU_MICROPHONE, CSMeter::onMenu)
	EVT_MENU(MENU_POWER,      CSMeter::onMenu)
	EVT_MENU(MENU_ALC,        CSMeter::onMenu)
	EVT_PAINT(CSMeter::onPaint)
END_EVENT_TABLE()

CSMeter::CSMeter(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
wxPanel(parent, id, pos, size, style, name),
m_width(size.GetWidth()),
m_height(size.GetHeight()),
m_background(NULL),
m_bitmap(NULL),
m_menu(NULL),
m_rxMenu(NULL),
m_txMenu(NULL),
m_rxMeter(-1),
m_txMeter(-1),
m_lastLevel(999.9F)
{
	m_bitmap     = new wxBitmap(m_width, m_height);
	m_background = new wxBitmap(m_width, m_height);

	m_rxMenu = new wxMenu();
	m_rxMenu->AppendRadioItem(MENU_I_INPUT,    _("I Input"));
	m_rxMenu->AppendRadioItem(MENU_Q_INPUT,    _("Q Input"));
	m_rxMenu->AppendRadioItem(MENU_SIGNAL,     _("Strength"));
	m_rxMenu->AppendRadioItem(MENU_AVG_SIGNAL, _("Avg Strength"));
	m_rxMenu->AppendRadioItem(MENU_AGC,        _("AGC"));

	m_txMenu = new wxMenu();
	m_txMenu->AppendRadioItem(MENU_MICROPHONE, _("Microphone"));
	m_txMenu->AppendRadioItem(MENU_POWER,      _("Power"));
	m_txMenu->AppendRadioItem(MENU_ALC,        _("ALC"));

	m_menu = new wxMenu();
	m_menu->Append(-1, _("Receive"),  m_rxMenu);
	m_menu->Append(-1, _("Transmit"), m_txMenu);

	createBackground();
	setLevel(0.0);
}

CSMeter::~CSMeter()
{
	delete m_bitmap;
	delete m_background;
	delete m_menu;
}

void CSMeter::setLevel(float level)
{
	if (level < 0.0F)
		level = 0.0F;

	if (level == m_lastLevel)
		return;

	wxMemoryDC dcBackground;
	dcBackground.SelectObject(*m_background);

	wxMemoryDC dc;
	dc.SelectObject(*m_bitmap);

	dc.Blit(0, 0, m_width, m_height, &dcBackground, 0, 0);
	dcBackground.SelectObject(wxNullBitmap);

	// Draw the pointer
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetPen(*wxWHITE_PEN);

	int centreX = SMETER_WIDTH / 2;
	int centreY = SMETER_WIDTH / 2 + 20;

	int endX;
	int endY;

	if (level <= 54.0F) {	// S0 to S9
		double angle = (M_PI / 180.0) * double(45.0F - level * 0.833333F);
		endX = centreX - int((SMETER_WIDTH + 15) * ::sin(angle) / 2.0 + 0.5);
		endY = centreY - int((SMETER_WIDTH + 15) * ::cos(angle) / 2.0 + 0.5);
	} else {			// dB over S9
		if (level > 94.0F)	// 40dB over S9
			level = 94.0F;
		double angle = (M_PI / 180.0) * double(level - 54.0F) * 1.125;
		endX = centreX + int((SMETER_WIDTH + 15) * ::sin(angle) / 2.0 + 0.5);
		endY = centreY - int((SMETER_WIDTH + 15) * ::cos(angle) / 2.0 + 0.5);
	}
	dc.DrawLine(centreX, centreY, endX, endY);

	dc.SelectObject(wxNullBitmap);

	wxClientDC clientDC(this);
	show(clientDC);

	m_lastLevel = level;
}

void CSMeter::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	show(dc);
}

void CSMeter::show(wxDC& dc)
{
	dc.DrawBitmap(*m_bitmap, 0, 0, false);
}

void CSMeter::createBackground()
{
	// Flood the graph area with black to start with
	wxMemoryDC dc;
	dc.SelectObject(*m_background);

	dc.SetBackground(*wxBLACK_BRUSH);
	dc.Clear();

	dc.SetBrush(*wxGREEN_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawEllipticArc(0, 20, SMETER_WIDTH, SMETER_WIDTH, 90.0, 135.0);

	dc.SetBrush(*wxRED_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawEllipticArc(0, 20, SMETER_WIDTH, SMETER_WIDTH, 45.0, 90.0);

	dc.SetPen(*wxWHITE_PEN);
	dc.SetTextForeground(*wxWHITE);

	int centreX = SMETER_WIDTH / 2;
	int centreY = SMETER_WIDTH / 2 + 20;

	// S0
	int endX = centreX - int((SMETER_WIDTH + 15) * 0.35355 + 0.5);
	int endY = centreY - int((SMETER_WIDTH + 15) * 0.35355 + 0.5);
	dc.DrawLine(centreX, centreY, endX, endY);
	dc.DrawText(wxT("0"), endX - 10, endY - 10);
	// S3
	endX = centreX - int((SMETER_WIDTH + 15) * 0.25 + 0.5);
	endY = centreY - int((SMETER_WIDTH + 15) * 0.43301 + 0.5);
	dc.DrawLine(centreX, centreY, endX, endY);
	dc.DrawText(wxT("3"), endX - 6, endY - 12);
	// S6
	endX = centreX - int((SMETER_WIDTH + 15) * 0.12941 + 0.5);
	endY = centreY - int((SMETER_WIDTH + 15) * 0.48296 + 0.5);
	dc.DrawLine(centreX, centreY, endX, endY);
	dc.DrawText(wxT("6"), endX - 5, endY - 14);
	// S9
	endX = centreX;
	endY = 15;
	dc.DrawLine(centreX, centreY, endX, endY);
	dc.DrawText(wxT("9"), endX - 5, endY - 15);
	// S9+20
	endX = centreX + int((SMETER_WIDTH + 15) * 0.19134 + 0.5);
	endY = centreY - int((SMETER_WIDTH + 15) * 0.46194 + 0.5);
	dc.DrawLine(centreX, centreY, endX, endY);
	dc.DrawText(wxT("+20"), endX - 5, endY - 12);
	// S9+40
	endX = centreX + int((SMETER_WIDTH + 15) * 0.35355 + 0.5);
	endY = centreY - int((SMETER_WIDTH + 15) * 0.35355 + 0.5);
	dc.DrawLine(centreX, centreY, endX, endY);
	dc.DrawText(wxT("+40"), endX - 10, endY - 10);

	dc.SetBrush(*wxBLACK_BRUSH);
	dc.SetPen(*wxBLACK_PEN);
	dc.DrawEllipticArc(5, 25, SMETER_WIDTH - 10, SMETER_WIDTH - 10, 40.0, 140.0);

	dc.SelectObject(wxNullBitmap);
}

void CSMeter::onMouse(wxMouseEvent& event)
{
	switch (m_rxMeter) {
		case METER_I_INPUT:
			m_rxMenu->Check(MENU_I_INPUT, true);
			break;
		case METER_Q_INPUT:
			m_rxMenu->Check(MENU_Q_INPUT, true);
			break;
		case METER_SIGNAL:
			m_rxMenu->Check(MENU_SIGNAL, true);
			break;
		case METER_AVG_SIGNAL:
			m_rxMenu->Check(MENU_AVG_SIGNAL, true);
			break;
		case METER_AGC:
			m_rxMenu->Check(MENU_AGC, true);
			break;
		default:
			::wxLogError(wxT("Unknown RX meter type = %d"), m_rxMeter);
			break;
	}

	switch (m_txMeter) {
		case METER_MICROPHONE:
			m_txMenu->Check(MENU_MICROPHONE, true);
			break;
		case METER_POWER:
			m_txMenu->Check(MENU_POWER, true);
			break;
		case METER_ALC:
			m_txMenu->Check(MENU_ALC, true);
			break;
		default:
			::wxLogError(wxT("Unknown TX meter type = %d"), m_txMeter);
			break;
	}

	int x = event.GetX();
	int y = event.GetY();

	PopupMenu(m_menu, x, y);
}

void CSMeter::onMenu(wxCommandEvent& event)
{
	switch (event.GetId()) {
		case MENU_I_INPUT:
			setRXMeter(METER_I_INPUT);
			break;
		case MENU_Q_INPUT:
			setRXMeter(METER_Q_INPUT);
			break;
		case MENU_SIGNAL:
			setRXMeter(METER_SIGNAL);
			break;
		case MENU_AVG_SIGNAL:
			setRXMeter(METER_AVG_SIGNAL);
			break;
		case MENU_AGC:
			setRXMeter(METER_AGC);
			break;
		case MENU_MICROPHONE:
			setTXMeter(METER_MICROPHONE);
			break;
		case MENU_POWER:
			setTXMeter(METER_POWER);
			break;
		case MENU_ALC:
			setTXMeter(METER_AGC);
			break;
		default:
			::wxLogError(wxT("Unknown meter type = %d"), event.GetId());
			break;
	}
}

void CSMeter::setRXMeter(int meter)
{
	m_rxMeter = meter;
}

void CSMeter::setTXMeter(int meter)
{
	m_txMeter = meter;
}

int CSMeter::getRXMeter() const
{
	return m_rxMeter;
}

int CSMeter::getTXMeter() const
{
	return m_txMeter;
}
