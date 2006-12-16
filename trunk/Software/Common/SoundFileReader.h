/*
 *   Copyright (C) 2002-2004,2006 by Jonathan Naylor G4KLX
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

#ifndef	SoundFileReader_H
#define	SoundFileReader_H

#include <wx/wx.h>

#if defined(__WINDOWS__)
#include <windows.h>
#include <mmsystem.h>
#else
#include <wx/ffile.h>
#endif

typedef unsigned char uint8;
typedef signed short  sint16;
typedef float         float32;
typedef double        float64;

enum {
	FORMAT_8BIT,
	FORMAT_16BIT,
	FORMAT_32BIT
};

#include "DataReader.h"

class CSoundFileReader : public IDataReader {

    public:
    CSoundFileReader(const wxString& fileName);
	virtual ~CSoundFileReader();

	virtual void setCallback(IDataCallback* callback, int id);

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void purge();

	virtual bool hasClock();
	virtual void close();

	virtual void clock();

	virtual void rewind();

    private:
	wxString       m_fileName;
	unsigned int   m_blockSize;
	IDataCallback* m_callback;
	int            m_id;
	float*         m_buffer;
	unsigned int   m_format;
	uint8*         m_buffer8;
	sint16*        m_buffer16;
	float32*       m_buffer32;
#if defined(__WINDOWS__)
	HMMIO          m_handle;
	MMCKINFO       m_parent;
	MMCKINFO       m_child;
	LONG           m_offset;
#else
	wxFFile*       m_file;
	wxFileOffset   m_offset;
#endif
};

#endif
