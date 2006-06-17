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

CSoundCardWriter::CSoundCardWriter(int api, int dev) :
m_api(api),
m_dev(dev),
m_stream(NULL)
{
}

CSoundCardWriter::~CSoundCardWriter()
{
}

bool CSoundCardWriter::open(unsigned int sampleRate, unsigned int blockSize)
{
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

	error = ::Pa_OpenStream(&m_stream, NULL, &params, double(sampleRate), blockSize, paNoFlag, NULL, NULL);
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
	wxASSERT(m_stream != NULL);
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);

	for (unsigned int i = 0; i < nSamples * 2; i++) {
		if (::fabs(buffer[i]) >= 1.0)
			::wxLogError(wxT("Invalid value in SoundCardWriter: %f"), buffer[i]);
	}

	PaError error = ::Pa_WriteStream(m_stream, buffer, nSamples);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from Pa_WriteStream() in SoundCardWriter"), error, ::Pa_GetErrorText(error));
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
}
