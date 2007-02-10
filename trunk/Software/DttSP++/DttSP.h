/* DttSP.h

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY
Copyright (C) 2006-2007 by Jonathan Naylor, G4KLX

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
	~CDttSP();

	void  releaseUpdate();
	void  ringBufferReset();

	void  setMode(SDRMODE m);
	void  setZeroIF(bool flag);
	void  setDCBlockFlag(bool flag);
	void  setRXFilter(double lowFreq, double highFreq);
	void  setTXFilter(double lowFreq, double highFreq);
	void  setRXFrequency(double freq);
	void  setTXFrequency(double freq);
	void  setANRFlag(bool flag);
	void  setBANRFlag(bool flag);
	void  setANRValues(unsigned int taps, unsigned int delay, float gain, float leak);
	void  setRXSquelchFlag(bool flag);
	void  setRXSquelchThreshold(float threshold);
	void  setANFFlag(bool flag);
	void  setBANFFlag(bool flag);
	void  setANFValues(unsigned int taps, unsigned int delay, float gain, float leak);
	void  setNBFlag(bool flag);
	void  setNBThreshold(float threshold);
	void  setNBSDROMFlag(bool flag);
	void  setNBSDROMThreshold(float threshold);
	void  setBinauralFlag(bool flag);
	void  setAGCMode(AGCMODE mode);
	void  setALCAttack(float attack);
	void  setCarrierLevel(float level);
	void  setALCDecay(float decay);
	void  setALCGainBottom(float gain);
	void  setALCHangTime(float hang);
	void  setRXCorrectIQ(float phase, float gain);
	void  setTXCorrectIQ(float phase, float gain);
	void  setSpectrumType(SPECTRUMtype type);
	void  setSpectrumWindowType(Windowtype window);
	void  setSpectrumPolyphaseFlag(bool flag);
	void  setCompressionFlag(bool flag);
	void  setCompressionLevel(float level);
	void  setTRX(TRXMODE trx);
	void  setALCGainTop(float gain);
	void  getSpectrum(float *results);
	void  getPhase(float* results, unsigned int numpoints);
	void  getScope(float* results, unsigned int numpoints);
	float getMeter(METERTYPE mt);
	void  setDeviation(float value);
	void  setRXPan(float pos);

	float getTXOffset() const;
	float getRXOffset() const;

	void  audioEntry(float* input_i, float* input_q, float* output_i, float* output_q, unsigned int nframes);
	void  audioEntry(float* input, float* output, unsigned int nframes);

	void  process();


    protected:
	void runMute();
	void runPass();
	void runPlay();
	void runSwitch();
	void processSamples(float* bufi, float* bufq, unsigned int n);
	void getHold();
	bool canHold();


    private:
	float        m_sampleRate;
	bool         m_running;
	bool         m_suspend;
#if defined(__WXMSW__) || defined(__WXGTK__) || defined(__WXMAC__)
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
