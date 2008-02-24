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

#ifndef	SDREmulatorApp_H
#define	SDREmulatorApp_H

#include <wx/wx.h>

#include "SDREmulatorFrame.h"

class CSDREmulatorApp : public wxApp {

    public:
	CSDREmulatorApp();
	virtual ~CSDREmulatorApp();

	virtual bool OnInit();
	virtual int  OnExit();
	virtual void OnInitCmdLine(wxCmdLineParser& parser);
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

#if defined(__WXDEBUG__)
	virtual void OnAssertFailure(const wxChar* file, int line, const wxChar* func, const wxChar* cond, const wxChar* msg);
#endif

    private:
	CSDREmulatorFrame* m_frame;
	bool               m_muted;
	wxString           m_address;
	unsigned int       m_controlPort;
	unsigned int       m_maxSamples;
	bool               m_delay;
};

DECLARE_APP(CSDREmulatorApp)

#endif
