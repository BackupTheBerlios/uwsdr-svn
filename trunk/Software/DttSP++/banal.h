/* banal.h
   stuff we're too embarrassed to declare otherwise
   
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

#ifndef _banal_h
#define _banal_h

#include "FromSys.h"
#include "DataTypes.h"


#ifdef NEED_MINMAX

inline unsigned int max(unsigned int val1, unsigned int val2)
{
	return (val1 > val2) ? val1 : val2;
}

inline int max(int val1, int val2)
{
	return (val1 > val2) ? val1 : val2;
}

inline double max(double val1, double val2)
{
	return (val1 > val2) ? val1 : val2;
}

inline unsigned int min(unsigned int val1, unsigned int val2)
{
	return (val1 < val2) ? val1 : val2;
}

inline int min(int val1, int val2)
{
	return (val1 < val2) ? val1 : val2;
}

inline double min(double val1, double val2)
{
	return (val1 < val2) ? val1 : val2;
}

#endif

#define abs(a) ((a) >= 0 ? (a) : -(a))

const REAL MONDO = 1E15F;
const REAL BITSY = 1E-15F;

inline REAL sqr(REAL x)
{
	return x * x;
}

extern unsigned int  npoof2(unsigned int);
extern unsigned int  nblock2(unsigned int);

#endif
