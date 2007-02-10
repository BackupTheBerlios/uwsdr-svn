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

#include "SoundCardReader.h"


int scrCallback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	wxASSERT(userData != NULL);

	CSoundCardReader* object = reinterpret_cast<CSoundCardReader*>(userData);

	return object->callback(input, nSamples, timeInfo, statusFlags);
}


CSoundCardReader::CSoundCardReader(int api, int dev) :
m_api(api),
m_dev(dev),
m_callback(NULL),
m_id(0),
m_stream(NULL)
{
}

CSoundCardReader::~CSoundCardReader()
{
}

void CSoundCardReader::setCallback(IDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

bool CSoundCardReader::open(float sampleRate, unsigned int blockSize)
{
	wxASSERT(m_callback != NULL);

	PaError error = ::Pa_Initialize();
	if (error != paNoError) {
		::wxLogError(wxT("Received %d:%s from Pa_Initialise() in SoundCardReader"), error, ::Pa_GetErrorText(error));
		return false;
	}

	PaDeviceIndex dev = ::Pa_HostApiDeviceIndexToDeviceIndex(m_api, m_dev);
	if (dev < 0) {
		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_HostApiDeviceIndexToDeviceIndex() in SoundCardReader for API:%d Dev:%d"), error, ::Pa_GetErrorText(dev), m_api, m_dev);
		return false;
	}

	const PaDeviceInfo* info = ::Pa_GetDeviceInfo(dev);

	PaStreamParameters params;
	params.device                    = dev;
	params.channelCount              = 2;
	params.sampleFormat              = paFloat32;
	params.hostApiSpecificStreamInfo = NULL;
	params.suggestedLatency          = info->defaultLowInputLatency;

	error = ::Pa_OpenStream(&m_stream, &params, NULL, sampleRate, blockSize, paNoFlag, &scrCallback, this);
	if (error != paNoError) {
		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_OpenStream() in SoundCardReader"), error, ::Pa_GetErrorText(error));
		return false;
	}

	error = ::Pa_StartStream(m_stream);
	if (error != paNoError) {
		::Pa_CloseStream(m_stream);
		m_stream = NULL;

		::Pa_Terminate();
		::wxLogError(wxT("Received %d:%s from Pa_StartStream() in SoundCardReader"), error, ::Pa_GetErrorText(error));
		return false;
	}

	return true;
}

int CSoundCardReader::callback(const void* input, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags)
{
	wxASSERT(m_callback != NULL);
	wxASSERT(input != NULL);

	m_callback->callback((float *)input, nSamples, m_id);

	return paContinue;
}

void CSoundCardReader::close()
{
	PaError error = ::Pa_AbortStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from Pa_AbortStream() in SoundCardReader"), error, ::Pa_GetErrorText(error));

	error = ::Pa_CloseStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from Pa_CloseStream() in SoundCardReader"), error, ::Pa_GetErrorText(error));

	error = ::Pa_Terminate();
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from Pa_Terminate() in SoundCardReader"), error, ::Pa_GetErrorText(error));

	delete this;
}

void CSoundCardReader::purge()
{
}

bool CSoundCardReader::hasClock()
{
	return true;
}

void CSoundCardReader::clock()
{
}
