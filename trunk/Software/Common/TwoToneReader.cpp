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

#include "TwoToneReader.h"

#include <wx/debug.h>
#include <wx/log.h>


CTwoToneReader::CTwoToneReader(float frequency1, float amplitude1, float frequency2, float amplitude2, IDataReader* reader) :
m_frequency1(frequency1),
m_amplitude1(amplitude1),
m_frequency2(frequency2),
m_amplitude2(amplitude2),
m_reader(reader),
m_blockSize(0),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_cosVal1(0.0F),
m_sinVal1(0.0F),
m_cosDelta1(0.0F),
m_sinDelta1(0.0F),
m_cosVal2(0.0F),
m_sinVal2(0.0F),
m_cosDelta2(0.0F),
m_sinDelta2(0.0F)
{
	wxASSERT(m_frequency1 > 0.0F);
	wxASSERT(m_amplitude1 >= 0.0 && m_amplitude1 < 1.0F);
	wxASSERT(m_frequency2 > 0.0F);
	wxASSERT(m_amplitude2 >= 0.0 && m_amplitude2 < 1.0F);
	wxASSERT((m_amplitude1 + m_amplitude2) < 1.0F);
}

CTwoToneReader::~CTwoToneReader()
{
	delete m_reader;
}

void CTwoToneReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CTwoToneReader::open(float sampleRate, unsigned int blockSize)
{
	wxASSERT(m_frequency1 < (sampleRate + 0.5F) / 2.0F);
	wxASSERT(m_frequency2 < (sampleRate + 0.5F) / 2.0F);

	if (m_reader != NULL) {
		m_reader->setCallback(this, 0);

		bool ret = m_reader->open(sampleRate, blockSize);
		if (!ret)
			return false;
	}

	m_blockSize = blockSize;

	m_buffer = new float[m_blockSize * 2];

	m_cosVal1 = m_cosVal2 = 1.0F;
	m_sinVal1 = m_sinVal2 = 0.0F;

	float delta = m_frequency1 / sampleRate * 2.0 * M_PI;
	m_cosDelta1 = ::cos(delta);
	m_sinDelta1 = ::sin(delta);

	delta = m_frequency2 / sampleRate * 2.0 * M_PI;
	m_cosDelta2 = ::cos(delta);
	m_sinDelta2 = ::sin(delta);

	return true;
}

void CTwoToneReader::close()
{
	if (m_reader != NULL)
		m_reader->close();

	delete[] m_buffer;
}

void CTwoToneReader::purge()
{
}

bool CTwoToneReader::hasClock()
{
	return m_reader != NULL;
}

void CTwoToneReader::clock()
{
	wxASSERT(m_callback != NULL);

	for (unsigned int i = 0; i < m_blockSize; i++) {
		float tmpVal = m_cosVal1 * m_cosDelta1 - m_sinVal1 * m_sinDelta1;
		m_sinVal1 = m_cosVal1 * m_sinDelta1 + m_sinVal1 * m_cosDelta1;
		m_cosVal1 = tmpVal;

		tmpVal = m_cosVal2 * m_cosDelta2 - m_sinVal2 * m_sinDelta2;
		m_sinVal2 = m_cosVal2 * m_sinDelta2 + m_sinVal2 * m_cosDelta2;
		m_cosVal2 = tmpVal;

		m_buffer[i * 2 + 0] = m_cosVal1 * m_amplitude1 + m_cosVal2 * m_amplitude2;
		m_buffer[i * 2 + 1] = m_sinVal1 * m_amplitude1 + m_cosVal2 * m_amplitude2;
	}

	m_callback->callback(m_buffer, m_blockSize, m_id);
}

void CTwoToneReader::callback(float* buffer, unsigned int nSamples, int id)
{
	::memset(buffer, 0x00, nSamples * 2 * sizeof(float));

	clock();
}
