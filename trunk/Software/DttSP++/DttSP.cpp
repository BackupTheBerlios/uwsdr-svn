/* DttSP.cpp

common defs and code for parm update 
   
This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006-5 by Frank Brickle, AB2KT and Bob McGwier, N4HY
Copyright (C) 2006-2008 by Jonathan Naylor, G4KLX

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

#include "DttSP.h"
#include "Defs.h"


CDttSP::CDttSP(float sampleRate, unsigned int audioSize) :
m_sampleRate(sampleRate),
m_running(true),
m_suspend(false),
m_update(NULL),
m_buffer(NULL),
m_rx(NULL),
m_tx(NULL),
m_meter(NULL),
m_spectrum(NULL),
m_inputI(NULL),
m_inputQ(NULL),
m_outputI(NULL),
m_outputQ(NULL),
m_bufferI(NULL),
m_bufferQ(NULL),
m_trx(RX),
m_trxNext(RX),
m_state(RUN_PLAY),
m_stateLast(RUN_PLAY),
m_tick(0),
m_want(0),
m_have(0),
m_fade(0),
m_tail(0),
m_frames(audioSize)
{
	ASSERT(sampleRate > 0.0F);

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
	m_update = new wxSemaphore();
	m_buffer = new wxSemaphore();
#elif defined(WIN32)
	m_update = ::CreateSemaphore(NULL, 0, 99, NULL);
	m_buffer = ::CreateSemaphore(NULL, 0, 99, NULL);;
#else
	::sem_init(&m_update, 0, 0);
	::sem_init(&m_buffer, 0, 0);
#endif

	m_meter    = new CMeter();
	m_spectrum = new CSpectrum(DEFSPEC, FFTW_ESTIMATE, SPEC_PWR);

	m_bufferI = new float[m_frames];
	m_bufferQ = new float[m_frames];
	::memset(m_bufferI, 0x00, m_frames * sizeof(float));
	::memset(m_bufferQ, 0x00, m_frames * sizeof(float));

	m_inputI  = new CRingBuffer(RINGSIZE, 1);
	m_inputQ  = new CRingBuffer(RINGSIZE, 1);
	m_outputI = new CRingBuffer(RINGSIZE, 1);
	m_outputQ = new CRingBuffer(RINGSIZE, 1);

	m_fade = m_frames / 5;
	m_tail = m_frames - m_fade;

	m_rx = new CRX(m_frames, FFTW_ESTIMATE, sampleRate, m_meter, m_spectrum);
	m_tx = new CTX(m_frames, FFTW_ESTIMATE, sampleRate, m_meter, m_spectrum);

	m_rx->setMode(DEFMODE);
	m_tx->setMode(DEFMODE);
}

CDttSP::~CDttSP()
{
	m_running = false;
	m_suspend = true;

	delete[] m_bufferI;
	delete[] m_bufferQ;

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
	delete m_update;
	delete m_buffer;
#elif defined(WIN32)
	::CloseHandle(m_update);
	::CloseHandle(m_buffer);
#else
	::sem_destroy(&m_update);
	::sem_destroy(&m_buffer);
#endif

	delete m_rx;
	delete m_tx;
	delete m_meter;
	delete m_spectrum;
	delete m_inputI;
	delete m_inputQ;
	delete m_outputI;
	delete m_outputQ;;
}

void CDttSP::setMode(SDRMODE m)
{
	SEM_WAIT(m_update);

	m_tx->setMode(m);
	m_rx->setMode(m);

	SEM_POST(m_update);
}

void CDttSP::setWeaver(bool flag)
{
	m_tx->setWeaver(flag);
	m_rx->setWeaver(flag);
}

void CDttSP::setDCBlockFlag(bool flag)
{
	m_tx->setDCBlockFlag(flag);
}

void CDttSP::setRXFilter(double lowFreq, double highFreq)
{
	SEM_WAIT(m_update);

	m_rx->setFilter(lowFreq, highFreq);

	SEM_POST(m_update);
}

void CDttSP::setTXFilter(double lowFreq, double highFreq)
{
	SEM_WAIT(m_update);

	m_tx->setFilter(lowFreq, highFreq);

	SEM_POST(m_update);
}

void CDttSP::releaseUpdate()
{
	SEM_POST(m_update);
}

void CDttSP::setRXFrequency(double freq)
{
	if (::fabs(freq) >= 0.5 * m_sampleRate)
		return;

	m_rx->setFrequency(freq);
}

void CDttSP::setTXFrequency(double freq)
{
	if (::fabs(freq) >= 0.5 * m_sampleRate)
		return;

	m_tx->setFrequency(freq);
}

void CDttSP::setANRFlag(bool flag)
{
	m_rx->setANRFlag(flag);
}

void CDttSP::setBANRFlag(bool flag)
{
	m_rx->setBANRFlag(flag);
}

void CDttSP::setANRValues(unsigned int taps, unsigned int delay, float gain, float leak)
{
	m_rx->setANRValues(taps, delay, gain, leak);
}

void CDttSP::setRXSquelchFlag(bool flag)
{
	m_rx->setSquelchFlag(flag);
}

void CDttSP::setRXSquelchThreshold(float threshold)
{
	m_rx->setSquelchThreshold(threshold);
}

void CDttSP::setANFFlag(bool flag)
{
	m_rx->setANFFlag(flag);
}

void CDttSP::setBANFFlag(bool flag)
{
	m_rx->setBANFFlag(flag);
}

void CDttSP::setANFValues(unsigned int taps, unsigned int delay, float gain, float leak)
{
	m_rx->setANFValues(taps, delay, gain, leak);
}

void CDttSP::setNBFlag(bool flag)
{
	m_rx->setNBFlag(flag);
}

void CDttSP::setNBThreshold(float threshold)
{
	m_rx->setNBThreshold(threshold);
}

void CDttSP::setNBSDROMFlag(bool flag)
{
	m_rx->setNBSDROMFlag(flag);
}

void CDttSP::setNBSDROMThreshold(float threshold)
{
	m_rx->setNBSDROMThreshold(threshold);
}

void CDttSP::setBinauralFlag(bool flag)
{
	m_rx->setBinauralFlag(flag);
}

void CDttSP::setAGCMode(AGCMODE mode)
{
	m_rx->setAGCMode(mode);
}

void CDttSP::setALCAttack(float attack)
{
	m_tx->setALCAttack(attack);
}

void CDttSP::setCarrierLevel(float level)
{
	m_tx->setAMCarrierLevel(level);
}

void CDttSP::setALCDecay(float decay)
{
	m_tx->setALCDecay(decay);
}

void CDttSP::setALCGainBottom(float gain)
{
	m_tx->setALCGainBottom(gain);
}

void CDttSP::setALCHangTime(float hang)
{
	m_tx->setALCHangTime(hang);
}

void CDttSP::setRXCorrectIQ(float phase, float gain)
{
	m_rx->setIQ(0.001F * phase, 1.0F + 0.001F * gain);
}

void CDttSP::setTXCorrectIQ(float phase, float gain)
{
	m_tx->setIQ(0.001F * phase, 1.0F + 0.001F * gain);
}

void CDttSP::setSpectrumType(SPECTRUMtype type)
{
	switch (type) {
		case SPEC_AUDIO:
			m_rx->setSpectrumType(SPEC_RX_POST_DET);
			m_tx->setSpectrumType(SPEC_TX_POST_ALC);
			break;
		case SPEC_IQ:
			m_rx->setSpectrumType(SPEC_RX_PRE_FILT);
			m_tx->setSpectrumType(SPEC_TX_POST_FILT);
			break;
	}
}

void CDttSP::setSpectrumWindowType(Windowtype window)
{
	m_spectrum->setWindowType(window);
}

void CDttSP::setSpectrumPolyphaseFlag(bool flag)
{
	m_spectrum->setPolyphaseFlag(flag);
}

void CDttSP::setCompressionFlag(bool flag)
{
	m_tx->setCompressionFlag(flag);
}

void CDttSP::setCompressionLevel(float level)
{
	m_tx->setCompressionLevel(level);
}

void CDttSP::setTRX(TRXMODE trx)
{
	SEM_WAIT(m_update);

	switch (trx) {
		case TX:
			switch (m_tx->getMode()) {
				case CWU:
				case CWL:
					m_want = 0;
					break;
				default:
					m_want = int(2 * m_sampleRate / 48000);
					break;
			}
			break;

		case RX:
			m_want = int(1 * m_sampleRate / 48000);
			break;
	}

	m_trxNext = trx;
	m_have    = 0;

	if (m_state != RUN_SWITCH)
		m_stateLast = m_state;

	m_state = RUN_SWITCH;

	SEM_POST(m_update);
}

void CDttSP::setALCGainTop(float gain)
{
	m_tx->setALCGainTop(gain);
}

void CDttSP::getSpectrum(float *results)
{
	ASSERT(results != NULL);

	m_spectrum->setScale(SPEC_PWR);

	SEM_WAIT(m_update);
	m_spectrum->snapSpectrum();
	SEM_POST(m_update);

	m_spectrum->computeSpectrum(results);
}

void CDttSP::getPhase(float* results, unsigned int numpoints)
{
	ASSERT(results != NULL);

	m_spectrum->setScale(SPEC_PWR);

	SEM_WAIT(m_update);
	m_spectrum->snapScope();
	SEM_POST(m_update);

	m_spectrum->computeScopeComplex(results, numpoints);
}

void CDttSP::getScope(float* results, unsigned int numpoints)
{
	ASSERT(results != NULL);

	m_spectrum->setScale(SPEC_PWR);

	SEM_WAIT(m_update);
	m_spectrum->snapScope();
	SEM_POST(m_update);

	m_spectrum->computeScopeReal(results, numpoints);
}

float CDttSP::getMeter(METERTYPE mt)
{
	float returnval = -200.0F;

	SEM_WAIT(m_update);

	switch (m_trx) {
		case RX:
			switch (mt) {
				case SIGNAL_STRENGTH:
					returnval = m_meter->getRXMeter(RX_SIGNAL_STRENGTH);
					break;
				case AVG_SIGNAL_STRENGTH:
					returnval = m_meter->getRXMeter(RX_AVG_SIGNAL_STRENGTH);
					break;
				case ADC_REAL:
					returnval = m_meter->getRXMeter(RX_ADC_REAL);
					break;
				case ADC_IMAG:
					returnval = m_meter->getRXMeter(RX_ADC_IMAG);
					break;
				case AGC_GAIN:
					returnval = m_meter->getRXMeter(RX_AGC_GAIN);
					break;
				default:
					returnval = -200.0F;
					break;
			}
			break;

		case TX:
			switch (mt) {
				case MIC:
					returnval = m_meter->getTXMeter(TX_MIC);
					break;
				case PWR:
					returnval = m_meter->getTXMeter(TX_PWR);
					break;
				case ALC:
					returnval = m_meter->getTXMeter(TX_ALC);
					break;
				case EQtap:
					returnval = m_meter->getTXMeter(TX_EQtap);
					break;
				case LEVELER:
					returnval = m_meter->getTXMeter(TX_LEVELER);
					break;
				case COMP:
					returnval = m_meter->getTXMeter(TX_COMP);
					break;
				case ALC_G:
					returnval = m_meter->getTXMeter(TX_ALC_G);
					break;
				case LVL_G:
					returnval = m_meter->getTXMeter(TX_LVL_G);
					break;
				default:
					returnval = -200.0F;
					break;
			}
			break;
    }

	SEM_POST(m_update);

	return returnval;
}

void CDttSP::setDeviation(float value)
{
	m_tx->setFMDeviation(value);
	m_rx->setFMDeviation(value);
}

void CDttSP::ringBufferReset()
{
	SEM_WAIT(m_update);

	m_inputI->clear();
	m_inputQ->clear();
	m_outputI->clear();
	m_outputQ->clear();

	SEM_POST(m_update);
}

// [pos]  0.0 <= pos <= 1.0
void CDttSP::setRXPan(float pos)
{
	if (pos < 0.0F || pos > 1.0F)
		return;

	m_rx->setAzim(pos);
}

float CDttSP::getTXOffset() const
{
	return m_tx->getOffset();
}

float CDttSP::getRXOffset() const
{
	return m_rx->getOffset();
}

void CDttSP::audioEntry(float* input_i, float* input_q, float* output_i, float* output_q, unsigned int nframes)
{
	ASSERT(input_i != NULL);
	ASSERT(input_q != NULL);
	ASSERT(output_i != NULL);
	ASSERT(output_q != NULL);

	if (m_suspend) {
		::memset(output_i, 0x00, nframes * sizeof(float));
		::memset(output_q, 0x00, nframes * sizeof(float));
		return;
	}

	if (m_outputI->dataSpace() >= nframes && m_outputQ->dataSpace() >= nframes) {
		m_outputI->getData(output_i, nframes);
		m_outputQ->getData(output_q, nframes);
	} else {
		m_inputI->clear();
		m_inputQ->clear();
		m_outputI->clear();
		m_outputQ->clear();

		::memset(output_i, 0x00, nframes * sizeof(float));
		::memset(output_q, 0x00, nframes * sizeof(float));

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
		wxLogError(wxT("Not enough data in the output ring buffer"));
#elif defined(WIN32)
		// No WIN32 logging yet
#else
		::syslog(LOG_ERR, "Not enough data in the output ring buffer");
#endif
	}

	// input: copy from port to ring
	if (m_inputI->freeSpace() >= nframes && m_inputQ->freeSpace() >= nframes) {
		m_inputI->addData(input_i, nframes);
		m_inputQ->addData(input_q, nframes);
	} else {
		m_inputI->clear();
		m_inputQ->clear();
		m_outputI->clear();
		m_outputQ->clear();

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
		wxLogError(wxT("Not enough space in the input ring buffer"));
#elif defined(WIN32)
		// No WIN32 logging yet
#else
		::syslog(LOG_ERR, "Not enough space in the input ring buffer");
#endif
	}

	// if enough accumulated in ring, fire dsp
	if (m_inputI->dataSpace() >= m_frames && m_inputQ->dataSpace() >= m_frames)
		SEM_POST(m_buffer);
}

void CDttSP::audioEntry(float* input, float* output, unsigned int nframes)
{
	ASSERT(input != NULL);
	ASSERT(output != NULL);

	if (m_suspend) {
		::memset(output, 0x00, 2 * nframes * sizeof(float));
		return;
	}

	if (m_outputI->dataSpace() >= nframes && m_outputQ->dataSpace() >= nframes) {
		unsigned int i, j;
		/* The following code is broken up in this manner to minimize
		   cache hits */
		for (i = 0, j = 0; i < nframes; i++, j += 2)
			m_outputI->getData(&output[j], 1);
		for (i = 0, j = 1; i < nframes; i++, j += 2)
			m_outputQ->getData(&output[j], 1);
	} else {
		m_inputI->clear();
		m_inputQ->clear();
		m_outputI->clear();
		m_outputQ->clear();

		::memset(output, 0x00, 2 * nframes * sizeof(float));

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
		wxLogError(wxT("Not enough data in the output ring buffer"));
#elif defined(WIN32)
		// No WIN32 logging yet
#else
		::syslog(LOG_ERR, "Not enough data in the output ring buffer");
#endif
	}

	// input: copy from port to ring
	if (m_inputI->freeSpace() >= nframes && m_inputQ->freeSpace() >= nframes) {
		unsigned int i, j;
		/* The following code is broken up in this manner to minimize
		   cache hits */
		for (i = 0, j = 0; i < nframes; i++, j += 2)
			m_inputI->addData(&input[j], 1);
		for (i = 0, j = 1; i < nframes; i++, j += 2)
			m_inputQ->addData(&input[j], 1);
	} else {
		m_inputI->clear();
		m_inputQ->clear();
		m_outputI->clear();
		m_outputQ->clear();

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
		wxLogError(wxT("Not enough space in the input ring buffer"));
#elif defined(WIN32)
		// No WIN32 logging yet
#else
		::syslog(LOG_ERR, "Not enough space in the input ring buffer");
#endif
	}

	// if enough accumulated in ring, fire dsp
	if (m_inputI->dataSpace() >= m_frames && m_inputQ->dataSpace() >= m_frames)
		SEM_POST(m_buffer);
}

