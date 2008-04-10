/*
 *   Copyright (C) 2006-2008 by Jonathan Naylor G4KLX
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

#ifndef	RingBuffer_H
#define	RingBuffer_H

#include <wx/wx.h>

enum RBSTATE {
	RBSTATE_EMPTY,
	RBSTATE_FULL,
	RBSTATE_DATA
};

class CRingBuffer {

    public:
	CRingBuffer(unsigned int length, unsigned int step);
	~CRingBuffer();

	unsigned int addData(const float* buffer, unsigned int nSamples);
	unsigned int getData(float* buffer, unsigned int nSamples);

	void clear();

	unsigned int freeSpace() const;
	unsigned int dataSpace() const;

	bool isEmpty() const;
	bool isFull() const;

#if defined(__WXDEBUG__)
	void dump(const wxString& title) const;
#endif

    private:
	unsigned int m_length;
	unsigned int m_step;
	float*       m_buffer;
	unsigned int m_iPtr;
	unsigned int m_oPtr;
	RBSTATE      m_state;
};

#endif
