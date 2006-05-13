/* SSBDemod.cpp

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

#include "SSBDemod.h"

#include <wx/wx.h>


CSSBDemod::CSSBDemod(unsigned int size, COMPLEX* ivec, COMPLEX* ovec) :
m_size(size),
m_ibuf(NULL),
m_obuf(NULL)
{
	wxASSERT(ivec != NULL);
	wxASSERT(ovec != NULL);

	m_ibuf = newCXB(size, ivec);
	m_obuf = newCXB(size, ovec);
}

CSSBDemod::~CSSBDemod()
{
	delCXB(m_ibuf);
	delCXB(m_obuf);
}

void CSSBDemod::demodulate()
{
	for (unsigned int i = 0; i < m_size; i++)
		CXBdata(m_obuf, i) = CXBdata(m_ibuf, i);
}
