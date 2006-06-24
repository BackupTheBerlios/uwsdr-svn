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

#ifndef	SoundFileWriter_H
#define	SoundFileWriter_H

#include <wx/wx.h>

#ifdef __WINDOWS__
#include <windows.h>
#include <mmsystem.h>

typedef unsigned char uint8;
typedef signed short  sint16;
#else
#include <sndfile.h>
#endif

#include "DataWriter.h"

class CSoundFileWriter : public IDataWriter {

    public:
    CSoundFileWriter(const wxString& fileName, unsigned int sampleWidth = 16);
	virtual ~CSoundFileWriter();

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void write(const float* buffer, unsigned int length);

	virtual void close();

    private:
	wxString     m_fileName;
	float        m_sampleWidth;
#ifdef __WINDOWS__
	unsigned int m_blockSize;
	HMMIO        m_handle;
	MMCKINFO     m_parent;
	MMCKINFO     m_child;
	uint8*       m_buffer8;
	sint16*      m_buffer16;
#else
	SNDFILE*     m_file;
#endif
};

#endif
