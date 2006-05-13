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

#include "RingBuffer.h"

const int STATE_EMPTY = 0;
const int STATE_FULL  = 1;
const int STATE_DATA  = 2;

CRingBuffer::CRingBuffer(unsigned int length) :
m_length(length),
m_buffer(NULL),
m_iPtr(0),
m_oPtr(0),
m_state(STATE_EMPTY)
{
	m_buffer = new float[length * 2];

	::memset(m_buffer, 0x00, length * 2 * sizeof(float));
}

CRingBuffer::~CRingBuffer()
{
	delete[] m_buffer;
}

unsigned int CRingBuffer::addData(float* buffer, unsigned int nSamples)
{
	unsigned int space = freeSpace();

	if (nSamples >= space) {
		nSamples = space;
		m_state = STATE_FULL;
	} else {
		m_state = STATE_DATA;
	}

	for (unsigned int i = 0; i < nSamples * 2; i++) {
		m_buffer[m_iPtr++] = buffer[i];

		if (m_iPtr == (m_length * 2))
			m_iPtr = 0;
	}

	return nSamples;
}

unsigned int CRingBuffer::getData(float* buffer, unsigned int nSamples)
{
	unsigned int space = dataSpace();

	if (nSamples >= space) {
		nSamples = space;
		m_state = STATE_EMPTY;
	} else {
		m_state = STATE_DATA;
	}

	for (unsigned int i = 0; i < nSamples * 2; i++) {
		buffer[i] = m_buffer[m_oPtr++];

		if (m_oPtr == (m_length * 2))
			m_oPtr = 0;
	}

	return nSamples;
}

void CRingBuffer::clear()
{
	m_iPtr  = 0;
	m_oPtr  = 0;
	m_state = STATE_EMPTY;

	::memset(m_buffer, 0x00, m_length * 2);
}

unsigned int CRingBuffer::freeSpace() const
{
	if (isEmpty())
		return m_length;

	if (isFull())
		return 0;

	if (m_oPtr > m_iPtr)
		return (m_oPtr - m_iPtr) / 2;

	return m_length - (m_iPtr - m_oPtr) / 2;
}

unsigned int CRingBuffer::dataSpace() const
{
	if (isEmpty())
		return 0;

	if (isFull())
		return m_length;

	if (m_iPtr >= m_oPtr)
		return (m_iPtr - m_oPtr) / 2;

	return m_length - (m_oPtr - m_iPtr) / 2;
}

bool CRingBuffer::isEmpty() const
{
	return m_state == STATE_EMPTY;
}

bool CRingBuffer::isFull() const
{
	return m_state == STATE_FULL;
}