void CDttSP::getHold()
{
	if (m_outputI->freeSpace() < m_frames) {
		// pathology
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
		wxLogError(wxT("Not enough space in the output ring buffer"));
#elif defined(WIN32)
		// No WIN32 logging yet
#else
		::syslog(LOG_ERR, "Not enough space in the output ring buffer");
#endif
	} else {
		m_outputI->addData(m_bufferI, m_frames);
		m_outputQ->addData(m_bufferQ, m_frames);
	}

	if (m_inputI->dataSpace() < m_frames) {
		// pathology
		::memset(m_bufferI, 0x00, m_frames * sizeof(float));
		::memset(m_bufferQ, 0x00, m_frames * sizeof(float));

#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
		wxLogError(wxT("Not enough data in the output ring buffer"));
#elif defined(WIN32)
		// No WIN32 logging yet
#else
		::syslog(LOG_ERR, "Not enough data in the output ring buffer");
#endif
	} else {
		m_inputI->getData(m_bufferI, m_frames);
		m_inputQ->getData(m_bufferQ, m_frames);
    }
}

bool CDttSP::canHold()
{
	return m_inputI->dataSpace() >= m_frames && m_inputQ->dataSpace() >= m_frames;
}

void CDttSP::process()
{
	while (m_running) {
		SEM_WAIT(m_buffer);

		while (canHold()) {
			getHold();

			SEM_WAIT(m_update);

			switch (m_state) {
				case RUN_MUTE:
					runMute();
					break;

				case RUN_PASS:
					runPass();
					break;

				case RUN_PLAY:
					runPlay();
					break;

				case RUN_SWITCH:
					runSwitch();
					break;
			}

			SEM_POST(m_update);
		}
	}
}

