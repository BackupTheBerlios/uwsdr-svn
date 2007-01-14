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

#include "SpectrumDisplay.h"

#include "UWSDRDefs.h"

#include <wx/image.h>

enum {
	MENU_PANADAPTER1 = 16531,
	MENU_PANADAPTER2,
	MENU_WATERFALL,
	MENU_PRE_FILT,
	MENU_POST_FILT,
	MENU_POST_AGC,
	MENU_100MS,
	MENU_200MS,
	MENU_300MS,
	MENU_400MS,
	MENU_500MS,
	MENU_1000MS
};

BEGIN_EVENT_TABLE(CSpectrumDisplay, wxPanel)
	EVT_PAINT(CSpectrumDisplay::onPaint)
	EVT_LEFT_DOWN(CSpectrumDisplay::onLeftMouse)
	EVT_RIGHT_DOWN(CSpectrumDisplay::onRightMouse)
	EVT_MENU(MENU_PANADAPTER1, CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_PANADAPTER2, CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_WATERFALL,   CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_PRE_FILT,    CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_POST_FILT,   CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_POST_AGC,    CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_100MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_200MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_300MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_400MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_500MS,  CSpectrumDisplay::onMenu)
	EVT_MENU(MENU_1000MS, CSpectrumDisplay::onMenu)
END_EVENT_TABLE()


