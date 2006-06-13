/* Squelch.cpp

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004,2005,2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

The authors can be reached by email at

ab2kt@arrl.net
or
rwmcgwier@comcast.net

or by paper mail at

The DTTS Microwave Society
6 Kathleen Place
Bridgewater, NJ 08807
*/

#include "Squelch.h"

#include <wx/wx.h>


CSquelch::CSquelch(REAL threshold, REAL offset) :
m_thresh(threshold),
m_offset(offset),
m_power(0.0F),
m_set(false),
m_running(false),
m_num(0)
{
}

CSquelch::~CSquelch()
{
}

bool CSquelch::isSquelch(CXB* buf)
{
	wxASSERT(buf != NULL);

	unsigned int n = CXBhave(buf);

	m_power = 0.0F;
	for (unsigned int i = 0; i < n; i++)
		m_power += Csqrmag(CXBdata(buf, i));

	return (m_offset + 10.0F * (REAL)::log10(m_power + 1e-17)) < m_thresh;
}

void CSquelch::doSquelch(CXB* buf)
{
	wxASSERT(buf != NULL);

	m_set = true;

	if (!m_running) {
		unsigned int m = m_num;
		unsigned int n = CXBhave(buf) - m;

		for (unsigned int i = 0; i < m; i++)
			CXBdata(buf, i) = Cscl(CXBdata(buf, i), 1.0F - REAL(i) / REAL(m));

		::memset((CXBbase(buf) + m), 0, n * sizeof(COMPLEX));
		m_running = true;
	} else {
		::memset(CXBbase(buf), 0x00, CXBhave(buf) * sizeof(COMPLEX));
	}
}

void CSquelch::noSquelch(CXB* buf)
{
	wxASSERT(buf != NULL);

	if (m_running) {
		unsigned int m = m_num;

		for (unsigned int i = 0; i < m; i++)
			CXBdata(buf, i) = Cscl(CXBdata(buf, i), REAL(i) / REAL(m));

		m_running = false;
	}
}
