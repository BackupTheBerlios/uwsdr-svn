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

CDCBlock::CDCBlock(DCBMode mode, float sampleRate, CXB* buf) :
m_mode(mode),
m_sampleRate(sampleRate),
m_buf(buf),
m_input(),
m_output()
{
	wxASSERT(buf != NULL);
	wxASSERT(sampleRate > 0.0F);

	::memset(m_input,  0x00, BLKMEM * sizeof(float));
	::memset(m_output, 0x00, BLKMEM * sizeof(float));
}

CDCBlock::~CDCBlock()
{
}

void CDCBlock::block()
{
	unsigned int i;
	unsigned int n = CXBhave(m_buf);

	switch (m_mode) {
		case DCB_LOW:
			for (i = 0; i < n; i++) {
				float x = CXBreal(m_buf, i);
				float y = 0.0F;
				if (m_sampleRate == 11025.0F)
					y = butterworthHighpass_100_2_11025(x);
				else if (m_sampleRate == 48000.0F)
					y = butterworthHighpass_100_2_48000(x);
				else if (m_sampleRate == 96000.0F)
					y = butterworthHighpass_100_2_96000(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		case DCB_MED:
			for (i = 0; i < n; i++) {
				float x = CXBreal(m_buf, i);
				float y = 0.0F;
				if (m_sampleRate == 11025.0F)
					y = butterworthHighpass_100_4_11025(x);
				else if (m_sampleRate == 48000.0F)
					y = butterworthHighpass_100_4_48000(x);
				else if (m_sampleRate == 96000.0F)
					y = butterworthHighpass_100_4_96000(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		case DCB_HIGH:
			for (i = 0; i < n; i++) {
				float x = CXBreal(m_buf, i);
				float y = 0.0F;
				if (m_sampleRate == 11025.0F)
					y = butterworthHighpass_100_6_11025(x);
				else if (m_sampleRate == 48000.0F)
					y = butterworthHighpass_100_6_48000(x);
				else if (m_sampleRate == 96000.0F)
					y = butterworthHighpass_100_6_96000(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		case DCB_SUPER:
			for (i = 0; i < n; i++) {
				float x = CXBreal(m_buf, i);
				float y = 0.0F;
				if (m_sampleRate == 11025.0F)
					y = butterworthHighpass_100_8_11025(x);
				else if (m_sampleRate == 48000.0F)
					y = butterworthHighpass_100_8_48000(x);
				else if (m_sampleRate == 96000.0F)
					y = butterworthHighpass_100_8_96000(x);
				CXBdata(m_buf, i) = Cmplx(y, 0.0);
			}
			break;

		default:
			break;
	}
}

float CDCBlock::butterworthHighpass_100_2_11025(float xin)
{
	for (int i = 1; i < 2; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[2] = xin / 1.041121583F;

	m_output[2] = float((m_input[0] + m_input[2])
		  + -2.0 * m_input[1]
		  + -0.9225658767 * m_output[0]
		  + 1.9194445715 * m_output[1]);

	return m_output[2];
}

float CDCBlock::butterworthHighpass_100_4_11025(float xin)
{
	for (int i = 1; i < 4; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[4] = xin / 1.077316558F;

	m_output[4] = float((m_input[0] + m_input[4])
		  + -4.0 * (m_input[1] + m_input[3])
		  + 6.0 * m_input[2]
		  + -0.8616151567 * m_output[0]
		  + 3.5747664761 * m_output[1]
		  + -5.5642479461 * m_output[2]
		  + 3.8510868243 * m_output[3]);

	return m_output[4];
}

float CDCBlock::butterworthHighpass_100_6_11025(float xin)
{
	for (int i = 1; i < 6; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[6] = xin / 1.116407326F;

	m_output[6] = float((m_input[0] + m_input[6])
		  + -6.0 * (m_input[1] + m_input[5])
		  + 15.0 * (m_input[2] + m_input[4])
		  + -20.0 * m_input[3]
		  + -0.8023329811 * m_output[0]
		  + 4.9906619299 * m_output[1]
		  + -12.9378695520 * m_output[2]
		  + 17.8928993560 * m_output[3]
		  + -13.9231708470 * m_output[4]
		  + 5.7798120636 * m_output[5]);

	return m_output[6];
}

float CDCBlock::butterworthHighpass_100_8_11025(float xin)
{
	for (int i = 1; i < 8; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[8] = xin / 1.157296969F;

	m_output[8] = float((m_input[0] + m_input[8])
		  + -8.0 * (m_input[1] + m_input[7])
		  + 28.0 * (m_input[2] + m_input[6])
		  + -56.0 * (m_input[3] + m_input[5])
		  + 70.0 * m_input[4]
		  + -0.7466384796 * m_output[0]
		  + 6.1912148518 * m_output[1]
		  + -22.4646074400 * m_output[2]
		  + 46.5869431820 * m_output[3]
		  + -60.3936379700 * m_output[4]
		  + 50.1165141440 * m_output[5]
		  + -25.9976696520 * m_output[6]
		  + 7.7078813631 * m_output[7]);

	return m_output[8];
}

float CDCBlock::butterworthHighpass_100_2_48000(float xin)
{
	for (int i = 1; i < 2; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[2] = xin / 1.009297482F;

	m_output[2] = float((m_input[0] + m_input[2])
		  + -2.0 * m_input[1]
		  + -0.9816611902 * m_output[0]
		  + 1.9814914708 * m_output[1]);

	return m_output[2];
}

float CDCBlock::butterworthHighpass_100_4_48000(float xin)
{
	for (int i = 1; i < 4; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[4] = xin / 1.012F;

	m_output[4] = float((m_input[0] + m_input[4])
		  + -4.0 * (m_input[1] + m_input[3])
		  + 6.0 * m_input[2]
		  + -0.976340271 * m_output[0]
		  + 3.928738552 * m_output[1]
		  + -5.928454312 * m_output[2]
		  + 3.976056024 * m_output[3]);

	return m_output[4];
}

float CDCBlock::butterworthHighpass_100_6_48000(float xin)
{
	for (int i = 1; i < 6; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[6] = xin / 1.025606415F;

	m_output[6] = float((m_input[0] + m_input[6])
		  + -6.0 * (m_input[1] + m_input[5])
		  + 15.0 * (m_input[2] + m_input[4])
		  + -20.0 * m_input[3]
		  + -0.9506891622 * m_output[0]
		  + 5.7522090378 * m_output[1]
		  + -14.5019247580 * m_output[2]
		  + 19.4994114580 * m_output[3]
		  + -14.7484389800 * m_output[4]
		  + 5.9494324049 * m_output[5]);

	return m_output[6];
}

float CDCBlock::butterworthHighpass_100_8_48000(float xin)
{
	for (int i = 1; i < 8; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[8] = xin / 1.034112352F;

	m_output[8] = float((m_input[0] + m_input[8])
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
		  + -27.5326449810 * m_output[6]
		  + 7.9329138172 * m_output[7]);

	return m_output[8];
}

float CDCBlock::butterworthHighpass_100_2_96000(float xin)
{
	for (int i = 1; i < 2; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[2] = xin / 1.004638729F;

	m_output[2] = float((m_input[0] + m_input[2])
		  + -2.0 * m_input[1]
		  + -0.9907866988 * m_output[0]
		  + 1.9907440595 * m_output[1]);

	return m_output[2];
}

float CDCBlock::butterworthHighpass_100_4_96000(float xin)
{
	for (int i = 1; i < 4; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[4] = xin / 1.008588121F;

	m_output[4] = float((m_input[0] + m_input[4])
		  + -4.0 * (m_input[1] + m_input[3])
		  + 6.0 * m_input[2]
		  + -0.9830425195 * m_output[0]
		  + 3.9489829109 * m_output[1]
		  + -5.9488375389 * m_output[2]
		  + 3.9828971456 * m_output[3]);

	return m_output[4];
}

float CDCBlock::butterworthHighpass_100_6_96000(float xin)
{
	for (int i = 1; i < 6; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[6] = xin / 1.012724241F;

	m_output[6] = float((m_input[0] + m_input[6])
		  + -6.0 * (m_input[1] + m_input[5])
		  + 15.0 * (m_input[2] + m_input[4])
		  + -20.0 * m_input[3]
		  + -0.9750291254 * m_output[0]
		  + 5.8748311623 * m_output[1]
		  + -14.7490308780 * m_output[2]
		  + 19.7483969000 * m_output[3]
		  + -14.8738801890 * m_output[4]
		  + 5.9747121299 * m_output[5]);

	return m_output[6];
}

float CDCBlock::butterworthHighpass_100_8_96000(float xin)
{
	for (int i = 1; i < 8; i++) {
		m_input[i - 1]  = m_input[i];
		m_output[i - 1] = m_output[i];
	}

	m_input[8] = xin / 1.016915759F;

	m_output[8] = float((m_input[0] + m_input[8])
		  + -8.0 * (m_input[1] + m_input[7])
		  + 28.0 * (m_input[2] + m_input[6])
		  + -56.0 * (m_input[3] + m_input[5])
		  + 70.0 * m_input[4]
		  + -0.9670079498 * m_output[0]
		  + 7.7685052437 * m_output[1]
		  + -27.3038585390 * m_output[2]
		  + 54.8369922380 * m_output[3]
		  + -68.8342101900 * m_output[4]
		  + 55.2988508250 * m_output[5]
		  + -27.7657231490 * m_output[6]
		  + 7.9664515216 * m_output[7]);

	return m_output[8];
}
