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

#ifndef	SignalReader_H
#define	SignalReader_H

#include <wx/wx.h>

#include "DataReader.h"

class CSignalReader : public wxThread, public IDataReader {

    public:
    CSignalReader(unsigned int frequency, float noiseAmplitude, float signalAmplitude);
	virtual ~CSignalReader();

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(unsigned int sampleRate, unsigned int blockSize);

	virtual void* Entry();

	virtual void close();

    private:
	unsigned int   m_frequency;
	float          m_noiseAmplitude;
	float          m_signalAmplitude;
	unsigned int   m_sampleRate;
	unsigned int   m_blockSize;
	IDataCallback* m_callback;
	int            m_id;
};

#endif
