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

#include "CWKeyer.h"

#include "UWSDRApp.h"


const int DOT_LEN    = 1;
const int DASH_LEN   = 3;
const int SYMBOL_GAP = 1;
const int LETTER_GAP = 3;
const int WORD_GAP   = 7;


CCWKeyer::CCWKeyer() :
m_sampleRate(0.0F),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_dotLen(0),
m_text(),
m_stop(false),
m_buffer(NULL),
m_dotBuffer(NULL),
m_dashBuffer(NULL),
m_silBuffer(NULL),
m_cwBuffer(NULL)
{
}

CCWKeyer::~CCWKeyer()
{
}

bool CCWKeyer::open(float sampleRate, unsigned int blockSize)
{
	wxASSERT(sampleRate > 0.0F);

	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	m_buffer = new float[m_blockSize * 2 * 2];

	// Allocate buffers based on the slowest speed that we handle
	unsigned int dotLen = calcDotLength(5);

	m_dotBuffer  = new float[dotLen * DOT_LEN];
	m_dashBuffer = new float[dotLen * DASH_LEN];
	m_silBuffer  = new float[dotLen * DOT_LEN];

	::memset(m_silBuffer, 0x00, dotLen * DOT_LEN * sizeof(float));

	m_cwBuffer = new CRingBuffer(dotLen * 10, 1);

	return true;
}

void CCWKeyer::close()
{
	m_stop = true;

	delete[] m_buffer;
	delete[] m_dotBuffer;
	delete[] m_dashBuffer;
	delete[] m_silBuffer;
	delete   m_cwBuffer;
}

void CCWKeyer::abort()
{
	m_stop = true;
}

void CCWKeyer::purge()
{
	// Anything to do ?
}

bool CCWKeyer::hasClock()
{
	return false;
}

/*
 * This is called from the "software interrupt" generated by the sound card carrying
 * the microphone audio. It is called once per BLOCK_SIZE samples.
 */
void CCWKeyer::clock()
{
	wxASSERT(m_callback != NULL);
	wxASSERT(m_cwBuffer != NULL);

	// Aborted or end of transmission
	if (m_stop || (m_cwBuffer->dataSpace() == 0 && m_text.length() == 0)) {
		for (unsigned int i = 0; i < m_blockSize * 2; i++)
			m_buffer[i] = 0.0F;
		m_callback->callback(m_buffer, m_blockSize, m_id);

		m_stop = false;
		::wxGetApp().sendCW(0, wxEmptyString);

		return;
	}

	if (m_cwBuffer->dataSpace() < m_blockSize)
		fillBuffer();

	for (unsigned int i = 0; i < m_blockSize; i++) {
		float f;
		unsigned int n = m_cwBuffer->getData(&f, 1);

		// No more data, so fill with silence
		if (n == 0)
			f = 0.0F;

		m_buffer[i * 2 + 0] = f;
		m_buffer[i * 2 + 1] = f;
	}

	m_callback->callback(m_buffer, m_blockSize, m_id);
}

void CCWKeyer::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

/*
 * Set the speed in WPM and generate the dot length in samples as well as the
 * dot and dash samples.
 */
void CCWKeyer::send(unsigned int speed, const wxString& text)
{
	wxASSERT(speed >= 5 && speed <= 30);
	wxASSERT(m_cwBuffer != NULL);

	m_dotLen = calcDotLength(speed);

	createSymbol(m_dotBuffer,  m_dotLen * DOT_LEN);
	createSymbol(m_dashBuffer, m_dotLen * DASH_LEN);

	if (text.length() == 0)
		return;

	m_cwBuffer->clear();

	m_stop = false;
	m_text = text;
}

/*
 * Create a sine wave of the correct tone of the desired length and shape it with
 * a raised cosine to minimise key clicks.
 */
void CCWKeyer::createSymbol(float* buffer, unsigned int len)
{
	wxASSERT(buffer != NULL);

	float delta = float(CW_OFFSET) / m_sampleRate * 2.0 * M_PI;

	float cosVal = 1.0F;
	float sinVal = 0.0F;
	float tmpVal;

	float cosDelta = ::cos(delta);
	float sinDelta = ::sin(delta);

	for (unsigned int i = 0; i < len; i++) {
		tmpVal = cosVal * cosDelta - sinVal * sinDelta;
		sinVal = cosVal * sinDelta + sinVal * cosDelta;
		cosVal = tmpVal;

		buffer[i] = sinVal;
	}

	unsigned int l = (m_dotLen * DOT_LEN) / 10;

	// Now shape it
	for (unsigned int j = 0; j < l; j++) {
		float ampl = 0.5F * (1.0F + ::cos(M_PI + M_PI * (float(j) / float(l))));

		buffer[j]       *= ampl;
		buffer[len - j] *= ampl;
	}
}

void CCWKeyer::fillBuffer()
{
	wxASSERT(m_cwBuffer != NULL);

	while (m_cwBuffer->freeSpace() > (m_dotLen * DASH_LEN * 3) && m_text.length() > 0) {
		wxChar c = m_text.GetChar(0);

		switch (c) {
			case wxT('.'):
				m_cwBuffer->addData(m_dotBuffer, m_dotLen * DOT_LEN);
				m_cwBuffer->addData(m_silBuffer, m_dotLen * DOT_LEN);
				break;

			case wxT('-'):
				m_cwBuffer->addData(m_dashBuffer, m_dotLen * DASH_LEN);
				m_cwBuffer->addData(m_silBuffer,  m_dotLen * DOT_LEN);
				break;

			case wxT(' '):
				m_cwBuffer->addData(m_silBuffer, m_dotLen * DOT_LEN);
				m_cwBuffer->addData(m_silBuffer, m_dotLen * DOT_LEN);
				m_cwBuffer->addData(m_silBuffer, m_dotLen * DOT_LEN);
				break;
		}

		m_text = m_text.Right(m_text.length() - 1);
	}
}

/*
 * Calculate the length of a dot in samples from the speed in words per minute.
 * At 12 WPM a dot is 1/10 of a second.
 */
unsigned int CCWKeyer::calcDotLength(int speed)
{
	return (unsigned int)(m_sampleRate * 1.2F / float(speed) + 0.5F);
}

// We are always active in CW mode
bool CCWKeyer::isActive() const
{
	return true;
}
