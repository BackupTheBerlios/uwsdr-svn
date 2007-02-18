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

#include "SoundFileWriter.h"

#include <wx/debug.h>
#include <wx/log.h>


void CSoundFileWriter::enable(bool enable)
{
	m_enabled = enable;
}

void CSoundFileWriter::disable()
{
	enable(false);
}

#if defined(__WINDOWS__)

CSoundFileWriter::CSoundFileWriter(const wxString& fileName, unsigned int channels, unsigned int sampleWidth) :
m_fileName(fileName),
m_channels(channels),
m_sampleWidth(sampleWidth),
m_blockSize(0),
m_buffer8(NULL),
m_buffer16(NULL),
m_enabled(false),
m_handle(NULL),
m_parent(),
m_child()
{
	wxASSERT(channels == 1 || channels == 2);
	wxASSERT(sampleWidth == 8 || sampleWidth == 16);
}

CSoundFileWriter::~CSoundFileWriter()
{
}

bool CSoundFileWriter::open(float sampleRate, unsigned int blockSize)
{
	m_blockSize = blockSize;
	m_enabled   = false;

	m_handle = ::mmioOpen((CHAR *)m_fileName.c_str(), 0, MMIO_WRITE | MMIO_CREATE | MMIO_ALLOCBUF);
	if (m_handle == NULL) {
		::wxLogError(wxT("SoundFileWriter: could not open the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	m_parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	m_parent.cksize  = 0;

	MMRESULT res = ::mmioCreateChunk(m_handle, &m_parent, MMIO_CREATERIFF);
	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("SoundFileWriter: could not write to file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	m_child.ckid   = mmioFOURCC('f', 'm', 't', ' ');
	m_child.cksize = sizeof(WAVEFORMATEX);

	res = ::mmioCreateChunk(m_handle, &m_child, 0);
	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("SoundFileWriter: could not write to the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	WAVEFORMATEX format;
	format.wBitsPerSample  = m_sampleWidth;
	format.wFormatTag      = WAVE_FORMAT_PCM;
	format.nChannels       = m_channels;
	format.nSamplesPerSec  = int(sampleRate + 0.5F);
	format.nAvgBytesPerSec = int(sampleRate + 0.5F) * m_channels * m_sampleWidth / 8;
	format.nBlockAlign     = m_channels * m_sampleWidth / 8;
	format.cbSize          = 0;

	LONG n = ::mmioWrite(m_handle, (CHAR *)&format, sizeof(WAVEFORMATEX));
	if (n != sizeof(WAVEFORMATEX)) {
		::wxLogError(wxT("SoundFileWriter: could not write to the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	::mmioAscend(m_handle, &m_child, 0);

	m_child.ckid   = mmioFOURCC('d', 'a', 't', 'a');
	m_child.cksize = 0;

	res = ::mmioCreateChunk(m_handle, &m_child, 0);
	if (res != MMSYSERR_NOERROR) {
		::wxLogError(wxT("SoundFileWriter: could not write to the file %s in SoundFileWriter"), m_fileName.c_str());
		return false;
	}

	if (m_sampleWidth == 8)
		m_buffer8  = new uint8[blockSize * m_channels];
	else
		m_buffer16 = new sint16[blockSize * m_channels];

	return true;
}

void CSoundFileWriter::write(const float* buffer, unsigned int length)
{
	wxASSERT(m_handle != NULL);
	wxASSERT(buffer != NULL);
	wxASSERT(length > 0 && length <= m_blockSize);

	if (!m_enabled)
		return;

	if (m_sampleWidth == 8) {
		for (unsigned int i = 0; i < (length * m_channels); i++)
			m_buffer8[i] = uint8(buffer[i] * 128.0 + 127.0);

		LONG bytes = length * m_channels * sizeof(uint8);

		LONG n = ::mmioWrite(m_handle, (char *)m_buffer8, bytes);
		if (n != bytes)
			::wxLogError(wxT("SoundFileWriter: error from mmioWrite(), wanted %ld available %ld"), bytes, n);
	} else {
		for (unsigned int i = 0; i < (length * m_channels); i++)
			m_buffer16[i] = sint16(buffer[i] * 32768.0);

		LONG bytes = length * m_channels * sizeof(sint16);

		LONG n = ::mmioWrite(m_handle, (char *)m_buffer16, bytes);
		if (n != bytes)
			::wxLogError(wxT("SoundFileWriter: error from mmioWrite(), wanted %ld available %ld"), bytes, n);
	}
}

void CSoundFileWriter::close()
{
	if (m_handle != NULL) {
		::mmioAscend(m_handle, &m_child, 0);
		::mmioAscend(m_handle, &m_parent, 0);

		::mmioClose(m_handle, 0);
		m_handle = NULL;
	}

	delete[] m_buffer8;
	delete[] m_buffer16;

	delete this;
}

#else

CSoundFileWriter::CSoundFileWriter(const wxString& fileName, unsigned int channels, unsigned int sampleWidth) :
m_fileName(fileName),
m_channels(channels),
m_sampleWidth(sampleWidth),
m_blockSize(0),
m_buffer8(NULL),
m_buffer16(NULL),
m_enabled(false),
m_file(NULL),
m_offset1(0),
m_offset2(0),
m_length(0)
{
	wxASSERT(channels == 1 || channels == 2);
	wxASSERT(sampleWidth == 8 || sampleWidth == 16);
}

CSoundFileWriter::~CSoundFileWriter()
{
}

bool CSoundFileWriter::open(float sampleRate, unsigned int blockSize)
{
	m_blockSize = blockSize;
	m_length    = 0;
	m_enabled   = false;

	m_file = new wxFFile(m_fileName, "wb");

	bool ret = m_file->IsOpened();
	if (!ret) {
		::wxLogError(wxT("SoundFileWriter: could not open the file %s in SoundFileWriter"), m_fileName.c_str());

		delete m_file;
		m_file = NULL;

		return false;
	}

	m_file->Write("RIFF", 4);					// 4 bytes, file signature

	m_offset1 = m_file->Tell();

	wxUint32 uint32 = 0;
	m_file->Write(&uint32, sizeof(wxUint32));	// 4 bytes, length of file, filled in later

	m_file->Write("WAVE", 4);					// 4 bytes, RIFF file type

	m_file->Write("fmt ", 4);					// 4 bytes, chunk signature

	uint32 = wxUINT32_SWAP_ON_BE(wxUint32(16));
	m_file->Write(&uint32, sizeof(wxUint32));	// 4 bytes, length of "fmt " chunk

	wxUint16 uint16 = wxUINT16_SWAP_ON_BE(wxUint16(1));
	m_file->Write(&uint16, sizeof(uint16));		// 2 bytes, PCM/uncompressed

	uint16 = wxUINT16_SWAP_ON_BE(wxUint16(m_channels));
	m_file->Write(&uint16, sizeof(uint16));		// 2 bytes, no of channels
	
	uint32 = wxUINT32_SWAP_ON_BE(wxUint32(sampleRate + 0.5F));
	m_file->Write(&uint32, sizeof(wxUint32));	// 4 bytes, sample rate

	uint32 = wxUINT32_SWAP_ON_BE(wxUint32(int(sampleRate + 0.5F) * m_channels * m_sampleWidth / 8));
	m_file->Write(&uint32, sizeof(wxUint32));	// 4 bytes, average bytes per second

	uint16 = wxUINT16_SWAP_ON_BE(wxUint16(m_channels * m_sampleWidth / 8));
	m_file->Write(&uint16, sizeof(uint16));		// 2 bytes, block alignment

	uint16 = wxUINT16_SWAP_ON_BE(wxUint16(m_sampleWidth));
	m_file->Write(&uint16, sizeof(uint16));		// 2 bytes, significant bits per sample

	m_file->Write("data", 4);					// 4 bytes, chunk signature

	m_offset2 = m_file->Tell();

	uint32 = 0;
	m_file->Write(&uint32, sizeof(wxUint32));	// 4 bytes, length of "data" chunk, filled in later

	if (m_sampleWidth == 8)
		m_buffer8  = new uint8[blockSize * m_channels];
	else
		m_buffer16 = new sint16[blockSize * m_channels];

	return true;
}

void CSoundFileWriter::write(const float* buffer, unsigned int length)
{
	wxASSERT(m_file != NULL);
	wxASSERT(buffer != NULL);
	wxASSERT(length > 0 && length <= m_blockSize);

	if (!m_enabled)
		return;

	if (m_sampleWidth == 8) {
		for (unsigned int i = 0; i < (length * m_channels); i++)
			m_buffer8[i] = uint8(buffer[i] * 128.0 + 127.0);

		unsigned int bytes = length * m_channels * sizeof(uint8);

		unsigned int n = m_file->Write(m_buffer8, bytes);
		if (n != bytes)
			::wxLogError(wxT("SoundFileWriter: error from wxFFile::Write(), wanted %u available %u"), bytes, n);

		m_length += n;
	} else {
		for (unsigned int i = 0; i < (length * m_channels); i++)
			m_buffer16[i] = sint16(buffer[i] * 32768.0);

		unsigned int bytes = length * m_channels * sizeof(sint16);

		unsigned int n = m_file->Write(m_buffer16, bytes);
		if (n != bytes)
			::wxLogError(wxT("SoundFileWriter: error from wxFFile::Write(), wanted %u available %u"), bytes, n);

		m_length += n;
	}
}

void CSoundFileWriter::close()
{
	if (m_file != NULL) {
		if ((m_length % 2) != 0) {
			unsigned char c = 0;
			m_file->Write(&c, 1);
		}

		wxUint32 length = wxUINT32_SWAP_ON_BE(m_length);

		m_file->Seek(m_offset2);
		m_file->Write(&length, sizeof(wxUint32));

		length = wxUINT32_SWAP_ON_BE(m_length + 36);

		m_file->Seek(m_offset1);
		m_file->Write(&length, sizeof(wxUint32));

		m_file->Close();
		delete m_file;
		m_file = NULL;
	}

	delete[] m_buffer8;
	delete[] m_buffer16;

	delete this;
}

#endif
