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
				    1.0F,	// Target output 
				    2.0F,	// Attack time constant in ms
				    500,	// Decay time constant in ms
				    1.0,	// Slope
				    500,	//Hangtime in ms
				    uni.samplerate,	// Sample rate
				    31622.8F,	// Maximum gain as a multipler, linear not dB
				    0.00001F,	// Minimum gain as a multipler, linear not dB
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

  rx.ssb = new CSSBDemod(rx.buf.o, rx.buf.o);

  /* noise reduction */
  rx.anf.gen = new CLMS(rx.buf.o,	// CXB signal,
			    64,	// int delay,
			    0.01F,	// REAL adaptation_rate,
			    0.00001F,	// REAL leakage,
			    45,	// int adaptive_filter_size,
			    LMS_INTERFERENCE);

  rx.anf.flag = false;
  rx.banf.gen = new CBlockLMS(rx.buf.o, 0.00001f, 0.005f, BLMS_INTERFERENCE, uni.wisdom.bits);
  rx.banf.flag = false;

  rx.anr.gen = new CLMS(rx.buf.o,	// CXB signal,
			    64,	// int delay,
			    0.01f,	// REAL adaptation_rate,
			    0.00001f,	// REAL leakage,
			    45,	// int adaptive_filter_size,
			    LMS_NOISE);

  rx.anr.flag = false;
  rx.banr.gen = new CBlockLMS(rx.buf.o, 0.00001f, 0.005f, BLMS_NOISE, uni.wisdom.bits);
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

  rx.squelch.gen = new CSquelch(rx.buf.o, -150.0F, 0.0F, uni.buflen - 48);

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

  tx.am  = new CAMMod(0.5F, tx.buf.i, tx.buf.i);
  tx.fm  = new CFMMod(5000.0F, uni.samplerate, tx.buf.i, tx.buf.i);
  tx.ssb = new CSSBMod(tx.buf.i, tx.buf.i);

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

  tx.squelch.gen = new CSquelch(tx.buf.i, -40.0F, -30.0F, uni.buflen - 48);

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
  delete tx.squelch.gen;
  delCXB(tx.buf.o);
  delCXB(tx.buf.i);

  /* RX */
  delete rx.cpd.gen;
  delete rx.spot.gen;
  delete rx.agc.gen;
  delete rx.nb_sdrom.gen;
  delete rx.nb.gen;
  delete rx.grapheq.gen;
  delete rx.anf.gen;
  delete rx.anr.gen;
  delete rx.banf.gen;
  delete rx.banr.gen;
  delete rx.am;
  delete rx.fm;
  delete rx.osc.gen;
  delete rx.rit.gen;
  delete rx.filt;
  delete rx.iqfix;
  delete rx.squelch.gen;
  delCXB(rx.buf.o);
  delCXB(rx.buf.i);

  /* all */
  delete uni.spec.gen;
  delete uni.meter.gen;
}


/* all */
static void do_rx_meter(CXB* buf, RXMETERTAP tap)
{
	REAL agcGain = 0.0F;
	if (rx.agc.flag && rx.agc.gen != NULL)
		agcGain = rx.agc.gen->getGain();

	if (uni.meter.flag)
		uni.meter.gen->setRXMeter(tap, buf, agcGain);
}

static void do_tx_meter(CXB* buf, TXMETERTYPE type)
{
	REAL alcGain = 0.0F;
	if (tx.alc.flag && tx.alc.gen != NULL)
		alcGain = tx.alc.gen->getGain();

	REAL levelerGain = 0.0F;
	if (tx.leveler.flag && tx.leveler.gen != NULL)
		levelerGain = tx.leveler.gen->getGain();

	if (uni.meter.flag)
		uni.meter.gen->setTXMeter(type, buf, alcGain, levelerGain);
}

static void do_rx_spectrum(CXB* buf, SPECTRUMtype type)
{
	if (uni.spec.flag && type == uni.spec.type)
		uni.spec.gen->setData(buf);
}

static void do_tx_spectrum(CXB* buf)
{
	if (uni.spec.flag)
		uni.spec.gen->setData(buf);
}

//========================================================================
/* RX processing */

/* pre-condition for (nearly) all RX modes */
static void do_rx_pre()
{
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

	if (rx.tick == 0UL)
		rx.filt->reset();

	rx.filt->filter();

	CXBhave(rx.buf.o) = CXBhave(rx.buf.i);

	do_rx_meter(rx.buf.o, RXMETER_POST_FILT);
	do_rx_spectrum(rx.buf.o, SPEC_POST_FILT);

	if (rx.cpd.flag)
		rx.cpd.gen->process();

	if (rx.squelch.gen->isSquelch())
		rx.squelch.gen->doSquelch();
	else
		rx.agc.gen->process();

	do_rx_meter(rx.buf.o,RXMETER_POST_AGC);
	do_rx_spectrum(rx.buf.o, SPEC_POST_AGC);
}

