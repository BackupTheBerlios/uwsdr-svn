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

#ifndef	SDRDescrFile_H
#define	SDRDescrFile_H

#include <wx/wx.h>

#include "Frequency.h"

class CSDRDescrFile {

    public:
	CSDRDescrFile(const wxString& fileName);
	virtual ~CSDRDescrFile();

	virtual wxString     getName() const;
	virtual CFrequency   getMaxFreq() const;
	virtual CFrequency   getMinFreq() const;
	virtual float        getStepSize() const;
	virtual float        getSampleRate() const;
	virtual unsigned int getProtocolVersion() const;
	virtual bool         getReceiveOnly() const;
	virtual bool         isValid() const;

    private:
	wxString     m_name;
	CFrequency   m_maxFreq;
	CFrequency   m_minFreq;
	float        m_stepSize;
	float        m_sampleRate;
	unsigned int m_protocolVersion;
	bool         m_receiveOnly;
	bool         m_valid;
};

#endif
