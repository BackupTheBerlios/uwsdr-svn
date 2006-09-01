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

#include "Frequency.h"

CFrequency::CFrequency(unsigned int mhz, double hz) :
m_mhz(mhz),
m_hz(hz)
{
}

CFrequency::CFrequency(const CFrequency& frequency) :
m_mhz(frequency.m_mhz),
m_hz(frequency.m_hz)
{
}

CFrequency::CFrequency(const wxString& freq) :
m_mhz(0),
m_hz(0.0)
{
	setFrequency(freq);
}

CFrequency::CFrequency() :
m_mhz(0),
m_hz(0.0)
{
}

CFrequency::~CFrequency()
{
}

void CFrequency::addMHz(int mhz)
{
	m_mhz += mhz;
}

void CFrequency::addHz(double hz)
{
	if (hz < 0.0) {
		while (-hz > m_hz) {
			m_mhz -= 1;
			m_hz  += 1000000.0;
		}

		m_hz += hz;
	} else {
		m_hz += hz;

		while (m_hz >= 1000000.0) {
			m_mhz += 1;
			m_hz  -= 1000000.0;
		}
	}
}

void CFrequency::setMHz(unsigned int mhz)
{
	m_mhz = mhz;
}

void CFrequency::setHz(double hz)
{
	m_hz = hz;
}

bool CFrequency::setFrequency(const wxString& freq)
{
	if (freq.Freq(wxT('.')) > 1)
		return false;

	int n = freq.Length();

	bool error = false;
	for (int i = 0; i < n; i++) {
		wxChar c = freq.GetChar(i);

		switch (c) {
			case wxT('0'): case wxT('1'): case wxT('2'): case wxT('3'):
			case wxT('4'): case wxT('5'): case wxT('6'): case wxT('7'):
			case wxT('8'): case wxT('9'): case wxT('.'):
				break;
			default:
				error = true;
				break;
		}
	}

	if (error)
		return false;

	int pos = freq.Find(wxT('.'));

	if (pos == -1) {
		unsigned long temp;
		freq.ToULong(&temp);
		m_mhz = temp;

		m_hz  = 0.0;
	} else {
		wxString hertz = freq.Mid(pos + 1);

		hertz.Append(wxT("00000000"));
		hertz.Truncate(8);				// Maybe wrong
		hertz.insert(6, wxT("."));

		unsigned long temp;
		freq.Left(pos).ToULong(&temp);
		m_mhz = temp;

		hertz.ToDouble(&m_hz);
	}

	return true;
}


wxString CFrequency::getString(unsigned int decimals) const
{
	wxASSERT(decimals < 9);

	wxString hertz;
	hertz.Printf(wxT("%08d"), int(m_hz * 100.0F + 0.5F));

	hertz.Truncate(decimals);

	wxString text;
	text.Printf(wxT("%u.%s"), m_mhz, hertz.c_str());

	return text;
}

unsigned int CFrequency::getMHz() const
{
	return m_mhz;
}

double CFrequency::getHz() const
{
	return m_hz;
}
