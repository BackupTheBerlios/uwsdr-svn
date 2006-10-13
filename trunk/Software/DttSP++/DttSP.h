/* DttSP.h

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

#ifndef _dttsp_h
#define _dttsp_h

#include "RX.h"
#include "TX.h"
#include "Meter.h"
#include "Spectrum.h"
#include "RingBuffer.h"

typedef enum {
   RUN_MUTE,
   RUN_PASS,
   RUN_PLAY,
   RUN_SWITCH
} RUNMODE;



class CDttSP {
    public:
	CDttSP(float sampleRate, unsigned int audioSize);
	virtual ~CDttSP();

	virtual void  releaseUpdate();
	virtual void  ringBufferReset();

	virtual void  setMode(SDRMODE m);
	virtual void  setDCBlockFlag(bool flag);
	virtual void  setRXFilter(double lowFreq, double highFreq);
	virtual void  setTXFilter(double lowFreq, double highFreq);
	virtual void  setRXFrequency(double freq);
	virtual void  setRITFrequency(double freq);
	virtual void  setTXFrequency(double freq);
	virtual void  setANRFlag(bool flag);
	virtual void  setBANRFlag(bool flag);
	virtual void  setANRValues(unsigned int taps, unsigned int delay, float gain, float leak);
	virtual void  setRXSquelchFlag(bool flag);
	virtual void  setRXSquelchThreshold(float threshold);
	virtual void  setTXSquelchFlag(bool flag);
	virtual void  setTXSquelchThreshold(float threshold);
	virtual void  setANFFlag(bool flag);
	virtual void  setBANFFlag(bool flag);
	virtual void  setANFValues(unsigned int taps, unsigned int delay, float gain, float leak);
	virtual void  setNBFlag(bool flag);
	virtual void  setNBThreshold(float threshold);
	virtual void  setNBSDROMFlag(bool flag);
	virtual void  setNBSDROMThreshold(float threshold);
	virtual void  setBinauralFlag(bool flag);
	virtual void  setAGCMode(AGCMODE mode);
	virtual void  setALCAttack(float attack);
	virtual void  setCarrierLevel(float level);
	virtual void  setALCDecay(float decay);
	virtual void  setALCGainBottom(float gain);
	virtual void  setALCHangTime(float hang);
	virtual void  setLevelerFlag(bool state);
	virtual void  setLevelerAttack(float attack);
	virtual void  setLevelerDecay(float decay);
	virtual void  setLevelerGainTop(float gain);
	virtual void  setLevelerHangTime(float hang);
	virtual void  setRXCorrectIQ(float phase, float gain);
	virtual void  setTXCorrectIQ(float phase, float gain);
	virtual void  setSpectrumType(SPECTRUMtype type);
	virtual void  setSpectrumWindowType(Windowtype window);
	virtual void  setSpectrumPolyphaseFlag(bool flag);
	virtual void  setCompressionFlag(bool flag);
	virtual void  setCompressionLevel(float level);
	virtual void  setTRX(TRXMODE trx);
	virtual void  setALCGainTop(float gain);
	virtual void  setSpotToneFlag(bool flag);
	virtual void  setSpotToneValues(float gain, float freq, float rise, float fall);
	virtual void  getSpectrum(float *results);
	virtual void  getPhase(float* results, unsigned int numpoints);
	virtual void  getScope(float* results, unsigned int numpoints);
	virtual float getMeter(METERTYPE mt);
	virtual void  setDeviation(float value);
	virtual void  setRXPan(float pos);

	virtual void  audioEntry(float* input_i, float* input_q, float* output_i, float* output_q, unsigned int nframes);
	virtual void  audioEntry(float* input, float* output, unsigned int nframes);

	virtual void  process();


    protected:
	virtual void runMute();
	virtual void runPass();
	virtual void runPlay();
	virtual void runSwitch();
	virtual void processSamples(float* bufi, float* bufq, unsigned int n);
	virtual void getHold();
	virtual bool canHold();


    private:
	float        m_sampleRate;
	bool         m_running;
	bool         m_suspend;
#if defined(__WXMSW__) || defined(__WXGTK__)
	wxSemaphore* m_update;
	wxSemaphore* m_buffer;
#elif defined(WIN32)
	HANDLE		 m_update;
	HANDLE		 m_buffer;
#else
	sem_t		 m_update;
	sem_t		 m_buffer;
#endif
	CRX*         m_rx;
	CTX*         m_tx;
	CMeter*      m_meter;
	CSpectrum*   m_spectrum;
	CRingBuffer* m_inputI;
	CRingBuffer* m_inputQ;
	CRingBuffer* m_outputI;
	CRingBuffer* m_outputQ;
	float*       m_bufferI;
	float*       m_bufferQ;
	TRXMODE      m_trx;
	TRXMODE      m_trxNext;
	RUNMODE      m_state;
	RUNMODE      m_stateLast;
	unsigned int m_tick;
	unsigned int m_want;
	unsigned int m_have;
	unsigned int m_fade;
	unsigned int m_tail;
	unsigned int m_frames;
};

#endif
