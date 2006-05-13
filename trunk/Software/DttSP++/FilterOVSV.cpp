/* FilterOVSV.cpp

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

#include "FilterOVSV.h"
#include "filter.h"
#include "cxops.h"
#include "banal.h"
#include "bufvec.h"

#include <wx/wx.h>


CFilterOVSV::CFilterOVSV(unsigned int ncoef, unsigned int pbits, REAL sampleRate) :
m_ncoef(ncoef),
m_pbits(pbits),
m_samprate(sampleRate),
m_bufLen(0),
m_fftLen(0),
m_zfvec(NULL),
m_zivec(NULL),
m_zovec(NULL),
m_zrvec(NULL),
m_pfwd(),
m_pinv(),
m_scale(0.0F)
{
	wxASSERT(sampleRate > 0.0F);

	m_bufLen = nblock2(ncoef - 1);
	m_fftLen = 2 * m_bufLen;

	m_zrvec = (COMPLEX*)::fftw_malloc(m_fftLen * sizeof(COMPLEX));
	m_zfvec = (COMPLEX*)::fftw_malloc(m_fftLen * sizeof(COMPLEX));
	m_zivec = (COMPLEX*)::fftw_malloc(m_fftLen * sizeof(COMPLEX));
	m_zovec = (COMPLEX*)::fftw_malloc(m_fftLen * sizeof(COMPLEX));

	wxASSERT(m_zrvec != NULL);
	wxASSERT(m_zfvec != NULL);
	wxASSERT(m_zivec != NULL);
	wxASSERT(m_zovec != NULL);

	::memset(m_zrvec, 0, m_fftLen * sizeof(COMPLEX));
	::memset(m_zfvec, 0, m_fftLen * sizeof(COMPLEX));
	::memset(m_zivec, 0, m_fftLen * sizeof(COMPLEX));
	::memset(m_zovec, 0, m_fftLen * sizeof(COMPLEX));

	/* prepare transforms for signal */
	m_pfwd = ::fftwf_plan_dft_1d(m_fftLen, (fftwf_complex *)m_zrvec, (fftwf_complex *)m_zivec, FFTW_FORWARD, pbits);
	m_pinv = ::fftwf_plan_dft_1d(m_fftLen, (fftwf_complex *)m_zivec, (fftwf_complex *)m_zovec, FFTW_BACKWARD, pbits);

	m_scale = 1.0F / REAL(m_fftLen);

	normalize_vec_COMPLEX(m_zfvec, m_fftLen);
}

CFilterOVSV::~CFilterOVSV()
{
	::fftw_free(m_zfvec);
	::fftw_free(m_zivec);
	::fftw_free(m_zovec);
	::fftw_free(m_zrvec);

	::fftwf_destroy_plan(m_pfwd);
	::fftwf_destroy_plan(m_pinv);
}

void CFilterOVSV::setFilter(REAL lowFreq, REAL highFreq)
{
	ComplexFIR* coefs = newFIR_Bandpass_COMPLEX(lowFreq, highFreq, m_samprate, m_ncoef);

	COMPLEX* zcvec = (COMPLEX*)::fftw_malloc(m_fftLen * sizeof(COMPLEX));

	fftwf_plan ptmp = ::fftwf_plan_dft_1d(m_fftLen, (fftwf_complex *)zcvec, (fftwf_complex *)m_zfvec, FFTW_FORWARD, m_pbits);

#ifdef LHS
	for (unsigned int i = 0; i < m_ncoef; i++)
		zcvec[i] = FIRtap(coefs, i);
#else
	for (unsigned int i = 0; i < m_ncoef; i++)
		zcvec[m_fftLen - m_ncoef + i] = FIRtap(coefs, i);
#endif

	::fftwf_execute(ptmp);
	::fftwf_destroy_plan(ptmp);
	::fftw_free(zcvec);

	delFIR_Bandpass_COMPLEX(coefs);

	normalize_vec_COMPLEX(m_zfvec, m_fftLen);
}

void CFilterOVSV::filter()
{
	unsigned int i, j;

	/* input sig -> z */
	::fftwf_execute(m_pfwd);

	/* convolve in z */
	unsigned int m = m_fftLen;
	for (i = 0; i < m; i++)
		m_zivec[i] = Cmul(m_zivec[i], m_zfvec[i]);

	/* z convolved sig -> time output sig */
	::fftwf_execute(m_pinv);

	/* scale */
	unsigned int n = m_bufLen;
	for (i = 0; i < n; i++) {
		m_zovec[i].re *= m_scale;
		m_zovec[i].im *= m_scale;
	}

	/* prepare input sig vec for next fill */
	for (i = 0, j = n; i < n; i++, j++)
		m_zrvec[i] = m_zrvec[j];
}

void CFilterOVSV::reset()
{
	::memset(m_zrvec, 0, m_fftLen * sizeof(COMPLEX));
}

/* where to put next batch of samples to filter */
COMPLEX* CFilterOVSV::fetchPoint()
{
	return m_zrvec + m_bufLen;
}

/* how many samples to put there */
unsigned int CFilterOVSV::fetchSize()
{
	return m_fftLen - m_bufLen;
}

/* where samples should be taken from after filtering */
#ifdef LHS
COMPLEX* CFilterOVSV::storePoint()
{
	return m_zovec + m_buflen;
}
#else
COMPLEX* CFilterOVSV::storePoint()
{
	return m_zovec;
}
#endif

/* how many samples to take */
/* NB strategy. This is the number of good samples in the
   left half of the true buffer. Samples in right half
   are circular artifacts and are ignored. */
unsigned int CFilterOVSV::storeSize()
{
	return m_fftLen - m_bufLen;
}

COMPLEX* CFilterOVSV::getZFvec()
{
	return m_zfvec;
}
