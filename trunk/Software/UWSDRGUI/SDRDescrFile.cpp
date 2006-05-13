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

#include "SDRDescrFile.h"

#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/file.h>

CSDRDescrFile::CSDRDescrFile(const wxString& fileName) :
m_name(),
m_maxFreq(),
m_minFreq(),
m_stepSize(0),
m_sampleRate(0),
m_protocolVersion(0),
m_receiveOnly(true),
m_valid(false)
{
	if (!wxFile::Exists(fileName))
		return;

	wxTextFile file;

	bool ret = file.Open(fileName);
	if (!ret)
		return;

	unsigned int nLines = file.GetLineCount();

	for (unsigned int i = 0; i < nLines; i++) {
		wxString line = file.GetLine(i);

		if (line.Length() > 0 && line.GetChar(0) != wxT('#')) {
			if (line.Left(5).Cmp(wxT("name=")) == 0)
				m_name = line.Mid(5);
			else if (line.Left(9).Cmp(wxT("highFreq=")) == 0)
				m_maxFreq.setFrequency(line.Mid(9));
			else if (line.Left(8).Cmp(wxT("lowFreq=")) == 0)
				m_minFreq.setFrequency(line.Mid(8));
			else if (line.Left(9).Cmp(wxT("stepSize=")) == 0)
				m_stepSize = ::atoi(line.Mid(9).c_str());
			else if (line.Left(11).Cmp(wxT("sampleRate=")) == 0)
				m_sampleRate = ::atoi(line.Mid(11).c_str());
			else if (line.Left(16).Cmp(wxT("protocolVersion=")) == 0)
				m_protocolVersion = ::atoi(line.Mid(16).c_str());
			else if (line.Left(12).Cmp(wxT("receiveOnly=")) == 0)
				m_receiveOnly = ::atoi(line.Mid(12).c_str()) == 1;
		}
	}

	m_valid = true;

	file.Close();
}

CSDRDescrFile::~CSDRDescrFile()
{
}

wxString CSDRDescrFile::getName() const
{
	return m_name;
}

CFrequency CSDRDescrFile::getMaxFreq() const
{
	return m_maxFreq;
}

CFrequency CSDRDescrFile::getMinFreq() const
{
	return m_minFreq;
}

unsigned int CSDRDescrFile::getStepSize() const
{
	return m_stepSize;
}

unsigned int CSDRDescrFile::getSampleRate() const
{
	return m_sampleRate;
}

unsigned int CSDRDescrFile::getProtocolVersion() const
{
	return m_protocolVersion;
}

bool CSDRDescrFile::getReceiveOnly() const
{
	return m_receiveOnly;
}

bool CSDRDescrFile::isValid() const
{
	return m_valid;
}
