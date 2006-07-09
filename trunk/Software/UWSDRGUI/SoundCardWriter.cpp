/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
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


int scwCallback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	wxASSERT(userData != NULL);

	CSoundCardWriter* object = reinterpret_cast<CSoundCardWriter*>(userData);

	return object->callback(output, nSamples, timeInfo, statusFlags);
}


CSoundCardWriter::CSoundCardWriter(int api, int dev) :
m_api(api),
m_dev(dev),
m_stream(NULL),
m_buffer(NULL)
{
}

CSoundCardWriter::~CSoundCardWriter()
{
}

bool CSoundCardWriter::open(float sampleRate, unsigned int blockSize)
{
	m_buffer = new CRingBuffer(blockSize * 5, 2);

	PaError error = ::Pa_Initialize();
	if (error != paNoError) {
		::wxLogError(wxT("Received %d:%s from Pa_Initialise() in SoundCardWriter"), error, ::Pa_GetErrorText(error));
		return false;
	}

	PaDeviceIndex dev = ::Pa_HostApiDeviceIndexToDeviceIndex(m_api, m_dev);

	PaStreamParameters params;
	params.device                    = dev;
	params.channelCount              = 2;
	params.sampleFormat              = paFloat32;
	params.hostApiSpecificStreamInfo = NULL;
	params.suggestedLatency          = PaTime(0);

	error = ::Pa_OpenStream(&m_stream, NULL, &params, sampleRate, blockSize, paNoFlag, &scwCallback, this);
	if (error != paNoError) {
		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_OpenStream() in SoundCardWriter"), error, ::Pa_GetErrorText(error));
		return false;
	}

	error = ::Pa_StartStream(m_stream);
	if (error != paNoError) {
		::Pa_CloseStream(m_stream);
		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_StartStream() in SoundCardWriter"), error, ::Pa_GetErrorText(error));
		return false;
	}

	return true;
}

void CSoundCardWriter::write(const float* buffer, unsigned int nSamples)
{
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);

	unsigned int n = m_buffer->addData(buffer, nSamples);

	if (n != nSamples)
		::wxLogError(wxT("Buffer overrun in SoundCardWriter, wanted=%u have=%u"), nSamples, n);
}

int CSoundCardWriter::callback(void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags)
{
	wxASSERT(output != NULL);

	// No output data may not be a problem, we could be on transmit
	if (m_buffer->dataSpace() >= nSamples)
		m_buffer->getData((float*)output, nSamples);
	else
		::memset(output, 0x00, nSamples * 2 * sizeof(float));

	return paContinue;
}

void CSoundCardWriter::close()
{
	PaError error = ::Pa_AbortStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from Pa_AbortStream() in SoundCardWriter"), error, ::Pa_GetErrorText(error));

	error = ::Pa_CloseStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from Pa_CloseStream() in SoundCardWriter"), error, ::Pa_GetErrorText(error));

	error = ::Pa_Terminate();
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from Pa_Terminate() in SoundCardWriter"), error, ::Pa_GetErrorText(error));

	delete m_buffer;
}
