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
	uni.meter.gen->reset();
}

void reset_spectrum()
{
	uni.spec.gen->reinitSpectrum();
}

void reset_counters()
{
	rx.tick = 0UL;
}

/* global and general info,
   not specifically attached to
   tx, rx, or scheduling */
static void setup_all(float rate, unsigned int buflen, SDRMODE mode, unsigned int specsize, unsigned int cpdsize)
{
  uni.samplerate = rate;
  uni.buflen = buflen;
  uni.mode.sdr = mode;
  uni.mode.trx = RX;

  uni.wisdom.bits = FFTW_ESTIMATE;

  uni.meter.gen = new CMeter();

  uni.spec.gen = new CSpectrum(specsize, uni.wisdom.bits, SPEC_PWR);
  uni.spec.type = SPEC_POST_FILT;

  uni.cpdlen = cpdsize;

  uni.tick = 0UL;
}

/* purely rx */
static void setup_rx()
{
  rx.filt = new CFilterOVSV(uni.buflen, uni.wisdom.bits, uni.samplerate, -4800.0F, 4800.0F);

  /* buffers */
  /* note we overload the internal filter buffers
     we just created */
  rx.buf.i = newCXB(rx.filt->fetchSize(), rx.filt->fetchPoint());
  rx.buf.o = newCXB(rx.filt->storeSize(), rx.filt->storePoint());

  /* conditioning */
  rx.iqfix = new CCorrectIQ(rx.buf.i);

  /* conversion */
  rx.osc.gen = new COscillator(rx.buf.i, -uni.samplerate / 4.0F, 0.0, uni.samplerate);

  // RIT
  rx.rit.gen = new COscillator(rx.buf.i, 0.0F, 0.0, uni.samplerate);

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
  rx.am = new CAMDemod(uni.samplerate,	// float samprate
			 0.0F,	// float f_initial
			 -500.0F,	// float f_lobound,
			 500.0F,	// float f_hibound,
			 400.0F,	// float f_bandwid,
			 rx.buf.o,	// COMPLEX *ivec,
			 rx.buf.o,	// COMPLEX *ovec,
			 AMdet		// AM Mode AMdet == rectifier,
			 );			// SAMdet == synchronous detector

  rx.fm = new CFMDemod(uni.samplerate,	// float samprate
			 0.0F,	// float f_initial
			 -6000.0F,	// float f_lobound
			 6000.0F,	// float f_hibound
			 5000.0F,	// float f_bandwid
			 rx.buf.o,	// COMPLEX *ivec
			 rx.buf.o);	// COMPLEX *ovec

  rx.ssb = new CSSBDemod(rx.buf.o, rx.buf.o);

  /* noise reduction */
  rx.anf.gen = new CLMS(rx.buf.o,	// CXB signal,
			    64,	// int delay,
			    0.01F,	// float adaptation_rate,
			    0.00001F,	// float leakage,
			    45,	// int adaptive_filter_size,
			    LMS_INTERFERENCE);

  rx.anf.flag = false;
  rx.banf.gen = new CBlockLMS(rx.buf.o, 0.00001f, 0.005f, BLMS_INTERFERENCE, uni.wisdom.bits);
  rx.banf.flag = false;

  rx.anr.gen = new CLMS(rx.buf.o,	// CXB signal,
			    64,	// int delay,
			    0.01f,	// float adaptation_rate,
			    0.00001f,	// float leakage,
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
    float pos = 0.5,		// 0 <= pos <= 1, left->right
      theta = (float) ((1.0 - pos) * M_PI / 2.0);
    rx.azim = Cmplx ((float) cos (theta), (float) sin (theta));
  }

  rx.tick = 0UL;
}

/* how the outside world sees it */
void setup_workspace(float rate, unsigned int buflen, SDRMODE mode, unsigned int specsize, unsigned int cpdsize)
{
	setup_all(rate, buflen, mode, specsize, cpdsize);

	setup_rx();

	tx = new CTX(uni.buflen, uni.wisdom.bits, uni.cpdlen, uni.samplerate, uni.meter.gen, uni.spec.gen);
}

void destroy_workspace()
{
  delete tx;

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
	float agcGain = 0.0F;
	if (rx.agc.flag && rx.agc.gen != NULL)
		agcGain = rx.agc.gen->getGain();

	uni.meter.gen->setRXMeter(tap, buf, agcGain);
}

static void do_rx_spectrum(CXB* buf, SPECTRUMtype type)
{
	if (type == uni.spec.type)
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

	rx.iqfix->process();

	/* 2nd IF conversion happens here */
	rx.osc.gen->mix();

	/* filtering, metering, spectrum, squelch, & AGC */
	do_rx_spectrum(rx.buf.i, SPEC_PRE_FILT);

	/* IF shift */
	rx.rit.gen->mix();

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

		case TX: {
				CXB* iBuf = tx->getIBuf();
				CXB* oBuf = tx->getOBuf();

				for (i = 0; i < n; i++) {
					CXBreal(iBuf, i) = bufi[i];
					CXBimag(iBuf, i) = bufq[i];
				}
				CXBhave(iBuf) = n;

				tx->process();

				n = CXBhave(oBuf);
				for (i = 0; i < n; i++) {
					bufi[i] = CXBreal(oBuf, i);
					bufq[i] = CXBimag(oBuf, i);
				}
			}
			break;
	}

	uni.tick++;
}