CSpectrumDisplay::CSpectrumDisplay(wxWindow* parent, int id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
wxPanel(parent, id, pos, size, style, name),
m_width(size.GetWidth()),
m_height(size.GetHeight()),
m_dbScale(0.0F),
m_background(NULL),
m_bitmap(NULL),
m_sampleRate(0.0F),
m_bandwidth(0.0F),
m_menu(NULL),
m_speedMenu(NULL),
m_posMenu(NULL),
m_typeMenu(NULL),
m_type(SPECTRUM_NONE),
m_speed(0),
m_position(0),
m_factor(1),
m_ticks(0),
m_pick(0.0F),
m_offset(0.0F)
{
	m_bitmap     = new wxBitmap(m_width, m_height);
	m_background = new wxBitmap(m_width, m_height);

	m_dbScale = float(m_height - 17) / 40.0F;

	m_speedMenu = new wxMenu();
	m_speedMenu->AppendRadioItem(MENU_100MS,  wxT("100 ms"));
	m_speedMenu->AppendRadioItem(MENU_200MS,  wxT("200 ms"));
	m_speedMenu->AppendRadioItem(MENU_300MS,  wxT("300 ms"));
	m_speedMenu->AppendRadioItem(MENU_400MS,  wxT("400 ms"));
	m_speedMenu->AppendRadioItem(MENU_500MS,  wxT("500 ms"));
	m_speedMenu->AppendRadioItem(MENU_1000MS, wxT("1 s"));

	m_posMenu = new wxMenu();
	m_posMenu->AppendRadioItem(MENU_PRE_FILT,  _("Pre Filter"));
	m_posMenu->AppendRadioItem(MENU_POST_FILT, _("Post Filter"));
	m_posMenu->AppendRadioItem(MENU_POST_AGC,  _("Post AGC"));

	m_typeMenu = new wxMenu();
	m_typeMenu->AppendRadioItem(MENU_PANADAPTER1, _("Panadapter 1"));
	m_typeMenu->AppendRadioItem(MENU_PANADAPTER2, _("Panadapter 2"));
	m_typeMenu->AppendRadioItem(MENU_WATERFALL,   _("Waterfall"));

	m_menu = new wxMenu();
	m_menu->Append(-1, _("Position"), m_posMenu);
	m_menu->Append(-1, _("Type"),     m_typeMenu);
	m_menu->Append(-1, _("Refresh"),  m_speedMenu);
}

CSpectrumDisplay::~CSpectrumDisplay()
{
	delete m_background;
	delete m_bitmap;
	delete m_menu;
}

void CSpectrumDisplay::setSampleRate(float sampleRate)
{
	m_sampleRate = sampleRate;
}

void CSpectrumDisplay::setBandwidth(float bandwidth)
{
	m_bandwidth = bandwidth;

	switch (m_type) {
		case SPECTRUM_PANADAPTER1:
		case SPECTRUM_PANADAPTER2:
			createPanadapter();
			break;
		case SPECTRUM_WATERFALL:
			createWaterfall();
			break;
		default:
			::wxLogError(wxT("Unknown spectrum type = %d"), m_type);
			break;
	}

	wxClientDC clientDC(this);
	show(clientDC);
}

void CSpectrumDisplay::showSpectrum(const float* spectrum, float bottom, float offset)
{
	wxASSERT(spectrum != NULL);

	m_offset = offset;

	if ((m_ticks % m_factor) == 0) {
		switch (m_type) {
			case SPECTRUM_PANADAPTER1:
				drawPanadapter1(spectrum, bottom);
				break;
			case SPECTRUM_PANADAPTER2:
				drawPanadapter2(spectrum, bottom);
				break;
			case SPECTRUM_WATERFALL:
				drawWaterfall(spectrum, bottom);
				break;
			default:
				break;
		}

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
	double incrX = double(right - left) / 10.0;

	dc.SetPen(*wxCYAN_PEN);

	dc.DrawLine(left, top, left, bottom);
	dc.DrawLine(right, top, right, bottom);
	dc.DrawLine(middleX, top, middleX, bottom);
	dc.DrawLine(left, bottom, right, bottom);
	dc.DrawLine(left, top, right, top);

	wxFont font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
	font.SetPointSize(11);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	dc.SetFont(font);

	dc.SetTextForeground(*wxCYAN);

	// Draw the frequency lines
	for (unsigned int i = 1; i < 10; i++) {
		int x = left + int(i * incrX + 0.5);
		dc.DrawLine(x, top /* lowY */, x, bottom);
	}

	// Draw the dB lines, every 10 dB
	for (unsigned int dB = 0; true; dB += 10) {
		int y = bottom - int(float(dB) * m_dbScale + 0.5);
		if (y < top)
			break;

		dc.DrawLine(left, y, right, y);

		wxString text;
		text.Printf(wxT("%u"), dB);

		wxCoord width, height;
		dc.GetTextExtent(text, &width, &height);

		dc.DrawText(text, left + 2, y - height);
		dc.DrawText(text, right - width - 2, y - height);
	}

	wxString text;
	text.Printf(wxT("-%.1f kHz"), m_bandwidth / 2000.0F);
	dc.DrawText(text, left, bottom);

	text = wxT("0");
	wxCoord height, width;
	dc.GetTextExtent(text, &width, &height);
	dc.DrawText(text, middleX - width / 2, bottom);

	text.Printf(wxT("+%.1f kHz"), m_bandwidth / 2000.0F);
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

	wxFont font = wxSystemSettings::GetFont(wxSYS_SYSTEM_FONT);
	font.SetPointSize(11);
	font.SetWeight(wxFONTWEIGHT_BOLD);
	dc.SetFont(font);

	dc.SetTextForeground(*wxCYAN);

	wxString text;
	text.Printf(wxT("-%.1f kHz"), m_bandwidth / 2000.0F);
	dc.DrawText(text, left, bottom);

	text = wxT("0");
	wxCoord height, width;
	dc.GetTextExtent(text, &width, &height);
	dc.DrawText(text, middleX - width / 2, bottom);

	text.Printf(wxT("+%.1f kHz"), m_bandwidth / 2000.0F);
	dc.GetTextExtent(text, &width, &height);
	dc.DrawText(text, right - width, bottom);

	dc.SelectObject(wxNullBitmap);
}

void CSpectrumDisplay::drawPanadapter1(const float* spectrum, float bottom)
{
	wxMemoryDC dc, dcBack;

	dc.SelectObject(*m_bitmap);
	dcBack.SelectObject(*m_background);

	dc.Blit(0, 0, m_width, m_height, &dcBack, 0, 0);

	dc.SetPen(*wxGREEN_PEN);

	int firstBin = int(float(SPECTRUM_SIZE) / 2.0F - m_bandwidth / 2.0F * float(SPECTRUM_SIZE) / m_sampleRate + 0.5);
	int lastBin  = int(float(SPECTRUM_SIZE) / 2.0F + m_bandwidth / 2.0F * float(SPECTRUM_SIZE) / m_sampleRate + 0.5);

	float binsPerPixel = float(lastBin - firstBin) / float(m_width - 5);

	int binOffset = int(float(lastBin - firstBin) * (m_offset / m_bandwidth) + 0.5F);
	firstBin += binOffset;
	lastBin  += binOffset;

	int lastX = 0, lastY = 0;
	for (int x = 2; x < (m_width - 3); x++) {
		int bin = firstBin + int(float(x - 2) * binsPerPixel + 0.5F);

		float value = -200.0F;
		for (int i = 0; i < int(binsPerPixel + 0.5F); i++) {
			float val = spectrum[bin++];

			if (val > value)
				value = val;
		}

		int y = int((value - bottom) * m_dbScale + 0.5F);
		if (y < 0)
			y = 0;
		if (y > (m_height - 18))
			y = m_height - 18;

		y = m_height - 15 - y;

		if (x > 2)
			dc.DrawLine(lastX, lastY, x, y);

		lastX = x;
		lastY = y;
	}

	dcBack.SelectObject(wxNullBitmap);
	dc.SelectObject(wxNullBitmap);
}

void CSpectrumDisplay::drawPanadapter2(const float* spectrum, float bottom)
{
	wxMemoryDC dc, dcBack;

	dc.SelectObject(*m_bitmap);
	dcBack.SelectObject(*m_background);

	dc.Blit(0, 0, m_width, m_height, &dcBack, 0, 0);

	dc.SetPen(*wxGREEN_PEN);

	int firstBin = int(float(SPECTRUM_SIZE) / 2.0F - m_bandwidth / 2.0F * float(SPECTRUM_SIZE) / m_sampleRate + 0.5);
	int lastBin  = int(float(SPECTRUM_SIZE) / 2.0F + m_bandwidth / 2.0F * float(SPECTRUM_SIZE) / m_sampleRate + 0.5);

	float binsPerPixel = float(lastBin - firstBin) / float(m_width - 5);

	int binOffset = int(float(lastBin - firstBin) * (m_offset / m_bandwidth) + 0.5F);
	firstBin += binOffset;
	lastBin  += binOffset;

	for (int x = 2; x < (m_width - 3); x++) {
		int bin = firstBin + int(float(x - 2) * binsPerPixel + 0.5F);

		float value = -200.0F;
		for (int i = 0; i < int(binsPerPixel + 0.5F); i++) {
			float val = spectrum[bin++];

			if (val > value)
				value = val;
		}

		int y = int((value - bottom) * m_dbScale + 0.5F);
		if (y < 0)
			y = 0;
		if (y > (m_height - 18))
			y = m_height - 18;

		y = m_height - 15 - y;

		dc.DrawLine(x, m_height - 15, x, y);
	}

	dcBack.SelectObject(wxNullBitmap);
	dc.SelectObject(wxNullBitmap);
}

/*
 * Could we gain speed by storing the wxImage instead of the wxBitmap?
 */
void CSpectrumDisplay::drawWaterfall(const float* spectrum, float bottom)
{
	int firstBin = int(float(SPECTRUM_SIZE) / 2.0F - m_bandwidth / 2.0F * float(SPECTRUM_SIZE) / m_sampleRate + 0.5);
	int lastBin  = int(float(SPECTRUM_SIZE) / 2.0F + m_bandwidth / 2.0F * float(SPECTRUM_SIZE) / m_sampleRate + 0.5);

	float binsPerPixel = float(lastBin - firstBin) / float(m_width - 5);

	int binOffset = int(float(lastBin - firstBin) * (m_offset / m_bandwidth) + 0.5F);
	firstBin += binOffset;
	lastBin  += binOffset;

	// Do all of the work on a wxImage
	wxImage image = m_bitmap->ConvertToImage();

	unsigned char* data = image.GetData();

	// Scroll the image up by one pixel
	::memcpy(data, data + m_width * 3, (m_height - 16) * m_width * 3);

	unsigned char* imgOffset = data + m_width * (m_height - 16) * 3;

	for (int x = 2; x < (m_width - 3); x++) {
		int bin = firstBin + int(float(x - 2) * binsPerPixel + 0.5);

		float value = -200.0F;
		for (int i = 0; i < int(binsPerPixel + 0.5); i++) {
			float val = spectrum[bin++];

			if (val > value)
				value = val;
		}

		float percent = (value - bottom) / 40.0F;
		if (percent < 0.0F)
			percent = 0.0F;
		if (percent > 1.0F)
			percent = 1.0F;

		unsigned char r;
		unsigned char g;
		unsigned char b;

		if (percent <= 0.33333333F) {			// use a gradient between low and mid colors
			percent *= 3.0F;

			r = (unsigned char)(percent * 255.0F + 0.5F);
			g = 0;
			b = 0;
		} else if (percent <= 0.66666666F) {		// use a gradient between mid and high colors
			percent = (percent - 0.33333333F) * 3.0F;

			r = 255;
			g = (unsigned char)(percent * 255.0F + 0.5F);
			b = 0;
		} else {
			percent = (percent - 0.66666666F) * 3.0F;

			r = 255;
			g = 255;
			b = (unsigned char)(percent * 255.0F + 0.5F);
		}

		*imgOffset++ = r;
		*imgOffset++ = g;
		*imgOffset++ = b;
	}

	delete m_bitmap;
	m_bitmap = new wxBitmap(image);
}

void CSpectrumDisplay::onLeftMouse(wxMouseEvent& event)
{
   float x     = float(event.GetX() - 2);
   float width = float(m_width - 5);

   m_pick = m_bandwidth * x / width - m_bandwidth / 2.0F;

   if (m_pick < -m_bandwidth / 2.0F)
      m_pick = 0.0F;
   else if (m_pick > m_bandwidth / 2.0F)
      m_pick = 0.0F;
}

void CSpectrumDisplay::onRightMouse(wxMouseEvent& event)
{
	switch (m_type) {
		case SPECTRUM_PANADAPTER1:
			m_typeMenu->Check(MENU_PANADAPTER1, true);
			break;
		case SPECTRUM_PANADAPTER2:
			m_typeMenu->Check(MENU_PANADAPTER2, true);
			break;
		case SPECTRUM_WATERFALL:
			m_typeMenu->Check(MENU_WATERFALL, true);
			break;
		default:
			::wxLogError(wxT("Unknown spectrum type = %d"), m_type);
			break;
	}

	switch (m_position) {
		case SPECTRUM_PRE_FILT:
			m_posMenu->Check(MENU_PRE_FILT, true);
			break;
		case SPECTRUM_POST_FILT:
			m_posMenu->Check(MENU_POST_FILT, true);
			break;
		case SPECTRUM_POST_AGC:
			m_posMenu->Check(MENU_POST_AGC, true);
			break;
		default:
			::wxLogError(wxT("Unknown spectrum position = %d"), m_type);
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
			::wxLogError(wxT("Unknown spectrum speed = %d"), m_speed);
			break;
	}

	int x = event.GetX();
	int y = event.GetY();

	PopupMenu(m_menu, x, y);
}

void CSpectrumDisplay::onMenu(wxCommandEvent& event)
{
	switch (event.GetId()) {
		case MENU_PANADAPTER1:
			setType(SPECTRUM_PANADAPTER1);
			break;
		case MENU_PANADAPTER2:
			setType(SPECTRUM_PANADAPTER2);
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
		case MENU_POST_AGC:
			setPosition(SPECTRUM_POST_AGC);
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
			::wxLogError(wxT("Unknown spectrum menu item = %d"), event.GetId());
			break;
	}
}

void CSpectrumDisplay::setType(int type)
{
	if (type == m_type)
		return;

	switch (type) {
		case SPECTRUM_PANADAPTER1:
		case SPECTRUM_PANADAPTER2:
			createPanadapter();
			break;
		case SPECTRUM_WATERFALL:
			createWaterfall();
			break;
		default:
			::wxLogError(wxT("Unknown spectrum type = %d"), type);
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
			::wxLogError(wxT("Unknown spectrum speed = %d"), speed);
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

float CSpectrumDisplay::getFreqPick()
{
   float pick = m_pick;

   m_pick = 0.0F;

   return pick;
}
