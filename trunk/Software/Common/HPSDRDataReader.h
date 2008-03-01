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

#ifndef	HPSDRDataReader_H
#define	HPSDRDataReader_H

#include "DataReader.h"
#include "HPSDRReaderWriter.h"

class CHPSDRDataReader : public IDataReader {
    public:
	CHPSDRDataReader(CHPSDRReaderWriter* hpsdr);
	virtual ~CHPSDRDataReader();

	virtual bool open(float sampleRate, unsigned int blockSize);
	virtual void setCallback(IDataCallback* callback, int id);
	virtual void close();

	virtual void purge();

	virtual bool hasClock();
	virtual void clock();

    protected:
	CHPSDRReaderWriter* m_hpsdr;
};

#endif
