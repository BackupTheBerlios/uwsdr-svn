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
#include "Log.h"


IMPLEMENT_APP(CSDREmulatorApp)

CSDREmulatorApp::CSDREmulatorApp() :
wxApp(),
m_frame(NULL),
m_address(),
m_controlPort(0),
m_dataPort(0),
m_maxSamples(2048)
{
}

CSDREmulatorApp::~CSDREmulatorApp()
{
}

void CSDREmulatorApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.AddSwitch(wxT("s"), wxEmptyString, wxT("Disable_Receiver_Mute"), wxCMD_LINE_PARAM_OPTIONAL);

	parser.AddParam(wxT("IP_Address"),   wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);
	parser.AddParam(wxT("Control_Port"), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_OPTION_MANDATORY);
	parser.AddParam(wxT("Data_Port"),    wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_OPTION_MANDATORY);

	parser.AddParam(wxT("UDP_Size"),     wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);

	wxApp::OnInitCmdLine(parser);
}

bool CSDREmulatorApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!wxApp::OnCmdLineParsed(parser))
		return false;

	m_muted = !parser.Found(wxT("s"));

	m_address = parser.GetParam(0);

	long temp;

	parser.GetParam(1).ToLong(&temp);

	if (temp < 1L || temp >= 65536L) {
		::wxMessageBox(wxT("The Control Port number must be between 1 and 65536"));
		return false;
	}

	m_controlPort = temp;

	parser.GetParam(2).ToLong(&temp);

	if (temp < 1L || temp >= 65536L) {
		::wxMessageBox(wxT("The Data Port number must be between 1 and 65536"));
		return false;
	}

	m_dataPort = temp;

	size_t count = parser.GetParamCount();
	if (count < 4)
		return true;

	parser.GetParam(3).ToLong(&temp);

	if (temp < 210L || temp > 2048L) {
		::wxMessageBox(wxT("The UDP Size number must be between 210 and 2048"));
		return false;
	}

	m_maxSamples = temp;

	return true;
}

bool CSDREmulatorApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	wxLog* logger = new CLog(wxT("Emulator.log"));
	wxLog::SetActiveTarget(logger);

	::wxLogMessage(wxT("Starting the SDREmulator"));
	::wxLogMessage(wxT("GUI is at address: %s, control port: %u, data port: %u, max samples: %u"), m_address.c_str(), m_controlPort, m_dataPort, m_maxSamples);

	m_frame = new CSDREmulatorFrame(m_address, m_controlPort, m_dataPort, m_muted, m_maxSamples);
	m_frame->Show();

	SetTopWindow(m_frame);

	return true;
}

int CSDREmulatorApp::OnExit()
{
	::wxLogMessage(wxT("Ending the SDREmulator"));

	return 0;
}

#if defined(__WXDEBUG__)
void CSDREmulatorApp::OnAssertFailure(const wxChar* file, int line, const wxChar* func, const wxChar* cond, const wxChar* msg)
{
	::wxLogFatalError(wxT("Assertion failed on line %d of %s in file %s: %s %s"), line, func, file, cond, msg);
}
#endif
