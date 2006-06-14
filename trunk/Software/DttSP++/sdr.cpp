/* sdr.cpp

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY.

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

void reset_meters()
{
	if (uni.meter.flag)
		uni.meter.gen->reset();
}

void reset_spectrum()
{
	if (uni.spec.flag)
		uni.spec.gen->reinitSpectrum();
}

void reset_counters()
{
	rx.tick = 0UL;
	tx.tick = 0UL;
}

/* global and general info,
   not specifically attached to
   tx, rx, or scheduling */
static void setup_all(REAL rate, unsigned int buflen, SDRMODE mode, unsigned int specsize, unsigned int cpdsize)
{
  uni.samplerate = rate;
  uni.buflen = buflen;
  uni.mode.sdr = mode;
  uni.mode.trx = RX;

  uni.wisdom.bits = FFTW_ESTIMATE;

  uni.meter.gen = new CMeter();
  uni.meter.flag = true;

  uni.spec.gen = new CSpectrum(specsize, uni.wisdom.bits, SPEC_PWR);
  uni.spec.type = SPEC_POST_FILT;
  uni.spec.flag = true;

  uni.cpdlen = cpdsize;

  uni.tick = 0UL;
}

/* purely rx */
static void setup_rx()
{
  /* conditioning */
  rx.iqfix = new CCorrectIQ();

  rx.filt = new CFilterOVSV(uni.buflen, uni.wisdom.bits, uni.samplerate, -4800.0F, 4800.0F);

  /* buffers */
  /* note we overload the internal filter buffers
     we just created */
  rx.buf.i = newCXB(rx.filt->fetchSize(), rx.filt->fetchPoint());
  rx.buf.o = newCXB(rx.filt->storeSize(), rx.filt->storePoint());

  /* conversion */
  rx.osc.gen = new COscillator(-uni.samplerate / 4.0F, 0.0, uni.samplerate);

  // RIT
  rx.rit.gen = new COscillator(0.0F, 0.0, uni.samplerate);

  rx.agc.gen = new CAGC(agcLONG,	// mode kept around for control reasons alone
				    rx.buf.o,	// input buffer
				    1.0f,	// Target output 
				    2.0f,	// Attack time constant in ms
				    500,	// Decay time constant in ms
				    1.0,	// Slope
				    500,	//Hangtime in ms
				    uni.samplerate,	// Sample rate
				    2511.886f,	// Maximum gain as a multipler, linear not dB
				    0.00001f,	// Minimum gain as a multipler, linear not dB
				    1.0 	// Set the current gain
    );

  rx.agc.flag = true;

  rx.grapheq.gen = new CGraphicEQ(rx.buf.o, uni.samplerate, uni.wisdom.bits);
  rx.grapheq.flag = false;


  /* demods */
  rx.am = new CAMDemod(uni.samplerate,	// REAL samprate
			 0.0F,	// REAL f_initial
			 -500.0F,	// REAL f_lobound,
			 500.0F,	// REAL f_hibound,
			 400.0F,	// REAL f_bandwid,
			 rx.buf.o,	// COMPLEX *ivec,
			 rx.buf.o,	// COMPLEX *ovec,
			 AMdet		// AM Mode AMdet == rectifier,
			 );			// SAMdet == synchronous detector

  rx.fm = new CFMDemod(uni.samplerate,	// REAL samprate
			 0.0F,	// REAL f_initial
			 -6000.0F,	// REAL f_lobound
			 6000.0F,	// REAL f_hibound
			 5000.0F,	// REAL f_bandwid
			 rx.buf.o,	// COMPLEX *ivec
			 rx.buf.o);	// COMPLEX *ovec

  /* noise reduction */
  rx.anf.gen = new_lmsr(rx.buf.o,	// CXB signal,
			    64,	// int delay,
			    0.01F,	// REAL adaptation_rate,
			    0.00001F,	// REAL leakage,
			    45,	// int adaptive_filter_size,
			    LMADF_INTERFERENCE);

  rx.anf.flag = false;
  rx.banf.gen =
    new_blms (rx.buf.o, 0.00001f, 0.005f, LMADF_INTERFERENCE,
	      uni.wisdom.bits);
  rx.banf.flag = false;

  rx.anr.gen = new_lmsr (rx.buf.o,	// CXB signal,
			    64,	// int delay,
			    0.01f,	// REAL adaptation_rate,
			    0.00001f,	// REAL leakage,
			    45,	// int adaptive_filter_size,
			    LMADF_NOISE);

  rx.anr.flag = false;
  rx.banr.gen = new_blms (rx.buf.o, 0.00001f, 0.005f, LMADF_NOISE, uni.wisdom.bits);
  rx.banr.flag = false;

  rx.nb.gen = new CNoiseBlanker(rx.buf.i, 3.3F);
  rx.nb.flag = false;

  rx.nb_sdrom.gen = new CNoiseBlanker(rx.buf.i, 2.5F);
  rx.nb_sdrom.flag = false;

  rx.spot.gen = new CSpotTone(-12.0,	// gain
				   700.0,	// freq
				   5.0,	// ms rise
				   5.0,	// ms fall
				   uni.buflen,	// length of spot tone buffer
				   uni.samplerate	// sample rate
    );
  rx.spot.flag = false;

  ::memset(&rx.squelch, 0x00, sizeof(rx.squelch));
  rx.squelch.thresh = -150.0;
  rx.squelch.power = 0.0;
  rx.squelch.flag = rx.squelch.running = rx.squelch.set = false;
  rx.squelch.num = uni.buflen - 48;

  rx.cpd.gen = new CCompand(uni.cpdlen, 0.0F, rx.buf.o);
  rx.cpd.flag = false;

  rx.mode = uni.mode.sdr;
  rx.bin.flag = false;

  {
    REAL pos = 0.5,		// 0 <= pos <= 1, left->right
      theta = (REAL) ((1.0 - pos) * M_PI / 2.0);
    rx.azim = Cmplx ((REAL) cos (theta), (IMAG) sin (theta));
  }

  rx.tick = 0UL;
}

