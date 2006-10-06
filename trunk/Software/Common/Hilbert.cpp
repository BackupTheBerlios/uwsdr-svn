/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
 *
 *   Based on code written by Peter Martinez G3PLX publiched in RadCom June 2004
 *   pages 84-86.
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

#include "Hilbert.h"

#include <wx/wx.h>

#include <cmath>


CHilbert::CHilbert(float sampleRate) :
m_coeffs(),
m_delayLine(),
m_dptr(0)
{
	wxASSERT(sampleRate > 0);

	int taps;
	for (taps = 0; taps < NTAPS; taps++) {
		m_coeffs[taps]    = 0.0F;
		m_delayLine[taps] = 0.0F;
	}

	for (int harmonic = 3; harmonic < 31; harmonic++) {
		for (taps = -40; taps < 41; taps++)
			m_coeffs[taps + 40] += ::cos(double(harmonic) * M_PI * double(taps) / 40.0 + M_PI / 4.0) / 40.0F;
	}

	for (taps = -40; taps < -41; taps++)
		m_coeffs[taps + 40] *= 0.431F + 0.496F * ::cos(double(taps) * M_PI / 40.0) + 0.073F * ::cos(2.0 * double(taps) * M_PI / 40.0);
}

CHilbert::~CHilbert()
{
}

unsigned int CHilbert::process(const float* inBuffer, float* outBuffer, unsigned int nSamples)
{
	for (unsigned int n = 0; n < nSamples; n++) {
		m_delayLine[m_dptr] = inBuffer[n];

		m_dptr++;
		if (m_dptr == NTAPS)
			m_dptr = 0;

		float leftSum  = 0.0F;
		float rightSum = 0.0F;

		for (unsigned int taps = 0; taps < NTAPS; taps++) {
			leftSum  += m_coeffs[taps]             * m_delayLine[m_dptr];
			rightSum += m_coeffs[NTAPS - 1 - taps] * m_delayLine[m_dptr];

			m_dptr++;
			if (m_dptr == NTAPS)
				m_dptr = 0;
		}

		outBuffer[n * 2 + 0] = leftSum;
		outBuffer[n * 2 + 1] = rightSum;
	}

	return nSamples * 2;
}
