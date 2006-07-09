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

#include "SoundFileWriter.h"

#include <wx/debug.h>
#include <wx/log.h>

#ifdef __WINDOWS__

CSoundFileWriter::CSoundFileWriter(const wxString& fileName, unsigned int sampleWidth) :
m_fileName(fileName),
m_sampleWidth(sampleWidth),
m_blockSize(0),
m_handle(NULL),
m_parent(),
m_child(),
m_buffer8(NULL),
m_buffer16(NULL)
{
	wxASSERT(sampleWidth == 8 || sampleWidth == 16);
}

CSoundFileWriter::~CSoundFileWriter()
{
}

bool CSoundFileWriter::open(float sampleRate, unsigned int blockSize)
{
	m_blockSize = blockSize;

	m_handle = ::mmioOpen((CHAR *)m_fileName.c_str(), 0, MMIO_WRITE | MMIO_CREATE | MMIO_ALLOCBUF);
	if (m_handle == NULL) {
		::wxLogError(wxT("Could not open the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	m_parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	m_parent.cksize  = 0;

	MMRESULT res = ::mmioCreateChunk(m_handle, &m_parent, MMIO_CREATERIFF);
	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("Could not write to file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	m_child.ckid   = mmioFOURCC('f', 'm', 't', ' ');
	m_child.cksize = sizeof(WAVEFORMATEX);

	res = ::mmioCreateChunk(m_handle, &m_child, 0);
	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("Could not write to the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	WAVEFORMATEX format;
	format.wBitsPerSample  = m_sampleWidth;
	format.wFormatTag      = WAVE_FORMAT_PCM;
	format.nChannels       = 2;
	format.nSamplesPerSec  = int(sampleRate + 0.5F);
	format.nAvgBytesPerSec = int(sampleRate + 0.5F) * 2 * m_sampleWidth / 8;
	format.nBlockAlign     = 2 * m_sampleWidth / 8;
	format.cbSize          = 0;

	LONG n = ::mmioWrite(m_handle, (CHAR *)&format, sizeof(WAVEFORMATEX));
	if (n != sizeof(WAVEFORMATEX)) {
		::wxLogError(wxT("Could not write to the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	::mmioAscend(m_handle, &m_child, 0);

	m_child.ckid   = mmioFOURCC('d', 'a', 't', 'a');
	m_child.cksize = 0;

	res = ::mmioCreateChunk(m_handle, &m_child, 0);
	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("Could not write to the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	if (m_sampleWidth == 8)
		m_buffer8  = new uint8[blockSize * 2];
	else
		m_buffer16 = new sint16[blockSize * 2];

	return true;
}

void CSoundFileWriter::write(const float* buffer, unsigned int length)
{
	wxASSERT(buffer != NULL);
	wxASSERT(length > 0 && length <= m_blockSize);

	if (m_sampleWidth == 8) {
		for (unsigned int i = 0; i < length * 2; i++)
			m_buffer8[i] = uint8(buffer[i] * 128.0 + 127.0);

		LONG bytes = length * 2 * sizeof(uint8);

		LONG n = ::mmioWrite(m_handle, (char *)m_buffer8, bytes);
		if (n != bytes)
			::wxLogError(wxT("Error from mmioWrite(), wanted %ld available %ld"), bytes, n);
	} else {
		for (unsigned int i = 0; i < length * 2; i++)
			m_buffer16[i] = sint16(buffer[i] * 32768.0);

		LONG bytes = length * 2 * sizeof(sint16);

		LONG n = ::mmioWrite(m_handle, (char *)m_buffer16, bytes);
		if (n != bytes)
			::wxLogError(wxT("Error from mmioWrite(), wanted %ld available %ld"), bytes, n);
	}
}

void CSoundFileWriter::close()
{
	::mmioAscend(m_handle, &m_child, 0);
	::mmioAscend(m_handle, &m_parent, 0);

	::mmioClose(m_handle, 0);

	delete[] m_buffer8;
	delete[] m_buffer16;

	m_buffer8  = NULL;
	m_buffer16 = NULL;
}

#else

CSoundFileWriter::CSoundFileWriter(const wxString& fileName, unsigned int sampleWidth) :
m_fileName(fileName),
m_sampleWidth(sampleWidth),
m_file(NULL)
{
	wxASSERT(sampleWidth == 8 || sampleWidth == 16);
}

CSoundFileWriter::~CSoundFileWriter()
{
}

bool CSoundFileWriter::open(float sampleRate, unsigned int blockSize)
{
	SF_INFO info;
	info.samplerate = int(sampleRate + 0.5F);
	info.channels   = 2;
	info.format     = SF_FORMAT_WAV;

	if (m_sampleWidth == 8)
		info.format |= SF_FORMAT_PCM_U8;
	else
		info.format |= SF_FORMAT_PCM_16;

	int ret = ::sf_format_check(&info);

	if (!ret)
		return false;

	m_file = ::sf_open(m_fileName.mb_str(), SFM_WRITE, &info);
	if (m_file == NULL) {
		::wxLogError(wxT("Could not open the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	::wxLogMessage(wxT("Opened the sound file in SoundFileWriter"));

	return true;
}

void CSoundFileWriter::write(const float* buffer, unsigned int length)
{
	wxASSERT(buffer != NULL);

	::sf_write_float(m_file, buffer, length);
}

void CSoundFileWriter::close()
{
	::sf_close(m_file);
}

#endif
