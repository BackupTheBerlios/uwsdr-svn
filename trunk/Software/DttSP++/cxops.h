/* cxops.h
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

#ifndef _cxops_h
#define _cxops_h

#include "complex.h"
#include "datatypes.h"

extern COMPLEX cxzero;
extern COMPLEX cxone;
extern COMPLEX cxJ;
extern COMPLEX cxminusone;
extern COMPLEX cxminusJ;

extern inline COMPLEX Cscl(COMPLEX, REAL);
extern inline COMPLEX Cadd(COMPLEX, COMPLEX);
extern inline COMPLEX Csub(COMPLEX, COMPLEX);
extern inline COMPLEX Cmul(COMPLEX, COMPLEX);
extern inline COMPLEX Cdiv(COMPLEX, COMPLEX);
extern inline REAL Cmag(COMPLEX);
extern inline REAL Cabs(COMPLEX);
extern inline REAL Csqrmag(COMPLEX);
extern inline COMPLEX Cmplx(REAL, IMAG);
extern inline COMPLEX Conjg(COMPLEX);
extern inline COMPLEX Cexp(COMPLEX);

#endif