static void do_rx_post()
{
	if (!rx.squelch.gen->isSet()) {
		rx.squelch.gen->noSquelch();

		// spotting tone
		if (rx.spot.flag) {
			// remember whether it's turned itself off during this pass
			rx.spot.flag = rx.spot.gen->generate();

			unsigned int n = CXBhave(rx.buf.o);

			for (unsigned int i = 0; i < n; i++)
				CXBdata(rx.buf.o, i) = Cadd(CXBdata(rx.buf.o, i), CXBdata(rx.spot.gen->getData(), i));
		}
	}

	if (rx.grapheq.flag)
		rx.grapheq.gen->equalise();

	do_rx_spectrum(rx.buf.o, SPEC_POST_DET);
}

/* demod processing */

static void do_rx_SBCW()
{
	rx.ssb->demodulate();

	if (rx.bin.flag) {
		if (rx.banr.flag && rx.anr.flag)
			rx.banr.gen->process();

		if (rx.banf.flag && rx.anf.flag)
			rx.banf.gen->process();
	} else {
		if (rx.anr.flag) {
			if (rx.banr.flag)
				rx.banr.gen->process();
			else
				rx.anr.gen->process();
		}

		if (rx.anf.flag) {
			if (rx.banf.flag)
				rx.banf.gen->process();
			else
				rx.anf.gen->process();
		}
/*
		for (unsigned int i = 0; i < CXBhave(rx.buf.o); i++)
			CXBimag(rx.buf.o, i) = CXBreal(rx.buf.o, i);
*/
	}
}

static void do_rx_AM()
{
	rx.am->demodulate();

	if (rx.anf.flag) {
		if (!rx.banf.flag)
			rx.anf.gen->process();
		else
			rx.banf.gen->process();
	}
}

static void do_rx_FM()
{
	rx.fm->demodulate();
}

/* overall dispatch for RX processing */
static void do_rx()
{
	do_rx_pre();

	switch (rx.mode) {
		case USB:
		case LSB:
		case CWU:
		case CWL:
			do_rx_SBCW();
			break;

		case AM:
		case SAM:
			do_rx_AM();
			break;

		case FMN:
			do_rx_FM();
			break;
	}

	do_rx_post ();
}

/* TX processing */
static void do_tx_pre()
{
/*
	unsigned int n = CXBhave (tx.buf.i);

	for (unsigned int i = 0; i < n; i++)
		CXBdata(tx.buf.i, i) = Cmplx(CXBimag(tx.buf.i, i), 0.0F);
*/
	if (tx.dcb.flag)
		tx.dcb.gen->block();

	do_tx_meter(tx.buf.i, TX_MIC);

	if (tx.squelch.gen->isSquelch()) {
		tx.squelch.gen->doSquelch();
	} else {
		if (!tx.squelch.gen->isSet())
			tx.squelch.gen->noSquelch();

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
	}

	if (tx.alc.flag)
		tx.alc.gen->process();

	do_tx_meter(tx.buf.i, TX_ALC);
}

static void do_tx_post()
{
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


/* general TX processing dispatch */
static void do_tx()
{
	do_tx_pre();

	switch (tx.mode) {
		case USB:
		case LSB:
		case CWU:
		case CWL:
			tx.ssb->modulate();
			break;

		case AM:
		case SAM:
			tx.am->modulate();
			break;

		case FMN:
			tx.fm->modulate();
			break;
	}

	do_tx_post();
}

//========================================================================
/* overall buffer processing;
   come here when there are buffers to work on */

void process_samples(float* bufi, float* bufq, unsigned int n)
{
	unsigned int i;

	switch (uni.mode.trx) {
		case RX:
			for (i = 0; i < n; i++) {
				CXBreal(rx.buf.i, i) = bufi[i];
				CXBimag(rx.buf.i, i) = bufq[i];
			}
			CXBhave(rx.buf.i) = n;

			// run the receiver
			do_rx();
			rx.tick++;

			n = CXBhave(rx.buf.o);
			for (i = 0; i < n; i++) {
				bufi[i] = CXBreal(rx.buf.o, i);
				bufq[i] = CXBimag(rx.buf.o, i);
			}
			break;

		case TX:
			for (i = 0; i < n; i++) {
				CXBreal(tx.buf.i, i) = bufi[i];
				CXBimag(tx.buf.i, i) = bufq[i];
			}
			CXBhave(tx.buf.i) = n;

			do_tx();
			tx.tick++;

			n = CXBhave(tx.buf.o);
			for (i = 0; i < n; i++) {
				bufi[i] = CXBreal(tx.buf.o, i);
				bufq[i] = CXBimag(tx.buf.o, i);
			}
			break;
	}

	uni.tick++;
}
