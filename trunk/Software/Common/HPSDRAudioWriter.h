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

#ifndef	HPSDRAudioWriter_H
#define	HPSDRAudioWriter_H

#include "DataWriter.h"
#include "HPSDRReaderWriter.h"

class CHPSDRAudioWriter : public IDataWriter {
    public:
	CHPSDRAudioWriter(CHPSDRReaderWriter* hpsdr);
	virtual ~CHPSDRAudioWriter();

	virtual bool open(float sampleRate, unsigned int blockSize);
	virtual void write(const float* buffer, unsigned int nSamples);
	virtual void close();

	virtual void enable(bool enable = true);
	virtual void disable();

    protected:
	CHPSDRReaderWriter* m_hpsdr;
};

#endif