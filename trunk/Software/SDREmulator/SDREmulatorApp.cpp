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

#include <wx/cmdline.h>

#include "SDREmulatorApp.h"

IMPLEMENT_APP(CSDREmulatorApp)

CSDREmulatorApp::CSDREmulatorApp() :
wxApp(),
m_frame(NULL),
m_port(0)
{
}

CSDREmulatorApp::~CSDREmulatorApp()
{
}

void CSDREmulatorApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.AddParam(wxT("Port number"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_OPTION_MANDATORY);

	wxApp::OnInitCmdLine(parser);
}

bool CSDREmulatorApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!wxApp::OnCmdLineParsed(parser))
		return false;

	m_port = ::atoi(parser.GetParam(0).c_str());

	if (m_port < 1 || m_port >= 65536) {
		::wxMessageBox(wxT("Port number must be between 1 and 65536"));
		return false;
	}

	return true;
}

bool CSDREmulatorApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	m_frame = new CSDREmulatorFrame(m_port);
	m_frame->Show();

	SetTopWindow(m_frame);

	return true;
}
