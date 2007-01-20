/*
 *   Copyright (C) 2002-2004,2007 by Jonathan Naylor G4KLX
 *   Copyright (C) 1999-2001 by Thomas Sailor HB9JNX
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

#ifndef SerialControl_H
#define	SerialControl_H

#include "PortControl.h"

#include <wx/wx.h>

#if defined(__WINDOWS__)
#include <windows.h>
#endif

enum {
	PIN_RTS,
	PIN_DTR
};

class CSerialControl : public IPortControl {

    public:
	CSerialControl(const wxString& device, int pin);
	virtual ~CSerialControl();

	virtual bool open();
	virtual bool keyTX();
	virtual bool unkeyTX();
	virtual void close();

	static wxArrayString getDevices();

    private:
	wxString m_device;
	int      m_pin;
#if defined(__WINDOWS__)
	HANDLE   m_handle;
#else
	int      m_fd;
#endif
};

#endif