/* purely tx */
static void setup_tx()
{
  /* conditioning */
  tx.iqfix = new CCorrectIQ();

  tx.filt = new CFilterOVSV(uni.buflen, uni.wisdom.bits, uni.samplerate, 300.0F, 3000.0F);

  /* buffers */
  tx.buf.i = newCXB(tx.filt->fetchSize(), tx.filt->fetchPoint());
  tx.buf.o = newCXB(tx.filt->storeSize(), tx.filt->storePoint());

  tx.dcb.flag = false;
  tx.dcb.gen = new CDCBlock(DCB_MED, tx.buf.i);

  /* conversion */
  tx.osc.phase = 0.0;
  tx.osc.gen = new COscillator(0.0, tx.osc.phase, uni.samplerate);

  tx.am = new CAMMod(0.5F, tx.buf.i, tx.buf.i);

  tx.fm = new CFMMod(5000.0F, uni.samplerate, tx.buf.i, tx.buf.i);

  tx.leveler.gen = new CAGC(agcLONG,	// mode kept around for control reasons
				tx.buf.i,	// input buffer
				1.1f,	// Target output
				2,	// Attack time constant in ms
				500,	// Decay time constant in ms
				1,	// Slope
				500,	//Hangtime in ms
				uni.samplerate,	// Sample rate
				5.62f,	// Maximum gain as a multipler, linear not dB
				1.0,	// Minimum gain as a multipler, linear not dB
				1.0		// Set the current gain
    );
  tx.leveler.flag = true;

  tx.grapheq.gen = new CGraphicEQ(tx.buf.i, uni.samplerate, uni.wisdom.bits);
  tx.grapheq.flag = false;


  ::memset(&tx.squelch, 0x00, sizeof(tx.squelch));
  tx.squelch.thresh = -40.0;
  tx.squelch.power = 0.0;
  tx.squelch.flag = false;
  tx.squelch.running = tx.squelch.set = false;
  tx.squelch.num = uni.buflen - 48;

  tx.alc.gen = new CAGC(agcLONG,	// mode kept around for control reasons alone
			    tx.buf.i,	// input buffer
			    1.2f,	// Target output 
			    2,	// Attack time constant in ms
			    10,	// Decay time constant in ms
			    1,	// Slope
			    500,	//Hangtime in ms
			    uni.samplerate, 1.0,	// Maximum gain as a multipler, linear not dB
			    .000001f,	// Minimum gain as a multipler, linear not dB
			    1.0		// Set the current gain
    );
  tx.alc.flag = true;

  tx.spr.gen = new CSpeechProc(0.4F, 3.0, tx.buf.i);
  tx.spr.flag = false;

  tx.cpd.gen = new CCompand(uni.cpdlen, -3.0F, tx.buf.i);
  tx.cpd.flag = false;

  tx.mode = uni.mode.sdr;

  tx.tick = 0UL;
  /* not much else to do for TX */
}

