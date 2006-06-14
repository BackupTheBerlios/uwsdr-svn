/* AMMod.cpp

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

#include "AMMod.h"

#include <wx/wx.h>


CAMMod::CAMMod(REAL level, CXB* in, CXB* out) :
m_carrierLevel(level),
m_input(in),
m_output(out)
{
	wxASSERT(level >= 0.0F && level <= 1.0F);
	wxASSERT(in != NULL);
	wxASSERT(out != NULL);
}

CAMMod::~CAMMod()
{
}

void CAMMod::setCarrierLevel(REAL level)
{
	wxASSERT(level >= 0.0F && level <= 1.0F);

	m_carrierLevel = level;
}

void CAMMod::modulate()
{
	unsigned int n = CXBhave(m_input);

	for (unsigned int i = 0; i < n; i++)
		CXBdata(m_output, i) = Cmplx(m_carrierLevel + (1.0F - m_carrierLevel) * CXBreal(m_input, i), 0.0F);

	CXBhave(m_output) = n;
}