void CDttSP::runMute()
{
	::memset(m_bufferI, 0x00, m_frames * sizeof(float));
	::memset(m_bufferQ, 0x00, m_frames * sizeof(float));

	m_tick++;
}

void CDttSP::runPass()
{
	m_tick++;
}

void CDttSP::runPlay()
{
	processSamples(m_bufferI, m_bufferQ, m_frames);
}

void CDttSP::runSwitch()
{
	if (m_have == 0) {
		// first time
		// apply ramp down
		for (unsigned int i = 0; i < m_fade; i++) {
			float w = 1.0F - float(i) / float(m_fade);

			m_bufferI[i] *= w;
			m_bufferQ[i] *= w;
		}

		::memset(m_bufferI + m_fade, 0x00, m_tail);
		::memset(m_bufferQ + m_fade, 0x00, m_tail);

		m_have++;
	} else if (m_have < m_want) {
		// in medias res
		::memset(m_bufferI, 0x00, m_frames * sizeof(float));
		::memset(m_bufferQ, 0x00, m_frames * sizeof(float));

		m_have++;
	} else {
		// last time
		// apply ramp up
		for (unsigned int i = 0; i < m_fade; i++) {
			float w = float(i) / float(m_fade);

			m_bufferI[i] *= w;
			m_bufferQ[i] *= w;
		}

		m_trx   = m_trxNext;
		m_state = m_stateLast;

		m_want = 0;
		m_have = 0;
	}

	processSamples(m_bufferI, m_bufferQ, m_frames);
}

