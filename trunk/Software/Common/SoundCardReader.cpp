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


int scrCallback(const void* input, void* WXUNUSED(output), unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
{
	wxASSERT(userData != NULL);

	CSoundCardReader* object = reinterpret_cast<CSoundCardReader*>(userData);

	return object->callback(input, nSamples, timeInfo, statusFlags);
}


CSoundCardReader::CSoundCardReader(int dev) :
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
		::wxLogError(wxT("SoundCardReader: received %d:%s from Pa_Initialise()"), error, ::Pa_GetErrorText(error));
		return false;
	}

	const PaDeviceInfo* info = ::Pa_GetDeviceInfo(m_dev);
	if (info == NULL) {
		::wxLogError(wxT("SoundCardReader: received NULL from Pa_GetDeviceInfo()"));
		return false;
	}

	PaStreamParameters params;
	params.device                    = m_dev;
	params.channelCount              = 2;
	params.sampleFormat              = paFloat32;
	params.hostApiSpecificStreamInfo = NULL;
	params.suggestedLatency          = info->defaultLowInputLatency;

	error = ::Pa_OpenStream(&m_stream, &params, NULL, sampleRate, blockSize, paNoFlag, &scrCallback, this);
	if (error != paNoError) {
		::Pa_Terminate();
		::wxLogError(wxT("SoundCardReader: received %d:%s from Pa_OpenStream()"), error, ::Pa_GetErrorText(error));
		return false;
	}

	error = ::Pa_StartStream(m_stream);
	if (error != paNoError) {
		::Pa_CloseStream(m_stream);
		m_stream = NULL;

		::Pa_Terminate();
		::wxLogError(wxT("SoundCardReader: received %d:%s from Pa_StartStream()"), error, ::Pa_GetErrorText(error));
		return false;
	}

	::wxLogMessage(wxT("SoundCardReader: started with device %d"), m_dev);

	return true;
}

int CSoundCardReader::callback(const void* input, unsigned long nSamples, const PaStreamCallbackTimeInfo* WXUNUSED(timeInfo), PaStreamCallbackFlags WXUNUSED(statusFlags))
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
		::wxLogError(wxT("SoundCardReader: received %d:%s from Pa_AbortStream()"), error, ::Pa_GetErrorText(error));

	error = ::Pa_CloseStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("SoundCardReader: received %d:%s from Pa_CloseStream()"), error, ::Pa_GetErrorText(error));

	error = ::Pa_Terminate();
	if (error != paNoError)
		::wxLogError(wxT("SoundCardReader: received %d:%s from Pa_Terminate()"), error, ::Pa_GetErrorText(error));

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
