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
#include "GraphicEQ.h"
#include "SpeechProc.h"
#include "Compand.h"
#include "Meter.h"
#include "Spectrum.h"


class CTX {
    public:
	CTX(unsigned int bufLen, unsigned int bits, unsigned int cpdLen, float sampleRate, CMeter* meter, CSpectrum* spectrum);
	virtual ~CTX();

	virtual void process();

	virtual CXB* getIBuf();
	virtual CXB* getOBuf();

	virtual SDRMODE getMode() const;

	virtual void setMode(SDRMODE mode);

	virtual void setDCBlockFlag(bool flag);

	virtual void setFilter(double lowFreq, double highFreq);

	virtual void setFrequency(double freq);

	virtual void setCompandFlag(bool flag);
	virtual void setCompandFactor(float factor);

	virtual void setSquelchFlag(bool flag);
	virtual void setSquelchThreshold(float threshold);

	virtual void setAMCarrierLevel(float level);

	virtual void setFMDeviation(float deviation);

	virtual void setIQPhase(float phase);
	virtual void setIQGain(float gain);

	virtual void setLevelerFlag(bool flag);

	virtual void setGraphicEQFlag(bool flag);
	virtual void setGraphicEQValues(float preamp, float gain0, float gain1, float gain2);

	virtual void setCompressionFlag(bool flag);
	virtual void setCompressionLevel(float level);

	virtual void setNotchFlag(bool flag);


    private:
	CMeter*       m_meter;
	CSpectrum*    m_spectrum;

    CXB*          m_iBuf;
	CXB*          m_oBuf;

	CCorrectIQ*   m_iq;
	bool          m_dcBlockFlag;

	CDCBlock*     m_dcBlock;

	COscillator*  m_oscillator;

	CFilterOVSV*  m_filter;

	IMod*         m_modulator;
	CAMMod*       m_amModulator;
	CFMMod*       m_fmModulator;
	CSSBMod*      m_ssbModulator;

	CSquelch*     m_squelch;

	CAGC*         m_leveler;
	bool          m_levelerFlag;

	CAGC*         m_alc;
	bool          m_alcFlag;

	CGraphicEQ*   m_graphicEQ;
	bool          m_graphicEQFlag;

	CSpeechProc*  m_speechProc;
	bool          m_speechProcFlag;

    CCompand*     m_compander;
    bool          m_companderFlag;

	SDRMODE       m_mode;

	unsigned long m_tick;

	void meter(CXB* buf, TXMETERTYPE type);
	void spectrum(CXB* buf);
};

#endif
