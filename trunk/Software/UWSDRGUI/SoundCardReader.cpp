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

#include "SoundCardReader.h"

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
	m_callback = callback;
	m_id       = id;
}

bool CSoundCardReader::open(unsigned int sampleRate, unsigned int blockSize)
{
	wxASSERT(m_callback != NULL);

	PaError error = ::PA_Initialize();
	if (error != paNoError) {
		::wxLogError(_("Received %d:%s from PA_Initialise() in SoundCardReader"), error, ::PA_GetErrorText(error));
		return false;
	}

	PaDeviceIndex dev = ::PA_HostApiDeviceIndexToDeviceIndex(m_api, m_dev);

	PaStreamParameters params;
	params.device                    = dev;
	params.channelCount              = 2;
	params.sampleFormat              = paFloat32;
	params.hostApiSpecificStreamInfo = NULL;
	params.suggestedLatency          = PaTime(0);

	error = ::PA_OpenStream(&m_stream, &params, NULL, double(sampleRate), blockSize, paNoFlag, cCallback, this);
	if (error != paNoError) {
		::PA_Terminate();
		::wxLogError(_("Received %d:%s from PA_OpenStream() in SoundCardReader"), error, ::PA_GetErrorText(error));
		return false;
	}

	error = ::PA_StartStream(m_stream);
	if (error != paNoError) {
		::PA_CloseStream(m_stream);
		::PA_Terminate();
		::wxLogError(_("Received %d:%s from PA_StartStream() in SoundCardReader"), error, ::PA_GetErrorText(error));
		return false;
	}

	return true;
}

int CSoundCardReader::cCallback(const void* input, void* output, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	CSoundCardReader* object = static_cast<CSoundCardReader*>(userData);

	return object->callback(input, nSamples, timeInfo, statusFlags);
}

int CSoundCardReader::callback(const void* input, unsigned long nSamples, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags)
{
	wxASSERT(m_callback != NULL);

	m_callback->callback((float *)input, nSamples, m_id);

	return paContinue;
}

void CSoundCardReader::close()
{
	PaError error = ::PA_AbortStream(m_stream);
	if (error != paNoError)
		::wxLogError(_("Received %d:%s from PA_AbortStream() in SoundCardReader"), error, ::PA_GetErrorText(error));

	error = ::PA_CloseStream(m_stream);
	if (error != paNoError)
		::wxLogError(_("Received %d:%s from PA_CloseStream() in SoundCardReader"), error, ::PA_GetErrorText(error));

	error = ::PA_Terminate();
	if (error != paNoError)
		::wxLogError(_("Received %d:%s from PA_Terminate() in SoundCardReader"), error, ::PA_GetErrorText(error));
}
