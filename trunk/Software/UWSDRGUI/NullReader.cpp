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


CNullReader::CNullReader() :
wxThread(),
m_sampleRate(0),
m_blockSize(0),
m_callback(NULL),
m_id(0)
{
}

CNullReader::~CNullReader()
{
}

void CNullReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CNullReader::open(unsigned int sampleRate, unsigned int blockSize)
{
	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	Create();
	Run();

	return true;
}

void* CNullReader::Entry()
{
	wxASSERT(m_callback != NULL);

	long interval = (1000L * m_blockSize) / m_sampleRate;

	float* buffer = new float[m_blockSize * 2];

	::wxStartTimer();

	while (!TestDestroy()) {
		for (unsigned int i = 0; i < m_blockSize * 2; i++)
			buffer[i] = 0.0F;

		m_callback->callback(buffer, m_blockSize, m_id);

		long diff = ::wxGetElapsedTime();

		int sleepTime = interval - diff;
		if (sleepTime > 0)
			Sleep(sleepTime);

		::wxStartTimer();
	}

	delete[] buffer;

	return (void*)0;
}

void CNullReader::close()
{
	Delete();
}
