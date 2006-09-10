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

#ifndef	UWSDRApp_H
#define	UWSDRApp_H

#include <wx/wx.h>
#include <wx/html/helpctrl.h>

#include "UWSDRFrame.h"
#include "SDRParameters.h"

class CUWSDRApp : public wxApp {

    public:
	CUWSDRApp();
	virtual ~CUWSDRApp();

	virtual bool OnInit();
	virtual int  OnExit();
	virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
	virtual void OnInitCmdLine(wxCmdLineParser& parser);

	bool readDescrFile();
	bool readConfig();
	void writeConfig();

	void showHelp(int id);

	void sendCW(unsigned int speed, const wxString& text);

#if defined(__WXDEBUG__)
	virtual void OnAssert(const wxChar* file, int line, const wxChar* cond, const wxChar* msg);
#endif

    protected:
	wxString getHelpDir();

    private:
	CUWSDRFrame*          m_frame;
	CSDRParameters*       m_parameters;
	wxHtmlHelpController* m_help;
};

DECLARE_APP(CUWSDRApp)

#endif
