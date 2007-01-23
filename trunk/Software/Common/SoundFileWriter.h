/*
 *   Copyright (C) 2002-2004,2006-2007 by Jonathan Naylor G4KLX
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

#if defined(__WINDOWS__)
#include <windows.h>
#include <mmsystem.h>
#else
#include <wx/ffile.h>
#endif

typedef unsigned char uint8;
typedef signed short  sint16;


#include "DataWriter.h"

class CSoundFileWriter : public IDataWriter {

    public:
    CSoundFileWriter(const wxString& fileName, unsigned int channels = 2, unsigned int sampleWidth = 16);
	virtual ~CSoundFileWriter();

	virtual bool open(float sampleRate, unsigned int blockSize);

	virtual void write(const float* buffer, unsigned int length);

	virtual void close();

	virtual void enable(bool enable = true);
	virtual void disable();

    private:
	wxString     m_fileName;
	unsigned int m_channels;
	unsigned int m_sampleWidth;
	unsigned int m_blockSize;
	uint8*       m_buffer8;
	sint16*      m_buffer16;
	bool         m_enabled;
#if defined(__WINDOWS__)
	HMMIO        m_handle;
	MMCKINFO     m_parent;
	MMCKINFO     m_child;
#else
	wxFFile*     m_file;
	wxFileOffset m_offset1;
	wxFileOffset m_offset2;
	wxUint32     m_length;
#endif
};

#endif
