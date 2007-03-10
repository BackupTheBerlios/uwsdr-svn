/* FIR.h

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY
Copyright (C) 2006-2007 by Jonathan Naylor, G4KLX

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

#ifndef _fir_h
#define _fir_h

#include "Complex.h"

class CFIR {
    public:
	static COMPLEX* lowpass(float cutoff, float samprate, unsigned int size);
	static COMPLEX* bandpass(float lo, float hi, float samprate, unsigned int size);
	static COMPLEX* highpass(float cutoff, float samprate, unsigned int size);
	static COMPLEX* hilbert(float lo, float hi, float samprate, unsigned int size);
	static COMPLEX* bandstop(float lo, float hi, float samprate, unsigned int size);
};

#endif
