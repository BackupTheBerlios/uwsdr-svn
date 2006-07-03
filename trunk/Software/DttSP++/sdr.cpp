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

  uni.cpdlen = cpdsize;

  uni.tick = 0UL;
}

/* how the outside world sees it */
void setup_workspace(float rate, unsigned int buflen, SDRMODE mode, unsigned int specsize, unsigned int cpdsize)
{
	setup_all(rate, buflen, mode, specsize, cpdsize);

	rx = new CRX(uni.buflen, uni.wisdom.bits, uni.cpdlen, uni.samplerate, uni.meter.gen, uni.spec.gen);

	tx = new CTX(uni.buflen, uni.wisdom.bits, uni.cpdlen, uni.samplerate, uni.meter.gen, uni.spec.gen);
}

void destroy_workspace()
{
  delete tx;

  delete rx;

  /* all */
  delete uni.spec.gen;
  delete uni.meter.gen;
}

//========================================================================
/* overall buffer processing;
   come here when there are buffers to work on */

void process_samples(float* bufi, float* bufq, unsigned int n)
{
	unsigned int i;

	switch (uni.mode.trx) {
		case RX: {
				CXB* iBuf = rx->getIBuf();
				CXB* oBuf = rx->getOBuf();

				for (i = 0; i < n; i++) {
					CXBreal(iBuf, i) = bufi[i];
					CXBimag(iBuf, i) = bufq[i];
				}
				CXBhave(iBuf) = n;

				rx->process();

				n = CXBhave(oBuf);
				for (i = 0; i < n; i++) {
					bufi[i] = CXBreal(oBuf, i);
					bufq[i] = CXBimag(oBuf, i);
				}
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
