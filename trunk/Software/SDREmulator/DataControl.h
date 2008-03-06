/*
 *   Copyright (C) 2006-2008 by Jonathan Naylor G4KLX
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

#include "DataReader.h"
#include "DataWriter.h"
#include "DataCallback.h"
#include "RawDataCallback.h"
#include "RingBuffer.h"
#include "SignalReader.h"
#include "SoundFileReader.h"
#include "UDPDataReader.h"
#include "NullWriter.h"
#include "NullReader.h"
#include "UDPDataWriter.h"


enum {
	SOURCE_INTERNAL_1,
	SOURCE_INTERNAL_2,
	SOURCE_SOUNDFILE,
	SOURCE_SOUNDCARD
};

class CDataControl : public wxThread, public IDataCallback, public IRawDataCallback {
    public:
	CDataControl(float sampleRate, const wxString& address, int port, int inDev, int outDev);

	virtual bool  setSoundFileReader(const wxString& fileName);

	virtual bool  open();
	virtual void* Entry();
	virtual void  close();

	virtual void  setCallback(IRawDataCallback* callback, int id);

	virtual void  callback(float* buffer, unsigned int nSamples, int id);
	virtual bool  callback(char* buffer, unsigned int len, int id);

	virtual void  setSource(int source);
	virtual void  setTX(bool transmit);
	virtual void  setMute(bool mute);

	virtual void  sendACK(const wxString& command);
	virtual void  sendNAK(const wxString& command);
	virtual void  sendData(const float* buffer, unsigned int nSamples);

    protected:
	virtual ~CDataControl();

    private:
	float             m_sampleRate;
	wxString          m_address;
	int               m_port;
	int               m_inDev;
	int               m_outDev;
	CSignalReader*    m_internal1Reader;
	CSignalReader*    m_internal2Reader;
	IDataReader*      m_soundCardReader;
	CSoundFileReader* m_soundFileReader;
	CUDPDataWriter*   m_rxWriter;
	CNullWriter*      m_nullWriter;
	IDataWriter*      m_soundCardWriter;
	CUDPDataReader*   m_txReader;
	wxSemaphore       m_waiting;
	int               m_txSequence;
	unsigned int      m_rxSequence;
	CRingBuffer       m_txRingBuffer;
	CRingBuffer       m_rxRingBuffer;
	float*            m_txBuffer;
	float*            m_rxBuffer;
	float*            m_txSockBuffer;
	unsigned char*    m_rxSockBuffer;
	IRawDataCallback* m_callback;
	int               m_id;
	int               m_source;
	bool              m_transmit;
	bool              m_mute;
	bool              m_running;

	bool openIO();
	void closeIO();

#if defined(__WXDEBUG__)
	void dumpBuffer(const wxString& title, float* buffer, unsigned int nSamples) const;
#endif
};

#endif
