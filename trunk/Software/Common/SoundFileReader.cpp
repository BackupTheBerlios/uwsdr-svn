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

#include "SoundFileReader.h"

#include <wx/debug.h>
#include <wx/log.h>
#include <wx/datetime.h>


void CSoundFileReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

#if defined(__WINDOWS__)

CSoundFileReader::CSoundFileReader(const wxString& fileName) :
m_fileName(fileName),
m_sampleRate(0.0F),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_sampleWidth(0),
m_handle(NULL),
m_parent(),
m_child(),
m_buffer8(NULL),
m_buffer16(NULL)
{
}

CSoundFileReader::~CSoundFileReader()
{
	delete[] m_buffer8;
	delete[] m_buffer16;
}

bool CSoundFileReader::open(float sampleRate, unsigned int blockSize)
{
	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	m_handle = ::mmioOpen((CHAR *)m_fileName.c_str(), 0, MMIO_READ | MMIO_ALLOCBUF);

	if (m_handle == NULL) {
		::wxLogError(wxT("Could not open the file %s in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	MMCKINFO parent;
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	MMRESULT res = ::mmioDescend(m_handle, &parent, 0, MMIO_FINDRIFF);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	MMCKINFO child;
	child.ckid = mmioFOURCC('f', 'm', 't', ' ');

	res = ::mmioDescend(m_handle, &child, &parent, MMIO_FINDCHUNK);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	WAVEFORMATEX format;

	LONG len = ::mmioRead(m_handle, (char *)&format, child.cksize);

	if (len != LONG(child.cksize)) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	if (format.wFormatTag != WAVE_FORMAT_PCM) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	if (format.nSamplesPerSec != sampleRate) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	if (format.nChannels != 2) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	m_sampleWidth = format.wBitsPerSample;

	if (m_sampleWidth != 8 && m_sampleWidth != 16) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	res = ::mmioAscend(m_handle, &child, 0);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');

	res = ::mmioDescend(m_handle, &child, &parent, MMIO_FINDCHUNK);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	if (m_sampleWidth == 8)
		m_buffer8 =  new uint8[m_blockSize * 2];
	else
		m_buffer16 = new sint16[m_blockSize * 2];

	m_buffer = new float[m_blockSize * 2];

	return true;
}

void CSoundFileReader::purge()
{
}

bool CSoundFileReader::hasClock()
{
	return false;
}

void CSoundFileReader::clock()
{
	LONG n;

	if (m_sampleWidth == 8) {
		n = ::mmioRead(m_handle, (char *)m_buffer8, m_blockSize * 2 * sizeof(uint8));

		if (n <= 0)
			return;

		for (int i = 0; i < n; i++)
			m_buffer[i] = (float(m_buffer8[i]) - 127.0) / 128.0;
	} else {
		n = ::mmioRead(m_handle, (char *)m_buffer16, m_blockSize * 2 * sizeof(sint16));

		if (n <= 0)
			return;

		n /= 2;

		for (int i = 0; i < n; i++)
			m_buffer[i] = float(m_buffer16[i]) / 32768.0;
	}

	m_callback->callback(m_buffer, n / 2, m_id);
}

void CSoundFileReader::close()
{
	wxASSERT(m_handle != NULL);

	::mmioClose(m_handle, 0);

	delete[] m_buffer;
}

#else

CSoundFileReader::CSoundFileReader(const wxString& fileName) :
m_fileName(fileName),
m_sampleRate(0.0F),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_file(NULL)
{
}

CSoundFileReader::~CSoundFileReader()
{
}

bool CSoundFileReader::open(float sampleRate, unsigned int blockSize)
{
	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	SF_INFO info;
	info.format = 0;
	m_file = ::sf_open(m_fileName.mb_str(), SFM_READ, &info);

	if (m_file == NULL) {
		::wxLogError(wxT("Could not open the file %s in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	if (info.samplerate != int(sampleRate + 0.5F)) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	if (info.channels != 2) {
		::wxLogError(wxT("%s is corrupt in SoundFileReader"), m_fileName.c_str());
		return false;
	}

	m_buffer = new float[m_blockSize * 2];

	return true;
}

void CSoundFileReader::clock()
{
	int n = ::sf_read_float(m_file, m_buffer, m_blockSize);

	if (n <= 0)
		return;

	m_callback->callback(m_buffer, n, m_id);
}

void CSoundFileReader::close()
{
	::sf_close(m_file);

	delete[] m_buffer;
}

#endif
