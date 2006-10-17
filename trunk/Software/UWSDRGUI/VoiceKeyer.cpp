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

#include "VoiceKeyer.h"

#include "UWSDRApp.h"
#include "UWSDRDefs.h"


CVoiceKeyer::CVoiceKeyer() :
m_sampleRate(0.0F),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_status(VOICE_STOPPED),
m_file(NULL)
{
}

CVoiceKeyer::~CVoiceKeyer()
{
}

bool CVoiceKeyer::open(float sampleRate, unsigned int blockSize)
{
	wxASSERT(sampleRate > 0.0F);

	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	return true;
}

void CVoiceKeyer::close()
{
	if (m_file != NULL) {
		m_file->close();
		delete m_file;
	}

	m_status = VOICE_STOPPED;
}

void CVoiceKeyer::abort()
{
	if (m_file != NULL) {
		m_file->close();
		delete m_file;
		m_file = NULL;
	}

	m_status = VOICE_STOPPED;
}

void CVoiceKeyer::purge()
{
	wxASSERT(m_file != NULL);

	m_file->purge();
}

bool CVoiceKeyer::hasClock()
{
	return false;
}

void CVoiceKeyer::clock()
{
	wxASSERT(m_file != NULL);
	wxASSERT(m_status != VOICE_STOPPED);

	m_file->clock();
}

void CVoiceKeyer::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

void CVoiceKeyer::send(const wxString& fileName, int status)
{
	wxASSERT(status == VOICE_SINGLE || status == VOICE_CONTINUOUS);

	m_file = new CSoundFileReader(fileName);

	bool ret = m_file->open(m_sampleRate, m_blockSize);
	if (!ret) {
		::wxLogError(wxT("Unable to open sound file %s"), fileName.c_str());
		delete m_file;
		m_file = NULL;
		return;
	}

	m_file->setCallback(this, 0);

	m_status = status;
}

void CVoiceKeyer::callback(float* buffer, unsigned int nSamples, int id)
{
	wxASSERT(m_callback != NULL);

	// EOF in single mode means the end of transmission
	if (nSamples == 0 && m_status == VOICE_SINGLE) {
		m_status = VOICE_STOPPED;
		nSamples = m_blockSize;
		::memset(buffer, 0x00, m_blockSize * 2 * sizeof(float));
		::wxGetApp().sendAudio(wxEmptyString, VOICE_STOPPED);
	}

	// Restart the sound file and send a block of silence for now
	if (nSamples == 0 && m_status == VOICE_CONTINUOUS) {
		m_file->rewind();
		nSamples = m_blockSize;
		::memset(buffer, 0x00, m_blockSize * 2 * sizeof(float));
	}

	// Convert to our callback id
	m_callback->callback(buffer, nSamples, m_id);
}

bool CVoiceKeyer::isActive() const
{
	return m_status == VOICE_SINGLE || m_status == VOICE_CONTINUOUS;
}
