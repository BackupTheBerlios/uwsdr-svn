/*
 *   Copyright (C) 2006-2008 by Jonathan Naylor G4KLX
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
m_type(TYPE_UWSDR1),
m_maxFreq(),
m_minFreq(),
m_offset(),
m_stepSize(0U),
m_sampleRate(0.0F),
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

		if (line.length() > 0 && line.GetChar(0) != wxT('#')) {
			if (line.Left(5).Cmp(wxT("name=")) == 0)
				m_name = line.Mid(5);
			else if (line.Left(5).Cmp(wxT("type=")) == 0) {
				wxString type = line.Mid(5);

				if (type.Cmp(wxT("audiorx")) == 0)
					m_type = TYPE_AUDIORX;
				else if (type.Cmp(wxT("audiotxrx")) == 0)
					m_type = TYPE_AUDIOTXRX;
				else if (type.Cmp(wxT("demo")) == 0)
					m_type = TYPE_DEMO;
				else if (type.Cmp(wxT("uwsdr1")) == 0)
					m_type = TYPE_UWSDR1;
				else if (type.Cmp(wxT("hpsdr")) == 0)
					m_type = TYPE_HPSDR;
				else {
					::wxLogError(wxT("Unknown type - %s in the .sdr file"), type.c_str());
					return;
				}
			} else if (line.Left(9).Cmp(wxT("highFreq=")) == 0)
				m_maxFreq.set(line.Mid(9));
			else if (line.Left(8).Cmp(wxT("lowFreq=")) == 0)
				m_minFreq.set(line.Mid(8));
			else if (line.Left(8).Cmp(wxT("offset=")) == 0)
				m_offset.set(line.Mid(7));
			else if (line.Left(9).Cmp(wxT("stepSize=")) == 0) {
				unsigned long temp;
				line.Mid(9).ToULong(&temp);
				m_stepSize = temp;
			} else if (line.Left(11).Cmp(wxT("sampleRate=")) == 0) {
				double temp;
				line.Mid(11).ToDouble(&temp);
				m_sampleRate = temp;
			} else if (line.Left(12).Cmp(wxT("receiveOnly=")) == 0) {
				unsigned long temp;
				line.Mid(12).ToULong(&temp);
				m_receiveOnly = temp == 1L;
			}
		}
	}

	if (m_type == TYPE_AUDIORX || m_type == TYPE_AUDIOTXRX) {
		if (m_minFreq != m_maxFreq) {
			::wxLogError(wxT("The min and max frequencies must be the same for \"type=audiorx\" or \"type=audiotxrx\""));
			return;
		}
	}

	if (m_type == TYPE_AUDIORX)
		m_receiveOnly = true;

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

SDRTYPE CSDRDescrFile::getType() const
{
	return m_type;
}

CFrequency CSDRDescrFile::getMaxFreq() const
{
	return m_maxFreq;
}

CFrequency CSDRDescrFile::getMinFreq() const
{
	return m_minFreq;
}

CFrequency CSDRDescrFile::getOffset() const
{
	return m_offset;
}

unsigned int CSDRDescrFile::getStepSize() const
{
	return m_stepSize;
}

float CSDRDescrFile::getSampleRate() const
{
	return m_sampleRate;
}

bool CSDRDescrFile::getReceiveOnly() const
{
	return m_receiveOnly;
}

bool CSDRDescrFile::isValid() const
{
	return m_valid;
}
