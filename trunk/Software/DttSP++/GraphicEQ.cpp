/* GraphicEQ.cpp
 
 PCM frequency domain equalizer

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

#include "GraphicEQ.h"
#include "FromSys.h"
#include "FIR.h"

#include <wx/wx.h>


const REAL EQ_Num[] = { 0.9987f, -1.9969f, 0.9987f };
const REAL EQ_Den[] = { 1.9969f, -0.9974f };


CGraphicEQ::CGraphicEQ(CXB* d, REAL sampleRate, unsigned int bits) :
m_data(d),
m_sampleRate(sampleRate),
m_bits(bits),
m_p(NULL),
m_in(NULL),
m_out(NULL),
m_num(),
m_den(),
m_notchFlag(false)
{
	wxASSERT(d != NULL);
	wxASSERT(sampleRate > 0.0F);

	m_p = new CFilterOVSV(256, bits, sampleRate, -6000.0F, 6000.0F);

	m_in  = newCXB(256, m_p->fetchPoint());
	m_out = newCXB(256, m_p->storePoint());

	::memset(m_num, 0x00, 9 * sizeof(COMPLEX));
	::memset(m_den, 0x00, 6 * sizeof(COMPLEX));
}

CGraphicEQ::~CGraphicEQ()
{
	delCXB(m_in);
	delCXB(m_out);

	delete m_p;
}

void CGraphicEQ::setNotchFlag(bool setit)
{
	m_notchFlag = setit;
}

bool CGraphicEQ::getNotchFlag() const
{
	return m_notchFlag;
}

void CGraphicEQ::setEQ(REAL preamp, REAL gain0, REAL gain1, REAL gain2)
{
	unsigned int i;

	COMPLEX* tmpcoef = new COMPLEX[257];
	::memset(tmpcoef, 0x00, 257 * sizeof(COMPLEX));

	COMPLEX* filtcoef = (COMPLEX*)::fftw_malloc(512 * sizeof(COMPLEX));
	wxASSERT(filtcoef != NULL);
	::memset(filtcoef, 0x00, 512 * sizeof(COMPLEX));

	preamp = dB2lin(preamp) * 0.5F;
	gain0  = dB2lin(gain0) * preamp;
	gain1  = dB2lin(gain1) * preamp;
	gain2  = dB2lin(gain2) * preamp;

	COMPLEX* tmpfilt = CFIR::bandpass(-400.0F, 400.0F, m_sampleRate, 257);
	for (i = 0; i < 257; i++)
		tmpcoef[i] = Cscl(tmpfilt[i], gain0);
	delete[] tmpfilt;

	tmpfilt = CFIR::bandpass(400.0F, 1500.0F, m_sampleRate, 257);
	for (i = 0; i < 257; i++)
		tmpcoef[i] = Cadd(tmpcoef[i], Cscl(tmpfilt[i], gain1));
	delete[] tmpfilt;

	tmpfilt = CFIR::bandpass(-1500.0F, -400.0F, m_sampleRate, 257);
	for (i = 0; i < 257; i++)
		tmpcoef[i] = Cadd(tmpcoef[i], Cscl(tmpfilt[i], gain1));
	delete[] tmpfilt;

	tmpfilt = CFIR::bandpass(1500.0F, 6000.0F, m_sampleRate, 257);
	for (i = 0; i < 257; i++)
		tmpcoef[i] = Cadd(tmpcoef[i], Cscl(tmpfilt[i], gain2));
	delete[] tmpfilt;

	tmpfilt = CFIR::bandpass(-6000.0F, -1500.0F, m_sampleRate, 257);
	for (i = 0; i < 257; i++)
		tmpcoef[i] = Cadd(tmpcoef[i], Cscl(tmpfilt[i], gain2));
	delete[] tmpfilt;

	for (i = 0; i < 257; i++)
		filtcoef[255 + i] = tmpcoef[i];

	fftwf_plan ptmp = ::fftwf_plan_dft_1d(512, (fftwf_complex *)filtcoef,
								(fftwf_complex *)m_p->getZFvec(),
								FFTW_FORWARD, m_bits);

	::fftwf_execute(ptmp);
	::fftwf_destroy_plan(ptmp);
	::fftw_free(filtcoef);

	delete[] tmpcoef;
}

void CGraphicEQ::equalise()
{
	unsigned int sigsize = CXBhave(m_data);
	unsigned int sigidx = 0;

	do {
		::memcpy(CXBbase(m_in), &CXBdata(m_data, sigidx), 256 * sizeof(COMPLEX));

		m_p->filter();

		::memcpy(&CXBdata(m_data, sigidx), CXBbase(m_out), 256 * sizeof(COMPLEX));

		sigidx += 256;
	} while (sigidx < sigsize);

	if (m_notchFlag) {
		for (unsigned int i = 0; i < sigsize; i++) {
			for (unsigned int j = 0; j < 3; j++) {
				unsigned int k = 3 * j;
				unsigned int l = 2 * j;

				m_num[k] = CXBdata(m_data, i);

				COMPLEX numfilt;
				numfilt.re = m_num[k].re * EQ_Num[0] + m_num[k + 1].re * EQ_Num[1] + m_num[k + 2].re * EQ_Num[2];
				numfilt.im = m_num[k].im * EQ_Num[0] + m_num[k + 1].im * EQ_Num[1] + m_num[k + 2].im * EQ_Num[2];

				m_num[k + 2] = m_num[k + 1];
				m_num[k + 1] = m_num[k];

				CXBdata(m_data, i) = Cmplx(numfilt.re + m_den[l].re * EQ_Den[0] +
										m_den[l + 1].re * EQ_Den[1],
										numfilt.im + m_den[l].im * EQ_Den[0] +
										m_den[l + 1].im * EQ_Den[1]);

				m_den[l + 1] = m_den[l];
				m_den[l + 0] = CXBdata(m_data, i);
			}
		}
	}
}

REAL CGraphicEQ::dB2lin(REAL dB)
{
	return (REAL)::pow(10.0, dB / 20.0);
}
