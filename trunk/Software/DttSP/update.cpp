/* update.c

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

#include <common.h>

////////////////////////////////////////////////////////////////////////////
// for commands affecting RX, which RX is Listening

#define RL (uni.multirx.lis)

////////////////////////////////////////////////////////////////////////////

PRIVATE REAL INLINE
dB2lin (REAL dB)
{
  return (REAL) pow (10.0, (REAL) dB / 20.0);
}

PRIVATE int
setRXFilter (int n, char **p)
{
  REAL low_frequency = (REAL) (REAL) atof (p[0]),
    high_frequency = (REAL) (REAL) atof (p[1]);
  int ncoef = uni.buflen + 1;
  int i, fftlen = 2 * uni.buflen;
  fftwf_plan ptmp;
  COMPLEX *zcvec;

  if (fabs (low_frequency) >= 0.5 * uni.samplerate)
    return -1;
  if (fabs (high_frequency) >= 0.5 * uni.samplerate)
    return -2;
  if ((low_frequency + 10) >= high_frequency)
    return -3;
  delFIR_COMPLEX (rx[RL].filt.coef);

  rx[RL].filt.coef = newFIR_Bandpass_COMPLEX (low_frequency,
					      high_frequency,
					      uni.samplerate, ncoef);

  zcvec = newvec_COMPLEX (fftlen, "filter z vec in setFilter");
  ptmp = fftwf_plan_dft_1d (fftlen,
			    (fftwf_complex *) zcvec,
			    (fftwf_complex *) rx[RL].filt.ovsv->zfvec,
			    FFTW_FORWARD, uni.wisdom.bits);
#ifdef LHS
  for (i = 0; i < ncoef; i++)
    zcvec[i] = rx[RL].filt.coef->coef[i];
#else
  for (i = 0; i < ncoef; i++)
    zcvec[fftlen - ncoef + i] = rx[RL].filt.coef->coef[i];
#endif
  fftwf_execute (ptmp);
  fftwf_destroy_plan (ptmp);
  delvec_COMPLEX (zcvec);
  normalize_vec_COMPLEX (rx[RL].filt.ovsv->zfvec, rx[RL].filt.ovsv->fftlen);
  memcpy ((char *) rx[RL].filt.save, (char *) rx[RL].filt.ovsv->zfvec,
	  rx[RL].filt.ovsv->fftlen * sizeof (COMPLEX));

  return 0;
}


PRIVATE int
setTXFilter (int n, char **p)
{
  REAL low_frequency = (REAL) (REAL) atof (p[0]),
    high_frequency = (REAL) (REAL) atof (p[1]);
  int ncoef = uni.buflen + 1;
  int i, fftlen = 2 * uni.buflen;
  fftwf_plan ptmp;
  COMPLEX *zcvec;

  if (fabs (low_frequency) >= 0.5 * uni.samplerate)
    return -1;
  if (fabs (high_frequency) >= 0.5 * uni.samplerate)
    return -2;
  if ((low_frequency + 10) >= high_frequency)
    return -3;
  delFIR_COMPLEX (tx.filt.coef);
  tx.filt.coef = newFIR_Bandpass_COMPLEX (low_frequency,
					  high_frequency,
					  uni.samplerate, ncoef);

  zcvec = newvec_COMPLEX (fftlen, "filter z vec in setFilter");
//  ptmp = fftw_create_plan(fftlen, FFTW_FORWARD, uni.wisdom.bits);
  ptmp = fftwf_plan_dft_1d (fftlen,
			    (fftwf_complex *) zcvec,
			    (fftwf_complex *) tx.filt.ovsv->zfvec,
			    FFTW_FORWARD, uni.wisdom.bits);

#ifdef LHS
  for (i = 0; i < ncoef; i++)
    zcvec[i] = tx.filt.coef->coef[i];
#else
  for (i = 0; i < ncoef; i++)
    zcvec[fftlen - ncoef + i] = tx.filt.coef->coef[i];
#endif
  fftwf_execute (ptmp);
//         (fftw_complex *) zcvec,
//         (fftw_complex *) tx.filt.ovsv->zfvec);
  fftwf_destroy_plan (ptmp);
  delvec_COMPLEX (zcvec);
  normalize_vec_COMPLEX (tx.filt.ovsv->zfvec, tx.filt.ovsv->fftlen);
  memcpy ((char *) tx.filt.save,
	  (char *) tx.filt.ovsv->zfvec,
	  tx.filt.ovsv->fftlen * sizeof (COMPLEX));

  return 0;
}

PRIVATE int
setFilter (int n, char **p)
{
  if (n == 2)
    return setRXFilter (n, p);
  else
    {
      int trx = atoi (p[2]);
      if (trx == RX)
	return setRXFilter (n, p);
      else if (trx == TX)
	return setTXFilter (n, p);
      else
	return -1;
    }
}

// setMode <mode> [TRX]
PRIVATE int
setMode (int n, char **p)
{
  SDRMODE mode = SDRMODE(atoi (p[0]));
  if (n > 1)
    {
      TRXMODE trx = TRXMODE(atoi (p[1]));
      switch (trx)
	{
	case TX:
	  tx.mode = mode;
	  break;
	case RX:
	default:
	  rx[RL].mode = mode;
	  break;
	}
    }
  else
    tx.mode = rx[RL].mode = uni.mode.sdr = mode;
  if (rx[RL].mode == AM)
    rx[RL].am.gen->mode = AMdet;
  if (rx[RL].mode == SAM)
    rx[RL].am.gen->mode = SAMdet;
  return 0;
}

PRIVATE int
setOsc (int n, char **p)
{
  double newfreq = (REAL) atof (p[0]);
  if (fabs (newfreq) >= 0.5 * uni.samplerate)
    return -1;
  newfreq *= 2.0 * M_PI / uni.samplerate;
  if (n > 1)
    {
      int trx = atoi (p[1]);
      switch (trx)
	{
	case TX:
	  tx.osc.gen->Frequency = newfreq;
	  break;
	case RX:
	default:
	  rx[RL].osc.gen->Frequency = newfreq;
	  break;
	}
    }
  else
    tx.osc.gen->Frequency = rx[RL].osc.gen->Frequency = newfreq;
  return 0;
}

PRIVATE int
setBlkNR (int n, char **p)
{
  rx[RL].banr.flag = atoi (p[0]) ? true : false;
  return 0;
}
PRIVATE int
setBlkNRval (int n, char **p)
{
  REAL adaptation_rate = (REAL) atof (p[0]);
  rx[RL].banr.gen->adaptation_rate = adaptation_rate;
  return 0;
}

PRIVATE int
setBlkANF (int n, char **p)
{
  rx[RL].banf.flag = atoi (p[0]) ? true : false;
  return 0;
}
PRIVATE int
setBlkANFval (int n, char **p)
{
  REAL adaptation_rate = (REAL) atof (p[0]);
  rx[RL].banf.gen->adaptation_rate = adaptation_rate;
  return 0;
}

PRIVATE int
setNB (int n, char **p)
{
  rx[RL].nb.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setNBvals (int n, char **p)
{
  REAL threshold = (REAL) atof (p[0]);
  rx[RL].nb.gen->threshold = rx[RL].nb.thresh = threshold;
  return 0;
}

PRIVATE int
setSDROM (int n, char **p)
{
  rx[RL].nb_sdrom.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setSDROMvals (int n, char **p)
{
  REAL threshold = (REAL) atof (p[0]);
  rx[RL].nb_sdrom.gen->threshold = rx[RL].nb_sdrom.thresh = threshold;
  return 0;
}

PRIVATE int
setBIN (int n, char **p)
{
  rx[RL].bin.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setfixedAGC (int n, char **p)
{
  REAL gain = (REAL) atof (p[0]);
  if (n > 1)
    {
      int trx = atoi (p[1]);
      switch (trx)
	{
	case TX:
	  tx.leveler.gen->gain.now = gain;
	  break;
	case RX:
	default:
	  rx[RL].dttspagc.gen->gain.now = gain;
	  break;
	}
    }
  else
    tx.leveler.gen->gain.now = rx[RL].dttspagc.gen->gain.now = gain;
  return 0;
}

PRIVATE int
setRXAGCCompression (int n, char **p)
{
  REAL rxcompression = (REAL) atof (p[0]);
  rx[RL].dttspagc.gen->gain.top = (REAL) pow (10.0, rxcompression * 0.05);
  return 0;
}

PRIVATE int
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

PRIVATE int
setTXALCDecay (int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  tx.alc.gen->decay = (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  tx.alc.gen->one_m_decay = (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  return 0;
}

PRIVATE int
setTXALCHang (int n, char **p)
{
  REAL hang = (REAL) atof (p[0]);
  tx.alc.gen->hangtime = (REAL) 0.001 *hang;
  return 0;
}

PRIVATE int
setTXALCBot (int n, char **p)
{
  REAL bot = (REAL) atof (p[0]);
  tx.alc.gen->gain.bottom = bot;
  return 0;
}

PRIVATE int
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
PRIVATE int
setTXLevelerSt (int n, char **p)
{
  bool tmp = atoi (p[0])  ? true : false;
  tx.leveler.flag = tmp;
  return 0;
}

PRIVATE int
setTXLevelerDecay (int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  tx.leveler.gen->decay =
    (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  tx.leveler.gen->one_m_decay = (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  return 0;
}

PRIVATE int
setTXLevelerTop (int n, char **p)
{
  REAL top = (REAL) atof (p[0]);
  tx.leveler.gen->gain.top = top;
  return 0;
}

PRIVATE int
setTXLevelerHang (int n, char **p)
{
  REAL hang = (REAL) atof (p[0]);
  tx.leveler.gen->hangtime = (REAL) 0.001 *hang;
  return 0;
}


PRIVATE int
setRXAGC (int n, char **p)
{
  int setit = atoi (p[0]);
  rx[RL].dttspagc.gen->mode = 1;
  rx[RL].dttspagc.gen->attack =
    (REAL) (1.0 - exp (-1000 / (2.0 * uni.samplerate)));
  rx[RL].dttspagc.gen->one_m_attack =
    (REAL) (1.0 - rx[RL].dttspagc.gen->attack);
  rx[RL].dttspagc.gen->hangindex = rx[RL].dttspagc.gen->indx = 0;
  rx[RL].dttspagc.gen->sndx = (int) (uni.samplerate * 0.006f);
  rx[RL].dttspagc.gen->fastindx = FASTLEAD;
  switch (setit)
    {
    case agcOFF:
      rx[RL].dttspagc.gen->mode = agcOFF;
      rx[RL].dttspagc.flag = true;
      break;
    case agcSLOW:
      rx[RL].dttspagc.gen->mode = agcSLOW;
      rx[RL].dttspagc.gen->hangtime = (REAL) 0.5;
      rx[RL].dttspagc.gen->fasthangtime = (REAL) 0.1;
      rx[RL].dttspagc.gen->decay =
	(REAL) (1.0 - exp (-1000 / (500.0 * uni.samplerate)));
      rx[RL].dttspagc.gen->one_m_decay =
	(REAL) (1.0 - rx[RL].dttspagc.gen->decay);
      rx[RL].dttspagc.flag = true;
      break;
    case agcMED:
      rx[RL].dttspagc.gen->mode = agcMED;
      rx[RL].dttspagc.gen->hangtime = (REAL) 0.25;
      rx[RL].dttspagc.gen->fasthangtime = (REAL) 0.1;
      rx[RL].dttspagc.gen->decay =
	(REAL) (1.0 - exp (-1000 / (250.0 * uni.samplerate)));
      rx[RL].dttspagc.gen->one_m_decay =
	(REAL) (1.0 - rx[RL].dttspagc.gen->decay);
      rx[RL].dttspagc.flag = true;
      break;
    case agcFAST:
      rx[RL].dttspagc.gen->mode = agcFAST;
      rx[RL].dttspagc.gen->hangtime = (REAL) 0.1;
      rx[RL].dttspagc.gen->fasthangtime = (REAL) 0.1;
      rx[RL].dttspagc.gen->hangtime = (REAL) 0.1;
      rx[RL].dttspagc.gen->decay =
	(REAL) (1.0 - exp (-1000 / (100.0 * uni.samplerate)));
      rx[RL].dttspagc.gen->one_m_decay =
	(REAL) (1.0 - rx[RL].dttspagc.gen->decay);
      rx[RL].dttspagc.flag = true;
      break;
    case agcLONG:
      rx[RL].dttspagc.gen->mode = agcLONG;
      rx[RL].dttspagc.flag = true;
      rx[RL].dttspagc.gen->hangtime = 0.75;
      rx[RL].dttspagc.gen->fasthangtime = (REAL) 0.1;
      rx[RL].dttspagc.gen->decay = (REAL) (1.0 - exp (-0.5 / uni.samplerate));
      rx[RL].dttspagc.gen->one_m_decay =
	(REAL) (1.0 - rx[RL].dttspagc.gen->decay);
      break;
    }
  return 0;
}

PRIVATE int
setRXAGCAttack (int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  rx[RL].dttspagc.gen->mode = 1;
  rx[RL].dttspagc.gen->hangindex = rx[RL].dttspagc.gen->indx = 0;
  rx[RL].dttspagc.gen->sndx = (int) (uni.samplerate * 0.006f);
  rx[RL].dttspagc.gen->fasthangtime = (REAL) 0.1;
  rx[RL].dttspagc.gen->fastindx = FASTLEAD;
  rx[RL].dttspagc.gen->attack =
    (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  rx[RL].dttspagc.gen->one_m_attack =
    (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  rx[RL].dttspagc.gen->sndx = (int) (uni.samplerate * tmp * 0.003f);
  return 0;
}


PRIVATE int
setRXAGCDecay (int n, char **p)
{
  REAL tmp = (REAL) atof (p[0]);
  rx[RL].dttspagc.gen->decay =
    (REAL) (1.0 - exp (-1000.0 / (tmp * uni.samplerate)));
  rx[RL].dttspagc.gen->one_m_decay =
    (REAL) exp (-1000.0 / (tmp * uni.samplerate));
  return 0;
}

PRIVATE int
setRXAGCHang (int n, char **p)
{
  REAL hang = (REAL) atof (p[0]);
  rx[RL].dttspagc.gen->hangtime = (REAL) 0.001 *hang;
  return 0;
}
PRIVATE int
setRXAGCSlope (int n, char **p)
{
  REAL slope = (REAL) atof (p[0]);
  rx[RL].dttspagc.gen->slope = (REAL) dB2lin (0.1f * slope);
  return 0;
}
PRIVATE int
setRXAGCHangThreshold (int h, char **p)
{
  REAL hangthreshold = (REAL) (atof (p[0]));
  rx[RL].dttspagc.gen->hangthresh = (REAL) 0.01 *hangthreshold;
  return 0;
}

PRIVATE int
setRXAGCLimit (int n, char **p)
{
  REAL limit = (REAL) atof (p[0]);
  rx[RL].dttspagc.gen->gain.top = limit;
  return 0;
}
PRIVATE int
setRXAGCTop (int n, char **p)
{
  REAL top = (REAL) atof (p[0]);
  rx[RL].dttspagc.gen->gain.top = top;
  return 0;
}


PRIVATE int
setRXAGCFix (int n, char **p)
{
  rx[RL].dttspagc.gen->gain.fix = (REAL) atof (p[0]);
  return 0;
}


PRIVATE int
setTXAGCFF (int n, char **p)
{
  tx.spr.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setTXAGCFFCompression (int n, char **p)
{
  REAL txcompression = (REAL) atof (p[0]);
  tx.spr.gen->MaxGain = (REAL) pow (10.0, txcompression * 0.05);
  tx.spr.gen->fac = (REAL)
    ((((0.0000401002 * txcompression) - 0.0032093390) * txcompression +
      0.0612862687) * txcompression + 0.9759745718);
  return 0;
}


PRIVATE int
setTXAGCLimit (int n, char **p)
{
  REAL limit = (REAL) atof (p[0]);
  tx.alc.gen->gain.top = limit;
  return 0;
}

PRIVATE int
setTXSpeechCompression (int n, char **p)
{
  tx.spr.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setTXSpeechCompressionGain (int n, char **p)
{
  tx.spr.gen->MaxGain = dB2lin ((REAL) atof (p[0]));
  return 0;
}


PRIVATE int
f2x (REAL f)
{
  REAL fix = tx.filt.ovsv->fftlen * f / uni.samplerate;
  return (int) (fix + 0.5);
}



PRIVATE int
setGrphRXEQ (int n, char **p)
{

  if (n < 4)
    return 0;
  else
    {
      int i;
      fftwf_plan ptmp;
      COMPLEX *filtcoef, *tmpcoef;
      ComplexFIR tmpfilt;
      REAL preamp, gain[3];

      filtcoef = newvec_COMPLEX (512, "filter for EQ");
      tmpcoef = newvec_COMPLEX (257, "tmp filter for EQ");

      preamp = (float) dB2lin ((REAL) atof (p[0])) * 0.5f;
      gain[0] = (float) dB2lin ((REAL) atof (p[1])) * preamp;
      gain[1] = (float) dB2lin ((REAL) atof (p[2])) * preamp;
      gain[2] = (float) dB2lin ((REAL) atof (p[3])) * preamp;

      tmpfilt = newFIR_Bandpass_COMPLEX (-400, 400, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cscl (tmpfilt->coef[i], gain[0]);
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (400, 1500, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[1]));
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (-1500, -400, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[1]));
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (1500, 6000, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[2]));
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (-6000, -1500, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[2]));
      delFIR_Bandpass_COMPLEX (tmpfilt);
      for (i = 0; i < 257; i++)
	filtcoef[254 + i] = tmpcoef[i];
      ptmp =
	fftwf_plan_dft_1d (512, (fftwf_complex *) filtcoef,
			   (fftwf_complex *) rx[RL].grapheq.gen->p->
			   zfvec, FFTW_FORWARD, uni.wisdom.bits);

      fftwf_execute (ptmp);
      fftwf_destroy_plan (ptmp);
      delvec_COMPLEX (filtcoef);
      delvec_COMPLEX (tmpcoef);
    }
  return 0;
}
PRIVATE int
setGrphTXEQ (int n, char **p)
{
  if (n < 4)
    return 0;
  else
    {
      int i;
      fftwf_plan ptmp;
      COMPLEX *filtcoef, *tmpcoef;
      ComplexFIR tmpfilt;
      REAL preamp, gain[3];

      filtcoef = newvec_COMPLEX (512, "filter for EQ");
      tmpcoef = newvec_COMPLEX (257, "tmp filter for EQ");

      preamp = (float) dB2lin ((REAL) atof (p[0])) * 0.5f;
      gain[0] = (float) dB2lin ((REAL) atof (p[1])) * preamp;
      gain[1] = (float) dB2lin ((REAL) atof (p[2])) * preamp;
      gain[2] = (float) dB2lin ((REAL) atof (p[3])) * preamp;

      tmpfilt = newFIR_Bandpass_COMPLEX (-400, 400, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cscl (tmpfilt->coef[i], gain[0]);
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (400, 1500, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[1]));
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (-1500, -400, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[1]));
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (1500, 6000, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[2]));
      delFIR_Bandpass_COMPLEX (tmpfilt);

      tmpfilt = newFIR_Bandpass_COMPLEX (-6000, -1500, uni.samplerate, 257);
      for (i = 0; i < 257; i++)
	tmpcoef[i] = Cadd (tmpcoef[i], Cscl (tmpfilt->coef[i], gain[2]));
      delFIR_Bandpass_COMPLEX (tmpfilt);
      for (i = 0; i < 257; i++)
	filtcoef[255 + i] = tmpcoef[i];
      ptmp =
	fftwf_plan_dft_1d (512, (fftwf_complex *) filtcoef,
			   (fftwf_complex *) tx.grapheq.gen->p->zfvec,
			   FFTW_FORWARD, uni.wisdom.bits);

      fftwf_execute (ptmp);
      fftwf_destroy_plan (ptmp);
      delvec_COMPLEX (filtcoef);
    }
  return 0;
}

PRIVATE int
setNotch160 (int n, char **p)
{
  tx.grapheq.gen->notchflag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setTXCarrierLevel (int n, char **p)
{
  tx.am.carrier_level = (float) atof (p[0]);
  return 0;
}



PRIVATE int
setANF (int n, char **p)
{
  rx[RL].anf.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setANFvals (int n, char **p)
{
  int taps = atoi (p[0]), delay = atoi (p[1]);
  REAL gain = (REAL) atof (p[2]), leak = (REAL) atof (p[3]);
  rx[RL].anf.gen->adaptive_filter_size = taps;
  rx[RL].anf.gen->delay = delay;
  rx[RL].anf.gen->adaptation_rate = gain;
  rx[RL].anf.gen->leakage = leak;
  return 0;
}

PRIVATE int
setNR (int n, char **p)
{
  rx[RL].anr.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setNRvals (int n, char **p)
{
  int taps = atoi (p[0]), delay = atoi (p[1]);
  REAL gain = (REAL) atof (p[2]), leak = (REAL) atof (p[3]);
  rx[RL].anr.gen->adaptive_filter_size = taps;
  rx[RL].anr.gen->delay = delay;
  rx[RL].anr.gen->adaptation_rate = gain;
  rx[RL].anr.gen->leakage = leak;
  return 0;
}

PRIVATE int
setcorrectIQ (int n, char **p)
{
  REAL phaseadjustment = (REAL) atof (p[0]),
    gainadjustment = (REAL) atof (p[1]);
  rx[RL].iqfix->phase = (REAL) (0.001 * phaseadjustment);
  rx[RL].iqfix->gain = (REAL) (1.0 + 0.001 * gainadjustment);
  return 0;
}

PRIVATE int
setcorrectIQphase (int n, char **p)
{
  REAL phaseadjustment = (REAL) atof (p[0]);
  rx[RL].iqfix->phase = (REAL) 0.001 *phaseadjustment;
  return 0;
}

PRIVATE int
setcorrectIQgain (int n, char **p)
{
  REAL gainadjustment = (REAL) atof (p[0]);
  rx[RL].iqfix->gain = (REAL) (1.0 + 0.001 * gainadjustment);
  return 0;
}

PRIVATE int
setcorrectTXIQ (int n, char **p)
{
  REAL phaseadjustment = (REAL) atof (p[0]),
    gainadjustment = (REAL) atof (p[1]);
  tx.iqfix->phase = (REAL) (0.001 * phaseadjustment);
  tx.iqfix->gain = (REAL) (1.0 + 0.001 * gainadjustment);
  return 0;
}

PRIVATE int
setcorrectTXIQphase (int n, char **p)
{
  REAL phaseadjustment = (REAL) atof (p[0]);
  tx.iqfix->phase = (REAL) (0.001 * phaseadjustment);
  return 0;
}

PRIVATE int
setcorrectTXIQgain (int n, char **p)
{
  REAL gainadjustment = (REAL) atof (p[0]);
  tx.iqfix->gain = (REAL) (1.0 + 0.001 * gainadjustment);
  return 0;
}

PRIVATE int
setSquelch (int n, char **p)
{
  rx[RL].squelch.thresh = (REAL) atof (p[0]);
  return 0;
}

PRIVATE int
setSquelchSt (int n, char **p)
{
  rx[RL].squelch.flag = atoi (p[0]) ? true : false;
  return 0;
}

PRIVATE int
setTXSquelch (int n, char **p)
{
  tx.squelch.thresh = (REAL) atof (p[0]);
  return 0;
}

PRIVATE int
setTXSquelchSt (int n, char **p)
{
  tx.squelch.flag = atoi (p[0]) ? true : false;
  return 0;
}


PRIVATE int
setTRX (int n, char **p)
{
  uni.mode.trx = TRXMODE(atoi (p[0]));
  return 0;
}

PRIVATE int
setRunState (int n, char **p)
{
  RUNMODE rs = RUNMODE(atoi (p[0]));
  top.state = rs;
  return 0;
}

PRIVATE int
setSpotToneVals (int n, char **p)
{
  REAL gain = (REAL) atof (p[0]),
    freq = (REAL) atof (p[1]),
    rise = (REAL) atof (p[2]), fall = (REAL) atof (p[3]);
  setSpotToneGenVals (rx[RL].spot.gen, gain, freq, rise, fall);
  return 0;
}

PRIVATE int
setSpotTone (int n, char **p)
{
  if (atoi (p[0]))
    {
      SpotToneOn (rx[RL].spot.gen);
      rx[RL].spot.flag = true;
    }
  else
    SpotToneOff (rx[RL].spot.gen);
  return 0;
}




PRIVATE int
setFinished (int n, char **p)
{
  top.running = false;
  return 0;
}

// next-trx-mode [nbufs-to-zap]
PRIVATE int
setSWCH (int n, char **p)
{
  top.swch.trx.next = TRXMODE(atoi (p[0]));
  if (n > 1)
    top.swch.bfct.want = atoi (p[1]);
  else
    top.swch.bfct.want = 0;
  top.swch.bfct.have = 0;
  if (top.state != RUN_SWCH)
    top.swch.run.last = top.state;
  top.state = RUN_SWCH;
  return 0;
}


PRIVATE int
setRingBufferReset (int n, char **p)
{
  extern void ringb_float_restart (ringb_float_t * rb, size_t nbytes);
  extern void ringb_float_reset (ringb_float_t * rb);
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

setJackResetSize (int n, char **p)
{
  extern void ringb_clear (ringb_t * rb, size_t nbytes);
  n = atoi (p[0]);
  ringb_float_reset (top.jack.ring.i.l);
  ringb_float_reset (top.jack.ring.i.r);
  ringb_float_reset (top.jack.auxr.i.l);
  ringb_float_reset (top.jack.auxr.i.r);
  ringb_float_restart (top.jack.ring.o.l, top.hold.size.frames);
  ringb_float_restart (top.jack.ring.o.r, top.hold.size.frames);
  return 0;
}
PRIVATE int
setRXListen (int n, char **p)
{
  int lis = atoi (p[0]);
  if (lis < 0 || lis >= uni.multirx.nrx)
    return -1;
  else
    {
      uni.multirx.lis = lis;
      return 0;
    }
}

PRIVATE int
setRXOn (int n, char **p)
{
  if (n < 1)
    {
      if (uni.multirx.act[RL])
	return -1;
      else
	{
	  uni.multirx.act[RL] = true;
	  uni.multirx.nac++;
	  rx[RL].tick = 0;
	  return 0;
	}
    }
  else
    {
      int k = atoi (p[0]);
      if (k < 0 || k >= uni.multirx.nrx)
	return -1;
      else
	{
	  if (uni.multirx.act[k])
	    return -1;
	  else
	    {
	      uni.multirx.act[k] = true;
	      uni.multirx.nac++;
	      rx[k].tick = 0;
	      return 0;
	    }
	}
    }
}

PRIVATE int
setRXOff (int n, char **p)
{
  if (n < 1)
    {
      if (!uni.multirx.act[RL])
	return -1;
      else
	{
	  uni.multirx.act[RL] = false;
	  --uni.multirx.nac;
	  return 0;
	}
    }
  else
    {
      int k = atoi (p[0]);
      if (k < 0 || k >= uni.multirx.nrx)
	return -1;
      else
	{
	  if (!uni.multirx.act[k])
	    return -1;
	  else
	    {
	      uni.multirx.act[k] = false;
	      --uni.multirx.nac;
	      return 0;
	    }
	}
    }
}

// [pos]  0.0 <= pos <= 1.0
PRIVATE int
setRXPan (int n, char **p)
{
  REAL pos, theta;
  if (n < 1)
    {
      pos = 0.5;
      theta = (REAL) ((1.0 - pos) * M_PI / 2.0);
      rx[RL].azim = Cmplx ((REAL) cos (theta), (REAL) sin (theta));
      return 0;
    }
  else
    {
      if ((pos = (REAL) atof (p[0])) < 0.0 || pos > 1.0)
	return -1;
      theta = (REAL) ((1.0 - pos) * M_PI / 2.0);
      rx[RL].azim = Cmplx ((REAL) cos (theta), (REAL) sin (theta));
      return 0;
    }
}

PRIVATE int
setAuxMixSt (int n, char **p)
{
  if (n < 1)
    {
      uni.mix.rx.flag = uni.mix.tx.flag = false;
      return 0;
    }
  else
    {
      bool flag = atoi (p[0]) ? true : false;
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
PRIVATE int
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

PRIVATE int
setCompandSt (int n, char **p)
{
  if (n < 1)
    {
      tx.cpd.flag = false;
      return 0;
    }
  else
    {
      bool flag = atoi (p[0]) ? true : false;
      if (n > 1)
	{
	  switch (atoi (p[1]))
	    {
	    case RX:
	      rx[RL].cpd.flag = flag;
	      break;
	    case TX:
	    default:
	      tx.cpd.flag = flag;
	      break;
	    }
	}
      else
	tx.cpd.flag = flag;
      return 0;
    }
}

PRIVATE int
setCompand (int n, char **p)
{
  if (n < 1)
    return -1;
  else
    {
      REAL fac = (REAL) atof (p[0]);
      if (n > 1)
	{
	  switch (atoi (p[1]))
	    {
	    case RX:
	      WSCReset (rx[RL].cpd.gen, fac);
	      break;
	    case TX:
	    default:
	      WSCReset (tx.cpd.gen, fac);
	      break;
	    }
	}
      else
	WSCReset (tx.cpd.gen, fac);
      return 0;
    }
}

PRIVATE int
setGrphTXEQcmd (int n, char **p)
{
  if (n < 1)
    {
      tx.grapheq.flag = false;
      return 0;
    }
  else
    {
      bool flag = atoi (p[0]) ? true : false;
      tx.grapheq.flag = flag;
    }
  return 0;
}


PRIVATE int
setGrphRXEQcmd (int n, char **p)
{
  if (n < 1)
    {
      rx[RL].grapheq.flag = false;
      return 0;
    }
  else
    {
      bool flag = atoi (p[0]) ? true : false;
      rx[RL].grapheq.flag = flag;
    }
  return 0;
}

PRIVATE int
setTXCompandSt (int n, char **p)
{
  if (n < 1)
    {
      tx.cpd.flag = false;
      return 0;
    }
  else
    {
      bool flag = atoi (p[0]) ? true : false;
      tx.cpd.flag = flag;
    }
  return 0;
}
PRIVATE int
setTXCompand (int n, char **p)
{
  if (n < 1)
    return -1;
  else
    {
      REAL fac = (REAL) atof (p[0]);
      WSCReset (tx.cpd.gen, fac);
    }
  return 0;
}

//------------------------------------------------------------------------
#if 0
// [type]
PRIVATE int
setMeterType (int n, char **p)
{
  if (n < 1)
    uni.meter.rx.type = SIGNAL_STRENGTH;
  else
    {
      METERTYPE type = (METERTYPE) atoi (p[0]);
      if (n > 1)
	{
	  int trx = atoi (p[1]);
	  switch (trx)
	    {
	    case TX:
	      uni.meter.tx.type = type;
	      break;
	    case RX:
	    default:
	      uni.meter.rx.type = type;
	      break;
	    }
	}
      else
	uni.meter.rx.type = type;
    }
  return 0;
}
#endif
PRIVATE int
setSpectrumPolyphase (int n, char **p)
{
  bool setit = atoi (p[0]) ? true : false;
  if (uni.spec.polyphase != setit)
    {
      if (setit)
	{
	  uni.spec.polyphase = true;
	  uni.spec.mask = (8 * uni.spec.size) - 1;
	  {
	    RealFIR WOLAfir;
	    REAL MaxTap = 0;
	    int i;
	    WOLAfir =
	      newFIR_Lowpass_REAL (1.0, (REAL) uni.spec.size,
				   8 * uni.spec.size - 1);
	    memset (uni.spec.window, 0, 8 * sizeof (REAL) * uni.spec.size);
	    memcpy (uni.spec.window, FIRcoef (WOLAfir),
		    sizeof (REAL) * (8 * uni.spec.size - 1));
	    for (i = 0; i < 8 * uni.spec.size; i++)
	      MaxTap = max (MaxTap, (REAL) fabs (uni.spec.window[i]));
	    MaxTap = 1.0f / MaxTap;
	    for (i = 0; i < 8 * uni.spec.size; i++)
	      {
		uni.spec.window[i] *= MaxTap;
	      }
	    delFIR_REAL (WOLAfir);
	  }
	}
      else
	{
	  uni.spec.polyphase = false;
	  uni.spec.mask = uni.spec.size - 1;
	  memset (uni.spec.window, 0, sizeof (REAL) * uni.spec.size);
	  makewindow (uni.spec.wintype, uni.spec.size - 1, uni.spec.window);
	}
      reinit_spectrum (&uni.spec);
//              fprintf(stderr,"size = %d polyphase=%1d winmid=%lf\n",
//                      uni.spec.mask+1,uni.spec.polyphase,uni.spec.window[uni.spec.mask>>1]),fflush(stderr);
    }
  return 0;
}
PRIVATE int
setSpectrumWindow (int n, char **p)
{
  Windowtype window = Windowtype(atoi (p[0]));
  if (!uni.spec.polyphase)
    makewindow (window, uni.spec.size, uni.spec.window);
  uni.spec.wintype = window;
  return 0;
}
PRIVATE int
setSpectrumType (int n, char **p)
{
  uni.spec.type = SPEC_POST_FILT;
  uni.spec.scale = SPEC_PWR;
  uni.spec.rxk = RL;
  switch (n)
    {
    case 3:
      uni.spec.rxk = atoi (p[2]);
    case 2:
      uni.spec.scale = atoi (p[1]);
    case 1:
      uni.spec.type = atoi (p[0]);
      break;
    case 0:
      break;
    default:
      return -1;
    }
  return uni.spec.type;
}

PRIVATE int
setDCBlockSt (int n, char **p)
{
  if (n < 1)
    {
      tx.dcb.flag = false;
      return 0;
    }
  else
    {
      tx.dcb.flag = atoi (p[0]) ? true : false;
      return 0;
    }
}

PRIVATE int
setDCBlock (int n, char **p)
{
  if (n < 1)
    return -1;
  else
    {
      resetDCBlocker (tx.dcb.gen, atoi (p[0]));
      return 0;
    }
}


//========================================================================

CTE update_cmds[] = {
  {"setANF", setANF}
  ,
  {"setANFvals", setANFvals}
  ,
  {"setBlkANF", setBlkANF}
  ,
  {"setBlkANFval", setBlkANFval}
  ,
  {"setBIN", setBIN}
  ,
  {"setFilter", setFilter}
  ,
  {"setFinished", setFinished}
  ,
  {"setMode", setMode}
  ,
  {"setNB", setNB}
  ,
  {"setNBvals", setNBvals}
  ,
  {"setNR", setNR}
  ,
  {"setNRvals", setNRvals}
  ,
  {"setBlkNR", setBlkNR}
  ,
  {"setBlkNRval", setBlkNRval}
  ,
  {"setOsc", setOsc}
  ,
  {"setRXAGC", setRXAGC}
  ,
  {"setRXAGCAttack", setRXAGCAttack}
  ,
  {"setRXAGCDecay", setRXAGCDecay}
  ,
  {"setRXAGCCompression", setRXAGCCompression}
  ,
  {"setRXAGCHang", setRXAGCHang}
  ,
  {"setRXAGCSlope", setRXAGCSlope}
  ,
  {"setRXAGCHangThreshold", setRXAGCHangThreshold}
  ,
  {"setRXAGCLimit", setRXAGCLimit}
  ,
  {"setRXAGCTop", setRXAGCTop}
  ,
  {"setRXAGCFix", setRXAGCFix}
  ,
  {"setTXALCAttack", setTXALCAttack}
  ,
  {"setTXALCDecay", setTXALCDecay}
  ,
  {"setTXALCBot", setTXALCBot}
  ,
  {"setTXALCHang", setTXALCHang}
  ,
  {"setTXLevelerSt", setTXLevelerSt}
  ,
  {"setTXLevelerAttack", setTXLevelerAttack}
  ,
  {"setTXLevelerDecay", setTXLevelerDecay}
  ,
  {"setTXLevelerHang", setTXLevelerHang}
  ,
  {"setTXLevelerTop", setTXLevelerTop}
  ,
  {"setTXSquelch", setTXSquelch}
  ,
  {"setTXSquelchSt", setTXSquelchSt}
  ,
  {"setRXAGCCompression", setRXAGCCompression}
  ,
  {"setRunState", setRunState}
  ,
  {"setSDROM", setSDROM}
  ,
  {"setSDROMvals", setSDROMvals}
  ,
  {"setSWCH", setSWCH}
  ,
  {"setSpotTone", setSpotTone}
  ,
  {"setSpotToneVals", setSpotToneVals}
  ,
  {"setSquelch", setSquelch}
  ,
  {"setSquelchSt", setSquelchSt}
  ,
  {"setTRX", setTRX}
  ,
  {"setTXAGCFFCompression", setTXAGCFFCompression}
  ,
  {"setTXAGCFF", setTXAGCFF}
  ,
  {"setTXAGCLimit", setTXAGCLimit}
  ,
  {"setTXCompandSt", setTXCompandSt}
  ,
  {"setTXCompand", setTXCompand}
  ,
  {"setTXCarrierLevel", setTXCarrierLevel}
  ,
  {"setGrphTXEQcmd", setGrphTXEQcmd}
  ,
  {"setGrphRXEQcmd", setGrphRXEQcmd}
  ,
  {"setGrphTXEQ", setGrphTXEQ}
  ,
  {"setGrphRXEQ", setGrphRXEQ}
  ,
  {"setNotch160", setNotch160}
  ,
  {"setTXSpeechCompression", setTXSpeechCompression}
  ,
  {"setTXSpeechCompressionGain", setTXSpeechCompressionGain}
  ,
  {"setcorrectIQ", setcorrectIQ}
  ,
  {"setcorrectIQgain", setcorrectIQgain}
  ,
  {"setcorrectIQphase", setcorrectIQphase}
  ,
  {"setcorrectTXIQ", setcorrectTXIQ}
  ,
  {"setcorrectTXIQgain", setcorrectTXIQgain}
  ,
  {"setcorrectTXIQphase", setcorrectTXIQphase}
  ,
  {"setfixedAGC", setfixedAGC}
  ,
  {"setRingBufferReset", setRingBufferReset}
  ,
  {"setRXListen", setRXListen}
  ,
  {"setRXOn", setRXOn}
  ,
  {"setRXOff", setRXOff}
  ,
  {"setRXPan", setRXPan}
  ,
  {"setAuxMixSt", setAuxMixSt}
  ,
  {"setAuxMixGain", setAuxMixGain}
  ,
  {"setCompandSt", setCompandSt}
  ,
  {"setCompand", setCompand}
  ,
#if 0
  {"setMeterType", setMeterType}
  ,
#endif
  {"setSpectrumType", setSpectrumType}
  ,
  {"setSpectrumWindow", setSpectrumWindow}
  ,
  {"setSpectrumPolyphase", setSpectrumPolyphase}
  ,
  {"setDCBlockSt", setDCBlockSt}
  ,
  {"setDCBlock", setDCBlock}
  ,
  {"setJackResetSize", setJackResetSize}
  ,
  {0, 0}
};

//........................................................................

int
do_update (char *str)
{
  SPLIT splt = &uni.update.splt;

  // append to replay file?
  if (*str == '!')
    {
      str++;			// strip !
    }

  split (splt, str);

  if (NF (splt) < 1)
    return -1;

  else
    {
      Thunk thk = Thunk_lookup (update_cmds, F (splt, 0));
      if (!thk)
	return -1;
      else
	{
	  int val;

	  top.sync.upd.sem->Wait();
	  val = (*thk) (NF (splt) - 1, Fptr (splt, 1));
	  top.sync.upd.sem->Post();

	  return val;
	}
    }
}

//------------------------------------------------------------------------

/*  Legacy DLL version of this code */
static char oldcommand[512] = "";
PRIVATE INLINE void
sendcommand (char *buffer)
{
  if (strcmp (buffer, oldcommand) != 0)
    do_update(buffer);
  strcpy (oldcommand, buffer);
}
void
Setup_SDR (REAL sampleRate, unsigned int audioSize)
{
  extern void setup (REAL sampleRate, unsigned int audioSize);
  setup (sampleRate, audioSize);
}
void
Destroy_SDR (void)
{
  extern void closeup ();
  closeup ();
}
void
SetMode (SDRMODE m)
{
  char buffer[64];
  sprintf (buffer, "!setMode %d\n", m);
  sendcommand (buffer);
}
void
AudioReset (void)
{
  extern bool reset_em;
  reset_em = true;
}
void
SetDCBlock (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setDCBlockSt %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}

