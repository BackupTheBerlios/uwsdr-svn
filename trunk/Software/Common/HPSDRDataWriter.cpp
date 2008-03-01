/*
 *   Copyright (C) 2008 by Jonathan Naylor G4KLX
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

#include "HPSDRDataWriter.h"

CHPSDRDataWriter::CHPSDRDataWriter(CHPSDRReaderWriter* hpsdr) :
m_hpsdr(hpsdr)
{
	wxASSERT(hpsdr != NULL);
}

CHPSDRDataWriter::~CHPSDRDataWriter()
{
}

bool CHPSDRDataWriter::open(float WXUNUSED(sampleRate), unsigned int WXUNUSED(blockSize))
{
	return true;
}

void CHPSDRDataWriter::write(const float* buffer, unsigned int nSamples)
{
	wxASSERT(buffer != NULL);

	m_hpsdr->writeData(buffer, nSamples);
}

void CHPSDRDataWriter::close()
{
}

void CHPSDRDataWriter::enable(bool WXUNUSED(enable))
{
}

void CHPSDRDataWriter::disable()
{
	enable(false);
}
