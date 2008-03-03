/*
 *   Copyright (C) 2008 by Jonathan Naylor G4KLX
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

#ifndef	HPSDRReaderWriter_H
#define	HPSDRReaderWriter_H


#include "USBBulkReaderWriter.h"
#include "SDRController.h"
#include "SocketCallback.h"
#include "RingBuffer.h"
#include "DataCallback.h"


class CHPSDRReaderWriter : public ISocketCallback, public ISDRController  {
    public:
	CHPSDRReaderWriter(unsigned int blockSize, int c0, int c1, int c2, int c3, int c4);
	virtual ~CHPSDRReaderWriter();

	virtual void setCallback(IControlInterface* callback);
	virtual void setDataCallback(IDataCallback* callback, int id);
	virtual void setAudioCallback(IDataCallback* callback, int id);

	virtual bool open();
	virtual void close();

	virtual void writeData(const float* buffer, unsigned int nSamples);
	virtual void writeAudio(const float* buffer, unsigned int nSamples);

	virtual void purgeData();
	virtual void purgeAudio();

	virtual void enableTX(bool on);
	virtual void enableRX(bool on);
	virtual void setTXAndFreq(bool transmit, const CFrequency& freq);

	virtual void setClockTune(unsigned int clock);

	virtual bool callback(char* buffer, unsigned int len, int id);

    private:
	CUSBBulkReaderWriter* m_usb;
	unsigned int          m_blockSize;
	CRingBuffer*          m_dataRingBuffer;
	CRingBuffer*          m_audioRingBuffer;
	char*                 m_usbBuffer;
	float*                m_cbBuffer;
	float*                m_dataBuffer;
	float*                m_audioBuffer;
	IDataCallback*        m_dataCallback;
	IDataCallback*        m_audioCallback;
	IControlInterface*    m_controlCallback;
	int                   m_dataId;
	int                   m_audioId;
	bool                  m_transmit;
	unsigned int          m_frequency;
	unsigned int          m_robin;
	int                   m_c0;
	int                   m_c1;
	int                   m_c2;
	int                   m_c3;
	int                   m_c4;
	bool                  m_ptt;
	bool                  m_key;

	void writeUSB();
};

#endif