void
SetFilter (double low_frequency, double high_frequency, int taps, TRXMODE trx)
{
  char buffer[64];
  sprintf (buffer, "!setFilter %f %f %d\n", low_frequency, high_frequency,
	   trx);
  sendcommand (buffer);
}

void
Release_Update (void)
{
  top.sync.upd.sem->Post();
}

void SetOsc (double newfreq)
{
  char buffer[64];
  sprintf (buffer, "!setOsc %lf 0\n", newfreq);
  sendcommand (buffer);
}

void
SetTXOsc (double newfreq)
{
  char buffer[64];
  sprintf (buffer, "!setOsc %lf 1\n", newfreq);
  sendcommand (buffer);
}

void
SetNR (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setNR %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}


void
SetBlkNR (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setBlkNR %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}

void
SetNRvals (int taps, int delay, double gain, double leak)
{
  char buffer[64];
  sprintf (buffer, "!setNRvals %d %d %12.9lf %12.9lf\n", taps, delay,
	   gain, leak);
  sendcommand (buffer);
  sprintf (buffer, "!setBlkNRval %12.9lf\n", min (0.1 * gain, 0.0002));
  sendcommand (buffer);
}

void
SetTXCompandSt (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setTXCompandSt %d\n", setit ? 1 : 0);
  sendcommand (buffer);
}
void
SetTXCompand (double setit)
{
  char buffer[64];
  sprintf (buffer, "!setTXCompand %8.4lf\n", -setit);
  sendcommand (buffer);
}
void
SetTXSquelchSt (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setTXSquelchSt %d\n", setit ? 1 : 0);
  sendcommand (buffer);
}
void
SetTXSquelchVal (float setit)
{
  char buffer[64];
  sprintf (buffer, "!setTXSquelch %f\n", setit);
  sendcommand (buffer);
}

