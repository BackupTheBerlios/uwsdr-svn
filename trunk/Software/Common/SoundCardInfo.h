/*
 *   Copyright (C) 2006-2007 by Jonathan Naylor G4KLX
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

#ifndef	SoundCardInfo_H
#define	SoundCardInfo_H

#include <wx/wx.h>

#include <vector>
using namespace std;

#include "portaudio.h"

class CSoundCardAPI {
    public:
	CSoundCardAPI(int api, const wxString& name, bool def, long inDef, long outDef) :
	m_api(api),
	m_name(name),
	m_def(def),
	m_inDef(inDef),
	m_outDef(outDef)
	{
	}

	~CSoundCardAPI()
	{
	}

	int getAPI() const
	{
		return m_api;
	}

	wxString getName() const
	{
		return m_name;
	}

	bool getDefault() const
	{
		return m_def;
	}

	long getInDefault() const
	{
		return m_inDef;
	}

	long getOutDefault() const
	{
		return m_outDef;
	}

    private:
	int      m_api;
	wxString m_name;
	bool     m_def;
	long     m_inDef;
	long     m_outDef;
};

class CSoundCardDev {
    public:
	CSoundCardDev(const wxString& name) :
	m_name(name),
	m_inDev(-1L),
	m_outDev(-1L),
	m_inDef(false),
	m_outDef(false),
	m_inChannels(0),
	m_outChannels(0)
	{
	}

	~CSoundCardDev()
	{
	}

	wxString getName() const
	{
		return m_name;
	}

	long getInDev() const
	{
		return m_inDev;
	}

	long getOutDev() const
	{
		return m_outDev;
	}

	bool getInDefault() const
	{
		return m_inDef;
	}

	bool getOutDefault() const
	{
		return m_outDef;
	}

	int getInChannels() const
	{
		return m_inChannels;
	}

	int getOutChannels() const
	{
		return m_outChannels;
	}

	void setIn(long dev, bool def, int channels)
	{
		m_inDev      = dev;
		m_inDef      = def;
		m_inChannels = channels;
	}

	void setOut(long dev, bool def, int channels)
	{
		m_outDev      = dev;
		m_outDef      = def;
		m_outChannels = channels;
	}

    private:
	wxString m_name;
	long     m_inDev;
	long     m_outDev;
	bool     m_inDef;
	bool     m_outDef;
	int      m_inChannels;
	int      m_outChannels;
};

class CSoundCardInfo {
    public:
	CSoundCardInfo();
	virtual ~CSoundCardInfo();

	bool enumerateAPIs();
	bool enumerateDevs(const CSoundCardAPI& api);

	vector<CSoundCardAPI*>& getAPIs();
	vector<CSoundCardDev*>& getDevs();

	CSoundCardAPI* findAPI(const wxString& name);
	CSoundCardDev* findDev(const wxString& name);

    private:
	vector<CSoundCardAPI*> m_apis;
	vector<CSoundCardDev*> m_devs;

	void freeAPIs();
	void freeDevs();
};

#endif
