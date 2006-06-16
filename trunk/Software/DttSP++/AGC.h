/* AGC.h

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

#ifndef _agc_h
#define _agc_h

#include "DataTypes.h"
#include "Complex.h"
#include "CXB.h"


const int FASTLEAD = 72;

typedef enum {
	agcOFF,
	agcLONG,
	agcSLOW,
	agcMED,
	agcFAST
} AGCMODE;

class CAGC {
    public:
	CAGC(AGCMODE mode, CXB* buff, REAL limit, REAL attack,
	     REAL decay, REAL slope, REAL hangtime, REAL samprate, REAL maxGain,
		 REAL minGain, REAL curgain);
	virtual ~CAGC();

	virtual void process();

	virtual AGCMODE getMode() const;
	virtual void    setMode(AGCMODE mode);

	virtual REAL getGain() const;
	virtual void setGain(REAL gain);

    private:
	AGCMODE      m_mode;
	REAL         m_samprate;
    REAL         m_gainTop;
	REAL         m_gainNow;
	REAL         m_gainFastNow;
	REAL         m_gainBottom;
	REAL         m_gainLimit;
	REAL         m_gainFix;
	REAL         m_attack;
	REAL         m_oneMAttack;
	REAL         m_decay;
	REAL         m_oneMDecay;
	REAL         m_slope;
	REAL         m_fastAttack;
	REAL         m_oneMFastAttack;
	REAL         m_fastDecay;
	REAL         m_oneMFastDecay;
	REAL         m_hangTime;
	REAL         m_hangThresh;
	REAL         m_fastHangTime;		//wa6ahl:  added to structure
	COMPLEX*     m_circ;
	CXB*         m_buff;
	unsigned int m_mask;
	unsigned int m_index;
	unsigned int m_sndex;
	unsigned int m_hangIndex;
	unsigned int m_fastIndex;
	unsigned int m_fastHang;			//wa6ahl:  added to structure
};

#endif
