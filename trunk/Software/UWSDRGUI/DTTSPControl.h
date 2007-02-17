/*
 *   Copyright (C) 2006-2007 by Jonathan Naylor G4KLX
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
 */

#ifndef	DTTSPControl_H
#define	DTTSPControl_H

#include <wx/wx.h>

#include "UWSDRDefs.h"


class CDTTSPControl : public wxThread {

    public:
	CDTTSPControl();
	virtual ~CDTTSPControl();

	virtual void open(float sampleRate, unsigned int blockSize);

	virtual void* Entry();

	virtual void setFilter(FILTERWIDTH filter);
	virtual void setMode(UWSDRMODE mode);
	virtual void setZeroIF(bool onOff);
	virtual void setTXAndFreq(bool transmit, float freq);
	virtual void setAGC(AGCSPEED agc);
	virtual void setDeviation(FMDEVIATION value);
	virtual void setSquelch(unsigned int value);

	virtual void setNB(bool onOff);
	virtual void setNBValue(unsigned int value);
	virtual void setNB2(bool onOff);
	virtual void setNB2Value(unsigned int value);

	virtual void setSP(bool onOff);
	virtual void setSPValue(unsigned int value);

	virtual void setCarrierLevel(unsigned int value);

	virtual void setALCValue(unsigned int attack, unsigned int decay, unsigned int hang);

	virtual void setRXIAndQ(int phase, int gain);
	virtual void setTXIAndQ(int phase, int gain);

	virtual float getMeter(METERPOS type);
	virtual void  getSpectrum(float* spectrum, SPECTRUMPOS pos);

	virtual float getTXOffset();
	virtual float getRXOffset();

	virtual void dataIO(const float* input, float* output, unsigned int nSamples);

	virtual void close();

    private:
	float        m_sampleRate;
	unsigned int m_blockSize;
	FILTERWIDTH  m_filter;
	UWSDRMODE    m_mode;
	bool         m_zeroIF;
	float        m_rxFreq;
	float        m_txFreq;
	bool         m_transmit;
	FMDEVIATION  m_deviation;
	AGCSPEED     m_agc;
	bool         m_nb;
	unsigned int m_nbValue;
	bool         m_nb2;
	unsigned int m_nb2Value;
	bool         m_sp;
	unsigned int m_spValue;
	unsigned int m_carrierLevel;
	unsigned int m_attack;
	unsigned int m_decay;
	unsigned int m_hang;
	int          m_rxPhase;
	int          m_rxGain;
	int          m_txPhase;
	int          m_txGain;
	unsigned int m_squelch;
	bool         m_started;

	void normaliseFilter();
};

#endif
