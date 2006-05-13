/* lmadf.cpp

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

#include "lmadf.h"
#include "banal.h"


LMSR* new_lmsr(CXB* signal,
	  int delay,
	  REAL adaptation_rate,
	  REAL leakage, int adaptive_filter_size, int filter_type)
{
	LMSR* lms = new LMSR;

	lms->signal = signal;
	lms->signal_size = CXBsize(lms->signal);
	lms->delay = delay;
	lms->size = 512;
	lms->mask = lms->size - 1;
	lms->delay_line = newvec_REAL(lms->size);
	lms->adaptation_rate = adaptation_rate;
	lms->leakage = leakage;
	lms->adaptive_filter_size = adaptive_filter_size;
	lms->adaptive_filter = newvec_REAL(128);
	lms->filter_type = filter_type;
	lms->delay_line_ptr = 0;

	return lms;
}

void del_lmsr(LMSR* lms)
{
	if (lms != NULL) {
		delvec_REAL(lms->delay_line);
		delvec_REAL(lms->adaptive_filter);
		delete lms;
	}
}

// just to make the algorithm itself a little clearer,
// get the admin stuff out of the way

#define ssiz (lms->signal_size)
#define asiz (lms->adaptive_filter_size)
#define dptr (lms->delay_line_ptr)
#define rate (lms->adaptation_rate)
#define leak (lms->leakage)

#define ssig(n) (CXBreal(lms->signal,(n)))
#define ssig_i(n) (CXBimag(lms->signal,(n)))

#define dlay(n) (lms->delay_line[(n)])

#define afil(n) (lms->adaptive_filter[(n)])
#define wrap(n) (((n) + (lms->delay) + (lms->delay_line_ptr)) & (lms->mask))
#define bump(n) (((n) + (lms->mask)) & (lms->mask))

PRIVATE void lmsr_adapt_i(LMSR* lms)
{
	REAL scl1 = REAL(1.0 - rate * leak);

	for (int i = 0; i < ssiz; i++) {
		dlay(dptr) = ssig(i);
		REAL accum = 0.0;
		REAL sum_sq = 0.0;

		int j;
		for (j = 0; j < asiz; j++) {
			int k = wrap(j);
			sum_sq += sqr(dlay(k));
			accum += afil(j) * dlay(k);
		}

		REAL error = ssig(i) - accum;
		ssig_i(i) = ssig(i) = error;

		REAL scl2 = REAL(rate / (sum_sq + 1e-10));
		error *= scl2;

		for (j = 0; j < asiz; j++) {
			int k = wrap(j);
			afil(j) = afil(j) * scl1 + error * dlay(k);
		}

		dptr = bump(dptr);
	}
}

PRIVATE void lmsr_adapt_n(LMSR* lms)
{
	REAL scl1 = REAL(1.0 - rate * leak);

	for (int i = 0; i < ssiz; i++) {
		dlay(dptr) = ssig(i);
		REAL accum = 0.0;
		REAL sum_sq = 0.0;

		int j;
		for (j = 0; j < asiz; j++) {
			int k = wrap(j);
			sum_sq += sqr(dlay(k));
			accum += afil(j) * dlay(k);
		}

		REAL error = ssig(i) - accum;
		ssig_i (i) = ssig(i) = accum;

		REAL scl2 = REAL(rate / (sum_sq + 1e-10));
		error *= scl2;

		for (j = 0; j < asiz; j++) {
			int k = wrap (j);
			afil(j) = afil(j) * scl1 + error * dlay(k);
		}

		dptr = bump(dptr);
	}
}


void lmsr_adapt(LMSR* lms)
{
	switch (lms->filter_type) {

		case LMADF_NOISE:
			lmsr_adapt_n(lms);
			break;

		case LMADF_INTERFERENCE:
			lmsr_adapt_i(lms);
			break;
	}
}

void del_blms(BLMS* blms)
{
	if (blms != NULL) {
		fftwf_destroy_plan(blms->Xplan);
		fftwf_destroy_plan(blms->Yplan);
		fftwf_destroy_plan(blms->Errhatplan);
		fftwf_destroy_plan(blms->UPDplan);
		fftwf_destroy_plan(blms->Wplan);

		delvec_COMPLEX(blms->update);
		delvec_COMPLEX(blms->Update);
		delvec_COMPLEX(blms->What);
		delvec_COMPLEX(blms->Xhat);
		delvec_COMPLEX(blms->error);
		delvec_COMPLEX(blms->Errhat);
		delvec_COMPLEX(blms->Yhat);
		delvec_COMPLEX(blms->y);
		delvec_COMPLEX(blms->delay_line);

		delete blms;
	}
}

BLMS* new_blms(CXB* signal, REAL adaptation_rate, REAL leak_rate, int filter_type,
	  int pbits)
{
	BLMS* tmp = new BLMS;

	tmp->delay_line = newvec_COMPLEX(256);
	tmp->y = newvec_COMPLEX(256);
	tmp->Yhat = newvec_COMPLEX(256);
	tmp->Errhat = newvec_COMPLEX(256);
	tmp->error = newvec_COMPLEX(256);
	tmp->Xhat = newvec_COMPLEX(256);
	tmp->What = newvec_COMPLEX(256);
	tmp->Update = newvec_COMPLEX(256);
	tmp->update = newvec_COMPLEX(256);
	tmp->adaptation_rate = adaptation_rate;
	tmp->leak_rate = 1.0f - leak_rate;
	tmp->signal = signal;
	tmp->filter_type = filter_type;

	tmp->Xplan = fftwf_plan_dft_1d(256,
		(fftwf_complex *)tmp->delay_line,
		(fftwf_complex *)tmp->Xhat,
		FFTW_FORWARD, pbits);

	tmp->Yplan = fftwf_plan_dft_1d(256,
		(fftwf_complex *)tmp->Yhat,
		(fftwf_complex *)tmp->y,
		FFTW_BACKWARD, pbits);

	tmp->Errhatplan = fftwf_plan_dft_1d(256,
		(fftwf_complex *)tmp->error,
		(fftwf_complex *)tmp->Errhat,
		FFTW_FORWARD, pbits);

	tmp->UPDplan = fftwf_plan_dft_1d(256,
		(fftwf_complex *)tmp->Errhat,
		(fftwf_complex *)tmp->update,
		FFTW_BACKWARD, pbits);

	tmp->Wplan = fftwf_plan_dft_1d(256,
		(fftwf_complex *)tmp->update,
		(fftwf_complex *)tmp->Update,
		FFTW_FORWARD, pbits);

	return tmp;
}

const float BLKSCL = 1.0f / 256.0f;

void blms_adapt(BLMS* blms)
{
	int sigsize = CXBhave(blms->signal);
	int sigidx = 0;

	do {
		::memcpy(blms->delay_line, &blms->delay_line[128], sizeof(COMPLEX) * 128);	// do overlap move
		::memcpy(&blms->delay_line[128], &CXBdata(blms->signal, sigidx), sizeof(COMPLEX) * 128);	// copy in new data

		fftwf_execute(blms->Xplan);	// compute transform of input data

		int j;
		for (j = 0; j < 256; j++) {
			blms->Yhat[j] = Cmul(blms->What[j], blms->Xhat[j]);	// Filter new signal in freq. domain
			blms->Xhat[j] = Conjg(blms->Xhat[j]);	// take input data's complex conjugate
		}

		fftwf_execute(blms->Yplan);	//compute output signal transform

		for (j = 128; j < 256; j++)
			blms->y[j] = Cscl(blms->y[j], BLKSCL);

		::memset(blms->y, 0, 128 * sizeof (COMPLEX));

		for (j = 128; j < 256; j++)
			blms->error[j] = Csub(blms->delay_line[j], blms->y[j]);	// compute error signal

		if (blms->filter_type)
			::memcpy(&CXBdata(blms->signal, sigidx), &blms->y[128], 128 * sizeof(COMPLEX));	// if noise filter, output y
		else
			::memcpy(&CXBdata(blms->signal, sigidx), &blms->error[128], 128 * sizeof(COMPLEX));	// if notch filter, output error

		fftwf_execute(blms->Errhatplan);	// compute transform of the error signal

		for (j = 0; j < 256; j++)
			blms->Errhat[j] = Cmul(blms->Errhat[j], blms->Xhat[j]);	// compute cross correlation transform

		fftwf_execute(blms->UPDplan);	// compute inverse transform of cross correlation transform

		for (j = 0; j < 128; j++)
			blms->update[j] = Cscl(blms->update[j], BLKSCL);

		::memset(&blms->update[128], 0, sizeof (COMPLEX) * 128);	// zero the last block of the update, so we get

		// filter coefficients only at front of buffer
		fftwf_execute(blms->Wplan);

		for (j = 0; j < 256; j++) {
			blms->What[j] = Cadd(Cscl(blms->What[j], blms->leak_rate),	// leak the W away
			Cscl(blms->Update[j], blms->adaptation_rate));	// update at adaptation rate
		}

		sigidx += 128;		// move to next block in the signal buffer
	} while (sigidx < sigsize);	// done?
}
