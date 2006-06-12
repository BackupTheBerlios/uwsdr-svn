/* Compand.cpp

waveshaping compander, mostly for speech

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

#include "Compand.h"
#include "FromSys.h"

#include <wx/wx.h>


// fac < 0: compression
// fac > 0: expansion

CCompand::CCompand(unsigned int npts, REAL fac, CXB* buff) :
m_npts(npts),
m_nend(npts - 1),
m_fac(fac),
m_tbl(NULL),
m_buff(NULL)
{
	wxASSERT(npts > 1);
	wxASSERT(buff != NULL);

	m_tbl  = new CRLB(npts);
	m_buff = newCXB(CXBsize(buff), CXBbase(buff));

	setFactor(fac);
}

CCompand::~CCompand()
{
	delete m_tbl;
	delCXB(m_buff);
}

void CCompand::process()
{
	unsigned int n = CXBsize(m_buff);

	for (unsigned int i = 0; i < n; i++) {
		COMPLEX val = CXBdata(m_buff, i);

		REAL mag = Cmag(val);
		REAL scl = lookup(mag);

		CXBdata(m_buff, i) = Cscl(val, 0.8F * scl);
	}
}

REAL CCompand::getFactor() const
{
	return m_fac;
}

void CCompand::setFactor(REAL fac)
{
	wxASSERT(m_tbl != NULL);

	if (fac == 0.0F) {	// just linear
		for (unsigned int i = 0; i < m_npts; i++)
			m_tbl->set(i, REAL(i) / REAL(m_nend));
	} else {				// exponential
		REAL del = fac / REAL(m_nend);
		REAL scl = REAL(1.0F - ::exp(fac));

		for (unsigned int i = 0; i < m_npts; i++)
			m_tbl->set(i, REAL((1.0 - ::exp(REAL(i) * del)) / scl));
	}

	m_fac = fac;
}

REAL CCompand::lookup(REAL x)
{
	wxASSERT(m_tbl != NULL);

	if (x <= 0.0F) 
		return 0.0F;

	REAL d = x - REAL(int(x));		// XXX

	unsigned int i = (unsigned int)(x * m_npts);

	REAL y;
	if (i < m_nend)
		y = m_tbl->get(i) + d * (m_tbl->get(i + 1) - m_tbl->get(i));
	else
		y = m_tbl->get(m_nend);

	return y / x;
}
