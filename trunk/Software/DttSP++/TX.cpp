/* TX.cpp

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

#include "TX.h"


CTX::CTX(unsigned int bufLen, unsigned int bits, unsigned int cpdLen, float sampleRate, CMeter* meter, CSpectrum* spectrum) :
m_meter(meter),
m_spectrum(spectrum),
m_iBuf(NULL),
m_oBuf(NULL),
m_iq(NULL),
m_dcBlockFlag(true),
m_dcBlock(NULL),
m_oscillator(NULL),
m_filter(NULL),
m_modulator(NULL),
m_amModulator(NULL),
m_fmModulator(NULL),
m_ssbModulator(NULL),
m_squelch(NULL),
m_leveler(NULL),
m_levelerFlag(true),
m_alc(NULL),
m_alcFlag(true),
m_speechProc(NULL),
m_speechProcFlag(false),
m_mode(USB),
m_tick(0UL)
{
	ASSERT(meter != NULL);
	ASSERT(spectrum != NULL);

	m_filter = new CFilterOVSV(bufLen, bits, sampleRate, 300.0F, 3000.0F);

	m_iBuf = newCXB(m_filter->fetchSize(), m_filter->fetchPoint());
	m_oBuf = newCXB(m_filter->storeSize(), m_filter->storePoint());

	m_iq = new CCorrectIQ(m_oBuf);

	m_dcBlock = new CDCBlock(DCB_MED, sampleRate, m_iBuf);

	m_oscillator = new COscillator(m_oBuf, 0.0, 0.0, sampleRate);

	m_amModulator  = new CAMMod(0.5F, m_iBuf);
	m_fmModulator  = new CFMMod(5000.0F, sampleRate, m_iBuf);
	m_ssbModulator = new CSSBMod(m_iBuf);
	m_modulator    = m_ssbModulator;

	m_squelch = new CSquelch(m_iBuf, -40.0F, -30.0F, bufLen - 48);

	m_leveler = new CAGC(agcLONG,	// mode kept around for control reasons
				m_iBuf,	// input buffer
				1.1F,	// Target output
				2,	// Attack time constant in ms
				500,	// Decay time constant in ms
				1,	// Slope
				500,	//Hangtime in ms
				sampleRate,	// Sample rate
				5.62F,	// Maximum gain as a multipler, linear not dB
				1.0,	// Minimum gain as a multipler, linear not dB
				1.0);		// Set the current gain

	m_alc = new CAGC(agcLONG,	// mode kept around for control reasons alone
			    m_iBuf,	// input buffer
			    1.2F,	// Target output 
			    2,	// Attack time constant in ms
			    10,	// Decay time constant in ms
			    1,	// Slope
			    500,	//Hangtime in ms
			    sampleRate,	// Sample rate
				1.0,	// Maximum gain as a multipler, linear not dB
			    0.000001F,	// Minimum gain as a multipler, linear not dB
			    1.0);		// Set the current gain

	m_speechProc = new CSpeechProc(0.4F, 3.0, m_iBuf);
}

CTX::~CTX()
{
	delete m_speechProc;
	delete m_alc;
	delete m_leveler;
	delete m_squelch;
	delete m_ssbModulator;
	delete m_fmModulator;
	delete m_amModulator;
	delete m_oscillator;
	delete m_dcBlock;
	delete m_iq;
	delCXB(m_oBuf);
	delCXB(m_iBuf);
	delete m_filter;
}

void CTX::process()
{
/*
	unsigned int n = CXBhave(m_iBuf);

	for (unsigned int i = 0; i < n; i++)
		CXBdata(m_iBuf, i) = Cmplx(CXBimag(m_iBuf, i), 0.0F);
*/
	if (m_dcBlockFlag)
		m_dcBlock->block();

	meter(m_iBuf, TX_MIC);

	if (m_squelch->isSquelch()) {
		m_squelch->doSquelch();
	} else {
		if (!m_squelch->isSet())
			m_squelch->noSquelch();

		meter(m_iBuf, TX_EQtap);

		if (m_levelerFlag)
			m_leveler->process();

		meter(m_iBuf, TX_LEVELER);

		if (m_speechProcFlag)
			m_speechProc->process();

		meter(m_iBuf, TX_COMP);
	}

	if (m_alcFlag)
		m_alc->process();

	meter(m_iBuf, TX_ALC);

	m_modulator->modulate();

	if (m_tick == 0UL)
		m_filter->reset();

	m_filter->filter();
	CXBhave(m_oBuf) = CXBhave(m_iBuf);

	m_spectrum->setData(m_oBuf);

	m_oscillator->mix();

	m_iq->process();

	meter(m_oBuf, TX_PWR);

	m_tick++;
}

