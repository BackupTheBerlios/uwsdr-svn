/*
 *   Copyright (C) 2007 by Jonathan Naylor G4KLX
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

#include "JackReaderWriter.h"

#if defined(USE_JACK)

int jcrwCallback(jack_nframes_t nFrames, void* userData)
{
	wxASSERT(userData != NULL);

	CJackReaderWriter* object = reinterpret_cast<CJackReaderWriter*>(userData);

	return object->callback(nFrames);
}


CJackReaderWriter::CJackReaderWriter(const wxString& name, unsigned int inChannels, unsigned int outChannels) :
m_name(name),
m_inChannels(inChannels),
m_outChannels(outChannels),
m_blockSize(0U),
m_callback(NULL),
m_id(0),
m_client(NULL),
m_inPort(NULL),
m_outPort(NULL),
m_buffer(NULL),
m_lastBuffer(NULL),
m_inBuffer(NULL),
m_outBuffer(NULL),
m_requests(0),
m_underruns(0),
m_overruns(0),
m_enabled(false),
m_opened(0),
m_active(false)
{
	wxASSERT(inChannels <= 2U);
	wxASSERT(outChannels <= 2U);
}

CJackReaderWriter::~CJackReaderWriter()
{
	delete   m_buffer;
	delete[] m_lastBuffer;
	delete[] m_inBuffer;
	delete[] m_outBuffer;
}

void CJackReaderWriter::setCallback(IDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

bool CJackReaderWriter::open(float sampleRate, unsigned int blockSize)
{
	if (m_opened > 0) {
		m_opened++;
		return true;
	}

	m_blockSize = blockSize;
	m_enabled   = false;

	jack_status_t status;
	m_client = ::jack_client_open(m_name.c_str(), JackNullOption, &status, NULL);
	if (m_client == NULL) {
		::wxLogError(wxT("JackReaderWriter: received 0x%02X from jack_client_open()"), status);
		return false;
	}

	::jack_set_process_callback(m_client, jcrwCallback, this);

	jack_nframes_t jsr = ::jack_get_sample_rate(m_client);
	if (float(jsr) != sampleRate) {
		::wxLogError(wxT("JackReaderWriter: incorrect Jack sample rate of " PRIu32), jsr);
		::jack_client_close(m_client);
		return false;
	}

	if (m_inChannels > 0U) {
		m_inPort = ::jack_port_register(m_client, "Input", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);

		if (m_inPort == NULL) {
			::wxLogError(wxT("JackReaderWriter: unable to open the Jack input port");
			::jack_client_close(m_client);
			return false;
		}
	}

	if (m_outChannels > 0U) {
		m_outPort = ::jack_port_register(m_client, "Output", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

		if (m_outPort == NULL) {
			::wxLogError(wxT("JackReaderWriter: unable to open the Jack output port");
			::jack_client_close(m_client);
			return false;
		}
	}

	int ret = ::jack_activate(m_client);
	if (ret != 0) {
		::wxLogError(wxT("JackReaderWriter: error %d when activating the Jack client", ret);
		::jack_client_close(m_client);
		return false;
	}

	m_opened++;
	m_active = true;

	::wxLogMessage(wxT("JackReaderWriter: started for client %s"), m_name.c_str());

	return true;
}

void CJackReaderWriter::write(const float* buffer, unsigned int nSamples)
{
	if (!m_enabled || m_outDev == -1)
		return;

	if (nSamples == 0U)
		return;

	wxASSERT(buffer != NULL);

	unsigned int n = m_buffer->addData(buffer, nSamples);

	if (n != nSamples)
		m_overruns++;
}

int CJackReaderWriter::callback(jack_nframes_t nFrames)
{
	m_requests++;

	if (m_inChannels > 0U) {
		wxASSERT(m_callback != NULL);
		wxASSERT(m_inBuffer != NULL);

		jack_default_audio_sample_t* input = ::jack_port_get_buffer(m_inPort, nFrames);

		if (m_inChannels == 1U) {
			for (unsigned int i = 0U; i < nFrames; i++) {
				m_inBuffer[i * 2U + 0U] = input[i];
				m_inBuffer[i * 2U + 1U] = input[i];
			}

			input = m_inBuffer;
		}

		m_callback->callback(input, nFrames, m_id);
	}

	if (m_outChannels > 0U) {
		wxASSERT(m_buffer != NULL);
		wxASSERT(m_outBuffer != NULL);
		wxASSERT(m_lastBuffer != NULL);

		float* output = ::jack_port_get_buffer(m_outPort, nFrames);

		if (m_buffer->dataSpace() >= nFrames) {
			m_buffer->getData(m_outBuffer, nFrames);
		} else {
			::memcpy(m_outBuffer, m_lastBuffer, nFrames * 2U * sizeof(float));
			m_underruns++;
		}

		::memcpy(m_lastBuffer, m_outBuffer, nFrames * 2U * sizeof(float));

		switch (m_outChannels) {
			case 1U: {
					for (unsigned int i = 0U; i < nFrames; i++)
						output[i] = m_outBuffer[i * 2U + 0U];
				}
				break;
			case 2U:
				::memcpy(output, m_outBuffer, nFrames * 2U * sizeof(float));
				break;
		}
	}

	return 0;
}

/*
 * Close the sound card on the first call to close() all later ones are NOPs until the last one which also
 * delete's the object.
 */
void CJackReaderWriter::close()
{
	if (!m_active) {
		if (m_opened == 0U) {
			delete this;
			return;
		}

		m_opened--;
		return;
	}

	m_opened--;
	m_active = false;

	int ret = ::jack_client_close(m_client);
	if (ret != 0)
		::wxLogError(wxT("JackReaderWriter: received %d from jack_client_close()"), ret);

	::wxLogMessage(wxT("JackReaderWriter: %u underruns and %u overruns from %u requests"), m_underruns, m_overruns, m_requests);
}

void CJackReaderWriter::enable(bool enable)
{
	m_enabled = enable;

	if (!enable)
		::memset(m_lastBuffer, 0x00, m_blockSize * 2U * sizeof(float));
}

void CJackReaderWriter::disable()
{
	enable(false);
}

void CJackReaderWriter::purge()
{
}

bool CJackReaderWriter::hasClock()
{
	return true;
}

void CJackReaderWriter::clock()
{
}

#endif
