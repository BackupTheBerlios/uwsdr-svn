/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
 *
 *   Based on code written by Peter Martinez G3PLX publiched in RadCom June 2004
 *   pages 84-86.
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

#ifndef	Hilbert_H
#define	Hilbert_H


const unsigned int NTAPS = 81;

class CHilbert {

    public:
	CHilbert(float sampleRate);
	virtual ~CHilbert();

	virtual unsigned int process(const float* inBuffer, float* outBuffer, unsigned int nSamples);

    private:
	float        m_coeffs[NTAPS];
	float        m_delayLine[NTAPS];
	unsigned int m_dptr;
};

#endif