/* how the outside world sees it */
void setup_workspace(REAL rate, unsigned int buflen, SDRMODE mode, unsigned int specsize, unsigned int cpdsize)
{
	setup_all(rate, buflen, mode, specsize, cpdsize);

	setup_rx();

	setup_tx();
}

void destroy_workspace()
{
  /* TX */
  delete tx.cpd.gen;
  delete tx.spr.gen;
  delete tx.leveler.gen;
  delete tx.alc.gen;
  delete tx.grapheq.gen;
  delete tx.osc.gen;
  delete tx.dcb.gen;
  delete tx.filt;
  delete tx.iqfix;
  delCXB(tx.buf.o);
  delCXB(tx.buf.i);

  /* RX */
  delete rx.cpd.gen;
  delete rx.spot.gen;
  delete rx.agc.gen;
  delete rx.nb_sdrom.gen;
  delete rx.nb.gen;
  delete rx.grapheq.gen;
  del_lmsr (rx.anf.gen);
  del_lmsr (rx.anr.gen);
  del_blms (rx.banf.gen);
  del_blms (rx.banr.gen);
  delete rx.am;
  delete rx.fm;
  delete rx.osc.gen;
  delete rx.rit.gen;
  delete rx.filt;
  delete rx.iqfix;
  delCXB(rx.buf.o);
  delCXB(rx.buf.i);

  /* all */
  delete uni.spec.gen;
  delete uni.meter.gen;
}


/* all */
static void do_rx_meter(CXB* buf, RXMETERTAP tap)
{
	wxASSERT(buf != NULL);

	REAL agcGain = 0.0F;
	if (rx.agc.flag && rx.agc.gen != NULL)
		agcGain = rx.agc.gen->getGain();

	if (uni.meter.flag) {
		wxASSERT(uni.meter.gen != NULL);
		uni.meter.gen->setRXMeter(tap, buf, agcGain);
	}
}

static void do_tx_meter(CXB* buf, TXMETERTYPE type)
{
	wxASSERT(buf != NULL);

	REAL alcGain = 0.0F;
	if (tx.alc.flag && tx.alc.gen != NULL)
		alcGain = tx.alc.gen->getGain();

	REAL levelerGain = 0.0F;
	if (tx.leveler.flag && tx.leveler.gen != NULL)
		levelerGain = tx.leveler.gen->getGain();

	if (uni.meter.flag) {
		wxASSERT(uni.meter.gen != NULL);
		uni.meter.gen->setTXMeter(type, buf, alcGain, levelerGain);
	}
}

static void do_rx_spectrum(CXB* buf, SPECTRUMtype type)
{
	wxASSERT(buf != NULL);

	if (uni.spec.flag && type == uni.spec.type) {
		wxASSERT(uni.spec.gen != NULL);
		uni.spec.gen->setData(buf);
	}
}

static void do_tx_spectrum(CXB* buf)
{
	wxASSERT(buf != NULL);

	if (uni.spec.flag) {
		wxASSERT(uni.spec.gen != NULL);
		uni.spec.gen->setData(buf);
	}
}

//========================================================================
/* RX processing */

static bool should_do_rx_squelch()
{
  if (rx.squelch.flag)
    {
      int i, n = CXBhave(rx.buf.o);
      rx.squelch.power = 0.0;

      for (i = 0; i < n; i++)
	rx.squelch.power += Csqrmag (CXBdata (rx.buf.o, i));

      return
	10.0 * log10 (rx.squelch.power + 1e-17) < rx.squelch.thresh;

    }
  else
    return rx.squelch.set = false;
}