void CDttSP::processSamples(float* bufi, float* bufq, unsigned int n)
{
	unsigned int i;

	switch (m_trx) {
		case RX: {
				CXB* iBuf = m_rx->getIBuf();
				CXB* oBuf = m_rx->getOBuf();

				for (i = 0; i < n; i++) {
					CXBreal(iBuf, i) = bufi[i];
					CXBimag(iBuf, i) = bufq[i];
				}
				CXBhave(iBuf) = n;

				m_rx->process();

				n = CXBhave(oBuf);
				for (i = 0; i < n; i++) {
					bufi[i] = CXBreal(oBuf, i);
					bufq[i] = CXBimag(oBuf, i);
				}
			}
			break;

		case TX: {
				CXB* iBuf = m_tx->getIBuf();
				CXB* oBuf = m_tx->getOBuf();

				for (i = 0; i < n; i++) {
					CXBreal(iBuf, i) = bufi[i];
					CXBimag(iBuf, i) = bufq[i];
				}
				CXBhave(iBuf) = n;

				m_tx->process();

				n = CXBhave(oBuf);
				for (i = 0; i < n; i++) {
					bufi[i] = CXBreal(oBuf, i);
					bufq[i] = CXBimag(oBuf, i);
				}
			}
			break;
	}

	m_tick++;
}
