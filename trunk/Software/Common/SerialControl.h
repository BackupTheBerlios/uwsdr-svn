/*
 *   Copyright (C) 2002-2004,2007,2008 by Jonathan Naylor G4KLX
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

#include <wx/wx.h>
#include <vector>

#if defined(__WINDOWS__)
#include <windows.h>
#endif

class CSerialControl;

const unsigned int MAX_DEVICE_NAME = 255U;

struct SSerialList {
	wxString        name;
	CSerialControl* ptr;
};

class CSerialControl : public wxThread {

    public:
    static CSerialControl* getInstance(const wxString& dev);

	virtual bool open();

	virtual bool setRTS(bool set);
	virtual bool setDTR(bool set);

	virtual bool getCTS() const;
	virtual bool getDSR() const;

	virtual void close();

	virtual void* Entry();

	virtual void clock();

	static wxArrayString getDevs();

    protected:
	CSerialControl(const wxString& device);
	virtual ~CSerialControl();

    private:
	static std::vector<SSerialList*> s_serialList;
	static wxMutex                   s_mutex;

	wxString     m_dev;
	unsigned int m_count;
	bool         m_rts;
	bool         m_dtr;
	bool         m_cts;
	bool         m_dsr;
	wxMutex      m_mutex;
	wxSemaphore  m_run;
	bool         m_exit;
#if defined(__WINDOWS__)
	HANDLE       m_handle;
#else
	int          m_fd;
#endif
};

#endif