void
SetANF (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setANF %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}

void
SetBlkANF (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setBlkANF %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}


void
SetANFvals (int taps, int delay, double gain, double leak)
{
  char buffer[64];
  sprintf (buffer, "!setANFvals %d %d %12.9lf %12.9lf\n", taps, delay,
	   gain, leak);
  sendcommand (buffer);
  sprintf (buffer, "!setBlkANFval %12.9lf\n", min (0.1 * gain, 0.0002));
  sendcommand (buffer);
}

void
SetNB (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setNB %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}

void
SetNBvals (REAL threshold)
{
  char buffer[64];
  sprintf (buffer, "!setNBvals %f\n", threshold);
  sendcommand (buffer);
}

void
SetSDROM (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setSDROM %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}

void
SetSDROMvals (REAL threshold)
{
  char buffer[64];
  sprintf (buffer, "!setSDROMvals %f\n", threshold);
  sendcommand (buffer);
}


void
SetBIN (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setBIN %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}
void
SetRXAGC (AGCMODE setit)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGC %d\n", setit);
  sendcommand (buffer);
}

void
SetRXAGCAttack (int attack)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGCAttack %d\n", attack);
  sendcommand (buffer);
}
void
SetRXAGCDecay (int decay)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGCDecay %d\n", decay);
  sendcommand (buffer);
}
void
SetRXAGCHang (int decay)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGCHang %d\n", decay);
  sendcommand (buffer);
}

