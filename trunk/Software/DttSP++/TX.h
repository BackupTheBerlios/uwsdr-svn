/* TX.h

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

#ifndef _tx_h
#define _tx_h

#include "Defs.h"
#include "CXB.h"
#include "CorrectIQ.h"
#include "DCBlock.h"
#include "Oscillator.h"
#include "FilterOVSV.h"
#include "Mod.h"
#include "AMMod.h"
#include "FMMod.h"
#include "SSBMod.h"
#include "Squelch.h"
#include "AGC.h"
#include "SpeechProc.h"
#include "Meter.h"
#include "Spectrum.h"


class CTX {
    public:
	CTX(unsigned int bufLen, unsigned int bits, float sampleRate, CMeter* meter, CSpectrum* spectrum);
	virtual ~CTX();

	virtual void process();

	virtual CXB* getIBuf();
	virtual CXB* getOBuf();

	virtual SDRMODE getMode() const;

	virtual void setMode(SDRMODE mode);
	virtual void setWeaver(bool flag);

	virtual void setDCBlockFlag(bool flag);

	virtual void setFilter(double lowFreq, double highFreq);

	virtual void setFrequency(double freq);

	virtual void setAMCarrierLevel(float level);

	virtual void setFMDeviation(float deviation);

	virtual void setIQ(float phase, float gain);

	virtual void setALCAttack(float attack);
	virtual void setALCDecay(float decay);
	virtual void setALCGainBottom(float bottom);
	virtual void setALCGainTop(float top);
	virtual void setALCHangTime(float hang);

	virtual void setCompressionFlag(bool flag);
	virtual void setCompressionLevel(float level);

	virtual float getDSPOffset();

    private:
	float         m_sampleRate;

	CMeter*       m_meter;
	CSpectrum*    m_spectrum;

    CXB*          m_iBuf;
	CXB*          m_oBuf;

	CCorrectIQ*   m_iq;

	CDCBlock*     m_dcBlock;
	bool          m_dcBlockFlag;

	COscillator*  m_oscillator;

	CFilterOVSV*  m_filter;

	IMod*         m_modulator;
	CAMMod*       m_amModulator;
	CFMMod*       m_fmModulator;
	CSSBMod*      m_ssbModulator;

	CAGC*         m_alc;

	CSpeechProc*  m_speechProc;
	bool          m_speechProcFlag;

	SDRMODE       m_mode;
	bool          m_weaver;

	unsigned long m_tick;

	void meter(CXB* buf, TXMETERTYPE type);
};

#endif
