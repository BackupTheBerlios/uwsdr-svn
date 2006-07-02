/* update.cpp

common defs and code for parm update 
   
This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006-5 by Frank Brickle, AB2KT and Bob McGwier, N4HY

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

#include "sdrexport.h"
#include "banal.h"
#include "RingBuffer.h"

#include <wx/wx.h>


////////////////////////////////////////////////////////////////////////////

void Setup_SDR(float sampleRate, unsigned int audioSize)
{
	extern void setup(float samplerate, unsigned int audioSize);
	setup(sampleRate, audioSize);
}

void Destroy_SDR()
{
	extern void closeup();
	closeup();
}

void SetMode(SDRMODE m)
{
	top.sync.upd.sem->Wait();

	tx->setMode(m);
	rx.mode = uni.mode.sdr = m;

	if (rx.mode == AM)
		rx.am->setMode(AMdet);
	if (rx.mode == SAM)
		rx.am->setMode(SAMdet);

	top.sync.upd.sem->Post();
}

void SetDCBlock(bool setit)
{
	tx->setDCBlockFlag(setit);
}

void SetFilter(double low_frequency, double high_frequency, int taps, TRXMODE trx)
{
	top.sync.upd.sem->Wait();

	switch (trx) {
		case TX:
			tx->setFilter(low_frequency, high_frequency);
			break;
		case RX:
			rx.filt->setFilter(low_frequency, high_frequency);
			rx.fm->setBandwidth(low_frequency, high_frequency);
			break;
	}

	top.sync.upd.sem->Post();
}

void Release_Update()
{
	top.sync.upd.sem->Post();
}

void SetOsc(double newfreq)
{
	if (::fabs(newfreq) >= 0.5 * uni.samplerate)
		return;

	rx.osc.gen->setFrequency(newfreq);
}

void SetRIT(double newfreq)
{
	rx.rit.gen->setFrequency(newfreq);
}

void SetTXOsc(double newfreq)
{
	if (::fabs(newfreq) >= 0.5 * uni.samplerate)
		return;

	tx->setFrequency(newfreq);
}

void SetNR(bool setit)
{
	rx.anr.flag = setit;
}

void SetBlkNR(bool setit)
{
	rx.banr.flag = setit;
}

void SetNRvals(unsigned int taps, unsigned int delay, double gain, double leak)
{
	rx.anr.gen->setAdaptiveFilterSize(taps);
	rx.anr.gen->setDelay(delay);
	rx.anr.gen->setAdaptationRate(gain);
	rx.anr.gen->setLeakage(leak);

	rx.banr.gen->setAdaptationRate(min(0.1F * gain, 0.0002F));
}

void SetTXCompandSt(bool setit)
{
	tx->setCompandFlag(setit);
}

void SetTXCompand(double setit)
{
	tx->setCompandFactor(-setit);
}

void SetTXSquelchSt(bool setit)
{
	tx->setSquelchFlag(setit);
}

void SetTXSquelchVal(float setit)
{
	tx->setSquelchThreshold(setit);
}

void SetANF(bool setit)
{
	rx.anf.flag = setit;
}

void SetBlkANF(bool setit)
{
	rx.banf.flag = setit;
}

void SetANFvals(unsigned int taps, unsigned int delay, double gain, double leak)
{
	rx.anf.gen->setAdaptiveFilterSize(taps);
	rx.anf.gen->setDelay(delay);
	rx.anf.gen->setAdaptationRate(gain);
	rx.anf.gen->setLeakage(leak);

	rx.banf.gen->setAdaptationRate(min(0.1F * gain, 0.0002F));
}

void SetNB(bool setit)
{
	rx.nb.flag = setit;
}

void SetNBvals(float threshold)
{
	rx.nb.gen->setThreshold(threshold);
}

void SetSDROM(bool setit)
{
	rx.nb_sdrom.flag = setit;
}

void SetSDROMvals(float threshold)
{
	rx.nb_sdrom.gen->setThreshold(threshold);
}

void SetBIN(bool setit)
{
	rx.bin.flag = setit;
}

void SetRXAGC(AGCMODE setit)
{
	rx.agc.gen->setMode(setit);
	rx.agc.flag = true;				// FIXME ??????
}

void SetTXALCAttack(float attack)
{
	tx->setALCAttack(attack);
}

void SetTXCarrierLevel(double setit)
{
	tx->setAMCarrierLevel(setit);
}

void SetTXALCDecay(float decay)
{
	tx->setALCDecay(decay);
}

void SetTXALCBot(float bot)
{
	tx->setALCGainBottom(bot);
}

void SetTXALCHang(float hang)
{
	tx->setALCHangTime(hang);
}

void SetTXLevelerSt(bool state)
{
	tx->setLevelerFlag(state);
}

void SetTXLevelerAttack(float attack)
{
	tx->setLevelerAttack(attack);
}

void SetTXLevelerDecay(float decay)
{
	tx->setLevelerDecay(decay);
}

void SetTXLevelerTop(float top)
{
	tx->setLevelerGainTop(top);
}

void SetTXLevelerHang(float hang)
{
	tx->setLevelerHangTime(hang);
}

void SetCorrectIQ(double phase, double gain)
{
	rx.iqfix->setPhase(float(0.001F * phase));
	rx.iqfix->setGain(float(1.0F + 0.001F * gain));
}

void SetCorrectTXIQ(double phase, double gain)
{
	tx->setIQPhase(float(0.001F * phase));
	tx->setIQGain(float(1.0F + 0.001F * gain));
}

void SetPWSmode(SPECTRUMtype type)
{
	uni.spec.type = type;
}

void SetWindow(Windowtype window)
{
	uni.spec.gen->setWindow(window);
}

void SetSpectrumPolyphase(bool setit)
{
	uni.spec.gen->setPolyphase(setit);
}

void SetGrphTXEQ(int *txeq)
{
	tx->setGraphicEQValues(float(txeq[0]), float(txeq[1]), float(txeq[2]), float(txeq[3]));
}

void SetGrphTXEQcmd(bool state)
{
	tx->setGraphicEQFlag(state);
}

void SetNotch160(bool state)
{
	tx->setNotchFlag(state);
}

void SetGrphRXEQ(int *rxeq)
{
	rx.grapheq.gen->setEQ(float(rxeq[0]), float(rxeq[1]), float(rxeq[2]), float(rxeq[3]));
}

void SetGrphRXEQcmd(bool state)
{
	rx.grapheq.flag = state;
}

void SetTXCompressionSt(bool setit)
{
	tx->setCompressionFlag(setit);
}

void SetTXCompressionLevel(float txc)
{
	tx->setCompressionLevel(txc);
}

void SetSquelchVal(float setit)
{
	rx.squelch.gen->setThreshold(setit);
}

void SetSquelchState(bool setit)
{
	rx.squelch.gen->setFlag(setit);
}

void SetTRX(TRXMODE setit)
{
	top.sync.upd.sem->Wait();

	switch (setit) {
		case TX:
			switch (tx->getMode()) {
				case CWU:
				case CWL:
					top.swch.bfct.want = 0;
					break;
				default:
					top.swch.bfct.want = int(2 * uni.samplerate / 48000);
					break;
			}

		case RX:
			top.swch.bfct.want = int(1 * uni.samplerate / 48000);
			break;
	}

	top.swch.trx.next = setit;
	top.swch.bfct.have = 0;

	if (top.state != RUN_SWCH)
		top.swch.run.last = top.state;

	top.state = RUN_SWCH;

	top.sync.upd.sem->Post();
}

void SetTXALCLimit(float limit)
{
	tx->setALCGainTop(limit);
}

void setSpotToneVals(float gain, float freq, float rise, float fall)
{
	rx.spot.gen->setValues(gain, freq, rise, fall);
}

void setSpotTone(bool setit)
{
	if (setit) {
		rx.spot.gen->on();
		rx.spot.flag = true;
	} else {
		rx.spot.gen->off();
		rx.spot.flag = false;
	}
}

void Process_Spectrum(float *results)
{
	wxASSERT(results != NULL);

	uni.spec.type = SPEC_POST_FILT;
	uni.spec.gen->setScale(SPEC_PWR);

	top.sync.upd.sem->Wait();
	uni.spec.gen->snapSpectrum();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeSpectrum(results);
}

void Process_Panadapter(float *results)
{
	wxASSERT(results != NULL);

	uni.spec.type = SPEC_PRE_FILT;
	uni.spec.gen->setScale(SPEC_PWR);

	top.sync.upd.sem->Wait();
	uni.spec.gen->snapSpectrum();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeSpectrum(results);
}

void Process_Phase(float *results, unsigned int numpoints)
{
	wxASSERT(results != NULL);

	top.sync.upd.sem->Wait();
	uni.spec.type = SPEC_POST_AGC;
	uni.spec.gen->setScale(SPEC_PWR);
	uni.spec.gen->snapScope();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeScopeComplex(results, numpoints);
}

void Process_Scope(float *results, unsigned int numpoints)
{
	wxASSERT(results != NULL);

	top.sync.upd.sem->Wait();
	uni.spec.type = SPEC_POST_AGC;
	uni.spec.gen->setScale(SPEC_PWR);
	uni.spec.gen->snapScope();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeScopeReal(results, numpoints);
}

float Calculate_Meters(METERTYPE mt)
{
	float returnval = -200.0F;

	top.sync.upd.sem->Wait();

	if (uni.mode.trx == RX) {
		switch (mt) {
			case SIGNAL_STRENGTH:
				returnval = uni.meter.gen->getRXMeter(RX_SIGNAL_STRENGTH);
				break;
			case AVG_SIGNAL_STRENGTH:
				returnval = uni.meter.gen->getRXMeter(RX_AVG_SIGNAL_STRENGTH);
				break;
			case ADC_REAL:
				returnval = uni.meter.gen->getRXMeter(RX_ADC_REAL);
				break;
			case ADC_IMAG:
				returnval = uni.meter.gen->getRXMeter(RX_ADC_IMAG);
				break;
			case AGC_GAIN:
				returnval = uni.meter.gen->getRXMeter(RX_AGC_GAIN);
				break;
			default:
				returnval = -200.0F;
				break;
		}
    } else {
		switch(mt) {
			case MIC:
				returnval = uni.meter.gen->getTXMeter(TX_MIC);
				break;
			case PWR:
				returnval = uni.meter.gen->getTXMeter(TX_PWR);
				break;
			case ALC:
				returnval = uni.meter.gen->getTXMeter(TX_ALC);
				break;
			case EQtap:
				returnval = uni.meter.gen->getTXMeter(TX_EQtap);
				break;
			case LEVELER:
				returnval = uni.meter.gen->getTXMeter(TX_LEVELER);
				break;
			case COMP:
				returnval = uni.meter.gen->getTXMeter(TX_COMP);
				break;
			case CPDR:
				returnval = uni.meter.gen->getTXMeter(TX_CPDR);
				break;
			case ALC_G:
				returnval = uni.meter.gen->getTXMeter(TX_ALC_G);
				break;
			case LVL_G:
				returnval = uni.meter.gen->getTXMeter(TX_LVL_G);
				break;
			default:
				returnval = -200.0F;
				break;
		}
    }

	top.sync.upd.sem->Post();

	return returnval;
}

void SetDeviation(float value)
{
	tx->setFMDeviation(value);
	rx.fm->setDeviation(value);
}

void RingBufferReset()
{
	top.sync.upd.sem->Wait();

	top.jack.ring.i.i->clear();
	top.jack.ring.i.q->clear();
	top.jack.ring.o.i->clear();
	top.jack.ring.o.q->clear();

	top.sync.upd.sem->Post();
}

// [pos]  0.0 <= pos <= 1.0
void SetRXPan(float pos)
{
	if (pos < 0.0F || pos > 1.0F)
		return;

	float theta = float((1.0 - pos) * M_PI / 2.0);

	rx.azim = Cmplx((float)::cos(theta), (float)::sin(theta));
}
