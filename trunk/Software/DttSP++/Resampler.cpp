/* Resampler.cpp

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

#include "Resampler.h"
#include "Utils.h"
#include "CXOps.h"


const unsigned int FILTER_LENGTH = 19839;

CResampler::CResampler(float sampRateIn, float sampRateOut, unsigned int filterMemoryBuffLength) :
m_filterMemoryBuff(NULL),
m_filter(NULL),
m_filterMemoryBuffLength(0),
m_indexFiltMemBuf(0),
m_interpFactor(0),
m_filterPhaseNum(0),
m_deciFactor(0),
m_mask(0)
{
	ASSERT(filterMemoryBuffLength > 0);
	ASSERT(sampRateIn > 0.0F);
	ASSERT(sampRateOut > 0.0F);

	m_interpFactor = (unsigned int)(28224000.0F / sampRateIn  + 0.5F);
	m_deciFactor   = (unsigned int)(28224000.0F / sampRateOut + 0.5F);

	m_filterMemoryBuffLength = nblock2(max(filterMemoryBuffLength, FILTER_LENGTH));
	m_mask = m_filterMemoryBuffLength - 1;

	m_filterMemoryBuff = new COMPLEX[m_filterMemoryBuffLength];
	::memset(m_filterMemoryBuff, 0x00, m_filterMemoryBuffLength * sizeof(COMPLEX));

	m_filter = CFIR::lowpass(0.45F, float(m_interpFactor), FILTER_LENGTH);
}

CResampler::~CResampler()
{
	delete[] m_filter;
	delete[] m_filterMemoryBuff;
}

void CResampler::process(CXB* inBuf, CXB* outBuf)
{
	ASSERT(inBuf != NULL);
	ASSERT(outBuf != NULL);

	unsigned int outLen = 0;
	unsigned int inLen  = CXBhave(inBuf);

	for (unsigned int i = 0; i < inLen; i++) {
		/*
		 * save data in circular buffer
		 */
		m_filterMemoryBuff[m_indexFiltMemBuf] = CXBdata(inBuf, i);

		unsigned int j = m_indexFiltMemBuf;
		unsigned int jj = j;

		/*
		 * circular addressing
		 */
		m_indexFiltMemBuf = (m_indexFiltMemBuf + 1) & m_mask;

		/*
		 * loop through each filter phase: interpolate then decimate
		 */
		while (m_filterPhaseNum < m_interpFactor) {
			j = jj;

			COMPLEX outVal = cxzero;

			/*
			 * perform convolution
			 */
			for (unsigned int k = m_filterPhaseNum; k < FILTER_LENGTH; k += m_interpFactor) {
				outVal = Cadd(outVal, Cscl(m_filterMemoryBuff[j], m_filter[k].re));

				/*
				 * circular adressing
				 */
				j = (j + m_mask) & m_mask;
			}

			/*
			 * scale the data
			 */
			CXBdata(outBuf, outLen) = Cscl(outVal, float(m_interpFactor));
			outLen++;

			if (outLen == CXBsize(outBuf)) {
				CXBhave(outBuf) = outLen;
				return;
			}

			/*
			 * increment interpolation phase # by decimation factor
			 */
			m_filterPhaseNum += m_deciFactor;
		}

		m_filterPhaseNum -= m_interpFactor;
	}

	CXBhave(outBuf) = outLen;
}
