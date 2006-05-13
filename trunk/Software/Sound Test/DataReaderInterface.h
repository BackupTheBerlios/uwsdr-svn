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

#ifndef	DataReaderInterface_H
#define	DataReaderInterface_H

#include "DataCallbackInterface.h"

class IDataReaderInterface {
    public:
	virtual bool open(unsigned int sampleRate, unsigned int blockSize) = 0;
	virtual void setCallback(IDataCallbackInterface* callback, int id) = 0;
	virtual void close() = 0;
};

#endif
