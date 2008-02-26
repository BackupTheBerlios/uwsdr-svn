/* update.cpp
   
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


static CDttSP* dttsp = NULL;

////////////////////////////////////////////////////////////////////////////

void Setup_SDR(float sampleRate, unsigned int audioSize)
{
	dttsp = new CDttSP(sampleRate, audioSize);
}

void Destroy_SDR()
{
	delete dttsp;
}

void SetMode(SDRMODE m)
{
	dttsp->setMode(m);
}

void SetWeaver(bool setit)
{
	dttsp->setWeaver(setit);
}

void SetDCBlock(bool setit)
{
	dttsp->setDCBlockFlag(setit);
}

void SetFilter(double low_frequency, double high_frequency, int WXUNUSED(taps), TRXMODE trx)
{
	switch (trx) {
		case TX:
			dttsp->setTXFilter(low_frequency, high_frequency);
			break;
		case RX:
			dttsp->setRXFilter(low_frequency, high_frequency);
			break;
	}
}

void Release_Update()
{
	dttsp->releaseUpdate();
}

void SetOsc(double freq)
{
	dttsp->setRXFrequency(freq);
}

void SetTXOsc(double freq)
{
	dttsp->setTXFrequency(freq);
}

void SetNR(bool setit)
{
	dttsp->setANRFlag(setit);
}

void SetBlkNR(bool setit)
{
	dttsp->setBANRFlag(setit);
}

void SetNRvals(unsigned int taps, unsigned int delay, double gain, double leak)
{
	dttsp->setANRValues(taps, delay, float(gain), float(leak));
}

void SetANF(bool setit)
{
	dttsp->setANFFlag(setit);
}

void SetBlkANF(bool setit)
{
	dttsp->setBANFFlag(setit);
}

void SetANFvals(unsigned int taps, unsigned int delay, double gain, double leak)
{
	dttsp->setANFValues(taps, delay, float(gain), float(leak));
}

void SetNB(bool setit)
{
	dttsp->setNBFlag(setit);
}

void SetNBvals(float threshold)
{
	dttsp->setNBThreshold(threshold);
}

void SetSDROM(bool setit)
{
	dttsp->setNBSDROMFlag(setit);
}

void SetSDROMvals(float threshold)
{
	dttsp->setNBSDROMThreshold(threshold);
}

void SetBIN(bool setit)
{
	dttsp->setBinauralFlag(setit);
}

void SetRXAGC(AGCMODE setit)
{
	dttsp->setAGCMode(setit);
}

void SetTXALCAttack(float attack)
{
	dttsp->setALCAttack(attack);
}

void SetTXCarrierLevel(float setit)
{
	dttsp->setCarrierLevel(setit);
}

void SetTXALCDecay(float decay)
{
	dttsp->setALCDecay(decay);
}

void SetTXALCBot(float bot)
{
	dttsp->setALCGainBottom(bot);
}

void SetTXALCHang(float hang)
{
	dttsp->setALCHangTime(hang);
}

void SetCorrectIQ(double phase, double gain)
{
	dttsp->setRXCorrectIQ(float(0.001F * phase), float(1.0F + 0.001F * gain));
}

void SetCorrectTXIQ(double phase, double gain)
{
	dttsp->setTXCorrectIQ(float(0.001F * phase), float(1.0F + 0.001F * gain));
}

void SetPWSmode(SPECTRUMtype type)
{
	dttsp->setSpectrumType(type);
}

void SetWindow(Windowtype window)
{
	dttsp->setSpectrumWindowType(window);
}

void SetSpectrumPolyphase(bool setit)
{
	dttsp->setSpectrumPolyphaseFlag(setit);
}

void SetTXCompressionSt(bool setit)
{
	dttsp->setCompressionFlag(setit);
}

void SetTXCompressionLevel(float txc)
{
	dttsp->setCompressionLevel(txc);
}

void SetSquelchVal(float setit)
{
	dttsp->setRXSquelchThreshold(setit);
}

void SetSquelchState(bool setit)
{
	dttsp->setRXSquelchFlag(setit);
}

void SetTRX(TRXMODE setit)
{
	dttsp->setTRX(setit);
}

void SetTXALCLimit(float limit)
{
	dttsp->setALCGainTop(limit);
}

void Process_Spectrum(float *results)
{
	dttsp->setSpectrumType(SPEC_POST_FILT);
	dttsp->getSpectrum(results);
}

void Process_Panadapter(float *results)
{
	dttsp->setSpectrumType(SPEC_PRE_FILT);
	dttsp->getSpectrum(results);
}

void Process_Phase(float *results, unsigned int numpoints)
{
	dttsp->setSpectrumType(SPEC_POST_AGC);
	dttsp->getPhase(results, numpoints);
}

void Process_Scope(float *results, unsigned int numpoints)
{
	dttsp->setSpectrumType(SPEC_POST_AGC);
	dttsp->getScope(results, numpoints);
}

float Calculate_Meters(METERTYPE mt)
{
	return dttsp->getMeter(mt);
}

void SetDeviation(float value)
{
	dttsp->setDeviation(value);
}

void RingBufferReset()
{
	dttsp->ringBufferReset();
}

// [pos]  0.0 <= pos <= 1.0
void SetRXPan(float pos)
{
	dttsp->setRXPan(pos);
}

float GetTXOffset()
{
	return dttsp->getTXOffset();
}

float GetRXOffset()
{
	return dttsp->getRXOffset();
}

void  Audio_Callback(float* input_i, float* input_q, float* output_i, float* output_q, unsigned int nframes)
{
	dttsp->audioEntry(input_i, input_q, output_i, output_q, nframes);
}

void  Audio_CallbackIL(float* input, float* output, unsigned int nframes)
{
	dttsp->audioEntry(input, output, nframes);
}

void process_samples_thread()
{
	dttsp->process();
}
