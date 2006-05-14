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

#include "SpectrumDisplay.h"

#include "UWSDRDefs.h"

#include <wx/image.h>

const int MENU_PANADAPTER = 16531;
const int MENU_WATERFALL  = 16532;
const int MENU_PRE_FILT   = 16533;
const int MENU_POST_FILT  = 16534;
const int MENU_100MS      = 16535;
const int MENU_200MS      = 16536;
const int MENU_300MS      = 16537;
const int MENU_400MS      = 16538;
const int MENU_500MS      = 16539;
const int MENU_1000MS     = 16540;

BEGIN_EVENT_TABLE(CSpectrumDisplay, wxPanel)
	EVT_PAINT(CSpectrumDisplay::onPaint)
	EVT_RIGHT_DOWN(CSpectrumDisplay::onMouse)
	EVT_MENU(MENU_PANADAPTER, CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_WATERFALL,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_PRE_FILT,   CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_POST_FILT,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_100MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_200MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_300MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_400MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_500MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_1000MS, CSpectrumDisplay::onMenu)
END_EVENT_TABLE()

const double FREQ = 5000.0;

const double DB_SCALE = 0.5;


CSpectrumDisplay::CSpectrumDisplay(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
wxPanel(parent, id, pos, size, style, name),
m_width(size.GetWidth()),
m_height(size.GetHeight()),
m_background(NULL),
m_bitmap(NULL),
m_sampleRate(0),
m_menu(NULL),
m_speedMenu(NULL),
m_posMenu(NULL),
m_typeMenu(NULL),
m_type(0),
m_speed(0),
m_position(0),
m_factor(1),
m_ticks(0)
{
	m_bitmap     = new wxBitmap(m_width, m_height);
	m_background = new wxBitmap(m_width, m_height);

	m_speedMenu = new wxMenu();
	m_speedMenu->AppendRadioItem(MENU_100MS,  wxT("100 ms"));
	m_speedMenu->AppendRadioItem(MENU_200MS,  wxT("200 ms"));
	m_speedMenu->AppendRadioItem(MENU_300MS,  wxT("300 ms"));
	m_speedMenu->AppendRadioItem(MENU_400MS,  wxT("400 ms"));
	m_speedMenu->AppendRadioItem(MENU_500MS,  wxT("500 ms"));
	m_speedMenu->AppendRadioItem(MENU_1000MS, wxT("1 s"));

	m_posMenu = new wxMenu();
	m_posMenu->AppendRadioItem(MENU_PRE_FILT,  _("Pre-Filter"));
	m_posMenu->AppendRadioItem(MENU_POST_FILT, _("Post-Filter"));

	m_typeMenu = new wxMenu();
	m_typeMenu->AppendRadioItem(MENU_PANADAPTER, _("Panadapter"));
	m_typeMenu->AppendRadioItem(MENU_WATERFALL,  _("Waterfall"));

	m_menu = new wxMenu();
	m_menu->Append(-1, _("Position"), m_posMenu);
	m_menu->Append(-1, _("Type"),     m_typeMenu);
	m_menu->Append(-1, _("Refresh"),  m_speedMenu);

	createPanadapter();

	wxClientDC clientDC(this);
	show(clientDC);
}

CSpectrumDisplay::~CSpectrumDisplay()
{
	delete m_background;
	delete m_bitmap;
	delete m_menu;
}

void CSpectrumDisplay::setSampleRate(unsigned int sampleRate)
{
	m_sampleRate = sampleRate;
}

void CSpectrumDisplay::showSpectrum(const float* spectrum, double scale)
{
	wxASSERT(spectrum != NULL);
	wxASSERT(scale > 0.0 && scale <= 1.0);

	if ((m_ticks % m_factor) == 0) {
		if (m_type == SPECTRUM_PANADAPTER)
			drawPanadapter(spectrum, scale);
		else if (m_type == SPECTRUM_WATERFALL)
			drawWaterfall(spectrum, scale);

		wxClientDC clientDC(this);
		show(clientDC);
	}

	m_ticks++;
}

void CSpectrumDisplay::onPaint(wxPaintEvent& event)
{
	wxPaintDC dc(this);

	show(dc);
}

void CSpectrumDisplay::show(wxDC& dc)
{
	dc.DrawBitmap(*m_bitmap, 0, 0, false);
}

void CSpectrumDisplay::createPanadapter()
{
	// Flood the graph area with black to start with
	wxMemoryDC dc;
	dc.SelectObject(*m_background);

	dc.SetBackground(*wxBLACK_BRUSH);
	dc.Clear();

	int left    = 2;
	int right   = m_width - 3;
	int top     = 3;
	int bottom  = m_height - 15;
	int middleX = left + (right - left) / 2;
	int lowY    = top + 13 * (bottom - top) / 15;
	double incrX = double(right - left) / 10.0;

	dc.SetPen(*wxCYAN_PEN);

	dc.DrawLine(left, top, left, bottom);
	dc.DrawLine(right, top, right, bottom);
	dc.DrawLine(middleX, top, middleX, bottom);
	dc.DrawLine(left, bottom, right, bottom);
	dc.DrawLine(left, top, right, top);

	for (int i = 1; i < 10; i++) {
		int x = left + int(i * incrX + 0.5);
		dc.DrawLine(x, top /* lowY */, x, bottom);
	}

	for (int j = 1; true; j += 10) {
		int y = bottom - int(double(j) / DB_SCALE + 0.5);
		if (y < top)
			break;

		dc.DrawLine(left, y, right, y);
	}

	dc.SetTextForeground(*wxCYAN);

	wxString text;
	text.Printf(wxT("-%.1f kHz"), FREQ / 1000.0);
	dc.DrawText(text, left, bottom);

	text = wxT("0");
	wxCoord height, width;
	dc.GetTextExtent(text, &width, &height);
	dc.DrawText(text, middleX - width / 2, bottom);

	text.Printf(wxT("+%.1f kHz"), FREQ / 1000.0);
	dc.GetTextExtent(text, &width, &height);
	dc.DrawText(text, right - width, bottom);

	wxMemoryDC dcScreen;
	dcScreen.SelectObject(*m_bitmap);

	dcScreen.Blit(0, 0, m_width, m_height, &dc, 0, 0);

	dcScreen.SelectObject(wxNullBitmap);
	dc.SelectObject(wxNullBitmap);
}

void CSpectrumDisplay::createWaterfall()
{
	// Flood the graph area with black to start with
	wxMemoryDC dc;
	dc.SelectObject(*m_bitmap);

	dc.SetBackground(*wxBLACK_BRUSH);
	dc.Clear();

	int left    = 2;
	int right   = m_width - 3;
	int bottom  = m_height - 15;
	int middleX = left + (right - left) / 2;

	dc.SetPen(*wxCYAN_PEN);

	dc.DrawLine(left, bottom, right, bottom);

	dc.SetTextForeground(*wxCYAN);

	wxString text;
	text.Printf(wxT("-%.1f kHz"), FREQ / 1000.0);
	dc.DrawText(text, left, bottom);

	text = wxT("0");
	wxCoord height, width;
	dc.GetTextExtent(text, &width, &height);
	dc.DrawText(text, middleX - width / 2, bottom);

	text.Printf(wxT("+%.1f kHz"), FREQ / 1000.0);
	dc.GetTextExtent(text, &width, &height);
	dc.DrawText(text, right - width, bottom);

	dc.SelectObject(wxNullBitmap);
}

void CSpectrumDisplay::drawPanadapter(const float* spectrum, double scale)
{
	wxMemoryDC dc, dcBack;

	dc.SelectObject(*m_bitmap);
	dcBack.SelectObject(*m_background);

	dc.Blit(0, 0, m_width, m_height, &dcBack, 0, 0);

	dc.SetPen(*wxGREEN_PEN);

	int firstBin = SPECTRUM_SIZE / 2 - int(FREQ * double(SPECTRUM_SIZE) / double(m_sampleRate) + 0.5);
	int lastBin  = SPECTRUM_SIZE / 2 + int(FREQ * double(SPECTRUM_SIZE) / double(m_sampleRate) + 0.5);

	double aveValue = 0.0;
	for (int i = firstBin; i < lastBin; i++)
		aveValue += spectrum[i] * scale;
	aveValue /= double(lastBin - firstBin);

	int binsPerPixel = int(double(lastBin - firstBin) / double(m_width - 5) + 0.5);

	int lastX, lastY;
	int bin = firstBin;
	for (int x = 2; x < (m_width - 3); x++) {
/*
		double value = -999.0;
		for (int i = 0; i < binsPerPixel; i++) {
			double val = spectrum[bin++] * scale;

			if (val > value)
				value = val;
		}
*/
		double value = 0.0;
		for (int i = 0; i < binsPerPixel; i++)
			value += spectrum[bin++] * scale;
		value /= double(binsPerPixel);

		int y = int((value - aveValue) / DB_SCALE + 0.5);
		if (y < 0)
			y = 0;
		if (y > (m_height - 12))
			y = m_height - 12;

		y = m_height - 15 - y;

		if (x > 2)
			dc.DrawLine(lastX, lastY, x, y);

		lastX = x;
		lastY = y;
	}

	dcBack.SelectObject(wxNullBitmap);
	dc.SelectObject(wxNullBitmap);
}

void CSpectrumDisplay::drawWaterfall(const float* spectrum, double scale)
{
	int firstBin = SPECTRUM_SIZE / 2 - int(FREQ * double(SPECTRUM_SIZE) / double(m_sampleRate) + 0.5);
	int lastBin  = SPECTRUM_SIZE / 2 + int(FREQ * double(SPECTRUM_SIZE) / double(m_sampleRate) + 0.5);

	double aveValue = 0.0;
	for (int i = firstBin; i < lastBin; i++)
		aveValue += spectrum[i] * scale;
	aveValue /= double(lastBin - firstBin);

	int binsPerPixel = int(double(lastBin - firstBin) / double(m_width - 5) + 0.5);

	// Do all of the work on a wxImage
	wxImage image = m_bitmap->ConvertToImage();

	unsigned char* data = image.GetData();

	// Scroll the image up by one pixel
	::memcpy(data, data + m_width * 3, (m_height - 16) * m_width * 3);

	unsigned char* offset = data + m_width * (m_height - 16) * 3;

	int bin = firstBin;
	for (int x = 2; x < (m_width - 3); x++) {
		double value = 0.0;
		for (int i = 0; i < binsPerPixel; i++)
			value += spectrum[bin++] * scale;
		value /= double(binsPerPixel);

		double percent = (value - aveValue) / 40.0;
		if (percent < 0.0)
			percent = 0.0;
		if (percent > 1.0)
			percent = 1.0;

		unsigned char r;
		unsigned char g;
		unsigned char b;

		if (percent <= 0.33333333) {			// use a gradient between low and mid colors
			percent *= 3.0;

			r = (unsigned char)(percent * 255.0 + 0.5);
			g = 0;
			b = 0;
		} else if (percent <= 0.66666666) {		// use a gradient between mid and high colors
			percent = (percent - 0.33333333) * 3.0;

			r = 255;
			g = (unsigned char)(percent * 255.0 + 0.5);
			b = 0;
		} else {
			percent = (percent - 0.66666666) * 3.0;

			r = 255;
			g = 255;
			b = (unsigned char)(percent * 255.0 + 0.5);
		}

		*offset++ = r;
		*offset++ = g;
		*offset++ = b;
	}

	delete m_bitmap;
	m_bitmap = new wxBitmap(image);
}

void CSpectrumDisplay::onMouse(wxMouseEvent& event)
{
	wxASSERT(m_menu != NULL);
	wxASSERT(m_speedMenu != NULL);

	switch (m_type) {
		case SPECTRUM_PANADAPTER:
			m_typeMenu->Check(MENU_PANADAPTER, true);
			break;
		case SPECTRUM_WATERFALL:
			m_typeMenu->Check(MENU_WATERFALL, true);
			break;
		default:
			::wxLogError(_("Unknown spectrum type = %d"), m_type);
			break;
	}

	switch (m_position) {
		case SPECTRUM_PANADAPTER:
			m_posMenu->Check(MENU_PRE_FILT, true);
			break;
		case SPECTRUM_WATERFALL:
			m_posMenu->Check(MENU_POST_FILT, true);
			break;
		default:
			::wxLogError(_("Unknown spectrum position = %d"), m_type);
			break;
	}

	switch (m_speed) {
		case SPECTRUM_100MS:
			m_speedMenu->Check(MENU_100MS, true);
			break;
		case SPECTRUM_200MS:
			m_speedMenu->Check(MENU_200MS, true);
			break;
		case SPECTRUM_300MS:
			m_speedMenu->Check(MENU_300MS, true);
			break;
		case SPECTRUM_400MS:
			m_speedMenu->Check(MENU_400MS, true);
			break;
		case SPECTRUM_500MS:
			m_speedMenu->Check(MENU_500MS, true);
			break;
		case SPECTRUM_1000MS:
			m_speedMenu->Check(MENU_1000MS, true);
			break;
		default:
			::wxLogError(_("Unknown spectrum speed = %d"), m_speed);
			break;
	}

	int x = event.GetX();
	int y = event.GetY();

	PopupMenu(m_menu, x, y);
}

void CSpectrumDisplay::onMenu(wxCommandEvent& event)
{
	switch (event.GetId()) {
		case MENU_PANADAPTER:
			setType(SPECTRUM_PANADAPTER);
			break;
		case MENU_WATERFALL:
			setType(SPECTRUM_WATERFALL);
			break;
		case MENU_PRE_FILT:
			setPosition(SPECTRUM_PRE_FILT);
			break;
		case MENU_POST_FILT:
			setPosition(SPECTRUM_POST_FILT);
			break;
		case MENU_100MS:
			setSpeed(SPECTRUM_100MS);
			break;
		case MENU_200MS:
			setSpeed(SPECTRUM_200MS);
			break;
		case MENU_300MS:
			setSpeed(SPECTRUM_300MS);
			break;
		case MENU_400MS:
			setSpeed(SPECTRUM_400MS);
			break;
		case MENU_500MS:
			setSpeed(SPECTRUM_500MS);
			break;
		case MENU_1000MS:
			setSpeed(SPECTRUM_1000MS);
			break;
		default:
			::wxLogError(_("Unknown spectrum menu item = %d"), event.GetId());
			break;
	}
}

void CSpectrumDisplay::setType(int type)
{
	if (type == m_type)
		return;

	switch (type) {
		case SPECTRUM_PANADAPTER:
			createPanadapter();
			break;
		case SPECTRUM_WATERFALL:
			createWaterfall();
			break;
		default:
			::wxLogError(_("Unknown spectrum type = %d"), type);
			break;
	}

	m_type = type;

	wxClientDC clientDC(this);
	show(clientDC);
}

void CSpectrumDisplay::setSpeed(int speed)
{
	if (speed == m_speed)
		return;

	switch (speed) {
		case SPECTRUM_100MS:
			m_factor = 1;
			break;
		case SPECTRUM_200MS:
			m_factor = 2;
			break;
		case SPECTRUM_300MS:
			m_factor = 3;
			break;
		case SPECTRUM_400MS:
			m_factor = 4;
			break;
		case SPECTRUM_500MS:
			m_factor = 5;
			break;
		case SPECTRUM_1000MS:
			m_factor = 10;
			break;
		default:
			::wxLogError(_("Unknown spectrum speed = %d"), speed);
			break;
	}

	m_speed = speed;
}

void CSpectrumDisplay::setPosition(int position)
{
	m_position = position;
}

int CSpectrumDisplay::getType() const
{
	return m_type;
}

int CSpectrumDisplay::getSpeed() const
{
	return m_speed;
}

int CSpectrumDisplay::getPosition() const
{
	return m_position;
}
