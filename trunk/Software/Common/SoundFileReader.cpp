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

void CSoundFileReader::purge()
{
}

bool CSoundFileReader::hasClock()
{
	return false;
}

#if defined(__WINDOWS__)

const int WAVE_FORMAT_IEEE_FLOAT = 3;


CSoundFileReader::CSoundFileReader(const wxString& fileName) :
m_fileName(fileName),
m_sampleRate(0.0F),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_handle(NULL),
m_parent(),
m_child(),
m_offset(0L),
m_format(99),
m_buffer8(NULL),
m_buffer16(NULL),
m_buffer32(NULL)
{
}

CSoundFileReader::~CSoundFileReader()
{
}

bool CSoundFileReader::open(float sampleRate, unsigned int blockSize)
{
	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	m_handle = ::mmioOpen((CHAR *)m_fileName.c_str(), 0, MMIO_READ | MMIO_ALLOCBUF);

	if (m_handle == NULL) {
		::wxLogError(wxT("Could not open the WAV file %s."), m_fileName.c_str());
		return false;
	}

	MMCKINFO parent;
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	MMRESULT res = ::mmioDescend(m_handle, &parent, 0, MMIO_FINDRIFF);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s has no \"WAVE\" header."), m_fileName.c_str());
		return false;
	}

	MMCKINFO child;
	child.ckid = mmioFOURCC('f', 'm', 't', ' ');

	res = ::mmioDescend(m_handle, &child, &parent, MMIO_FINDCHUNK);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s has no \"fmt \" chunk."), m_fileName.c_str());
		return false;
	}

	WAVEFORMATEX format;

	LONG len = ::mmioRead(m_handle, (char *)&format, child.cksize);

	if (len != LONG(child.cksize)) {
		::wxLogError(wxT("%s is corrupt, cannot read the WAVEFORMATEX structure."), m_fileName.c_str());
		return false;
	}
	
	if (format.wFormatTag != WAVE_FORMAT_PCM && format.wFormatTag != WAVE_FORMAT_IEEE_FLOAT) {
		::wxLogError(wxT("%s is not PCM or IEEE Float format, is %u."), m_fileName.c_str(), format.wFormatTag);
		return false;
	}

	if (format.nSamplesPerSec != DWORD(sampleRate)) {
		::wxLogError(wxT("%s has sample rate %lu, not %.0f"), m_fileName.c_str(), format.nSamplesPerSec, sampleRate);
		return false;
	}

	if (format.nChannels != 2) {
		::wxLogError(wxT("%s has no of channels %u, not 2."), m_fileName.c_str(), format.nChannels);
		return false;
	}

	if (format.wBitsPerSample == 8 && format.wFormatTag == WAVE_FORMAT_PCM) {
		m_format = FORMAT_8BIT;
	} else if (format.wBitsPerSample == 16 && format.wFormatTag == WAVE_FORMAT_PCM) {
		m_format = FORMAT_16BIT;
	} else if (format.wBitsPerSample == 32 && format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
		m_format = FORMAT_32BIT;
	} else {
		::wxLogError(wxT("%s has sample width %u and format %u."), m_fileName.c_str(), format.wBitsPerSample, format.wFormatTag);
		return false;
	}

	res = ::mmioAscend(m_handle, &child, 0);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s is corrupt, cannot ascend."), m_fileName.c_str());
		return false;
	}

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');

	res = ::mmioDescend(m_handle, &child, &parent, MMIO_FINDCHUNK);

	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("%s has no \"data\" chunk."), m_fileName.c_str());
		return false;
	}

	// Get the current location so we can rewind if needed
	m_offset = ::mmioSeek(m_handle, 0L, SEEK_CUR);

	switch (m_format) {
		case FORMAT_8BIT:
			m_buffer8 =  new uint8[m_blockSize * 2];
			break;
		case FORMAT_16BIT:
			m_buffer16 = new sint16[m_blockSize * 2];
			break;
		case FORMAT_32BIT:
			m_buffer32 = new float32[m_blockSize * 2];
			break;
	}

	m_buffer = new float[m_blockSize * 2];

	return true;
}

void CSoundFileReader::clock()
{
	wxASSERT(m_callback != NULL);
	wxASSERT(m_handle != NULL);

	LONG n;
	LONG i;

	switch (m_format) {
		case FORMAT_8BIT:
			n = ::mmioRead(m_handle, (char *)m_buffer8, m_blockSize * 2 * sizeof(uint8));

			if (n <= 0) {
				m_callback->callback(m_buffer, 0, m_id);
				return;
			}

			n /= sizeof(uint8);

			for (i = 0; i < n; i++)
				m_buffer[i] = (float(m_buffer8[i]) - 127.0) / 128.0;
			break;

		case FORMAT_16BIT:
			n = ::mmioRead(m_handle, (char *)m_buffer16, m_blockSize * 2 * sizeof(sint16));

			if (n <= 0) {
				m_callback->callback(m_buffer, 0, m_id);
				return;
			}

			n /= sizeof(sint16);

			for (i = 0; i < n; i++)
				m_buffer[i] = float(m_buffer16[i]) / 32768.0;
			break;

		case FORMAT_32BIT:
			n = ::mmioRead(m_handle, (char *)m_buffer32, m_blockSize * 2 * sizeof(float32));

			if (n <= 0) {
				m_callback->callback(m_buffer, 0, m_id);
				return;
			}

			n /= sizeof(float32);

			// Swap I and Q for SDR-1000 data
			for (i = 0; i < n / 2; i++) {
				float qData = m_buffer32[i * 2 + 0];
				float iData = m_buffer32[i * 2 + 1];

				m_buffer[i * 2 + 0] = iData;
				m_buffer[i * 2 + 1] = qData;
			}
			break;
	}

	m_callback->callback(m_buffer, n / 2, m_id);
}

void CSoundFileReader::rewind()
{
	wxASSERT(m_handle != NULL);

	::mmioSeek(m_handle, m_offset, SEEK_SET);
}

void CSoundFileReader::close()
{
	wxASSERT(m_handle != NULL);

	::mmioClose(m_handle, 0);
	m_handle = NULL;

	delete[] m_buffer;
	delete[] m_buffer8;
	delete[] m_buffer16;
	delete[] m_buffer32;
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
	wxASSERT(m_callback != NULL);
	wxASSERT(m_file != NULL);

	int n = ::sf_read_float(m_file, m_buffer, m_blockSize);

	if (n <= 0) {
		m_callback->callback(m_buffer, 0, m_id);
		return;
	}

	m_callback->callback(m_buffer, n, m_id);
}

void CSoundFileReader::rewind()
{
	wxASSERT(m_file != NULL);

	::sf_seek(m_file, 0, SEEK_SET);
}

void CSoundFileReader::close()
{
	wxASSERT(m_file != NULL);

	::sf_close(m_file);
	m_file = NULL;

	delete[] m_buffer;
}

#endif
