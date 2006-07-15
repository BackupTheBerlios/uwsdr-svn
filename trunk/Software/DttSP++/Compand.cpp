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


// fac < 0: compression
// fac > 0: expansion
CCompand::CCompand(unsigned int npts, float fac, CXB* buff) :
m_npts(npts),
m_nend(npts - 1),
m_fac(fac),
m_tbl(NULL),
m_buff(buff)
{
	ASSERT(npts > 1);
	ASSERT(buff != NULL);

	m_tbl  = new float[npts];
	::memset(m_tbl, 0x00, npts * sizeof(float));

	setFactor(fac);
}

CCompand::~CCompand()
{
	delete[] m_tbl;
}

void CCompand::process()
{
	unsigned int n = CXBhave(m_buff);

	for (unsigned int i = 0; i < n; i++) {
		COMPLEX val = CXBdata(m_buff, i);

		float mag = Cmag(val);
		float scl = lookup(mag);

		CXBdata(m_buff, i) = Cscl(val, 0.8F * scl);
	}
}

float CCompand::getFactor() const
{
	return m_fac;
}

void CCompand::setFactor(float fac)
{
	if (fac == 0.0F) {	// just linear
		for (unsigned int i = 0; i < m_npts; i++)
			m_tbl[i] = float(i) / float(m_nend);
	} else {				// exponential
		float del = fac / float(m_nend);
		float scl = float(1.0 - ::exp(fac));

		for (unsigned int i = 0; i < m_npts; i++)
			m_tbl[i] = float((1.0 - ::exp(float(i) * del)) / scl);
	}

	m_fac = fac;
}

float CCompand::lookup(float x)
{
	if (x <= 0.0F) 
		return 0.0F;

	float d = x - ::floor(x);

	unsigned int i = (unsigned int)(x * m_npts);

	float y;
	if (i < m_nend)
		y = m_tbl[i] + d * (m_tbl[i + 1] - m_tbl[i]);
	else
		y = m_tbl[m_nend];

	return y / x;
}
