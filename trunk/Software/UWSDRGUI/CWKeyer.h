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

#ifndef	CWKeyer_H
#define	CWKeyer_H

#include <wx/wx.h>

#include "DataReader.h"
#include "RingBuffer.h"

class CCWKeyer : public IDataReader {

    public:
    CCWKeyer();
	virtual ~CCWKeyer();

	virtual bool open(float sampleRate, unsigned int blockSize);
	virtual void close();

	virtual bool needsClock();
	virtual void clock();

	virtual void setCallback(IDataCallback* callback, int id);

	virtual void send(unsigned int speed, const wxString& text);
	virtual void abort();

    private:
	float          m_sampleRate;
	unsigned int   m_blockSize;
	IDataCallback* m_callback;
	int            m_id;
	unsigned int   m_dotLen;
	wxString       m_text;
	bool           m_stop;
	float*         m_buffer;
	float*         m_dotBuffer;
	float*         m_dashBuffer;
	float*         m_silBuffer;
	CRingBuffer*   m_cwBuffer;

	void         createSymbol(float* buffer, unsigned int len);
	unsigned int calcDotLength(int speed);
	void         fillBuffer();
};

#endif
