/* FMMod.cpp

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

#include "FMMod.h"

#include <wx/wx.h>


CFMMod::CFMMod(REAL sampleRate, REAL deviation, CXB* input, CXB* output) :
m_sampleRate(sampleRate),
m_deviation(0.0F),
m_input(input),
m_output(output),
m_phase(0.0F)
{
	wxASSERT(deviation >= 0.0F);
	wxASSERT(input != NULL);
	wxASSERT(output != NULL);

	m_deviation = REAL(deviation * M_PI / m_sampleRate);
}

CFMMod::~CFMMod()
{
}

void CFMMod::setDeviation(REAL value)
{
	wxASSERT(value >= 0.0F);

	m_deviation = REAL(value * M_PI / m_sampleRate);
}

// FIXME m_phase should go to the main oscillator
void CFMMod::modulate()
{
	unsigned int n = CXBhave(m_input);

	for (unsigned int i = 0; i < n; i++) {
		m_phase += CXBreal(m_input, i) * m_deviation;

		CXBdata(m_output, i) = Cmplx((REAL)::cos(m_phase), (IMAG)::sin(m_phase));
	}

	CXBhave(m_output) = n;
}
