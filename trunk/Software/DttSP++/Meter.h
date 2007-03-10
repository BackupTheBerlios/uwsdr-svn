/* Meter.h

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006-5 by Frank Brickle, AB2KT and Bob McGwier, N4HY
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

#ifndef _meter_h
#define _meter_h

#include "CXB.h"


const int RXMETERPTS = 5;
const int TXMETERPTS = 8;

enum METERTYPE {
	SIGNAL_STRENGTH,
	AVG_SIGNAL_STRENGTH,
	ADC_REAL,
	ADC_IMAG,
	AGC_GAIN,
	MIC,
	PWR,
	ALC,
	EQtap,
	LEVELER,
	COMP,
	ALC_G,
	LVL_G
};

enum RXMETERTAP {
	RXMETER_PRE_CONV,
	RXMETER_POST_FILT,
	RXMETER_POST_AGC
};

enum RXMETERTYPE {
	RX_SIGNAL_STRENGTH,
	RX_AVG_SIGNAL_STRENGTH,
	RX_ADC_REAL,
	RX_ADC_IMAG,
	RX_AGC_GAIN
};

enum TXMETERTYPE {
	TX_MIC,
	TX_PWR,
	TX_ALC,
	TX_EQtap,
	TX_LEVELER,
	TX_COMP,
	TX_ALC_G,
	TX_LVL_G
};


class CMeter {
    public:
	CMeter();
	~CMeter();

	void setRXMeter(RXMETERTAP tap, CXB* buf, float agcGain);
	void setTXMeter(TXMETERTYPE type, CXB* buf, float alcGain);

	float getRXMeter(RXMETERTYPE type) const;
	float getTXMeter(TXMETERTYPE type) const;

	void reset();

    private:
    float       m_rxval[RXMETERPTS];
	RXMETERTYPE m_rxmode;
	float       m_txval[TXMETERPTS];
	TXMETERTYPE m_txmode;
	float       m_micSave;
	float       m_alcSave;
	float       m_eqTapSave;
	float       m_levelerSave;
	float       m_compSave;
};

#endif