void
SetRXAGCSlope (int slope)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGCSlope %d\n", slope);
  sendcommand (buffer);
}

void
SetRXAGCHangThreshold (int hangthreshold)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGCHangThreshold %d\n", hangthreshold);
  sendcommand (buffer);
}

void
SetTXALCAttack (int attack)
{
  char buffer[64];
  sprintf (buffer, "!setTXALCAttack %d\n", attack);
  sendcommand (buffer);
}

void
SetTXCarrierLevel (double setit)
{
  char buffer[64];
  sprintf (buffer, "!setTXCarrierLevel %lf\n", setit);
  sendcommand (buffer);
}

void
SetTXALCDecay (int decay)
{
  char buffer[64];
  sprintf (buffer, "!setTXALCDecay %d\n", decay);
  sendcommand (buffer);
}

void
SetTXALCBot (double max_agc)
{
  char buffer[64];
  sprintf (buffer, "!setTXALCBot %f\n", dB2lin ((REAL) max_agc));
  sendcommand (buffer);
}

void
SetTXALCHang (int decay)
{
  char buffer[64];
  sprintf (buffer, "!setTXALCHang %d\n", decay);
  sendcommand (buffer);
}

void
SetTXLevelerSt (bool state)
{
  char buffer[64];
  sprintf (buffer, "!setTXLevelerSt %d\n", state ? 1 : 0);
  sendcommand (buffer);
}
void
SetTXLevelerAttack (int attack)
{
  char buffer[64];
  sprintf (buffer, "!setTXLevelerAttack %d\n", attack);
  sendcommand (buffer);
}
void
SetTXLevelerDecay (int decay)
{
  char buffer[64];
  sprintf (buffer, "!setTXLevelerDecay %d\n", decay);
  sendcommand (buffer);
}

