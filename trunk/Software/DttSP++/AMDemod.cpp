/* AMDemod.cpp

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

#include "AMDemod.h"
#include "fastrig.h"

#include <wx/wx.h>


CAMDemod::CAMDemod(REAL samprate, REAL f_initial, REAL f_lobound, REAL f_hibound, REAL f_bandwid, unsigned int size, COMPLEX* ivec, COMPLEX* ovec, AMMode mode) :
m_size(size),
m_ibuf(NULL),
m_obuf(NULL),
m_pllAlpha(0.0F),
m_pllBeta(0.0F),
m_pllFastAlpha(0.0F),
m_pllFreqF(0.0F),
m_pllFreqL(0.0F),
m_pllFreqH(0.0F),
m_pllPhase(0.0F),
m_pllIIRAlpha(0.0F),
m_pllDelay(cxJ),
m_lockCurr(0.5F),
m_lockPrev(1.0F),
m_dc(0.0F),
m_smooth(0.0F),
m_mode(mode)
{
	wxASSERT(samprate > 0.0F);
	wxASSERT(ivec != NULL);
	wxASSERT(ovec != NULL);

	m_ibuf = newCXB(size, ivec);
	m_obuf = newCXB(size, ovec);

	REAL fac = REAL(TWOPI / samprate);

	m_pllFreqF = f_initial * fac;
	m_pllFreqL = f_lobound * fac;
	m_pllFreqH = f_hibound * fac;

	m_pllIIRAlpha = f_bandwid * fac;				/* arm filter */
	m_pllAlpha = m_pllIIRAlpha * 0.3F;				/* pll bandwidth */
	m_pllBeta = m_pllAlpha * m_pllAlpha * 0.25F;	/* second order term */
	m_pllFastAlpha = m_pllAlpha;
}

CAMDemod::~CAMDemod()
{
	delCXB(m_ibuf);
	delCXB(m_obuf);
}

AMMode CAMDemod::getMode() const
{
	return m_mode;
}

void CAMDemod::setMode(AMMode mode)
{
	m_mode = mode;
}

void CAMDemod::demodulate()
{
	unsigned int i;

	switch (m_mode) {
		case SAMdet:
			for (i = 0; i < m_size; i++) {
				pll(CXBdata(m_ibuf, i));
				REAL demout = dem();

				CXBdata(m_obuf, i) = Cmplx(demout, demout);
			}
			break;

		case AMdet:
			for (i = 0; i < m_size; i++) {
				m_lockCurr = Cmag(CXBdata(m_ibuf, i));
				m_dc = 0.9999F * m_dc + 0.0001F * m_lockCurr;
				m_smooth = 0.5F * m_smooth + 0.5F * (m_lockCurr - m_dc);

				CXBdata(m_obuf, i) = Cmplx(m_smooth, m_smooth);
			}
			break;
	}
}

void CAMDemod::pll(COMPLEX sig)
{
	COMPLEX z = Cmplx((REAL)::cos(m_pllPhase), (REAL)::sin(m_pllPhase));

	m_pllDelay.re =  z.re * sig.re + z.im * sig.im;
	m_pllDelay.im = -z.im * sig.re + z.re * sig.im;

	REAL diff = Cmag(sig) * ATAN2(m_pllDelay.im, m_pllDelay.re);

	m_pllFreqF += m_pllBeta * diff;

	if (m_pllFreqF < m_pllFreqL)
		m_pllFreqF = m_pllFreqL;
	if (m_pllFreqF > m_pllFreqH)
		m_pllFreqF = m_pllFreqH;

	m_pllPhase += m_pllFreqF + m_pllAlpha * diff;

	while (m_pllPhase >= TWOPI)
		m_pllPhase -= (REAL) TWOPI;

	while (m_pllPhase < 0.0F)
		m_pllPhase += (REAL) TWOPI;
}

REAL CAMDemod::dem()
{
	m_lockCurr = REAL(0.999 * m_lockCurr + 0.001 * ::fabs(m_pllDelay.im));

	m_lockPrev = m_lockCurr;

	m_dc = 0.99F * m_dc + 0.01F * m_pllDelay.re;

	return m_pllDelay.re - m_dc;
}
