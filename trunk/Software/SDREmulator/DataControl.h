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
	SOURCE_INTERNAL,
	SOURCE_SOUNDFILE,
	SOURCE_SOUNDCARD
};

class CDataControl : public wxThread, public IDataCallback {
    public:
	CDataControl(float sampleRate, const wxString& address, int port, int api, long inDev, long outDev);
	virtual ~CDataControl();

	virtual bool  setSoundFileReader(const wxString& fileName);

	virtual void* Entry();

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

	CSignalReader*    m_internalReader;
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


	bool openIO();
	void closeIO();
};

#endif