void CTX::meter(CXB* buf, TXMETERTYPE type)
{
	float alcGain = 0.0F;
	if (m_alcFlag)
		alcGain = m_alc->getGain();

	float levelerGain = 0.0F;
	if (m_levelerFlag)
		levelerGain = m_leveler->getGain();

	m_meter->setTXMeter(type, buf, alcGain, levelerGain);
}

CXB* CTX::getIBuf()
{
	return m_iBuf;
}

CXB* CTX::getOBuf()
{
	return m_oBuf;
}

SDRMODE CTX::getMode() const
{
	return m_mode;
}

void CTX::setMode(SDRMODE mode)
{
	m_mode = mode;

	switch (m_mode) {
		case LSB:
		case USB:
		case CWL:
		case CWU:
			m_modulator = m_ssbModulator;
			break;

		case AM:
		case SAM:
			m_modulator = m_amModulator;
			break;

		case FMN:
			m_modulator = m_fmModulator;
			break;
	}
}

void CTX::setDCBlockFlag(bool flag)
{
	m_dcBlockFlag = flag;
}

void CTX::setFilter(double lowFreq, double highFreq)
{
	m_filter->setFilter(lowFreq, highFreq);
}

void CTX::setFrequency(double freq)
{
	m_oscillator->setFrequency(freq);
}

void CTX::setSquelchFlag(bool flag)
{
	m_squelch->setFlag(flag);
}

void CTX::setSquelchThreshold(float threshold)
{
	m_squelch->setThreshold(threshold);
}

void CTX::setAMCarrierLevel(float level)
{
	m_amModulator->setCarrierLevel(level);
}

void CTX::setFMDeviation(float deviation)
{
	m_fmModulator->setDeviation(deviation);
}

void CTX::setIQ(float phase, float gain)
{
	m_iq->setPhase(phase);
	m_iq->setGain(gain);
}

void CTX::setLevelerFlag(bool flag)
{
	m_levelerFlag = flag;
}

void CTX::setLevelerAttack(float attack)
{
	m_leveler->setAttack(attack);
}

void CTX::setLevelerDecay(float decay)
{
	m_leveler->setDecay(decay);
}

void CTX::setLevelerGainTop(float top)
{
	m_leveler->setGainTop(top);
}

void CTX::setLevelerGainBottom(float bottom)
{
	m_leveler->setGainBottom(bottom);
}

void CTX::setLevelerHangTime(float time)
{
	m_leveler->setHangTime(time);
}

void CTX::setALCAttack(float attack)
{
	m_alc->setAttack(attack);
}

void CTX::setALCDecay(float decay)
{
	m_alc->setDecay(decay);
}

void CTX::setALCGainTop(float top)
{
	m_alc->setGainTop(top);
}

void CTX::setALCGainBottom(float bottom)
{
	m_alc->setGainBottom(bottom);
}

void CTX::setALCHangTime(float time)
{
	m_alc->setHangTime(time);
}

void CTX::setCompressionFlag(bool flag)
{
	m_speechProcFlag = flag;
}

void CTX::setCompressionLevel(float level)
{
	m_speechProc->setCompression(level);
}
