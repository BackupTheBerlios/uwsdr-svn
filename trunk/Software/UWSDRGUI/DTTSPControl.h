/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
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

class CDTTSPControl : public wxThread {

    public:
	CDTTSPControl();
	virtual ~CDTTSPControl();

	virtual void open(unsigned int sampleRate, unsigned int blockSize);

	virtual void* Entry();

	virtual void setFilter(int filter);
	virtual void setMode(int mode);
	virtual void setTXAndFreq(bool transmit, double freq);
	virtual void setAGC(int agc);
	virtual void setSquelch(unsigned int value);

	virtual void setNB(bool onOff);
	virtual void setNBValue(unsigned int value);
	virtual void setNB2(bool onOff);
	virtual void setNB2Value(unsigned int value);

	virtual void setSP(bool onOff);
	virtual void setSPValue(unsigned int value);

	virtual void setRXIAndQ(int phase, int gain);
	virtual void setTXIAndQ(int phase, int gain);

	virtual float getMeter(int type);
	virtual void  getSpectrum(float* spectrum, int pos);

	virtual void dataIO(const float* input, float* output, unsigned int nSamples);

	virtual void close();

    private:
	unsigned int m_sampleRate;
	unsigned int m_blockSize;
	int          m_filter;
	int          m_mode;
	double       m_rxFreq;
	double       m_txFreq;
	bool         m_transmit;
	int          m_agc;
	bool         m_nb;
	unsigned int m_nbValue;
	bool         m_nb2;
	unsigned int m_nb2Value;
	bool         m_sp;
	unsigned int m_spValue;
	int          m_rxPhase;
	int          m_rxGain;
	int          m_txPhase;
	int          m_txGain;
	unsigned int m_squelch;

	void normaliseFilter();
};

#endif
