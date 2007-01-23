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

#include "SoundCardReaderWriter.h"


int scrwCallback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	wxASSERT(userData != NULL);

	CSoundCardReaderWriter* object = reinterpret_cast<CSoundCardReaderWriter*>(userData);

	return object->callback(input, output, nSamples, timeInfo, statusFlags);
}


CSoundCardReaderWriter::CSoundCardReaderWriter(int api, int dev) :
m_api(api),
m_dev(dev),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_stream(NULL),
m_buffer(NULL),
m_lastBuffer(NULL),
m_requests(0),
m_underruns(0),
m_overruns(0),
m_enabled(false),
m_opened(0)
{
}

CSoundCardReaderWriter::~CSoundCardReaderWriter()
{
}

void CSoundCardReaderWriter::setCallback(IDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

bool CSoundCardReaderWriter::open(float sampleRate, unsigned int blockSize)
{
	if (m_opened > 0) {
		m_opened++;
		return true;
	}

	m_blockSize = blockSize;
	m_enabled   = false;

	m_buffer = new CRingBuffer(blockSize * 5, 2);

	m_lastBuffer = new float[blockSize * 2];
	::memset(m_lastBuffer, 0x00, blockSize * 2 * sizeof(float));

	PaError error = ::Pa_Initialize();
	if (error != paNoError) {
		::wxLogError(wxT("Received %d:%s from Pa_Initialise() in SoundCardReaderWriter"), error, ::Pa_GetErrorText(error));
		return false;
	}

	PaDeviceIndex dev = ::Pa_HostApiDeviceIndexToDeviceIndex(m_api, m_dev);
	if (dev < 0) {
		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_HostApiDeviceIndexToDeviceIndex() in SoundCardReaderWriter for API:%d Dev:%d"), error, ::Pa_GetErrorText(dev), m_api, m_dev);
		return false;
	}

	PaStreamParameters params;
	params.device                    = dev;
	params.channelCount              = 2;
	params.sampleFormat              = paFloat32;
	params.hostApiSpecificStreamInfo = NULL;
	params.suggestedLatency          = PaTime(0);

	error = ::Pa_OpenStream(&m_stream, &params, &params, sampleRate, blockSize, paNoFlag, &scrwCallback, this);
	if (error != paNoError) {
		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_OpenStream() in SoundCardReaderWriter"), error, ::Pa_GetErrorText(error));
		return false;
	}

	error = ::Pa_StartStream(m_stream);
	if (error != paNoError) {
		::Pa_CloseStream(m_stream);
		m_stream = NULL;

		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_StartStream() in SoundCardReaderWriter"), error, ::Pa_GetErrorText(error));
		return false;
	}

	m_opened++;

	return true;
}

void CSoundCardReaderWriter::write(const float* buffer, unsigned int nSamples)
{
	if (!m_enabled)
		return;

	if (nSamples == 0)
		return;

	wxASSERT(buffer != NULL);

	unsigned int n = m_buffer->addData(buffer, nSamples);

	if (n != nSamples)
		m_overruns++;
}

int CSoundCardReaderWriter::callback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags)
{
	wxASSERT(input != NULL);
	wxASSERT(output != NULL);
	wxASSERT(m_callback != NULL);

	m_requests++;

	m_callback->callback((float *)input, nSamples, m_id);

	if (m_buffer->dataSpace() >= nSamples) {
		m_buffer->getData((float*)output, nSamples);
	} else {
		::memcpy(output, m_lastBuffer, nSamples * 2 * sizeof(float));
		m_underruns++;
	}

	::memcpy(m_lastBuffer, output, nSamples * 2 * sizeof(float));
	
	return paContinue;
}

void CSoundCardReaderWriter::close()
{
	if (m_opened > 1) {
		m_opened--;
		return;
	}

	if (m_stream != NULL) {
		PaError error = ::Pa_AbortStream(m_stream);
		if (error != paNoError)
			::wxLogError(wxT("Received %d:%s from Pa_AbortStream() in SoundCardReaderWriter"), error, ::Pa_GetErrorText(error));

		error = ::Pa_CloseStream(m_stream);
		if (error != paNoError)
			::wxLogError(wxT("Received %d:%s from Pa_CloseStream() in SoundCardReaderWriter"), error, ::Pa_GetErrorText(error));

		error = ::Pa_Terminate();
		if (error != paNoError)
			::wxLogError(wxT("Received %d:%s from Pa_Terminate() in SoundCardReaderWriter"), error, ::Pa_GetErrorText(error));

		m_stream = NULL;
	}

	delete   m_buffer;
	delete[] m_lastBuffer;

	m_opened  = 0;
	m_enabled = false;

	::wxLogMessage(wxT("SoundCardReaderWriter: %u underruns and %u overruns from %u requests"), m_underruns, m_overruns, m_requests);
}

void CSoundCardReaderWriter::enable(bool enable)
{
	m_enabled = enable;

	if (!enable)
		::memset(m_lastBuffer, 0x00, m_blockSize * 2 * sizeof(float));
}

void CSoundCardReaderWriter::disable()
{
	enable(false);
}

void CSoundCardReaderWriter::purge()
{
}

bool CSoundCardReaderWriter::hasClock()
{
	return true;
}

void CSoundCardReaderWriter::clock()
{
}
