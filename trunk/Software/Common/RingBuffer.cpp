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

// For semaphores and logging
#if defined(__WXMSW__) || defined(__WXGTK__) || (__WXMAC__)
#include <wx/wx.h>

#define	ASSERT(x)	wxASSERT((x))
#elif defined(WIN32)
#include <windows.h>
#include <cassert>

#define	ASSERT(x)	assert((x))
#else
#include <ctsdio>
#include <cassert>

#define	ASSERT(x)	assert((x))
#endif


const int STATE_EMPTY = 0;
const int STATE_FULL  = 1;
const int STATE_DATA  = 2;


CRingBuffer::CRingBuffer(unsigned int length, unsigned int step) :
m_length(length),
m_step(step),
m_buffer(NULL),
m_iPtr(0),
m_oPtr(0),
m_state(STATE_EMPTY)
{
	ASSERT(length > 0);
	ASSERT(step > 0);

	m_buffer = new float[length * step];

	::memset(m_buffer, 0x00, length * step * sizeof(float));
}

CRingBuffer::~CRingBuffer()
{
	delete[] m_buffer;
}

unsigned int CRingBuffer::addData(const float* buffer, unsigned int nSamples)
{
	unsigned int space = freeSpace();

	if (nSamples >= space) {
		nSamples = space;
		m_state = STATE_FULL;
	} else {
		m_state = STATE_DATA;
	}

	for (unsigned int i = 0; i < nSamples * m_step; i++) {
		m_buffer[m_iPtr++] = buffer[i];

		if (m_iPtr == (m_length * m_step))
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

	for (unsigned int i = 0; i < nSamples * m_step; i++) {
		buffer[i] = m_buffer[m_oPtr++];

		if (m_oPtr == (m_length * m_step))
			m_oPtr = 0;
	}

	return nSamples;
}

void CRingBuffer::clear()
{
	m_iPtr  = 0;
	m_oPtr  = 0;
	m_state = STATE_EMPTY;

	::memset(m_buffer, 0x00, m_length * m_step);
}

unsigned int CRingBuffer::freeSpace() const
{
	if (isEmpty())
		return m_length;

	if (isFull())
		return 0;

	if (m_oPtr > m_iPtr)
		return (m_oPtr - m_iPtr) / m_step;

	return m_length - (m_iPtr - m_oPtr) / m_step;
}

unsigned int CRingBuffer::dataSpace() const
{
	if (isEmpty())
		return 0;

	if (isFull())
		return m_length;

	if (m_iPtr >= m_oPtr)
		return (m_iPtr - m_oPtr) / m_step;

	return m_length - (m_oPtr - m_iPtr) / m_step;
}

bool CRingBuffer::isEmpty() const
{
	return m_state == STATE_EMPTY;
}

bool CRingBuffer::isFull() const
{
	return m_state == STATE_FULL;
}

#if defined(__WXDEBUG__)
void CRingBuffer::dump(const wxString& title) const
{
	::wxLogMessage(title);
	::wxLogMessage(wxT("Length: 0x%05X  Step: %u  oPtr: 0x%05X  iPtr: 0x%05X  State: %d"), m_length, m_step, m_oPtr / m_step, m_iPtr / m_step, m_state);

	::wxLogMessage(wxT(":"));

	unsigned int n = 0;
	for (unsigned int i = 0; i < m_length; i += 16) {
		wxString text;
		text.Printf(wxT("%05X:  "), i);

		for (unsigned int j = 0; j < 16; j++) {
			for (unsigned int k = 0; k < m_step; k++) {
				if (k > 0)
					text.Append(wxT(":"));

				wxString buf;
				buf.Printf(wxT("%f"), m_buffer[n++]);
				text.Append(buf);
			}

			if ((i + j) >= m_length)
				break;

			text.Append(wxT("  "));
		}

		::wxLogMessage(text);
	}
}
#endif
