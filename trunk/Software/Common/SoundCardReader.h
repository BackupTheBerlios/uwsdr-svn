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

#ifndef	SoundCardReader_H
#define	SoundCardReader_H

#include <wx/wx.h>

#include "DataReader.h"

#include "portaudio.h"


extern "C" {
	int scrCallback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
}


class CSoundCardReader : public IDataReader {
    public:
	CSoundCardReader(int api, int dev);

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(float sampleRate, unsigned int blockSize);
	virtual void close();

	virtual void purge();

	virtual int  callback(const void* input, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);

	virtual bool hasClock();
	virtual void clock();

    protected:
	virtual ~CSoundCardReader();

    private:
	int            m_api;
	int            m_dev;
	IDataCallback* m_callback;
	int            m_id;
	PaStream*      m_stream;
};

#endif
