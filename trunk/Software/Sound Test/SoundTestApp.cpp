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

#include "SoundTestApp.h"

#include "NullReader.h"
#include "NullWriter.h"
#include "SoundCardReader.h"
#include "SoundCardWriter.h"
#include "SoundFileReader.h"
#include "SoundFileWriter.h"

IMPLEMENT_APP(CSoundTestApp)

CSoundTestApp::CSoundTestApp() :
wxApp(),
m_reader(NULL),
m_writer(NULL),
m_count(0)
{
}

CSoundTestApp::~CSoundTestApp()
{
}

bool CSoundTestApp::OnInit()
{
	m_reader = new CNullReader(1000, 0.1F);
	// m_reader = new CSoundFileReader("SoundTest.wav");
	// m_reader = new CSoundCardReader(0, 2);
	m_writer = new CSoundCardWriter(0, 5);
	// m_writer = new CSoundFileWriter("SoundTest.wav");

	m_reader->setCallback(this, 1);

	bool ret = m_writer->open(11025, 3000);
	if (!ret)
		return false;

	ret = m_reader->open(11025, 3000);
	if (!ret) {
		m_writer->close();
		return false;
	}

	::wxSleep(10);

	m_reader->close();
	m_writer->close();

	return false;
}

void CSoundTestApp::callback(float* buffer, unsigned int nSamples, int id)
{
	wxASSERT(m_writer != NULL);

	m_writer->write(buffer, nSamples);

	m_count++;
}

#ifdef __WXDEBUG__
void CSoundTestApp::OnAssert(const wxChar* file, int line, const wxChar* cond, const wxChar* msg)
{
	wxString text;
	text.Printf("ASSERT in %s on line %d with cod=%s and msg=%s\r\n",
		file, line, cond, msg);

	printf(text.c_str());
}
#endif
