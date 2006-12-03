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

#include "NullReader.h"

#include <wx/debug.h>
#include <wx/log.h>


CNullReader::CNullReader(IDataReader* reader) :
m_reader(reader),
m_blockSize(0),
m_buffer(NULL),
m_callback(NULL),
m_id(0)
{
}

CNullReader::~CNullReader()
{
	delete m_reader;
}

void CNullReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CNullReader::open(float sampleRate, unsigned int blockSize)
{
	if (m_reader != NULL) {
		m_reader->setCallback(this, 0);

		bool ret = m_reader->open(sampleRate, blockSize);
		if (!ret)
			return false;
	}

	m_blockSize = blockSize;

	m_buffer = new float[m_blockSize * 2];

	for (unsigned int i = 0; i < m_blockSize * 2; i++)
		m_buffer[i] = 0.0F;

	return true;
}

void CNullReader::close()
{
	if (m_reader != NULL)
		m_reader->close();

	delete[] m_buffer;
}

void CNullReader::purge()
{
}

bool CNullReader::hasClock()
{
	return m_reader != NULL;
}

void CNullReader::clock()
{
	wxASSERT(m_callback != NULL);
	wxASSERT(m_buffer != NULL);

	m_callback->callback(m_buffer, m_blockSize, m_id);
}

void CNullReader::callback(float* buffer, unsigned int nSamples, int id)
{
	::memset(buffer, 0x00, nSamples * 2 * sizeof(float));

	clock();
}
