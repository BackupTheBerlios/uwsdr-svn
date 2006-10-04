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

#include "SignalReader.h"

#include <wx/debug.h>
#include <wx/log.h>


CSignalReader::CSignalReader(unsigned int frequency, float noiseAmplitude, float signalAmplitude) :
wxThread(),
m_frequency(frequency),
m_noiseAmplitude(noiseAmplitude),
m_signalAmplitude(signalAmplitude),
m_sampleRate(0.0F),
m_blockSize(0),
m_callback(NULL),
m_id(0)
{
	wxASSERT(m_noiseAmplitude >= 0.0 && m_noiseAmplitude < 1.0);
	wxASSERT(m_signalAmplitude >= 0.0 && m_signalAmplitude < 1.0);
	wxASSERT((m_signalAmplitude + m_noiseAmplitude) < 1.0);
	wxASSERT(m_frequency > 0);
}

CSignalReader::~CSignalReader()
{
}

void CSignalReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CSignalReader::open(float sampleRate, unsigned int blockSize)
{
	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	Create();
	Run();

	return true;
}

void* CSignalReader::Entry()
{
	wxASSERT(m_callback != NULL);
	wxASSERT(m_frequency < (unsigned int)(m_sampleRate + 0.5F) / 2);

	unsigned int noiseSize = int(m_sampleRate + 0.5F) * 10 * 2;

	long interval = (1000L * m_blockSize) / int(m_sampleRate + 0.5F);

	float* buffer = new float[m_blockSize * 2];
	float* awgn   = new float[noiseSize];

	float delta = float(m_frequency) / m_sampleRate * 2.0 * M_PI;

	float cosVal = 1.0;
	float sinVal = 0.0;
	float tmpVal;

	float cosDelta = ::cos(delta);
	float sinDelta = ::sin(delta);

	for (unsigned int i = 0; i < noiseSize / 2; i++) {
		float x1, x2, w;

		do {
			x1 = (2.0 * (float(::rand()) / float(RAND_MAX)) - 1.0);
			x2 = (2.0 * (float(::rand()) / float(RAND_MAX)) - 1.0);
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0);

		w = ::sqrt((-2.0 * ::log(w)) / w);

		awgn[i * 2 + 0] = x1 * w * m_noiseAmplitude;
		awgn[i * 2 + 1] = x2 * w * m_noiseAmplitude;
	}

	unsigned int awgnN = 0;

	::wxStartTimer();

	while (!TestDestroy()) {
		for (unsigned int i = 0; i < m_blockSize; i++) {
			tmpVal = cosVal * cosDelta - sinVal * sinDelta;
			sinVal = cosVal * sinDelta + sinVal * cosDelta;
			cosVal = tmpVal;

			buffer[i * 2 + 0] = awgn[awgnN++] + cosVal * m_signalAmplitude;
			buffer[i * 2 + 1] = awgn[awgnN++] + sinVal * m_signalAmplitude;

			if (awgnN >= noiseSize)
				awgnN = 0;
		}

		m_callback->callback(buffer, m_blockSize, m_id);

		long diff = ::wxGetElapsedTime();

		int sleepTime = interval - diff;
		if (sleepTime > 0)
			Sleep(sleepTime);

		::wxStartTimer();
	}

	delete[] awgn;
	delete[] buffer;

	return (void*)0;
}

void CSignalReader::close()
{
	Delete();
}