static bool should_do_tx_squelch()
{
  if (tx.squelch.flag)
    {
      int i, n = CXBhave(tx.buf.i);
      tx.squelch.power = 0.0;

      for (i = 0; i < n; i++)
	tx.squelch.power += Csqrmag (CXBdata (tx.buf.i, i));

      return
	(-30 + 10.0 * log10 (tx.squelch.power + 1e-17)) < tx.squelch.thresh;

    }
  else
    return tx.squelch.set = false;
}

// apply squelch
// slew into silence first time

static void do_squelch()
{
  rx.squelch.set = true;

  if (!rx.squelch.running)
    {
      int i, m = rx.squelch.num, n = CXBhave (rx.buf.o) - m;

      for (i = 0; i < m; i++)
	CXBdata (rx.buf.o, i) =
	  Cscl (CXBdata (rx.buf.o, i), (REAL) (1.0 - (REAL) i / m));

      memset ((void *) (CXBbase (rx.buf.o) + m), 0x00, n * sizeof (COMPLEX));
      rx.squelch.running = true;

    }
  else
    memset ((void *) CXBbase (rx.buf.o),
	    0x00, CXBhave (rx.buf.o) * sizeof (COMPLEX));
}
static void do_tx_squelch()
{
  tx.squelch.set = true;

  if (!tx.squelch.running)
    {
      int i, m = tx.squelch.num, n = CXBhave (tx.buf.i) - m;

      for (i = 0; i < m; i++)
	CXBdata (tx.buf.i, i) =
	  Cscl (CXBdata (tx.buf.i, i), (REAL) (1.0 - (REAL) i / m));

      memset ((void *) (CXBbase (tx.buf.i) + m), 0x00, n * sizeof (COMPLEX));
      tx.squelch.running = true;

    }
  else
    memset ((void *) CXBbase (tx.buf.i),
	    0x00, CXBhave (tx.buf.i) * sizeof (COMPLEX));
}

// lift squelch
// slew out from silence to full scale

static void no_squelch()
{
  if (rx.squelch.running)
    {
      int i, m = rx.squelch.num;

      for (i = 0; i < m; i++)
	CXBdata (rx.buf.o, i) =
	  Cscl (CXBdata (rx.buf.o, i), (REAL) i / m);

      rx.squelch.running = false;
    }
}
static void no_tx_squelch()
{
  if (tx.squelch.running)
    {
      int i, m = tx.squelch.num;

      for (i = 0; i < m; i++)
	CXBdata (tx.buf.i, i) = Cscl (CXBdata (tx.buf.i, i), (REAL) i / m);

      tx.squelch.running = false;
    }
}

/* pre-condition for (nearly) all RX modes */
static void do_rx_pre()
{
	wxASSERT(rx.nb.gen != NULL);
	wxASSERT(rx.nb_sdrom.gen != NULL);
	wxASSERT(rx.iqfix != NULL);
	wxASSERT(rx.osc.gen != NULL);
	wxASSERT(rx.rit.gen != NULL);
	wxASSERT(rx.filt != NULL);
	wxASSERT(rx.cpd.gen != NULL);
	wxASSERT(rx.agc.gen != NULL);

	if (rx.nb.flag)
		rx.nb.gen->blank();

	if (rx.nb_sdrom.flag)
		rx.nb_sdrom.gen->sdromBlank();

	// metering for uncorrected values here
	do_rx_meter(rx.buf.i, RXMETER_PRE_CONV);

	rx.iqfix->process(rx.buf.i);

	/* 2nd IF conversion happens here */
	rx.osc.gen->mix(rx.buf.i);

	/* filtering, metering, spectrum, squelch, & AGC */
	do_rx_spectrum(rx.buf.i, SPEC_PRE_FILT);

	/* IF shift */
	rx.rit.gen->mix(rx.buf.i);

	if (rx.mode != SPEC) {
		if (rx.tick == 0UL)
			rx.filt->reset();

		rx.filt->filter();
	} else {
		::memcpy(CXBbase(rx.buf.o), CXBbase(rx.buf.i), sizeof(COMPLEX) * CXBhave(rx.buf.i));
	}

	CXBhave(rx.buf.o) = CXBhave(rx.buf.i);

	do_rx_meter(rx.buf.o, RXMETER_POST_FILT);
	do_rx_spectrum(rx.buf.o, SPEC_POST_FILT);

	if (rx.cpd.flag)
		rx.cpd.gen->process();

	if (should_do_rx_squelch())
		do_squelch();
	else
		rx.agc.gen->process();

	do_rx_meter(rx.buf.o,RXMETER_POST_AGC);
	do_rx_spectrum(rx.buf.o, SPEC_POST_AGC);
}

