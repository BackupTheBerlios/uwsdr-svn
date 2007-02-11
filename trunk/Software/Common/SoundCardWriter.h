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

#ifndef	SoundCardWriter_H
#define	SoundCardWriter_H

#include <wx/wx.h>

#include "DataWriter.h"
#include "RingBuffer.h"

#include "portaudio.h"


extern "C" {
	int scwCallback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
}


class CSoundCardWriter : public IDataWriter {
    public:
	CSoundCardWriter(int dev);

	virtual bool open(float sampleRate, unsigned int blockSize);
	virtual void write(const float* buffer, unsigned int nSamples);
	virtual void close();

	virtual void enable(bool enable = true);
	virtual void disable();

	virtual int  callback(void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);

    protected:
	virtual ~CSoundCardWriter();

    private:
	int          m_dev;
	unsigned int m_blockSize;
	PaStream*    m_stream;
	CRingBuffer* m_buffer;
	float*       m_lastBuffer;
	unsigned int m_requests;
	unsigned int m_underruns;
	unsigned int m_overruns;
	bool         m_enabled;
};

#endif
