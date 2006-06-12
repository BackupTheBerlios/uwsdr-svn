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
#include "thunk.h"
#include "ringb.h"

#include <wx/wx.h>


////////////////////////////////////////////////////////////////////////////

static REAL inline
dB2lin (REAL dB)
{
  return (REAL) pow (10.0, (REAL) dB / 20.0);
}

#ifdef notdef
static int
setTXALCAttack (int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  tx.alc.gen->attack = (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  tx.alc.gen->one_m_attack = (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  tx.alc.gen->sndx =
    (tx.alc.gen->indx +
     (int) (0.003 * uni.samplerate * tmp)) & tx.alc.gen->mask;
  tx.alc.gen->fastindx =
    (tx.alc.gen->sndx + FASTLEAD * tx.alc.gen->mask) & tx.alc.gen->mask;
  tx.alc.gen->fasthangtime = (REAL) 0.1;
  return 0;
}

static int
setTXALCDecay (int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  tx.alc.gen->decay = (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  tx.alc.gen->one_m_decay = (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  return 0;
}

static int
setTXALCHang (int n, char **p)
{
  REAL hang = (REAL) atof (p[0]);
  tx.alc.gen->hangtime = (REAL) 0.001 *hang;
  return 0;
}

static int
setTXALCBot (int n, char **p)
{
  REAL bot = (REAL) atof (p[0]);
  tx.alc.gen->gain.bottom = bot;
  return 0;
}

static int
setTXLevelerAttack (int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  tx.leveler.gen->attack =
    (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  tx.leveler.gen->one_m_attack =
    (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  tx.leveler.gen->sndx =
    (tx.leveler.gen->indx +
     (int) (0.003 * uni.samplerate * tmp)) & tx.leveler.gen->mask;
  tx.leveler.gen->fastindx =
    (tx.leveler.gen->sndx +
     FASTLEAD * tx.leveler.gen->mask) & tx.leveler.gen->mask;
  tx.leveler.gen->fasthangtime = (REAL) 0.1;	//wa6ahl: 100 ms
  return 0;
}

static int setTXLevelerDecay(int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  tx.leveler.gen->decay =
    (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  tx.leveler.gen->one_m_decay = (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  return 0;
}

static int setTXLevelerTop(int n, char **p)
{
	tx.leveler.gen->gain.top = REAL(::atof(p[0]));
	return 0;
}

static int setTXLevelerHang(int n, char **p)
{
  REAL hang = (REAL) atof (p[0]);
  tx.leveler.gen->hangtime = (REAL) 0.001 *hang;
  return 0;
}


static int
setTXAGCLimit (int n, char **p)
{
  REAL limit = (REAL) atof (p[0]);
  tx.alc.gen->gain.top = limit;
  return 0;
}
#endif

static int
setRingBufferReset (int n, char **p)
{
  ringb_float_reset (top.jack.ring.i.l);
  ringb_float_reset (top.jack.ring.i.r);
  ringb_float_reset (top.jack.auxr.i.l);
  ringb_float_reset (top.jack.auxr.i.r);
  ringb_float_restart (top.jack.ring.o.l, top.hold.size.frames);
  ringb_float_restart (top.jack.ring.o.r, top.hold.size.frames);
  ringb_float_restart (top.jack.auxr.o.l, top.hold.size.frames);
  ringb_float_restart (top.jack.auxr.o.r, top.hold.size.frames);
  return 0;
}

// [pos]  0.0 <= pos <= 1.0
static int
setRXPan (int n, char **p)
{
  REAL pos, theta;
  if (n < 1)
    {
      pos = 0.5;
      theta = (REAL) ((1.0 - pos) * M_PI / 2.0);
      rx.azim = Cmplx ((REAL) cos (theta), (REAL) sin (theta));
      return 0;
    }
  else
    {
      if ((pos = (REAL) atof (p[0])) < 0.0 || pos > 1.0)
	return -1;
      theta = (REAL) ((1.0 - pos) * M_PI / 2.0);
      rx.azim = Cmplx ((REAL) cos (theta), (REAL) sin (theta));
      return 0;
    }
}

static int
setAuxMixSt (int n, char **p)
{
  if (n < 1)
    {
      uni.mix.rx.flag = uni.mix.tx.flag = false;
      return 0;
    }
  else
    {
      bool flag = ::atoi(p[0]) ? true : false;
      if (n > 1)
	{
	  switch (atoi (p[1]))
	    {
	    case TX:
	      uni.mix.tx.flag = flag;
	      break;
	    case RX:
	    default:
	      uni.mix.rx.flag = flag;
	      break;
	    }
	}
      else
	uni.mix.rx.flag = uni.mix.tx.flag = flag;
      return 0;
    }
}

// [dB] NB both channels
static int
setAuxMixGain (int n, char **p)
{
  if (n < 1)
    {
      uni.mix.rx.gain = uni.mix.tx.gain = 1.0;
      return 0;
    }
  else
    {
      REAL gain = dB2lin ((REAL) atof (p[0]));
      if (n > 1)
	{
	  switch (atoi (p[1]))
	    {
	    case TX:
	      uni.mix.tx.gain = gain;
	      break;
	    case RX:
	    default:
	      uni.mix.rx.gain = gain;
	      break;
	    }
	}
      else
	uni.mix.rx.gain = uni.mix.tx.gain = gain;
      return 0;
    }
}

const CTE update_cmds[] = {
#ifdef notdef
  {"setTXALCAttack", setTXALCAttack}
  ,
  {"setTXALCDecay", setTXALCDecay}
  ,
  {"setTXALCBot", setTXALCBot}
  ,
  {"setTXALCHang", setTXALCHang}
  ,
  {"setTXAGCLimit", setTXAGCLimit}
  ,
  {"setTXLevelerAttack", setTXLevelerAttack}
  ,
  {"setTXLevelerDecay", setTXLevelerDecay}
  ,
  {"setTXLevelerHang", setTXLevelerHang}
  ,
  {"setTXLevelerTop", setTXLevelerTop}
  ,
  {"setfixedAGC", setfixedAGC}
  ,
#endif
  {"setRingBufferReset", setRingBufferReset}
  ,
  {"setRXPan", setRXPan}
  ,
  {"setAuxMixSt", setAuxMixSt}
  ,
  {"setAuxMixGain", setAuxMixGain}
  ,
  {NULL, (Thunk)NULL}
};

//........................................................................

int sendcommand(char *str)
{
	wxASSERT(top.sync.upd.sem != NULL);

	::wxLogMessage("Command = %s", str);

	int count = 0;
	char* args[20];

	char* p = ::strtok(str, " \t\n\r");
	while (p != NULL && count < 20) {
		args[count] = p;
		count++;

		p = ::strtok(NULL, " \t\n\r");
	}

	if (count < 1) {
		::wxLogError("Empty command");
		return -1;
	}

	Thunk thk = Thunk_lookup(update_cmds, args[0]);

	if (thk == NULL) {
		::wxLogError("Unknown command = %s", args[0]);
		return -1;
	}

	top.sync.upd.sem->Wait();
	int val = (*thk)(count - 1, args + 1);
	top.sync.upd.sem->Post();

	return val;
}

//------------------------------------------------------------------------

/*  Legacy DLL version of this code */

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
	wxASSERT(top.sync.upd.sem != NULL);
	wxASSERT(rx.am != NULL);

	top.sync.upd.sem->Wait();

	tx.mode = rx.mode = uni.mode.sdr = m;

	if (rx.mode == AM)
		rx.am->setMode(AMdet);
	if (rx.mode == SAM)
		rx.am->setMode(SAMdet);

	top.sync.upd.sem->Post();
}

void SetDCBlock(bool setit)
{
	tx.dcb.flag = setit;
}

void SetFilter(double low_frequency, double high_frequency, int taps, TRXMODE trx)
{
	wxASSERT(tx.filt != NULL);
	wxASSERT(rx.filt != NULL);
	wxASSERT(rx.fm != NULL);
	wxASSERT(top.sync.upd.sem != NULL);

	top.sync.upd.sem->Wait();

	switch (trx) {
		case TX:
			tx.filt->setFilter(low_frequency, high_frequency);
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
	wxASSERT(top.sync.upd.sem != NULL);

	top.sync.upd.sem->Post();
}

void SetOsc(double newfreq)
{
	wxASSERT(rx.osc.gen != NULL);

	if (::fabs(newfreq) >= 0.5 * uni.samplerate)
		return;

	rx.osc.gen->setFrequency(newfreq);
}

void SetTXOsc(double newfreq)
{
	wxASSERT(tx.osc.gen != NULL);

	if (::fabs(newfreq) >= 0.5 * uni.samplerate)
		return;

	tx.osc.gen->setFrequency(newfreq);
}

void SetNR(bool setit)
{
	rx.anr.flag = setit;
}

void SetBlkNR(bool setit)
{
	rx.banr.flag = setit;
}

void SetNRvals(int taps, int delay, double gain, double leak)
{
	rx.anr.gen->adaptive_filter_size = taps;
	rx.anr.gen->delay = delay;
	rx.anr.gen->adaptation_rate = gain;
	rx.anr.gen->leakage = leak;

	rx.banr.gen->adaptation_rate = min(0.1 * gain, 0.0002);
}

void SetTXCompandSt(bool setit)
{
	tx.cpd.flag = setit;
}

void SetTXCompand(double setit)
{
	wxASSERT(tx.cpd.gen != NULL);

	tx.cpd.gen->setFactor(-setit);
}

void SetTXSquelchSt(bool setit)
{
	tx.squelch.flag = setit;
}

void SetTXSquelchVal(float setit)
{
	tx.squelch.thresh = setit;
}

void SetANF(bool setit)
{
	rx.anf.flag = setit;
}

void SetBlkANF(bool setit)
{
	rx.banf.flag = setit;
}

void SetANFvals(int taps, int delay, double gain, double leak)
{
	rx.anf.gen->adaptive_filter_size = taps;
	rx.anf.gen->delay = delay;
	rx.anf.gen->adaptation_rate = gain;
	rx.anf.gen->leakage = leak;

	rx.banf.gen->adaptation_rate = min(0.1 * gain, 0.0002);
}

void SetNB(bool setit)
{
	rx.nb.flag = setit;
}

void SetNBvals(REAL threshold)
{
	wxASSERT(rx.nb.gen != NULL);

	rx.nb.gen->setThreshold(threshold);
}

void SetSDROM(bool setit)
{
	rx.nb_sdrom.flag = setit;
}

void SetSDROMvals(REAL threshold)
{
	wxASSERT(rx.nb_sdrom.gen != NULL);

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

void
SetTXALCAttack (int attack)
{
  char buffer[64];
  sprintf (buffer, "setTXALCAttack %d\n", attack);
  sendcommand (buffer);
}

void SetTXCarrierLevel(double setit)
{
	tx.am.carrier_level = setit;
}

void
SetTXALCDecay (int decay)
{
  char buffer[64];
  sprintf (buffer, "setTXALCDecay %d\n", decay);
  sendcommand (buffer);
}

void
SetTXALCBot (double max_agc)
{
  char buffer[64];
  sprintf (buffer, "setTXALCBot %f\n", dB2lin ((REAL) max_agc));
  sendcommand (buffer);
}

void
SetTXALCHang (int decay)
{
  char buffer[64];
  sprintf (buffer, "setTXALCHang %d\n", decay);
  sendcommand (buffer);
}

void SetTXLevelerSt(bool state)
{
	tx.leveler.flag = state;
}

void
SetTXLevelerAttack (int attack)
{
  char buffer[64];
  sprintf (buffer, "setTXLevelerAttack %d\n", attack);
  sendcommand (buffer);
}

void
SetTXLevelerDecay (int decay)
{
  char buffer[64];
  sprintf (buffer, "setTXLevelerDecay %d\n", decay);
  sendcommand (buffer);
}

void
SetTXLevelerTop (double top)
{
  char buffer[64];
  sprintf (buffer, "setTXLevelerTop %f\n", dB2lin ((REAL) top));
  sendcommand (buffer);
}

void
SetTXLevelerHang (int decay)
{
  char buffer[64];
  sprintf (buffer, "setTXLevelerHang %d\n", decay);
  sendcommand (buffer);
}

void SetCorrectIQ(double phase, double gain)
{
	wxASSERT(rx.iqfix != NULL);

	rx.iqfix->setPhase(REAL(0.001F * phase));
	rx.iqfix->setGain(REAL(1.0F + 0.001F * gain));
}

void SetCorrectTXIQ(double phase, double gain)
{
	wxASSERT(tx.iqfix != NULL);

	tx.iqfix->setPhase(REAL(0.001F * phase));
	tx.iqfix->setGain(REAL(1.0F + 0.001F * gain));
}

void SetPWSmode(SPECTRUMtype type)
{
	uni.spec.type = type;
}

void SetWindow(Windowtype window)
{
	wxASSERT(uni.spec.gen != NULL);

	uni.spec.gen->setWindow(window);
}

void SetSpectrumPolyphase(bool setit)
{
	wxASSERT(uni.spec.gen != NULL);

	uni.spec.gen->setPolyphase(setit);
}

void
SetTXEQ (int *txeq)
{
  char buffer[256];
  sprintf (buffer, "setTXEQ 0.0 %d 120.0 %d 230.0 %d 450.0 %d 800.0 \
%d 1150.0 %d 1450.0 %d 1800.0 %d 2150.0 \
%d 2450.0 %d 2800.0 %d 3250.0 %d 6000.0\n", txeq[0], txeq[1], txeq[2], txeq[3], txeq[4], txeq[5], txeq[6], txeq[7], txeq[8], txeq[9], txeq[10], txeq[11]);
  sendcommand (buffer);
}

void SetGrphTXEQ(int *txeq)
{
	wxASSERT(tx.grapheq.gen != NULL);

	tx.grapheq.gen->setEQ(REAL(txeq[0]), REAL(txeq[1]), REAL(txeq[2]), REAL(txeq[3]));
}

void SetGrphTXEQcmd(bool state)
{
	tx.grapheq.flag = state;
}

void SetNotch160(bool state)
{
	wxASSERT(tx.grapheq.gen != NULL);

	tx.grapheq.gen->setNotchFlag(state);
}

void SetGrphRXEQ(int *rxeq)
{
	wxASSERT(rx.grapheq.gen != NULL);

	rx.grapheq.gen->setEQ(REAL(rxeq[0]), REAL(rxeq[1]), REAL(rxeq[2]), REAL(rxeq[3]));
}

void SetGrphRXEQcmd(bool state)
{
	rx.grapheq.flag = state;
}

void SetTXAGCFF(bool setit)
{
	tx.spr.flag = setit;
}

void SetTXAGCFFCompression(REAL txc)
{
	wxASSERT(tx.spr.gen != NULL);

	tx.spr.gen->setCompression(txc);
}

void SetSquelchVal (float setit)
{
	rx.squelch.thresh = setit;
}

void SetSquelchState(bool setit)
{
	rx.squelch.flag = setit;
}

void SetTRX(TRXMODE setit)
{
  wxASSERT(top.sync.upd.sem != NULL);

  top.sync.upd.sem->Wait();

  switch (setit) {
      case TX:
		  switch (tx.mode) {
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

void SetTXAGCLimit (double limit)
{
  char buffer[64];
  sprintf (buffer, "setTXAGCLimit %lf\n", limit);
  sendcommand (buffer);
}

void setSpotToneVals(REAL gain, REAL freq, REAL rise, REAL fall)
{
	wxASSERT(rx.spot.gen != NULL);

	rx.spot.gen->setValues(gain, freq, rise, fall);
}

void setSpotTone(bool setit)
{
	wxASSERT(rx.spot.gen != NULL);

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
	wxASSERT(top.sync.upd.sem != NULL);
	wxASSERT(uni.spec.gen != NULL);

	uni.spec.type = SPEC_POST_FILT;
	uni.spec.gen->setScale(SPEC_PWR);

	top.sync.upd.sem->Wait();
	uni.spec.gen->snapSpectrum();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeSpectrum(results);
}

void Process_Panadapter(float *results)
{
	wxASSERT(top.sync.upd.sem != NULL);
	wxASSERT(uni.spec.gen != NULL);

	uni.spec.type = SPEC_PRE_FILT;
	uni.spec.gen->setScale(SPEC_PWR);

	top.sync.upd.sem->Wait();
	uni.spec.gen->snapSpectrum();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeSpectrum(results);
}

void Process_Phase(float *results, unsigned int numpoints)
{
	wxASSERT(top.sync.upd.sem != NULL);
	wxASSERT(uni.spec.gen != NULL);

	top.sync.upd.sem->Wait();
	uni.spec.type = SPEC_POST_AGC;
	uni.spec.gen->setScale(SPEC_PWR);
	uni.spec.gen->snapScope();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeScopeComplex(results, numpoints);
}

void Process_Scope(float *results, unsigned int numpoints)
{
	wxASSERT(top.sync.upd.sem != NULL);
	wxASSERT(uni.spec.gen != NULL);

	top.sync.upd.sem->Wait();
	uni.spec.type = SPEC_POST_AGC;
	uni.spec.gen->setScale(SPEC_PWR);
	uni.spec.gen->snapScope();
	top.sync.upd.sem->Post();

	uni.spec.gen->computeScopeReal(results, numpoints);
}

float Calculate_Meters(METERTYPE mt)
{
	wxASSERT(top.sync.upd.sem != NULL);
	wxASSERT(uni.meter.gen != NULL);

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
	wxASSERT(rx.fm != NULL);

	tx.fm.cvtmod2freq = value * M_PI / uni.samplerate;

	rx.fm->setDeviation(value);
}