void
SetTXLevelerTop (double top)
{
  char buffer[64];
  sprintf (buffer, "!setTXLevelerTop %f\n", dB2lin ((REAL) top));
  sendcommand (buffer);
}

void
SetTXLevelerHang (int decay)
{
  char buffer[64];
  sprintf (buffer, "!setTXLevelerHang %d\n", decay);
  sendcommand (buffer);
}

void
SetFixedAGC (double fixed_agc)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGCFix %f\n", dB2lin ((REAL) fixed_agc));
  sendcommand (buffer);
}

void
SetRXAGCTop (double max_agc)
{
  char buffer[64];
  sprintf (buffer, "!setRXAGCTop %f\n", dB2lin ((REAL) max_agc));
  sendcommand (buffer);
}



void
SetCorrectIQ (double phase, double gain)
{
  char buffer[64];
  sprintf (buffer, "!setcorrectIQ %6.2lf %6.2lf\n", phase, gain);
  sendcommand (buffer);
}
void
SetCorrectIQPhase (double phase)
{
  char buffer[64];
  sprintf (buffer, "!setcorrectIQphase %6.2lf\n", phase);
  sendcommand (buffer);
}
void
SetCorrectIQGain (double gain)
{
  char buffer[64];
  sprintf (buffer, "!setcorrectIQgain %6.2lf\n", gain);
  sendcommand (buffer);
}
void
SetCorrectTXIQ (double phase, double gain)
{
  char buffer[64];
  sprintf (buffer, "!setcorrectTXIQ %6.2lf %6.2lf\n", phase, gain);
  sendcommand (buffer);

}
void
SetCorrectTXIQPhase (double phase)
{
  char buffer[64];
  sprintf (buffer, "!setcorrectTXIQphase %6.2lf\n", phase);
  sendcommand (buffer);

}
void
SetCorrectTXIQGain (double gain)
{
  char buffer[64];
  sprintf (buffer, "!setcorrectTXIQgain %6.2lf\n", gain);
  sendcommand (buffer);
}


