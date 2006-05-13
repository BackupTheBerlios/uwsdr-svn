/* SpotTone.cpp

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

#include "SpotTone.h"
#include "fromsys.h"


//------------------------------------------------------------------------
// An ASR envelope on a complex phasor,
// with asynchronous trigger for R stage.
// A/R use sine shaping.
//------------------------------------------------------------------------

CSpotTone::CSpotTone(REAL gain,	REAL freq, REAL rise, REAL fall, unsigned int size, REAL sampleRate) :
m_curr(0.0F),
m_gain(0.0F),
m_mul(0.0F),
m_scl(0.0F),
m_sampleRate(sampleRate),
m_freq(0.0F),
m_gen(NULL),
m_riseDur(0.0F),
m_riseIncr(0.0F),
m_riseWant(0),
m_riseHave(0),
m_fallDur(0.0F),
m_fallIncr(0.0F),
m_fallWant(0),
m_fallHave(0),
m_size(size),
m_stage(SpotTone_IDLE),
m_buf(NULL)
{
	setValues(gain, freq, rise, fall);

	m_gen = new COscillator(freq, 0.0, sampleRate);

	m_buf = newCXB(size, NULL);
}

CSpotTone::~CSpotTone()
{
	delCXB(m_buf);
	delete m_gen;
}

// turn spotting on with current settings
void CSpotTone::on()
{
	// gain is in dB
	m_scl  = (REAL)::pow (10.0, m_gain / 20.0);
	m_curr = 0.0F;
	m_mul  = 0.0F;

	// A/R times are in msec
	m_riseWant = (unsigned int)(0.5 + m_sampleRate * (m_riseDur / 1e3));
	m_riseHave = 0;
	if (m_riseWant <= 1)
		m_riseIncr = 1.0F;
	else
		m_riseIncr = 1.0F / REAL(m_riseWant - 1);

	m_fallWant = (unsigned int)(0.5 + m_sampleRate * (m_fallDur / 1e3));
	m_fallHave = 0;
	if (m_fallWant <= 1)
		m_fallIncr = 1.0F;
	else
		m_fallIncr = 1.0F / REAL(m_fallWant - 1);

	// freq is in Hz
	m_gen->setFrequency(m_freq);
	m_gen->setPhase(0.0);

	m_stage = SpotTone_RISE;
}

// initiate turn-off
void CSpotTone::off()
{
	m_stage = SpotTone_FALL;
}

void CSpotTone::setValues(REAL gain, REAL freq, REAL rise, REAL fall)
{
	m_gain    = gain;
	m_freq    = freq;
	m_riseDur = rise;
	m_fallDur = fall;
}

bool CSpotTone::generate()
{
	m_gen->oscillate(m_buf);

	for (unsigned int i = 0; i < m_size; i++) {
		// in an envelope stage?
		if (m_stage == SpotTone_RISE) {
			// still going?
			if (m_riseHave++ < m_riseWant) {
				m_curr += m_riseIncr;
				m_mul   = REAL(m_scl * ::sin(m_curr * M_PI / 2.0));
			} else {
				// no, assert steady-state, force level
				m_curr  = 1.0F;
				m_mul   = m_scl;
				m_stage = SpotTone_STDY;
				// won't come back into envelopes
				// until FALL asserted from outside
			}
		} else if (m_stage == SpotTone_FALL) {
			// still going?
			if (m_fallHave++ < m_fallWant) {
				m_curr -= m_fallIncr;
				m_mul   = REAL(m_scl * ::sin(m_curr * M_PI / 2.0));
			} else {
				// no, assert trailing, force level
				m_curr  = 0.0F;
				m_mul   = 0.0F;
				m_stage = SpotTone_HOLD;
				// won't come back into envelopes hereafter
			}
		}

		// apply envelope
		// (same base as osc.gen internal buf)
		CXBdata(m_buf, i) = Cscl(CXBdata(m_buf, i), m_mul);
	}

	// indicate whether it's turned itself off
	// sometime during this pass
	return m_stage != SpotTone_HOLD;
}

CXB* CSpotTone::getData()
{
	return m_buf;
}
