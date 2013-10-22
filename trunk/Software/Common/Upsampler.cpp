/*
 *   Copyright (C) 2013 by Jonathan Naylor G4KLX
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

#include "Upsampler.h"

const float FIR_COEFFS[] = {
	0.00F, 0.00F
};

const unsigned int FIR_COEFFS_LEN = 50U;

CUpsampler::CUpsampler(unsigned int n) :
m_n(n - 1U),
m_buffer(NULL),
m_pos(0U)
{
	wxASSERT(n > 2U);

	m_buffer = new float[FIR_COEFFS_LEN];
	::memset(m_buffer, 0x00U, FIR_COEFFS_LEN * sizeof(float));
}

CUpsampler::~CUpsampler()
{
	delete[] m_buffer;
}

unsigned int CUpsampler::process(const float *in, unsigned int len, float *out)
{
	wxASSERT(in != NULL);
	wxASSERT(out != NULL);

	unsigned int m = 0U;

	for (unsigned int i = 0U; i < len; i++) {
		m_buffer[m_pos++] = in[i];
		m_pos %= FIR_COEFFS_LEN;

		out[m++] = calculate();

		for (unsigned int j = 0U; j < m_n; j++) {
			m_buffer[m_pos++] = 0.0F;
			m_pos %= FIR_COEFFS_LEN;

			out[m++] = calculate();
		}
	}

	return m;
}

float CUpsampler::calculate() const
{
	float acc = 0.0F;

	unsigned int pos = m_pos;

	for (unsigned int i = 0U; i < FIR_COEFFS_LEN; i++) {
		acc += m_buffer[pos++] * FIR_COEFFS[i];
		pos %= FIR_COEFFS_LEN;
	}

	return acc;
}
