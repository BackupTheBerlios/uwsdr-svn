/* RX.cpp

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

#include "RX.h"


CRX::CRX(unsigned int bufLen, unsigned int bits, unsigned int cpdLen, float sampleRate, CMeter* meter, CSpectrum* spectrum) :
m_meter(meter),
m_spectrum(spectrum),
m_type(SPEC_POST_FILT),
m_iBuf(NULL),
m_oBuf(NULL),
m_iq(NULL),
m_oscillator(NULL),
m_rit(NULL),
m_filter(NULL),
m_nb(NULL),
m_nbFlag(false),
m_nbSDROM(NULL),
m_nbSDROMFlag(false),
m_anr(NULL),
m_anrFlag(false),
m_anf(NULL),
m_anfFlag(false),
m_banr(NULL),
m_banrFlag(false),
m_banf(NULL),
m_banfFlag(false),
m_agc(NULL),
m_demodulator(NULL),
m_amDemodulator(NULL),
m_fmDemodulator(NULL),
m_ssbDemodulator(NULL),
m_spotTone(NULL),
m_spotToneFlag(false),
m_squelch(NULL),
m_compander(NULL),
m_companderFlag(false),
m_graphicEQ(NULL),
m_graphicEQFlag(false),
m_mode(USB),
m_binFlag(false),
m_azim(),
m_tick(0UL)
{
	ASSERT(meter != NULL);
	ASSERT(spectrum != NULL);

	m_filter = new CFilterOVSV(bufLen, bits, sampleRate, -4800.0F, 4800.0F);

	m_iBuf = newCXB(m_filter->fetchSize(), m_filter->fetchPoint());
	m_oBuf = newCXB(m_filter->storeSize(), m_filter->storePoint());

	m_iq = new CCorrectIQ(m_iBuf);

	m_oscillator = new COscillator(m_iBuf, -sampleRate / 4.0F, 0.0, sampleRate);

	m_rit = new COscillator(m_iBuf, 0.0F, 0.0, sampleRate);

	m_agc = new CAGC(agcLONG,	// mode kept around for control reasons alone
				    m_oBuf,	// input buffer
				    1.0F,	// Target output 
				    2.0F,	// Attack time constant in ms
				    500,	// Decay time constant in ms
				    1.0,	// Slope
				    500,	//Hangtime in ms
				    sampleRate,	// Sample rate
				    31622.8F,	// Maximum gain as a multipler, linear not dB
				    0.00001F,	// Minimum gain as a multipler, linear not dB
				    1.0); 	// Set the current gain

	m_graphicEQ = new CGraphicEQ(m_oBuf, sampleRate, bits);

	m_amDemodulator = new CAMDemod(sampleRate,	// float samprate
			 0.0F,	// float f_initial
			 -500.0F,	// float f_lobound,
			 500.0F,	// float f_hibound,
			 400.0F,	// float f_bandwid,
			 m_oBuf,	// COMPLEX *ivec,
			 m_oBuf,	// COMPLEX *ovec,
			 AMdet		// AM Mode AMdet == rectifier,
			 );			// SAMdet == synchronous detector

	m_fmDemodulator = new CFMDemod(sampleRate,	// float samprate
			 0.0F,	// float f_initial
			 -6000.0F,	// float f_lobound
			 6000.0F,	// float f_hibound
			 5000.0F,	// float f_bandwid
			 m_oBuf,	// COMPLEX *ivec
			 m_oBuf);	// COMPLEX *ovec

	m_ssbDemodulator = new CSSBDemod(m_oBuf, m_oBuf);

	m_demodulator = m_ssbDemodulator;

	m_anf = new CLMS(m_oBuf,	// CXB signal,
			    64,	// int delay,
			    0.01F,	// float adaptation_rate,
			    0.00001F,	// float leakage,
			    45,	// int adaptive_filter_size,
			    LMS_INTERFERENCE);

	m_banf = new CBlockLMS(m_oBuf, 0.00001f, 0.005f, BLMS_INTERFERENCE, bits);

	m_anr = new CLMS(m_oBuf,	// CXB signal,
			    64,	// int delay,
			    0.01f,	// float adaptation_rate,
			    0.00001f,	// float leakage,
			    45,	// int adaptive_filter_size,
			    LMS_NOISE);

	m_banr = new CBlockLMS(m_oBuf, 0.00001f, 0.005f, BLMS_NOISE, bits);

	m_nb = new CNoiseBlanker(m_iBuf, 3.3F);

	m_nbSDROM = new CNoiseBlanker(m_iBuf, 2.5F);

	m_spotTone = new CSpotTone(-12.0,	// gain
				   700.0,	// freq
				   5.0,	// ms rise
				   5.0,	// ms fall
				   bufLen,	// length of spot tone buffer
				   sampleRate	// sample rate
    );

	m_squelch = new CSquelch(m_oBuf, -150.0F, 0.0F, bufLen - 48);

	m_compander = new CCompand(cpdLen, 0.0F, m_oBuf);

	float pos = 0.5;		// 0 <= pos <= 1, left->right
	float theta = float((1.0 - pos) * M_PI / 2.0);
	m_azim = Cmplx((float)::cos(theta), (float)::sin(theta));
}

CRX::~CRX()
{
	delete m_compander;
	delete m_spotTone;
	delete m_agc;
	delete m_nbSDROM;
	delete m_nb;
	delete m_graphicEQ;
	delete m_anf;
	delete m_anr;
	delete m_banf;
	delete m_banr;
	delete m_amDemodulator;
	delete m_fmDemodulator;
	delete m_ssbDemodulator;
	delete m_oscillator;
	delete m_rit;
	delete m_iq;
	delete m_squelch;
	delCXB(m_oBuf);
	delCXB(m_iBuf);
	delete m_filter;
}

void CRX::process()
{
	if (m_nbFlag)
		m_nb->blank();

	if (m_nbSDROMFlag)
		m_nbSDROM->sdromBlank();

	meter(m_iBuf, RXMETER_PRE_CONV);

	m_iq->process();

	m_oscillator->mix();

	spectrum(m_iBuf, SPEC_PRE_FILT);

	m_rit->mix();

	if (m_tick == 0UL)
		m_filter->reset();

	m_filter->filter();
	CXBhave(m_oBuf) = CXBhave(m_iBuf);

	meter(m_oBuf, RXMETER_POST_FILT);
	spectrum(m_oBuf, SPEC_POST_FILT);

	if (m_companderFlag)
		m_compander->process();

	if (m_squelch->isSquelch())
		m_squelch->doSquelch();
	else
		m_agc->process();

	meter(m_oBuf,RXMETER_POST_AGC);
	spectrum(m_oBuf, SPEC_POST_AGC);

	m_demodulator->demodulate();

	// The chosen demodulator selects which noise reduction options are valid for that mode
	if (m_binFlag && m_demodulator->hasBinaural()) {
		if (m_banrFlag && m_anrFlag && m_demodulator->hasBlockANR() && m_demodulator->hasANR())
			m_banr->process();

		if (m_banfFlag && m_anfFlag && m_demodulator->hasBlockANF() && m_demodulator->hasANF())
			m_banf->process();
	} else {
		if (m_anrFlag && m_demodulator->hasANR()) {
			if (m_banrFlag && m_demodulator->hasBlockANR())
				m_banr->process();
			else
				m_anr->process();
		}

		if (m_anfFlag && m_demodulator->hasANF()) {
			if (m_banfFlag && m_demodulator->hasBlockANF())
				m_banf->process();
			else
				m_anf->process();
		}

//		for (unsigned int i = 0; i < CXBhave(m_oBuf); i++)
//			CXBimag(m_oBuf, i) = CXBreal(m_oBuf, i);
	}

	if (!m_squelch->isSet()) {
		m_squelch->noSquelch();

		// spotting tone
		if (m_spotToneFlag) {
			// remember whether it's turned itself off during this pass
			m_spotToneFlag = m_spotTone->generate();

			unsigned int n = CXBhave(m_oBuf);

			for (unsigned int i = 0; i < n; i++)
				CXBdata(m_oBuf, i) = Cadd(CXBdata(m_oBuf, i), CXBdata(m_spotTone->getData(), i));
		}
	}

	if (m_graphicEQFlag)
		m_graphicEQ->equalise();

	spectrum(m_oBuf, SPEC_POST_DET);

	m_tick++;
}

void CRX::meter(CXB* buf, RXMETERTAP tap)
{
	m_meter->setRXMeter(tap, buf, m_agc->getGain());
}

void CRX::spectrum(CXB* buf, SPECTRUMtype type)
{
	if (type == m_type)
		m_spectrum->setData(buf);
}

CXB* CRX::getIBuf()
{
	return m_iBuf;
}

CXB* CRX::getOBuf()
{
	return m_oBuf;
}

void CRX::setMode(SDRMODE mode)
{
	m_mode = mode;

	switch (m_mode) {
		case LSB:
		case USB:
		case CWL:
		case CWU:
			m_demodulator = m_ssbDemodulator;
			break;

		case AM:
			m_amDemodulator->setMode(AMdet);
			m_demodulator = m_amDemodulator;
			break;

		case SAM:
			m_amDemodulator->setMode(SAMdet);
			m_demodulator = m_amDemodulator;
			break;

		case FMN:
			m_demodulator = m_fmDemodulator;
			break;
	}
}

void CRX::setFilter(double lowFreq, double highFreq)
{
	m_filter->setFilter(lowFreq, highFreq);

	m_fmDemodulator->setBandwidth(lowFreq, highFreq);
}

void CRX::setFrequency(double freq)
{
	m_oscillator->setFrequency(freq);
}

void CRX::setRITFrequency(double freq)
{
	m_rit->setFrequency(freq);
}

void CRX::setCompandFlag(bool flag)
{
	m_companderFlag = flag;
}

void CRX::setCompandFactor(float factor)
{
	m_compander->setFactor(factor);
}

void CRX::setSquelchFlag(bool flag)
{
	m_squelch->setFlag(flag);
}

void CRX::setSquelchThreshold(float threshold)
{
	m_squelch->setThreshold(threshold);
}

void CRX::setFMDeviation(float deviation)
{
	m_fmDemodulator->setDeviation(deviation);
}

void CRX::setIQ(float phase, float gain)
{
	m_iq->setPhase(phase);
	m_iq->setGain(gain);
}

void CRX::setANFFlag(bool flag)
{
	m_anfFlag = flag;
}

void CRX::setBANFFlag(bool flag)
{
	m_banfFlag = flag;
}

void CRX::setANRValues(unsigned int adaptiveFilterSize, unsigned int delay, float adaptationRate, float leakage)
{
	m_anr->setAdaptiveFilterSize(adaptiveFilterSize);
	m_anr->setDelay(delay);
	m_anr->setAdaptationRate(adaptationRate);
	m_anr->setLeakage(leakage);

	m_banr->setAdaptationRate(min(0.1F * adaptationRate, 0.0002F));
}

void CRX::setANRFlag(bool flag)
{
	m_anrFlag = flag;
}

void CRX::setBANRFlag(bool flag)
{
	m_banrFlag = flag;
}

void CRX::setANFValues(unsigned int adaptiveFilterSize, unsigned int delay, float adaptationRate, float leakage)
{
	m_anf->setAdaptiveFilterSize(adaptiveFilterSize);
	m_anf->setDelay(delay);
	m_anf->setAdaptationRate(adaptationRate);
	m_anf->setLeakage(leakage);

	m_banf->setAdaptationRate(min(0.1F * adaptationRate, 0.0002F));
}

void CRX::setNBFlag(bool flag)
{
	m_nbFlag = flag;
}

void CRX::setNBThreshold(float threshold)
{
	m_nb->setThreshold(threshold);
}

void CRX::setNBSDROMFlag(bool flag)
{
	m_nbSDROMFlag = flag;
}

void CRX::setNBSDROMThreshold(float threshold)
{
	m_nbSDROM->setThreshold(threshold);
}

void CRX::setGraphicEQFlag(bool flag)
{
	m_graphicEQFlag = flag;
}

void CRX::setGraphicEQValues(float preamp, float gain0, float gain1, float gain2)
{
	m_graphicEQ->setEQ(preamp, gain0, gain1, gain2);
}

void CRX::setBinauralFlag(bool flag)
{
	m_binFlag = flag;
}

void CRX::setAGCMode(AGCMODE mode)
{
	m_agc->setMode(mode);
}

void CRX::setSpotToneFlag(bool flag)
{
	if (flag) {
		m_spotTone->on();
		m_spotToneFlag = true;
	} else {
		m_spotTone->off();
		m_spotToneFlag = false;
	}
}

void CRX::setSpotToneValues(float gain, float freq, float rise, float fall)
{
	m_spotTone->setValues(gain, freq, rise, fall);
}

void CRX::setAzim(float azim)
{
	float theta = float((1.0 - azim) * M_PI / 2.0);

	m_azim = Cmplx((float)::cos(theta), (float)::sin(theta));
}

void CRX::setSpectrumType(SPECTRUMtype type)
{
	m_type = type;
}

