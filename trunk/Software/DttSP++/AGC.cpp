/* AGC.cpp

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

#include "AGC.h"
#include "FromSys.h"
#include "banal.h"

#include <wx/wx.h>


CAGC::CAGC(AGCMODE mode, COMPLEX* vec, unsigned int bufSize,  REAL limit, REAL attack,
		   REAL decay, REAL slope, REAL hangtime, REAL samprate, REAL maxGain,
		   REAL minGain, REAL curGain) :
m_mode(mode),
m_samprate(samprate),
m_gainTop(maxGain),
m_gainNow(curGain),
m_gainFastNow(curGain),
m_gainBottom(minGain),
m_gainLimit(limit),
m_gainFix(10.0F),
m_attack(0.0F),
m_oneMAttack(0.0F),
m_decay(0.0F),
m_oneMDecay(0.0F),
m_slope(slope),
m_fastAttack(0.0F),
m_oneMFastAttack(0.0F),
m_fastDecay(0.0F),
m_oneMFastDecay(0.0F),
m_hangTime(hangtime * 0.001F),
m_hangThresh(minGain),
m_fastHangTime(48.0F * 0.001F),		//wa6ahl:  added to structure
m_circ(NULL),
m_buff(NULL),
m_mask(2 * bufSize),
m_index(0),
m_sndex(0),
m_hangIndex(0),
m_fastIndex(FASTLEAD),
m_fastHang(0)			//wa6ahl:  added to structure
{
	wxASSERT(vec != NULL);
	wxASSERT(samprate > 0.0F);

	m_attack     = REAL(1.0 - ::exp(-1000.0 / (attack * samprate)));
	m_oneMAttack = (REAL)::exp(-1000.0 / (attack * samprate));

	m_decay     = REAL(1.0 - ::exp(-1000.0 / (decay * samprate)));
	m_oneMDecay = (REAL)::exp(-1000.0 / (decay * samprate));

	m_fastAttack     = REAL(1.0 - ::exp(-1000.0 / (0.2 * samprate)));
	m_oneMFastAttack = (REAL)::exp(-1000.0 / (0.2 * samprate));

	m_fastDecay     = REAL(1.0 - ::exp(-1000.0 / (3.0 * samprate)));
	m_oneMFastDecay = (REAL)::exp(-1000.0 / (3.0 * samprate));

	m_sndex = (unsigned int)(samprate * attack * 0.003F);

	m_buff = newCXB(bufSize, vec);
	m_circ = newvec_COMPLEX(m_mask);
	m_mask -= 1;
}

CAGC::~CAGC()
{
	delCXB(m_buff);
	delvec_COMPLEX(m_circ);
}

// FIXME check braces
void CAGC::process()
{
	unsigned int hangTime     = (unsigned int)(m_samprate * m_hangTime);
	unsigned int fastHangTime = (unsigned int)(m_samprate * m_fastHangTime);

	REAL hangThresh = 0.0F;

	if (m_hangThresh > 0.0F)
		hangThresh = m_gainTop * m_hangThresh + m_gainBottom * (1.0F - m_hangThresh);

	if (m_mode == agcOFF) {
		for (unsigned int i = 0; i < CXBsize(m_buff); i++)
			CXBdata(m_buff, i) = Cscl(CXBdata(m_buff, i), m_gainFix);

		return;
	}

	for (unsigned int i = 0; i < CXBsize(m_buff); i++) {
		m_circ[m_index] = CXBdata(m_buff, i);	/* Drop sample into circular buffer */
		REAL tmp = 1.1F * Cmag(m_circ[m_index]);

		if (tmp != 0.0F)
			tmp = m_gainLimit / tmp;	// if not zero sample, calculate gain
		else
			tmp = m_gainNow;	// update. If zero, then use old gain

		if (tmp < hangThresh)
			m_hangIndex = hangTime;

		if (tmp >= m_gainNow) {
			if (m_hangIndex++ > hangTime)
				m_gainNow = m_oneMDecay * m_gainNow + m_decay * min(m_gainTop, tmp);
		} else {
			m_hangIndex = 0;
			m_gainNow = m_oneMAttack * m_gainNow + m_attack * max(tmp, m_gainBottom);
		}

		tmp = 1.2F * Cmag(m_circ[m_fastIndex]);

		if (tmp != 0.0)
			tmp = m_gainLimit / tmp;
		else
			tmp = m_gainFastNow;

		if (tmp > m_gainFastNow) {
			if (m_fastHang++ > fastHangTime)
				m_gainFastNow = min(m_oneMFastDecay * m_gainFastNow + m_fastDecay * min(m_gainTop, tmp), m_gainTop);
		} else {
			m_fastHang = 0;
			m_gainFastNow = max(m_oneMFastAttack * m_gainFastNow + m_fastAttack * max(tmp, m_gainBottom), m_gainBottom);
		}

		m_gainFastNow = max(min(m_gainFastNow, m_gainTop), m_gainBottom);
		m_gainNow     = max(min(m_gainNow,     m_gainTop), m_gainBottom);

		CXBdata(m_buff, i) = Cscl(m_circ[m_sndex], min(m_gainFastNow, min(m_slope * m_gainNow, m_gainTop)));

		m_index = (m_index + m_mask) & m_mask;
		m_sndex = (m_sndex + m_mask) & m_mask;

		m_fastIndex = (m_fastIndex + m_mask) & m_mask;
	}
}

REAL CAGC::getGain() const
{
	return m_gainNow;
}

void CAGC::setGain(REAL gain)
{
	m_gainNow = gain;
}

AGCMODE CAGC::getMode() const
{
	return m_mode;
}

void CAGC::setMode(AGCMODE mode)
{
	m_mode = mode;
	m_attack = REAL(1.0 - ::exp(-500.0 / m_samprate));
	m_oneMAttack = 1.0F - m_attack;
	m_hangIndex = 0;
	m_index = 0;
	m_sndex = (unsigned int)(m_samprate * 0.006F);
	m_fastIndex = FASTLEAD;

	switch (mode) {
		case agcOFF:
			break;

		case agcSLOW:
			m_hangTime = 0.5F;
			m_fastHangTime = 0.1F;
			m_decay = REAL(1.0 - ::exp(-2.0 / m_samprate));
			m_oneMDecay = 1.0F - m_decay;
			break;

		case agcMED:
			m_hangTime = 0.25F;
			m_fastHangTime = 0.1F;
			m_decay = REAL(1.0 - ::exp(-4.0 / m_samprate));
			m_oneMDecay = 1.0F - m_decay;
			break;

		case agcFAST:
			m_hangTime = 0.1F;
			m_fastHangTime = 0.1F;
			m_decay = REAL(1.0 - ::exp(-10.0 / m_samprate));
			m_oneMDecay = 1.0F - m_decay;
			break;

		case agcLONG:
			m_hangTime = 0.75F;
			m_fastHangTime = 0.1F;
			m_decay = REAL(1.0 - ::exp(-0.5 / m_samprate));
			m_oneMDecay = 1.0F - m_decay;
			break;
    }
}