void
SetPWSmode (SPECTRUMtype setit)
{
  char buffer[64];
  sprintf (buffer, "!setSpectrumType %d\n", setit);
  sendcommand (buffer);
}


void
SetWindow (Windowtype Windowset)
{
  char buffer[64];
  sprintf (buffer, "!setSpectrumWindow %d\n", Windowset);
  sendcommand (buffer);
}

void
SetSpectrumPolyphase (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setSpectrumPolyphase %d\n", setit ? 1 : 0);
  sendcommand (buffer);
}

void
SetTXEQ (int *txeq)
{
  char buffer[256];
  sprintf (buffer, "!setTXEQ 0.0 %d 120.0 %d 230.0 %d 450.0 %d 800.0 \
%d 1150.0 %d 1450.0 %d 1800.0 %d 2150.0 \
%d 2450.0 %d 2800.0 %d 3250.0 %d 6000.0\n", txeq[0], txeq[1], txeq[2], txeq[3], txeq[4], txeq[5], txeq[6], txeq[7], txeq[8], txeq[9], txeq[10], txeq[11]);
  sendcommand (buffer);
}
void
SetGrphTXEQ (int *txeq)
{
  char buffer[256];
  sprintf (buffer, "!setGrphTXEQ %d %d %d %d\n", txeq[0], txeq[1],
	   txeq[2], txeq[3]);
  sendcommand (buffer);
}

