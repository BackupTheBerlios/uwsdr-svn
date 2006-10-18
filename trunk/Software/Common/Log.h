/*
 *   Copyright (C) 2002,2003,2006 by Jonathan Naylor G4KLX
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

#ifndef	Log_H
#define	Log_H

#include <wx/log.h>
#include <wx/file.h>

class CLog : public wxLog {

    public:
	CLog(const wxString& fileName);
	virtual ~CLog();

	virtual void DoLog(wxLogLevel level, const wxChar* msg, time_t timestamp);
	virtual void DoLogString(const wxChar* msg, time_t timestamp);

    private:
	wxFile* m_file;
	char*   m_eol;
};

#endif