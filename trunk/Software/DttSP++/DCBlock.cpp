/* DCBlock.cpp

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

#include "DCBlock.h"
#include "FromSys.h"

#include <wx/wx.h>


// NB may have to ramify this a little
// for other sampling rates; maybe not  

// f == 0.002083 == 100 Hz at 48k

CDCBlock::CDCBlock(DCBMode mode, CXB* buf) :
m_mode(mode),
m_input(),
m_output(),
m_buf(NULL)
{
	wxASSERT(buf != NULL);

	::memset(m_input, 0, BLKMEM * sizeof(REAL));
	::memset(m_output, 0, BLKMEM * sizeof(REAL));

	m_buf = newCXB(CXBsize(buf), CXBbase(buf));
}

CDCBlock::~CDCBlock()
{
	delCXB(m_buf);
}

void CDCBlock::block()
{
	unsigned int i;

	switch (m_mode) {
		case DCB_LOW:
			for (i = 0; i < CXBsize(m_buf); i++) {
				REAL x = CXBreal(m_buf, i);
				REAL y = butterworthHighpass_100_2(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		case DCB_MED:
			for (i = 0; i < CXBsize(m_buf); i++) {
				REAL x = CXBreal(m_buf, i);
				REAL y = butterworthHighpass_100_4(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		case DCB_HIGH:
			for (i = 0; i < CXBsize(m_buf); i++) {
				REAL x = CXBreal(m_buf, i);
				REAL y = butterworthHighpass_100_6(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		case DCB_SUPER:
			for (i = 0; i < CXBsize(m_buf); i++) {
				REAL x = CXBreal(m_buf, i);
				REAL y = butterworthHighpass_100_8(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		default:
			break;
	}
}

REAL CDCBlock::butterworthHighpass_100_2(REAL xin)
{
	for (int i = 1; i < 2; i++) {
		m_input[i - 1] = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[2] = xin / 1.009297482F;

	m_output[2] = REAL((m_input[0] + m_input[2])
		  + -2.0 * m_input[1]
		  + -0.9816611902 * m_output[0] + 1.9814914708 * m_output[1]);

	return m_output[2];
}

REAL CDCBlock::butterworthHighpass_100_4(REAL xin)
{
	for (int i = 1; i < 4; i++) {
		m_input[i - 1] = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[4] = xin / 1.012F;

	m_output[4] = REAL((m_input[0] + m_input[4])
		  + -4.0 * (m_input[1] + m_input[3])
		  + 6.0 * m_input[2]
		  + -0.976340271 * m_output[0]
		  + 3.928738552 * m_output[1]
		  + -5.928454312 * m_output[2] + 3.976056024 * m_output[3]);

	return m_output[4];
}

REAL CDCBlock::butterworthHighpass_100_6(REAL xin)
{
	for (int i = 1; i < 6; i++) {
		m_input[i - 1] = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[6] = xin / 1.025606415F;

	m_output[6] = REAL((m_input[0] + m_input[6])
		  + -6.0 * (m_input[1] + m_input[5])
		  + 15.0 * (m_input[2] + m_input[4])
		  + -20.0 * m_input[3]
		  + -0.9506891622 * m_output[0]
		  + 5.7522090378 * m_output[1]
		  + -14.5019247580 * m_output[2]
		  + 19.4994114580 * m_output[3]
		  + -14.7484389800 * m_output[4] + 5.9494324049 * m_output[5]);

	return m_output[6];
}

REAL CDCBlock::butterworthHighpass_100_8(REAL xin)
{
	for (int i = 1; i < 8; i++) {
		m_input[i - 1] = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[8] = xin / 1.034112352F;

	m_output[8] = REAL((m_input[0] + m_input[8])
		  + -8.0 * (m_input[1] + m_input[7])
		  + 28.0 * (m_input[2] + m_input[6])
		  + -56.0 * (m_input[3] + m_input[5])
		  + 70.0 * m_input[4]
		  + -0.9351139781 * m_output[0]
		  + 7.5436450525 * m_output[1]
		  + -26.6244301320 * m_output[2]
		  + 53.6964633920 * m_output[3]
		  + -67.6854640540 * m_output[4]
		  + 54.6046308830 * m_output[5]
		  + -27.5326449810 * m_output[6] + 7.9329138172 * m_output[7]);

	return m_output[8];
}
