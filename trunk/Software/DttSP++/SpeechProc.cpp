/* SpeechProc.cpp
   
  This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY, Phil Harman, VK6APH
Based on Visual Basic code for SDR by Phil Harman

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

#include "SpeechProc.h"
#include "FromSys.h"
#include "banal.h"

#include <wx/wx.h>


CSpeechProc::CSpeechProc(REAL k, REAL maxCompression, CXB* spdat) :
m_CG(NULL),
m_buf(spdat),
m_lastCG(1.0F),
m_k(k),
m_maxGain(0.0F),
m_fac(0.0F)
{
	wxASSERT(spdat != NULL);

	unsigned int size = CXBsize(spdat);

	m_CG  = new REAL[size + 1];
	::memset(m_CG, 0x00, (size + 1) * sizeof(REAL));

	setCompression(maxCompression);
}

CSpeechProc::~CSpeechProc()
{
	delete[] m_CG;
}

void CSpeechProc::setCompression(REAL compression)
{
	m_maxGain = (REAL)::pow(10.0, compression * 0.05);

	m_fac = REAL((((0.0000401002 * compression) - 0.0032093390) * compression + 0.0612862687) * compression + 0.9759745718);
}

void CSpeechProc::process()
{
	unsigned int n = CXBhave(m_buf);
	unsigned int i;

	if (m_maxGain == 1.0F)
		return;

	// K was 0.4 in VB version, this value is better, perhaps due to filters that follow?
	REAL r = 0.0;
	for (i = 0; i < n; i++)
		r = max(r, Cmag(CXBdata(m_buf, i)));	// find the peak magnitude value in the sample buffer 

	m_CG[0] = m_lastCG;	// restore from last time

	for (i = 1; i <= n; i++) {
		REAL mag = Cmag(CXBdata(m_buf, i - 1));

		if (mag != 0.0F) {
			REAL val = m_CG[i - 1] * (1.0F - m_k) + (m_k * r / mag);	// Frerking's formula

			m_CG[i] = (val > m_maxGain) ? m_maxGain : val;
		} else {
			m_CG[i] = m_maxGain;
		}
	}

	m_lastCG = m_CG[n];	// save for next time 

	for (i = 0; i < n; i++)	// multiply each sample by its gain constant 
		CXBdata(m_buf, i) = Cscl(CXBdata(m_buf, i), REAL(m_CG[i] / (m_fac * ::pow(m_maxGain, 0.25F))));
}