static void do_rx_post()
{
	wxASSERT(rx.spot.gen != NULL);
	wxASSERT(rx.grapheq.gen != NULL);

	if (!rx.squelch.set) {
		no_squelch();

		// spotting tone
		if (rx.spot.flag) {
			// remember whether it's turned itself off during this pass
			rx.spot.flag = rx.spot.gen->generate();

			unsigned int n = CXBhave(rx.buf.o);

			for (unsigned int i = 0; i < n; i++)
				CXBdata(rx.buf.o, i) = Cadd(CXBdata(rx.buf.o, i), CXBdata(rx.spot.gen->getData(), i));
		}

		if (rx.grapheq.flag)
			rx.grapheq.gen->equalise();
	}
}

/* demod processing */

static void do_rx_SBCW()
{
	if (rx.bin.flag) {
		if (rx.banr.flag && rx.anr.flag)
			blms_adapt(rx.banr.gen);

		if (rx.banf.flag && rx.anf.flag)
			blms_adapt(rx.banf.gen);
	} else {
		if (rx.anr.flag) {
			if (rx.banr.flag)
				blms_adapt(rx.banr.gen);
			else
				lmsr_adapt(rx.anr.gen);
		}

		if (rx.anf.flag) {
			if (rx.banf.flag)
				blms_adapt(rx.banf.gen);
			else
				lmsr_adapt(rx.anf.gen);
		}
/*
		for (unsigned int i = 0; i < CXBhave(rx.buf.o); i++)
			CXBimag(rx.buf.o, i) = CXBreal(rx.buf.o, i);
*/
	}
}

static void do_rx_AM()
{
	wxASSERT(rx.am != NULL);

	rx.am->demodulate();

	if (rx.anf.flag) {
		if (!rx.banf.flag)
			lmsr_adapt (rx.anf.gen);
		else
			blms_adapt (rx.banf.gen);
	}
}

static void do_rx_FM()
{
	wxASSERT(rx.fm != NULL);

	rx.fm->demodulate();
}

static void do_rx_DRM()
{
}

static void do_rx_SPEC()
{
}

/* overall dispatch for RX processing */

static void do_rx()
{
	do_rx_pre();

	switch (rx.mode) {
		case DIGU:
		case DIGL:
		case USB:
		case LSB:
		case CWU:
		case CWL:
		case DSB:
			do_rx_SBCW();
			break;

		case AM:
		case SAM:
			do_rx_AM();
			break;

		case FMN:
			do_rx_FM();
			break;

		case DRM:
			do_rx_DRM();
			break;

		case SPEC:
		default:
			do_rx_SPEC();
			break;
	}

	do_rx_post ();
}

/* TX processing */
static void do_tx_pre()
{
	wxASSERT(tx.dcb.flag != NULL);
	wxASSERT(tx.grapheq.gen != NULL);
	wxASSERT(tx.leveler.gen != NULL);
	wxASSERT(tx.spr.gen != NULL);
	wxASSERT(tx.cpd.gen != NULL);
/*
	unsigned int n = CXBhave (tx.buf.i);

	for (unsigned int i = 0; i < n; i++)
		CXBdata(tx.buf.i, i) = Cmplx(CXBimag(tx.buf.i, i), 0.0F);
*/
	if (tx.dcb.flag)
		tx.dcb.gen->block();

	do_tx_meter(tx.buf.i, TX_MIC);

	if (should_do_tx_squelch ()) {
		do_tx_squelch ();
	} else {
		if (!tx.squelch.set)
			no_tx_squelch();

		if (tx.mode != DIGU && tx.mode != DIGL) {
			if (tx.grapheq.flag)
				tx.grapheq.gen->equalise();

			do_tx_meter(tx.buf.i, TX_EQtap);

			if (tx.leveler.flag)
				tx.leveler.gen->process();

			do_tx_meter(tx.buf.i, TX_LEVELER);

			if (tx.spr.flag)
				tx.spr.gen->process();

			do_tx_meter(tx.buf.i, TX_COMP);

			if (tx.cpd.flag)
				tx.cpd.gen->process();

			do_tx_meter(tx.buf.i, TX_CPDR);
		} else {
			do_tx_meter(tx.buf.i, TX_EQtap);
			do_tx_meter(tx.buf.i, TX_LEVELER);
			do_tx_meter(tx.buf.i, TX_COMP);
			do_tx_meter(tx.buf.i, TX_CPDR);
		}
	}
}

