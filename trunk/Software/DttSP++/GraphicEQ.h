/* GraphicEQ.h

  PCM time-domain equalizer

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

This is derived from equ.xmms:

 *   Copyright (C) 2002  Felipe Rivera <liebremx at users sourceforge net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   $Id: iir.c,v 1.10 2004/05/16 02:24:31 liebremx Exp $ */

#ifndef GRPHEQ_H
#define GRPHEQ_H

#include "DataTypes.h"
#include "Complex.h"
#include "CXB.h"
#include "FilterOVSV.h"


class CGraphicEQ {
    public:
	CGraphicEQ(CXB* d, REAL sampleRate, unsigned int bits);
	virtual ~CGraphicEQ();

	virtual bool getNotchFlag() const;
	virtual void setNotchFlag(bool setit);

	void setEQ(REAL preamp, REAL gain0, REAL gain1, REAL gain2);

	virtual void equalise();

    private:
	CXB*         m_data;
	REAL         m_sampleRate;
	unsigned int m_bits;
	CFilterOVSV* m_p;
	CXB*         m_in;
	CXB*         m_out;
	COMPLEX      m_num[9];
	COMPLEX      m_den[6];
	bool         m_notchFlag;

	REAL dB2lin(REAL db);
};

#endif
