/* bufvec.c

   creation, deletion, management for vectors and buffers 
   
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

#include "bufvec.h"
#include "fromsys.h"
#include "banal.h"

#include <wx/wx.h>


/*------------------------------------------------------------------------*/
/* allocate/free just vectors */

REAL* newvec_REAL(unsigned int size)
{
	REAL* p = new REAL[size];

	::memset(p, 0x00, size * sizeof(REAL));

	return p;
}

void delvec_REAL(REAL* vec)
{
	delete[] vec;
}

COMPLEX* newvec_COMPLEX(unsigned int size)
{
	COMPLEX* p = new COMPLEX[size];

	::memset(p, 0x00, size * sizeof(COMPLEX));

	return p;
}

void delvec_COMPLEX(COMPLEX* vec)
{
	delete[] vec;
}

void CXBscl(CXB* buff, REAL scl)
{
	for (unsigned int i = 0; i < CXBhave(buff); i++)
		CXBdata(buff, i) = Cscl(CXBdata(buff, i), scl);
}

REAL CXBpeak(CXB* buff)
{
	REAL maxsam = 0.0;
	for (unsigned int i = 0; i < CXBhave(buff); i++)
		maxsam = max(Cmag(CXBdata(buff, i)), maxsam);

	return maxsam;
}

/*------------------------------------------------------------------------*/
/* buffers (mainly i/o) */
/*------------------------------------------------------------------------*/
/* complex */

CXB* newCXB(unsigned int size, COMPLEX* base)
{
	CXB* p = new CXB;

	if (base != NULL) {
		CXBbase(p) = base;
		CXBmine(p) = false;
	} else {
		CXBbase(p) = newvec_COMPLEX(size);
		CXBmine(p) = true;
	}

	CXBsize(p) = size;
	CXBhave(p) = 0;

	return p;
}

void delCXB(CXB* p)
{
	if (p != NULL) {
		if (CXBmine(p))
			delvec_COMPLEX(CXBbase(p));

		delete p;
	}
}

/*------------------------------------------------------------------------*/
/* real */

RLB* newRLB(unsigned int size, REAL* base)
{
	RLB* p = new RLB;

	if (base != NULL) {
		RLBbase(p) = base;
		RLBmine(p) = false;
	} else {
		RLBbase(p) = newvec_REAL(size);
		RLBmine(p) = true;
	}

	RLBsize(p) = size;
	RLBhave(p) = 0;

	return p;
}

void delRLB(RLB* p)
{
	if (p != NULL) {
		if (RLBmine(p))
			delvec_REAL(RLBbase(p));

		delete p;
	}
}

//========================================================================
// return normalization constant

REAL normalize_vec_REAL(REAL* v, unsigned int n)
{
	if (v != NULL && n > 0) {
		REAL big = -MONDO;

		for (unsigned int i = 0; i < n; i++) {
			REAL a = abs(v[i]);
			big = max(big, a);
		}

		if (big > 0.0) {
			REAL scl = 1.0F / big;

			for (unsigned int i = 0; i < n; i++)
				v[i] *= scl;

			return scl;
		} else {
			return 0.0F;
		}
	} else {
		return 0.0F;
	}
}

REAL normalize_vec_COMPLEX(COMPLEX* z, unsigned int n)
{
	if (z != NULL && n > 0) {
		REAL big = -MONDO;

		for (unsigned int i = 0; i < n; i++) {
			REAL a = Cabs(z[i]);
			big = max(big, a);
		}

		if (big > 0.0F) {
			REAL scl = 1.0F / big;

			for (unsigned int i = 0; i < n; i++)
				z[i] = Cscl(z[i], scl);

			return scl;
		} else {
			return 0.0F;
		}
	} else {
		return 0.0F;
	}
}

void dumpREAL(REAL* buf, unsigned int size, char* text)
{
	wxLogDebug(text);

	for (unsigned int i = 0; i < size; i += 8)
		wxLogDebug("%04X: %g %g %g %g %g %g %g %g", i,
			buf[i + 0], buf[i + 1], buf[i + 2], buf[i + 3],
			buf[i + 4], buf[i + 5], buf[i + 6], buf[i + 7]);
}

void dumpCOMPLEX(COMPLEX* buf, unsigned int size, char* text)
{
	wxLogDebug(text);

	for (unsigned int i = 0; i < size; i += 8)
		wxLogDebug("%04X: %g:%g %g:%g %g:%g %g:%g %g:%g %g:%g %g:%g %g:%g", i,
			buf[i + 0].re, buf[i + 0].im, buf[i + 1].re, buf[i + 1].im,
			buf[i + 2].re, buf[i + 2].im, buf[i + 3].re, buf[i + 3].im,
			buf[i + 4].re, buf[i + 4].im, buf[i + 5].re, buf[i + 5].im,
			buf[i + 6].re, buf[i + 6].im, buf[i + 7].re, buf[i + 7].im);
}
