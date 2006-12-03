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

#ifndef	TwoToneReader_H
#define	TwoToneReader_H

#include <wx/wx.h>

#include "DataReader.h"
#include "DataCallback.h"


class CTwoToneReader : public IDataReader, public IDataCallback {

    public:
    CTwoToneReader(float frequency1, float amplitude1, float frequency2, float amplitude2, IDataReader* reader = NULL);
	virtual ~CTwoToneReader();

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void close();

	virtual void purge();

	virtual bool hasClock();
	virtual void clock();

	virtual void callback(float* buffer, unsigned int nSamples, int id);

    private:
	float          m_frequency1;
	float          m_amplitude1;
	float          m_frequency2;
	float          m_amplitude2;
	IDataReader*   m_reader;
	unsigned int   m_blockSize;
	IDataCallback* m_callback;
	int            m_id;
	float*         m_buffer;
	float          m_cosVal1;
	float          m_sinVal1;
	float          m_cosDelta1;
	float          m_sinDelta1;
	float          m_cosVal2;
	float          m_sinVal2;
	float          m_cosDelta2;
	float          m_sinDelta2;
};

#endif
