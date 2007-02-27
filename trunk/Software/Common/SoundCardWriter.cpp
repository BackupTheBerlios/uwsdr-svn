/*
 *   Copyright (C) 2006-2007 by Jonathan Naylor G4KLX
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

#include "SoundCardWriter.h"


int scwCallback(const void* WXUNUSED(input), void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	wxASSERT(userData != NULL);

	CSoundCardWriter* object = reinterpret_cast<CSoundCardWriter*>(userData);

	return object->callback(output, nSamples, timeInfo, statusFlags);
}


CSoundCardWriter::CSoundCardWriter(int dev, unsigned int channels) :
m_dev(dev),
m_channels(channels),
m_blockSize(0),
m_stream(NULL),
m_buffer(NULL),
m_lastBuffer(NULL),
m_inBuffer(NULL),
m_requests(0),
m_underruns(0),
m_overruns(0),
m_enabled(false)
{
	wxASSERT(channels == 1U || channels == 2U);
}

CSoundCardWriter::~CSoundCardWriter()
{
	delete   m_buffer;
	delete[] m_lastBuffer;
	delete[] m_inBuffer;
}

bool CSoundCardWriter::open(float sampleRate, unsigned int blockSize)
{
	m_blockSize = blockSize;
	m_enabled   = false;

	m_buffer = new CRingBuffer(blockSize * 5U, 2U);

	m_lastBuffer = new float[blockSize * 2U];
	::memset(m_lastBuffer, 0x00, blockSize * 2U * sizeof(float));

	m_inBuffer = new float[blockSize * 2U];

	PaError error = ::Pa_Initialize();
	if (error != paNoError) {
		::wxLogError(wxT("SoundCardWriter: received %d:%s from Pa_Initialise()"), error, ::Pa_GetErrorText(error));
		return false;
	}

	const PaDeviceInfo* info = ::Pa_GetDeviceInfo(m_dev);
	if (info == NULL) {
		::wxLogError(wxT("SoundCardWriter: received NULL from Pa_GetDeviceInfo()"));
		return false;
	}

	PaStreamParameters params;
	params.device                    = m_dev;
	params.channelCount              = m_channels;
	params.sampleFormat              = paFloat32;
	params.hostApiSpecificStreamInfo = NULL;
	params.suggestedLatency          = info->defaultLowOutputLatency;

	error = ::Pa_OpenStream(&m_stream, NULL, &params, sampleRate, blockSize, paNoFlag, &scwCallback, this);
	if (error != paNoError) {
		::Pa_Terminate();
		::wxLogError(wxT("SoundCardWriter: received %d:%s from Pa_OpenStream()"), error, ::Pa_GetErrorText(error));
		return false;
	}

	error = ::Pa_StartStream(m_stream);
	if (error != paNoError) {
		::Pa_CloseStream(m_stream);
		m_stream = NULL;

		::Pa_Terminate();
		::wxLogError(wxT("SoundCardWriter: Received %d:%s from Pa_StartStream()"), error, ::Pa_GetErrorText(error));
		return false;
	}

	::wxLogMessage(wxT("SoundCardWriter: started with device %d"), m_dev);

	return true;
}

void CSoundCardWriter::write(const float* buffer, unsigned int nSamples)
{
	if (!m_enabled)
		return;

	if (nSamples == 0U)
		return;

	wxASSERT(buffer != NULL);

	if (m_channels == 1U) {
		for (unsigned int i = 0U; i < nSamples; i++) {
			m_inBuffer[i * 2U + 0U] = buffer[i];
			m_inBuffer[i * 2U + 1U] = buffer[i];
		}

		buffer = m_inBuffer;
	}

	unsigned int n = m_buffer->addData(buffer, nSamples);

	if (n != nSamples)
		m_overruns++;
}

int CSoundCardWriter::callback(void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* WXUNUSED(timeInfo), PaStreamCallbackFlags WXUNUSED(statusFlags))
{
	wxASSERT(output != NULL);

	m_requests++;

	if (m_buffer->dataSpace() >= nSamples) {
		m_buffer->getData((float*)output, nSamples);
	} else {
		::memcpy(output, m_lastBuffer, nSamples * 2 * sizeof(float));
		m_underruns++;
	}

	::memcpy(m_lastBuffer, output, nSamples * 2 * sizeof(float));
	
	return paContinue;
}

void CSoundCardWriter::close()
{
	PaError error = ::Pa_AbortStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("SoundCardWriter: received %d:%s from Pa_AbortStream()"), error, ::Pa_GetErrorText(error));

	error = ::Pa_CloseStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("SoundCardWriter: received %d:%s from Pa_CloseStream()"), error, ::Pa_GetErrorText(error));

	error = ::Pa_Terminate();
	if (error != paNoError)
		::wxLogError(wxT("SoundCardWriter: received %d:%s from Pa_Terminate()"), error, ::Pa_GetErrorText(error));

	::wxLogMessage(wxT("SoundCardWriter: %u underruns and %u overruns from %u requests"), m_underruns, m_overruns, m_requests);

	delete this;
}

void CSoundCardWriter::enable(bool enable)
{
	m_enabled = enable;

	if (!enable)
		::memset(m_lastBuffer, 0x00, m_blockSize * 2U * sizeof(float));
}

void CSoundCardWriter::disable()
{
	enable(false);
}