void
SetGrphTXEQcmd (bool state)
{
  char buffer[64];
  sprintf (buffer, "!setGrphTXEQcmd %d\n", state ? 1 : 0);
  sendcommand (buffer);
}

void
SetNotch160 (bool state)
{
  char buffer[64];
  sprintf (buffer, "!setNotch160 %d\n", state ? 1 : 0);
  sendcommand (buffer);
}
void
SetGrphRXEQ (int *rxeq)
{
  char buffer[256];
  sprintf (buffer, "!setGrphRXEQ %d %d %d %d\n", rxeq[0], rxeq[1],
	   rxeq[2], rxeq[3]);
  sendcommand (buffer);
}

void
SetGrphRXEQcmd (bool state)
{
  char buffer[64];
  sprintf (buffer, "!setGrphRXEQcmd %d\n", state ? 1 : 0);
  sendcommand (buffer);
}

void
SetTXAGCFF (bool setit)
{
  char buffer[16];
  sprintf (buffer, "!setTXAGCFF %d\n", setit ? 1 : 0);
  sendcommand (buffer);
}
void
SetTXAGCFFCompression (REAL txc)
{
  char buffer[64];
  sprintf (buffer, "!setTXAGCFFCompression %12.10f\n", txc);
  sendcommand (buffer);
}


