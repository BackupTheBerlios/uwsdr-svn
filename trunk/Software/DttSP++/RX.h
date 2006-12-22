/* RX.h

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

#ifndef _rx_h
#define _rx_h

#include "Defs.h"
#include "CXB.h"
#include "Meter.h"
#include "Spectrum.h"
#include "CorrectIQ.h"
#include "Oscillator.h"
#include "FilterOVSV.h"
#include "NoiseBlanker.h"
#include "LMS.h"
#include "BlockLMS.h"
#include "AGC.h"
#include "Demod.h"
#include "AMDemod.h"
#include "FMDemod.h"
#include "SSBDemod.h"
#include "Squelch.h"


class CRX {
    public:
	CRX(unsigned int bufLen, unsigned int bits, float sampleRate, CMeter* meter, CSpectrum* spectrum);
	virtual ~CRX();

	virtual void process();

	virtual CXB* getIBuf();
	virtual CXB* getOBuf();

	virtual void setMode(SDRMODE mode);

	virtual void setZeroIF(bool flag);

	virtual void setFilter(double lowFreq, double highFreq);

	virtual void setFrequency(double freq);

	virtual void setSquelchFlag(bool flag);
	virtual void setSquelchThreshold(float threshold);

	virtual void setFMDeviation(float deviation);

	virtual void setIQ(float phase, float gain);

	virtual void setANFFlag(bool flag);
	virtual void setBANRFlag(bool flag);
	virtual void setANFValues(unsigned int adaptiveFilterSize, unsigned int delay, float adaptationRate, float leakage);

	virtual void setANRFlag(bool flag);
	virtual void setBANFFlag(bool flag);
	virtual void setANRValues(unsigned int adaptiveFilterSize, unsigned int delay, float adaptationRate, float leakage);

	virtual void setNBFlag(bool flag);
	virtual void setNBThreshold(float threshold);

	virtual void setNBSDROMFlag(bool flag);
	virtual void setNBSDROMThreshold(float threshold);

	virtual void setBinauralFlag(bool flag);

	virtual void setAGCMode(AGCMODE mode);

	virtual void setAzim(float azim);

	virtual void setSpectrumType(SPECTRUMtype type);

	virtual float getOffset() const;

    private:
	float          m_sampleRate;

	CMeter*        m_meter;
	CSpectrum*     m_spectrum;
	SPECTRUMtype   m_type;

	CXB*           m_iBuf;
	CXB*           m_oBuf;

	CCorrectIQ*    m_iq;

	COscillator*   m_oscillator1;
	COscillator*   m_oscillator2;

	CFilterOVSV*   m_filter;

	CNoiseBlanker* m_nb;
	bool           m_nbFlag;

	CNoiseBlanker* m_nbSDROM;
	bool           m_nbSDROMFlag;

	CLMS*          m_anr;
	bool           m_anrFlag;

	CLMS*          m_anf;
	bool           m_anfFlag;

	CBlockLMS*     m_banr;
	bool           m_banrFlag;

	CBlockLMS*     m_banf;
	bool           m_banfFlag;

	CAGC*          m_agc;

	IDemod*        m_demodulator;
	CAMDemod*      m_amDemodulator;
	CFMDemod*      m_fmDemodulator;
	CSSBDemod*     m_ssbDemodulator;

	CSquelch*      m_squelch;

	SDRMODE        m_mode;

    bool           m_binFlag;

	bool           m_zeroIF;

	double         m_freq;
	double         m_lowFreq;
	double         m_highFreq;

	COMPLEX        m_azim;
	unsigned long  m_tick;

	void meter(CXB* buf, RXMETERTAP tap);
	void spectrum(CXB* buf, SPECTRUMtype type);
};

#endif