static void do_tx_post()
{
	wxASSERT(tx.filt != NULL);
	wxASSERT(tx.osc.gen != NULL);
	wxASSERT(tx.iqfix != NULL);

	CXBhave(tx.buf.o) = CXBhave(tx.buf.i);

	if (tx.tick == 0UL)
		tx.filt->reset();

	tx.filt->filter();

	if (uni.spec.flag)
		do_tx_spectrum(tx.buf.o);

	tx.osc.gen->mix(tx.buf.o);

	tx.iqfix->process(tx.buf.o);

	do_tx_meter(tx.buf.o, TX_PWR);
}

/* modulator processing */

static void do_tx_SBCW()
{
	if (tx.alc.flag && tx.mode != DIGU && tx.mode != DIGL)
		tx.alc.gen->process();

	do_tx_meter(tx.buf.i, TX_ALC);

	if (tx.mode != DSB)
		CXBscl(tx.buf.i, 2.0F);
}

static void do_tx_AM()
{
	if (tx.alc.flag)
		tx.alc.gen->process();

	do_tx_meter(tx.buf.i, TX_ALC);

	tx.am->modulate();
}

static void do_tx_FM()
{
	if (tx.alc.flag)
		tx.alc.gen->process();

	do_tx_meter(tx.buf.i, TX_ALC);

	tx.fm->modulate();
}

static void do_tx_NIL()
{
	unsigned int n = CXBhave(tx.buf.i);

	for (unsigned int i = 0; i < n; i++)
		CXBdata(tx.buf.i, i) = cxzero;
}

/* general TX processing dispatch */

static void do_tx()
{
	do_tx_pre();

	switch (tx.mode) {
		case USB:
		case LSB:
		case CWU:
		case CWL:
		case DIGU:
		case DIGL:
		case DSB:
			do_tx_SBCW();
			break;

		case AM:
		case SAM:
			do_tx_AM();
			break;

		case FMN:
			do_tx_FM();
			break;

		case DRM:
		case SPEC:
		default:
			do_tx_NIL();
			break;
	}

	do_tx_post();
}

//========================================================================
/* overall buffer processing;
   come here when there are buffers to work on */

void process_samples(float* bufl, float* bufr, unsigned int n)
{
	unsigned int i;

	switch (uni.mode.trx) {
		case RX:
			for (i = 0; i < n; i++) {
				CXBimag(rx.buf.i, i) = bufl[i];
				CXBreal(rx.buf.i, i) = bufr[i];
			}
			CXBhave(rx.buf.i) = n;

			// run the receiver
			do_rx();
			rx.tick++;

			n = CXBhave(rx.buf.o);
			for (i = 0; i < n; i++) {
				bufl[i] = CXBimag(rx.buf.o, i);
				bufr[i] = CXBreal(rx.buf.o, i);
			}
			break;

		case TX:
			for (i = 0; i < n; i++) {
				CXBimag(tx.buf.i, i) = bufl[i];
				CXBreal(tx.buf.i, i) = bufr[i];
			}
			CXBhave(tx.buf.i) = n;

			do_tx();
			tx.tick++;

			n = CXBhave(tx.buf.o);
			for (i = 0; i < n; i++) {
				bufl[i] = CXBimag(tx.buf.o, i);
				bufr[i] = CXBreal(tx.buf.o, i);
			}
			break;
	}

	uni.tick++;
}
