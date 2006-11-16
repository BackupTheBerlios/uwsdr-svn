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

#ifndef	DataControl_H
#define	DataControl_H

#include <wx/wx.h>

#include "RingBuffer.h"
#include "SignalReader.h"
#include "SoundFileReader.h"
#include "SoundCardReader.h"
#include "SDRDataReader.h"
#include "NullWriter.h"
#include "NullReader.h"
#include "SoundCardWriter.h"
#include "SDRDataWriter.h"


enum {
	SOURCE_INTERNAL_1,
	SOURCE_INTERNAL_2,
	SOURCE_SOUNDFILE,
	SOURCE_SOUNDCARD
};

class CDataControl : public wxThread, public IDataCallback {
    public:
	CDataControl(float sampleRate, const wxString& address, int port, int api, long inDev, long outDev, unsigned int maxSamples, bool delay);
	virtual ~CDataControl();

	virtual bool  setSoundFileReader(const wxString& fileName);

	virtual bool  open();
	virtual void* Entry();
	virtual void  close();

	virtual void  callback(float* buffer, unsigned int nSamples, int id);

	virtual void  setSource(int source);
	virtual void  setTX(bool transmit);
	virtual void  setMute(bool mute);

    private:
	float          m_sampleRate;
	wxString       m_address;
	int            m_port;
	int            m_api;
	long           m_inDev;
	long           m_outDev;

	CSignalReader*    m_internal1Reader;
	CSignalReader*    m_internal2Reader;
	CSoundCardReader* m_soundCardReader;
	CSoundFileReader* m_soundFileReader;
	CSDRDataWriter*   m_rxWriter;

	CNullWriter*      m_nullWriter;
	CSoundCardWriter* m_soundCardWriter;
	CSDRDataReader*   m_txReader;

	wxSemaphore    m_waiting;

	CRingBuffer    m_txRingBuffer;
	CRingBuffer    m_rxRingBuffer;
	float*         m_txBuffer;
	float*         m_rxBuffer;

	int            m_source;
	bool           m_transmit;
	bool           m_mute;
	bool           m_running;
	unsigned int   m_maxSamples;
	bool           m_delay;

	bool openIO();
	void closeIO();

#if defined(__WXDEBUG__)
	void dumpBuffer(const wxString& title, float* buffer, unsigned int nSamples) const;
#endif
};

#endif
