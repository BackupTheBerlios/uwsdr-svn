/*
 *   Copyright (C) 2002-2004,2006 by Jonathan Naylor G4KLX
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

#ifndef	NullReader_H
#define	NullReader_H

#include <wx/wx.h>

#include "DataReaderInterface.h"

class CNullReader : public wxThread, public IDataReaderInterface {

    public:
    CNullReader(unsigned int frequency, float amplitude);
	virtual ~CNullReader();

	virtual void setCallback(IDataCallbackInterface* callback, int id);

	virtual bool open(unsigned int sampleRate, unsigned int blockSize);

	virtual void* Entry();

	virtual void close();

    private:
	unsigned int            m_frequency;
	float                   m_amplitude;
	unsigned int            m_sampleRate;
	unsigned int            m_blockSize;
	IDataCallbackInterface* m_callback;
	int                     m_id;
};

#endif
