/* bufvec.h

defs for vector and buffer data structures and utilities
   
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

#ifndef _bufvec_h
#define _bufvec_h

#include "DataTypes.h"
#include "Complex.h"
#include "CXOps.h"


typedef struct {
	COMPLEX* data;
	unsigned int size, have;
	bool mine;
} CXB;

/* all these should be OK rhs or lhs */

#define CXBbase(p) ((p)->data)
#define CXBdata(p, i) (CXBbase(p)[(i)])
#define CXBreal(p, i) (CXBbase(p)[(i)].re)
#define CXBimag(p, i) (CXBbase(p)[(i)].im)
#define CXBsize(p) ((p)->size)
#define CXBhave(p) ((p)->have)
#define CXBmine(p) ((p)->mine)

extern COMPLEX *newvec_COMPLEX(unsigned int size);
extern void delvec_COMPLEX(COMPLEX* buf);

extern CXB* newCXB(unsigned int size, COMPLEX* base);
extern void delCXB(CXB* p);

extern void CXBscl(CXB* buff, REAL scl);
extern REAL CXBpeak(CXB* buff);

extern REAL normalize_vec_REAL(REAL*, unsigned int n);
extern REAL normalize_vec_COMPLEX(COMPLEX*, unsigned int n);

#endif
