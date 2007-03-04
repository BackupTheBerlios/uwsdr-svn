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

#ifndef	DSPControl_H
#define	DSPControl_H

#include <wx/wx.h>

#include "UWSDRDefs.h"
#include "SoundFileWriter.h"
#include "DTTSPControl.h"
#include "CWKeyer.h"
#include "VoiceKeyer.h"
#include "SerialControl.h"
#include "DataReader.h"
#include "DataWriter.h"
#include "RingBuffer.h"


class CDSPControl : public wxThread, public IDataCallback {
    public:
	CDSPControl(float sampleRate);
	virtual ~CDSPControl();

	virtual void  setTXReader(IDataReader* reader);
	virtual void  setTXWriter(IDataWriter* writer);

	virtual void  setRXReader(IDataReader* reader);
	virtual void  setRXWriter(IDataWriter* writer);

	virtual void  setTXInControl(CSerialControl* control, INPIN pin);
	virtual void  setKeyInControl(CSerialControl* control, INPIN pin);

	virtual bool  open();
	virtual void* Entry();
	virtual void  close();

	virtual bool  openIO();
	virtual void  closeIO();

	virtual void  callback(float* buffer, unsigned int nSamples, int id);

	// Many of these are pass throughs to DTTSP
	virtual void setMode(UWSDRMODE mode);
	virtual void setZeroIF(bool onOff);
	virtual void swapIQ(bool swap);
	virtual void setFilter(FILTERWIDTH filter);
	virtual void setAGC(AGCSPEED agc);
	virtual void setDeviation(FMDEVIATION dev);
	virtual void setTXAndFreq(bool transmit, float freq);

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

	virtual bool setRecord(bool record);

	virtual void setAFGain(unsigned int value);
	virtual void setRFGain(unsigned int value);
	virtual void setMicGain(unsigned int value);
	virtual void setPower(unsigned int value);
	virtual void setSquelch(unsigned int value);

	virtual float getMeter(METERPOS type);
	virtual void  getSpectrum(float* spectrum, SPECTRUMPOS pos);

	virtual float getTXOffset();
	virtual float getRXOffset();

	virtual bool sendCW(unsigned int speed, const wxString& text, CWSTATUS state);
	virtual bool sendAudio(const wxString& fileName, VOICESTATUS state);

    private:
	CDTTSPControl*  m_dttsp;
	CCWKeyer*       m_cwKeyer;
	CVoiceKeyer*    m_voiceKeyer;

	float           m_sampleRate;

	IDataReader*    m_txReader;
	IDataWriter*    m_txWriter;
	IDataReader*    m_rxReader;
	IDataWriter*    m_rxWriter;

	CSerialControl* m_txInControl;
	INPIN           m_txInPin;
	CSerialControl* m_keyInControl;
	INPIN           m_keyInPin;

	wxSemaphore     m_waiting;

	CRingBuffer     m_txRingBuffer;
	CRingBuffer     m_rxRingBuffer;
	float*          m_txBuffer;
	float*          m_rxBuffer;
	float*          m_outBuffer;

	CSoundFileWriter* m_record;

	bool            m_transmit;
	bool            m_running;
	float           m_afGain;
	float           m_rfGain;
	float           m_micGain;
	float           m_power;
	UWSDRMODE       m_mode;
	bool            m_swap;

	int             m_clockId;

	bool            m_lastTXIn;
	bool            m_lastKeyIn;

	unsigned int    m_rxUnderruns;
	unsigned int    m_rxOverruns;
	unsigned int    m_txUnderruns;
	unsigned int    m_txOverruns;

	void scaleBuffer(float* buffer, unsigned int nSamples, float scale, bool swap = false);

#if defined(__WXDEBUG__)
	void dumpBuffer(const wxString& title, float* buffer, unsigned int nSamples) const;
#endif
};

#endif
