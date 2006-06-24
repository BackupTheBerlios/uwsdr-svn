/* Spectrum.h

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

#ifndef _spectrum_h
#define _spectrum_h

#include "DataTypes.h"
#include "Window.h"
#include "CXB.h"
#include "fftw3.h"


typedef enum {
	SPEC_MAG,
	SPEC_PWR
} SPECTRUMscale;


typedef enum {
	SPEC_PRE_FILT,
	SPEC_POST_FILT,
	SPEC_POST_AGC,
	SPEC_POST_DET
} SPECTRUMtype;


class CSpectrum {
    public:
	CSpectrum(unsigned int size, unsigned int planbits, SPECTRUMscale scale);
	virtual ~CSpectrum();

	virtual SPECTRUMscale getScale() const;
	virtual void setScale(SPECTRUMscale scale);

	virtual Windowtype getWindow() const;
	virtual void setWindow(Windowtype type);

	virtual bool getPolyphase() const;
	virtual void setPolyphase(bool setit);

	virtual void setData(CXB* buf);

	virtual void reinitSpectrum();

	virtual void snapSpectrum();
	virtual void computeSpectrum(float* spectrum);

	virtual void snapScope();
	virtual void computeScopeReal(float* results, unsigned int numpoints);
	virtual void computeScopeComplex(float* results, unsigned int numpoints);

    private:
	CXB*          m_accum;
	CXB*          m_timebuf;
	CXB*          m_freqbuf;
	unsigned int  m_fill;
	SPECTRUMscale m_scale;
	unsigned int  m_size;
	unsigned int  m_mask;
	Windowtype    m_winType;
	REAL*         m_window;
	unsigned int  m_planbits;
	fftwf_plan    m_plan;
	bool          m_polyphase;
};

#endif
