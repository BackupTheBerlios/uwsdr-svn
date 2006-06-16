/* SpotTone.h

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

#ifndef _spottone_h
#define _spottone_h

#include "DataTypes.h"
#include "CXB.h"
#include "Oscillator.h"


typedef enum {
	SpotTone_IDLE,
	SpotTone_WAIT,
	SpotTone_RISE,
	SpotTone_STDY,
	SpotTone_FALL,
	SpotTone_HOLD
} SPOTTONEstate;


class CSpotTone {
    public:
	CSpotTone(REAL gain, REAL freq, REAL rise, REAL fall, unsigned int size, REAL sampleRate);
	virtual ~CSpotTone();

	virtual void setValues(REAL gain, REAL freq, REAL rise, REAL fall);

	virtual void on();
	virtual void off();

	virtual bool generate();

	virtual CXB* getData();

    private:
	REAL          m_curr;
	REAL          m_gain;
	REAL          m_mul;
	REAL          m_scl;
	REAL          m_sampleRate;
    REAL          m_freq;
    COscillator*  m_gen;
    REAL          m_riseDur;
	REAL          m_riseIncr;
    unsigned int  m_riseWant;
	unsigned int  m_riseHave;
    REAL          m_fallDur;
	REAL          m_fallIncr;
    unsigned int  m_fallWant;
	unsigned int  m_fallHave;
	unsigned int  m_size;
	SPOTTONEstate m_stage;
	CXB*          m_buf;
};

#endif
