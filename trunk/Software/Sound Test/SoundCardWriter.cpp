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
	PaError error = ::PA_Initialize();
	if (error != paNoError) {
		::wxLogError(wxT("Received %d:%s from PA_Initialise() in SoundCardWriter"), error, ::PA_GetErrorText(error));
		return false;
	}

	PaDeviceIndex dev = ::PA_HostApiDeviceIndexToDeviceIndex(m_api, m_dev);

	PaStreamParameters params;
	params.device                    = dev;
	params.channelCount              = 2;
	params.sampleFormat              = paFloat32;
	params.hostApiSpecificStreamInfo = NULL;
	params.suggestedLatency          = PaTime(0);

	error = ::PA_OpenStream(&m_stream, NULL, &params, double(sampleRate), blockSize, paNoFlag, NULL, NULL);
	if (error != paNoError) {
		::PA_Terminate();
		::wxLogError(wxT("Received %d:%s from PA_OpenStream() in SoundCardWriter"), error, ::PA_GetErrorText(error));
		return false;
	}

	error = ::PA_StartStream(m_stream);
	if (error != paNoError) {
		::PA_CloseStream(m_stream);
		::PA_Terminate();
		::wxLogError(wxT("Received %d:%s from PA_StartStream() in SoundCardWriter"), error, ::PA_GetErrorText(error));
		return false;
	}

	return true;
}

void CSoundCardWriter::write(const float* buffer, unsigned int nSamples)
{
	wxASSERT(m_stream != NULL);
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);

	PaError error = ::PA_WriteStream(m_stream, buffer, nSamples);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from PA_WriteStream() in SoundCardWriter"), error, ::PA_GetErrorText(error));
}

void CSoundCardWriter::close()
{
	PaError error = ::PA_AbortStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from PA_AbortStream() in SoundCardWriter"), error, ::PA_GetErrorText(error));

	error = ::PA_CloseStream(m_stream);
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from PA_CloseStream() in SoundCardWriter"), error, ::PA_GetErrorText(error));

	error = ::PA_Terminate();
	if (error != paNoError)
		::wxLogError(wxT("Received %d:%s from PA_Terminate() in SoundCardWriter"), error, ::PA_GetErrorText(error));
}
