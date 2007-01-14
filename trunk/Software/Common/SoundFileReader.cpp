/*
 *   Copyright (C) 2002-2004,2006,7 by Jonathan Naylor G4KLX
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
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_format(99),
m_buffer8(NULL),
m_buffer16(NULL),
m_buffer32(NULL),
m_handle(NULL),
m_parent(),
m_child(),
m_offset(0L)
{
}

CSoundFileReader::~CSoundFileReader()
{
}

bool CSoundFileReader::open(float sampleRate, unsigned int blockSize)
{
	m_blockSize = blockSize;

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

	LONG n = 0L;
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
				m_buffer[i] = (float(m_buffer8[i]) - 127.0F) / 128.0F;
			break;

		case FORMAT_16BIT:
			n = ::mmioRead(m_handle, (char *)m_buffer16, m_blockSize * 2 * sizeof(sint16));

			if (n <= 0) {
				m_callback->callback(m_buffer, 0, m_id);
				return;
			}

			n /= sizeof(sint16);

			for (i = 0; i < n; i++)
				m_buffer[i] = float(m_buffer16[i]) / 32768.0F;
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
	if (m_handle != NULL) {
		::mmioClose(m_handle, 0);
		m_handle = NULL;
	}

	delete[] m_buffer;
	delete[] m_buffer8;
	delete[] m_buffer16;
	delete[] m_buffer32;
}

#else

const int WAVE_FORMAT_PCM        = 1;
const int WAVE_FORMAT_IEEE_FLOAT = 3;

CSoundFileReader::CSoundFileReader(const wxString& fileName) :
m_fileName(fileName),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_format(99),
m_buffer8(NULL),
m_buffer16(NULL),
m_buffer32(NULL),
m_file(NULL),
m_offset(0),
m_length(0),
m_read(0)
{
}

CSoundFileReader::~CSoundFileReader()
{
}

bool CSoundFileReader::open(float sampleRate, unsigned int blockSize)
{
	m_blockSize = blockSize;
	m_read      = 0;

	m_file = new wxFFile(m_fileName, "rb");

	bool ret = m_file->IsOpened();
	if (!ret) {
		::wxLogError(wxT("Could not open the WAV file %s."), m_fileName.c_str());

		delete m_file;
		m_file = NULL;

		return false;
	}

	unsigned char buffer[4];

	unsigned int n = m_file->Read(buffer, 4);
	if (n != 4 || ::memcmp(buffer, "RIFF", 4) != 0) {
		::wxLogError(wxT("%s has no \"RIFF\" signature."), m_fileName.c_str());
		return false;
	}

	n = m_file->Read(buffer, 4);
	if (n != 4) {
		::wxLogError(wxT("%s is corrupt, cannot read the file length."), m_fileName.c_str());
		return false;
	}

	n = m_file->Read(buffer, 4);
	if (n != 4 || ::memcmp(buffer, "WAVE", 4) != 0) {
		::wxLogError(wxT("%s has no \"WAVE\" header."), m_fileName.c_str());
		return false;
	}

	n = m_file->Read(buffer, 4);
	if (n != 4 || ::memcmp(buffer, "fmt ", 4) != 0) {
		::wxLogError(wxT("%s has no \"fmt \" chunk."), m_fileName.c_str());
		return false;
	}

	wxUint32 uint32;
	n = m_file->Read(&uint32, sizeof(wxUint32));

	wxUint32 length = wxUINT32_SWAP_ON_BE(uint32);
	if (n != sizeof(wxUint32) || length < 16) {
		::wxLogError(wxT("%s is corrupt, cannot read the WAVEFORMATEX structure length."), m_fileName.c_str());
		return false;
	}

	wxUint16 uint16;
	n = m_file->Read(&uint16, sizeof(wxUint16));

	wxUint16 compCode = wxUINT16_SWAP_ON_BE(uint16);
	if (n != sizeof(wxUint16) || (compCode != WAVE_FORMAT_PCM && compCode != WAVE_FORMAT_IEEE_FLOAT)) {
		::wxLogError(wxT("%s is not PCM or IEEE Float format, is %u."), m_fileName.c_str(), compCode);
		return false;
	}

	n = m_file->Read(&uint16, sizeof(wxUint16));

	wxUint16 channels = wxUINT16_SWAP_ON_BE(uint16);
	if (n != sizeof(wxUint16) || channels != 2) {
		::wxLogError(wxT("%s has no of channels %u, not 2."), m_fileName.c_str(), channels);
		return false;
	}

	n = m_file->Read(&uint32, sizeof(wxUint32));

	wxUint32 samplesPerSec = wxUINT32_SWAP_ON_BE(uint32);
	if (n != sizeof(wxUint32) || samplesPerSec != sampleRate) {
		::wxLogError(wxT("%s has sample rate %lu, not %.0f"), m_fileName.c_str(), (unsigned long)samplesPerSec, sampleRate);
		return false;
	}

	n = m_file->Read(&uint32, sizeof(wxUint32));

	if (n != sizeof(wxUint32)) {
		::wxLogError(wxT("%s is corrupt, cannot read the average bytes per second"), m_fileName.c_str());
		return false;
	}

	n = m_file->Read(&uint16, sizeof(wxUint16));

	if (n != sizeof(wxUint16)) {
		::wxLogError(wxT("%s is corrupt, cannot read the block align."), m_fileName.c_str());
		return false;
	}

	n = m_file->Read(&uint16, sizeof(wxUint16));

	if (n != sizeof(wxUint16)) {
		::wxLogError(wxT("%s is corrupt, cannot read the bitsPerSample."), m_fileName.c_str());
		return false;
	}

	wxUint16 bitsPerSample = wxUINT16_SWAP_ON_BE(uint16);

	if (bitsPerSample == 8 && compCode == WAVE_FORMAT_PCM) {
		m_format = FORMAT_8BIT;
	} else if (bitsPerSample == 16 && compCode == WAVE_FORMAT_PCM) {
		m_format = FORMAT_16BIT;
	} else if (bitsPerSample == 32 && compCode == WAVE_FORMAT_IEEE_FLOAT) {
		m_format = FORMAT_32BIT;
	} else {
		::wxLogError(wxT("%s has sample width %u and format %u."), m_fileName.c_str(), bitsPerSample, compCode);
		return false;
	}

	// Now drain any extra bytes of data
	if (length > 16)
		m_file->Seek(length - 16, wxFromCurrent);

	n = m_file->Read(buffer, 4);

	if (n != 4 || ::memcmp(buffer, "data", 4) != 0) {
		::wxLogError(wxT("%s has no \"data\" chunk."), m_fileName.c_str());
		return false;
	}

	n = m_file->Read(&uint32, sizeof(wxUint32));

	if (n != sizeof(wxUint32)) {
		::wxLogError(wxT("%s is corrupt, cannot read the \"data\" chunk size"), m_fileName.c_str());
		return false;
	}

	m_length = wxUINT32_SWAP_ON_BE(uint32);

	// Get the current location so we can rewind if needed
	m_offset = m_file->Tell();

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
	wxASSERT(m_file != NULL);

	unsigned int n = 0;
	unsigned int i;
	unsigned int readSize;

	switch (m_format) {
		case FORMAT_8BIT:
			readSize = m_blockSize * 2 * sizeof(uint8);

			if (readSize > (m_length - m_read))
				readSize = (m_length - m_read) / (2 * sizeof(uint8));

			n = m_file->Read(m_buffer8, readSize);

			if (n <= 0) {
				m_callback->callback(m_buffer, 0, m_id);
				return;
			}

			m_read += n;

			n /= sizeof(uint8);

			for (i = 0; i < n; i++)
				m_buffer[i] = (float(m_buffer8[i]) - 127.0F) / 128.0F;
			break;

		case FORMAT_16BIT:
			readSize = m_blockSize * 2 * sizeof(sint16);

			if (readSize > (m_length - m_read))
				readSize = (m_length - m_read) / (2 * sizeof(sint16));

			n = m_file->Read(m_buffer16, readSize);

			if (n <= 0) {
				m_callback->callback(m_buffer, 0, m_id);
				return;
			}

			m_read += n;

			n /= sizeof(sint16);

			for (i = 0; i < n; i++)
				m_buffer[i] = float(m_buffer16[i]) / 32768.0F;
			break;

		case FORMAT_32BIT:
			readSize = m_blockSize * 2 * sizeof(float32);

			if (readSize > (m_length - m_read))
				readSize = (m_length - m_read) / (2 * sizeof(float32));

			n = m_file->Read(m_buffer32, readSize);

			if (n <= 0) {
				m_callback->callback(m_buffer, 0, m_id);
				return;
			}

			m_read += n;

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
	wxASSERT(m_file != NULL);

	m_file->Seek(m_offset);

	m_read = 0;
}

void CSoundFileReader::close()
{
	if (m_file != NULL) {
		m_file->Close();
		delete m_file;
		m_file = NULL;
	}

	delete[] m_buffer;
	delete[] m_buffer8;
	delete[] m_buffer16;
	delete[] m_buffer32;
}

#endif
