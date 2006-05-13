/* Oscillator.cpp

This routine implements a common fixed-frequency oscillator

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY

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

#include "Oscillator.h"
#include "fromsys.h"

#include <wx/wx.h>


const double HUGE_PHASE = 1256637061.43593;

COscillator::COscillator(double frequency, double phase, REAL sampleRate) :
m_frequency(0.0),
m_sampleRate(sampleRate),
m_phase(phase),
m_delta(0.0)
{
	wxASSERT(sampleRate > 0.0F);

	setFrequency(frequency);
}

COscillator::~COscillator()
{
}

void COscillator::setPhase(double phase)
{
	m_phase = phase;
}

double COscillator::getPhase() const
{
	return m_phase;
}

void COscillator::setFrequency(double frequency)
{
	m_frequency = frequency;
	m_delta     = TWOPI * frequency / m_sampleRate;
}

double COscillator::getFrequency() const
{
	return m_frequency;
}

CXB* COscillator::oscillate(CXB* buf)
{
	wxASSERT(buf != NULL);

	if (m_phase > HUGE_PHASE)
		m_phase -= HUGE_PHASE;

	COMPLEX z     = Cmplx((REAL)::cos(m_phase), (IMAG)::sin(m_phase));
	COMPLEX delta = Cmplx((REAL)::cos(m_delta), (IMAG)::sin(m_delta));

	for (unsigned int i = 0; i < CXBsize(buf); i++) {
		CXBdata(buf, i) = z = Cmul(z, delta);

		m_phase += m_delta;
	}

	return buf;
}

CXB* COscillator::mix(CXB* buf)
{
	wxASSERT(buf != NULL);

	unsigned int len = CXBhave(buf);

	if (m_frequency == 0.0 || len == 0)
		return buf;

	if (m_phase > HUGE_PHASE)
		m_phase -= HUGE_PHASE;

	COMPLEX z     = Cmplx((REAL)::cos(m_phase), (IMAG)::sin(m_phase));
	COMPLEX delta = Cmplx((REAL)::cos(m_delta), (IMAG)::sin(m_delta));

	for (unsigned int i = 0; i < len; i++) {
		z = Cmul(z, delta);

		CXBdata(buf, i) = Cmul(CXBdata(buf, i), z);

		m_phase += m_delta;
	}

	return buf;
}

