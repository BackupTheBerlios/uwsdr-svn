/* Meter.cpp

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

#include "Meter.h"
#include "FromSys.h"
#include "Utils.h"


CMeter::CMeter() :
m_rxval(),
m_rxmode(RX_SIGNAL_STRENGTH),
m_txval(),
m_txmode(TX_PWR),
m_micSave(0.0F),
m_alcSave(0.0F),
m_eqTapSave(0.0F),
m_levelerSave(0.0F),
m_compSave(0.0F)
{
	int i;

	for (i = 0; i < RXMETERPTS; i++)
		m_rxval[i] = -200.0F;

	for (i = 0; i < TXMETERPTS; i++)
		m_txval[i] = -200.0F;
}

CMeter::~CMeter()
{
}

void CMeter::reset()
{
	int i;

	for (i = 0; i < RXMETERPTS; i++)
		m_rxval[i] = -200.0F;

	for (i = 0; i < TXMETERPTS; i++)
		m_txval[i] = -200.0F;
}

void CMeter::setRXMeter(RXMETERTAP tap, CXB* buf, float agcGain)
{
	ASSERT(buf != NULL);

	COMPLEX* vec = CXBbase(buf);

	unsigned int len = CXBhave(buf);
	unsigned int i;

	float temp1, temp2;

	switch (tap) {
		case RXMETER_PRE_CONV:
			temp1 = m_rxval[RX_ADC_REAL];
			for (i = 0; i < len; i++)
				temp1 = (float)max(::fabs(vec[i].re), temp1);
			m_rxval[RX_ADC_REAL] = float(20.0 * ::log10(temp1 + 1e-10));

			temp1 = m_rxval[RX_ADC_IMAG];
			for (i = 0; i < len; i++)
				temp1 = (float)max(::fabs(vec[i].im), temp1);
			m_rxval[RX_ADC_IMAG] = float(20.0 * ::log10(temp1 + 1e-10));
			break;

		case RXMETER_POST_FILT:
			temp1 = 0.0F;
			for (i = 0; i < len; i++)
				temp1 += Csqrmag(vec[i]);
			// temp1 /= float(len);

			m_rxval[RX_SIGNAL_STRENGTH] = float(10.0 * ::log10(temp1 + 1e-20));

			temp1 = m_rxval[RX_SIGNAL_STRENGTH];
			temp2 = m_rxval[RX_AVG_SIGNAL_STRENGTH];
			m_rxval[RX_AVG_SIGNAL_STRENGTH] = float(0.95 * temp2 + 0.05 * temp1);
			break;

		case RXMETER_POST_AGC:
			m_rxval[RX_AGC_GAIN] = float(20.0 * ::log10(agcGain + 1e-10));
			break;
	}
}

void CMeter::setTXMeter(TXMETERTYPE type, CXB* buf, float alcGain, float levelerGain)
{
	ASSERT(buf != NULL);

	COMPLEX *vec = CXBbase(buf);

	unsigned int len = CXBhave(buf);
	unsigned int i;

	float temp;

	switch (type) {
		case TX_MIC:
			for (i = 0; i < len; i++)
				m_micSave = float(0.9995 * m_micSave + 0.0005 * Csqrmag(vec[i]));
			m_txval[TX_MIC] = float(-10.0 * ::log10(m_micSave + 1e-16));
			break;

		case TX_PWR:
			temp = 0.0000001F;
			for (i = 0;	i < len; i++)
				temp += Csqrmag(vec[i]);
			m_txval[TX_PWR] = temp / float(len);
			break;

		case TX_ALC:
			for (i = 0; i < len; i++)
				m_alcSave = float(0.9995 * m_alcSave + 0.0005 * Csqrmag(vec[i]));
			m_txval[TX_ALC]   = float(-10.0 * ::log10(m_alcSave + 1e-16));
			m_txval[TX_ALC_G] = float(20.0 * ::log10(alcGain + 1e-16));
			break;

		case TX_EQtap:
			for (i = 0; i < len; i++)
				m_eqTapSave =	float(0.9995 * m_eqTapSave + 0.0005 * Csqrmag(vec[i]));
			m_txval[TX_EQtap] = float(-10.0 * ::log10(m_eqTapSave + 1e-16));
			break;

		case TX_LEVELER:
			for (i = 0; i < len; i++)
				m_levelerSave = float(0.9995 * m_levelerSave + 0.0005 * Csqrmag(vec[i]));
			m_txval[TX_LEVELER] = float(-10.0 * ::log10(m_levelerSave + 1e-16));
			m_txval[TX_LVL_G]   = float(20.0 * ::log10(levelerGain + 1e-16));
			break;

		case TX_COMP:
			for (i = 0; i < len; i++)
				m_compSave = float(0.9995 * m_compSave +	0.0005 * Csqrmag(vec[i]));
			m_txval[TX_COMP] = float(-10.0 * ::log10(m_compSave + 1e-16));
			break;

		case TX_ALC_G:
		case TX_LVL_G:
			break;
	}
}

float CMeter::getRXMeter(RXMETERTYPE type) const
{
	return m_rxval[type];
}

float CMeter::getTXMeter(TXMETERTYPE type) const
{
	return m_txval[type];
}
