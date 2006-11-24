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

#ifndef	DSPControl_H
#define	DSPControl_H

#include <wx/wx.h>

#include "SoundFileWriter.h"
#include "DTTSPControl.h"
#include "CWKeyer.h"
#include "VoiceKeyer.h"
#include "DataReader.h"
#include "DataWriter.h"
#include "RingBuffer.h"


class CDSPControl : public wxThread, public IDataCallback {
    public:
	CDSPControl(float sampleRate, float centreFreq);
	virtual ~CDSPControl();

	virtual void  setTXReader(IDataReader* reader);
	virtual void  setTXWriter(IDataWriter* writer);

	virtual void  setRXReader(IDataReader* reader);
	virtual void  setRXWriter(IDataWriter* writer);

	virtual bool  open();
	virtual void* Entry();
	virtual void  close();

	virtual bool  openIO();
	virtual void  closeIO();

	virtual void  callback(float* buffer, unsigned int nSamples, int id);

	// Many of these are pass throughs to DTTSP
	virtual void setMode(int mode);
	virtual void setFilter(int filter);
	virtual void setAGC(int agc);
	virtual void setDeviation(int dev);
	virtual void setTXAndFreq(bool transmit, float freq);
	virtual void setRIT(float freq);

	virtual void setNB(bool onOff);
	virtual void setNBValue(unsigned int value);
	virtual void setNB2(bool onOff);
	virtual void setNB2Value(unsigned int value);

	virtual void setSP(bool onOff);
	virtual void setSPValue(unsigned int value);

	virtual void setRXIAndQ(int phase, int gain);
	virtual void setTXIAndQ(int phase, int gain);

	virtual bool setRecord(bool record);

	virtual void setAFGain(unsigned int value);
	virtual void setRFGain(unsigned int value);
	virtual void setMicGain(unsigned int value);
	virtual void setPower(unsigned int value);
	virtual void setSquelch(unsigned int value);

	virtual float getMeter(int type);
	virtual void  getSpectrum(float* spectrum, int pos);

	virtual void sendCW(unsigned int speed, const wxString& text);
	virtual void sendAudio(const wxString& fileName, int state);

    private:
	CDTTSPControl* m_dttsp;
	CCWKeyer*      m_cwKeyer;
	CVoiceKeyer*   m_voiceKeyer;

	float          m_sampleRate;
	float          m_centreFreq;

	IDataReader*   m_txReader;
	IDataWriter*   m_txWriter;
	IDataReader*   m_rxReader;
	IDataWriter*   m_rxWriter;
	wxSemaphore    m_waiting;

	CRingBuffer    m_txRingBuffer;
	CRingBuffer    m_rxRingBuffer;
	float*         m_txBuffer;
	float*         m_rxBuffer;
	float*         m_outBuffer;

	CSoundFileWriter* m_record;

	bool           m_transmit;
	bool           m_running;
	float          m_afGain;
	float          m_rfGain;
	float          m_micGain;
	float          m_power;
	int            m_mode;

	int            m_clockId;

	void scaleBuffer(float* buffer, unsigned int nSamples, float scale);

#if defined(__WXDEBUG__)
	void dumpBuffer(const wxString& title, float* buffer, unsigned int nSamples) const;
#endif
};

#endif