void
SetSquelchVal (float setit)
{
  char buffer[64];
  sprintf (buffer, "!setSquelch %f\n", setit);
  sendcommand (buffer);
  return;
}


void
SetSquelchState (bool setit)
{
  char buffer[64];
  sprintf (buffer, "!setSquelchSt %u\n", setit ? 1 : 0);
  sendcommand (buffer);
}

void
SetTRX (TRXMODE setit)
{
  top.sync.upd.sem->Wait();
  switch (setit) {
      case TX:
		  switch (tx.mode) {
			case CWU:
			case CWL:
				top.swch.bfct.want = 0;
				break;
			default:
				top.swch.bfct.want = (int) (2 * uni.samplerate / 48000);
				break;
		  }
	  case RX:
		 top.swch.bfct.want = (int) (1 * uni.samplerate / 48000);
		 break;
  }
  top.swch.trx.next = setit;
  top.swch.bfct.have = 0;
  if (top.state != RUN_SWCH)
    top.swch.run.last = top.state;
  top.state = RUN_SWCH;
  top.sync.upd.sem->Post();
}



void
SetTXAGCLimit (double limit)
{
  char buffer[64];
  sprintf (buffer, "!setTXAGCLimit %lf\n", limit);
  sendcommand (buffer);
}



void
Process_Spectrum (float *results)
{
  uni.spec.type = SPEC_POST_FILT;
  uni.spec.scale = SPEC_PWR;
  top.sync.upd.sem->Wait();
  snap_spectrum (&uni.spec, uni.spec.type);
  top.sync.upd.sem->Post();
  compute_spectrum (&uni.spec);
  memcpy ((void *) results, uni.spec.output, uni.spec.size * sizeof (float));
}
void
Process_Panadapter (float *results)
{
  uni.spec.type = SPEC_PRE_FILT;
  uni.spec.scale = SPEC_PWR;
  top.sync.upd.sem->Wait();
  snap_spectrum (&uni.spec, uni.spec.type);
  top.sync.upd.sem->Post();
  compute_spectrum (&uni.spec);
  memcpy ((void *) results, uni.spec.output, uni.spec.size * sizeof (float));
}

void
Process_Phase (float *results, int numpoints)
{
  int i, j;
  top.sync.upd.sem->Wait();
  uni.spec.type = SPEC_POST_AGC;
  uni.spec.scale = SPEC_PWR;
  uni.spec.rxk = 0;
  snap_scope (&uni.spec, uni.spec.type);
  top.sync.upd.sem->Post();
  for (i = 0, j = 0; i < numpoints; i++, j += 2)
    {
      results[j] = (float) CXBreal (uni.spec.timebuf, i);
      results[j + 1] = (float) CXBimag (uni.spec.timebuf, i);
    }
}

void
Process_Scope (float *results, int numpoints)
{
  int i;
  top.sync.upd.sem->Wait();
  uni.spec.type = SPEC_POST_AGC;
  uni.spec.scale = SPEC_PWR;
  uni.spec.rxk = 0;

  snap_scope (&uni.spec, uni.spec.type);
  top.sync.upd.sem->Post();
  for (i = 0; i < numpoints; i++)
    {
      results[i] = (float) CXBreal (uni.spec.timebuf, i);
    }
}

float
Calculate_Meters (METERTYPE mt)
{
  float returnval = 0;
  top.sync.upd.sem->Wait();
  if (uni.mode.trx == RX)
    {
      switch (mt)
		{
		case SIGNAL_STRENGTH:
			returnval = uni.meter.rx.val[RL][RX_SIGNAL_STRENGTH];
			break;
		case AVG_SIGNAL_STRENGTH:
			returnval = (float) uni.meter.rx.val[RL][RX_AVG_SIGNAL_STRENGTH];
			break;
		case ADC_REAL:
			returnval = (float) uni.meter.rx.val[RL][RX_ADC_REAL];
			break;
		case ADC_IMAG:
			returnval = (float) uni.meter.rx.val[RL][RX_ADC_IMAG];
			break;
		case AGC_GAIN:
			returnval = (float) uni.meter.rx.val[RL][RX_AGC_GAIN];
			break;
		default:
			returnval = -200;
			break;
		}
		//fprintf(stderr,"type=%d val=%f\n",mt,returnval),fflush(stderr);
    }
  else
    {
		switch(mt) {
			case MIC:
				returnval = (float) uni.meter.tx.val[TX_MIC];
				break;
			case PWR:
				returnval = (float) uni.meter.tx.val[TX_PWR];
				break;
			case ALC:
				returnval = (float) uni.meter.tx.val[TX_ALC];
				break;
			case EQtap:
				returnval = (float) uni.meter.tx.val[TX_EQtap];
				break;
			case LEVELER:
				returnval = (float) uni.meter.tx.val[TX_LEVELER];
				break;
			case COMP:
				returnval = (float) uni.meter.tx.val[TX_COMP];
				break;
			case CPDR:
				returnval = (float) uni.meter.tx.val[TX_CPDR];
				break;
			case ALC_G:
				returnval = (float) uni.meter.tx.val[TX_ALC_G];
				break;
			case LVL_G:
				returnval = (float) uni.meter.tx.val[TX_LVL_G];
				break;
			default:
				returnval = -200;
		}
    }
  top.sync.upd.sem->Post();
  return returnval;
}


void *
NewResampler (int samplerate_in, int samplerate_out)
{
  ResSt tmp;
  int lcm = 28224000, interpFactor, deciFactor;
  interpFactor = lcm / samplerate_in;
  deciFactor = lcm / samplerate_out;
  tmp = newPolyPhaseFIR (32768, 0, interpFactor, 0, deciFactor);
  return (void *) tmp;
}

void
DoResampler (COMPLEX * input, COMPLEX * output, int numsamps, int *outsamps,
	     ResSt ptr)
{
  ptr->input = input;
  ptr->output = output;
  ptr->inputArrayLength = numsamps;
  PolyPhaseFIR (ptr);
  *outsamps = ptr->numOutputSamples;
}

void
DelPolyPhaseFIR (ResSt resst)
{
  extern void delPolyPhaseFIR (ResSt resst);
  delPolyPhaseFIR (resst);
}


void *
NewResamplerF (int samplerate_in, int samplerate_out)
{
  ResStF tmp;
  int lcm = 28224000, interpFactor, deciFactor;
  interpFactor = lcm / samplerate_in;
  deciFactor = lcm / samplerate_out;
  tmp = newPolyPhaseFIRF (32768, 0, interpFactor, 0, deciFactor);
  return (void *) tmp;
}

void
DoResamplerF (float *input, float *output, int numsamps, int *outsamps,
	      ResStF ptr)
{
  ptr->input = input;
  ptr->output = output;
  ptr->inputArrayLength = numsamps;
  PolyPhaseFIRF (ptr);
  *outsamps = ptr->numOutputSamples;
}

void
DelPolyPhaseFIRF (ResStF resst)
{
  extern void delPolyPhaseFIRF (ResStF resst);
  delPolyPhaseFIRF (resst);
}
