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

#include "SoundCardInfo.h"


CSoundCardInfo::CSoundCardInfo() :
m_apis(),
m_devs()
{
}

CSoundCardInfo::~CSoundCardInfo()
{
	freeAPIs();
	freeDevs();
}

bool CSoundCardInfo::enumerateAPIs()
{
	freeAPIs();
	freeDevs();

	PaError error = ::Pa_Initialize();
	if (error != paNoError)
		return false;

	PaHostApiIndex n = ::Pa_GetHostApiCount();

	if (n <= 0) {
		::Pa_Terminate();
		return false;
	}

	PaHostApiIndex defAPI = ::Pa_GetDefaultHostApi();

	for (PaHostApiIndex i = 0; i < n; i++) {
		const PaHostApiInfo* hostAPI = ::Pa_GetHostApiInfo(i);

		CSoundCardAPI* api = new CSoundCardAPI(i, hostAPI->name, i == defAPI, hostAPI->defaultInputDevice, hostAPI->defaultOutputDevice);
		m_apis.push_back(api);
	}

	::Pa_Terminate();

	return true;
}

bool CSoundCardInfo::enumerateDevs(const CSoundCardAPI& api)
{
	freeDevs();

	PaError error = ::Pa_Initialize();
	if (error != paNoError)
		return false;

	PaDeviceIndex n = ::Pa_GetDeviceCount();

	if (n <= 0) {
		::Pa_Terminate();
		return false;
	}

	printf("No of devices %d\n", n);

	for (PaDeviceIndex i = 0; i < n; i++) {
		const PaDeviceInfo* device = ::Pa_GetDeviceInfo(i);

		wxString name = device->name;

#if defined(__WINDOWS__)
		// Map the seperate names for the sound mapper under Windows to one name
		if (name.IsSameAs(wxT("Microsoft Sound Mapper - Input")))
			name = wxT("Microsoft Sound Mapper");
		if (name.IsSameAs(wxT("Microsoft Sound Mapper - Output")))
			name = wxT("Microsoft Sound Mapper");
#endif

		printf("N: %d Name: %s API: %d Input: %d Output: %d\n", i, device->name, device->hostApi, device->maxInputChannels, device->maxOutputChannels);

		if (device->maxInputChannels > 0 && device->hostApi == api.getAPI()) {
			CSoundCardDev* dev = findDev(name);

			if (dev == NULL) {
				printf("New input device\n");
				dev = new CSoundCardDev(name);
				m_devs.push_back(dev);
			}

			dev->setIn(i, i == api.getInDefault());
		}

		if (device->maxOutputChannels > 0 && device->hostApi == api.getAPI()) {
			CSoundCardDev* dev = findDev(name);

			if (dev == NULL) {
				printf("New output device\n");
				dev = new CSoundCardDev(name);
				m_devs.push_back(dev);
			}

			dev->setOut(i, i == api.getOutDefault());
		}
	}

	::Pa_Terminate();

	return true;
}

vector<CSoundCardAPI*>& CSoundCardInfo::getAPIs()
{
	return m_apis;
}

vector<CSoundCardDev*>& CSoundCardInfo::getDevs()
{
	return m_devs;
}

CSoundCardAPI* CSoundCardInfo::findAPI(const wxString& name)
{
	for (unsigned int i = 0; i < m_apis.size(); i++) {
		CSoundCardAPI* api = m_apis.at(i);

		if (api->getName().IsSameAs(name))
			return api;
	}

	return NULL;
}

CSoundCardDev* CSoundCardInfo::findDev(const wxString& name)
{
	for (unsigned int i = 0; i < m_devs.size(); i++) {
		CSoundCardDev* dev = m_devs.at(i);

		if (dev->getName().IsSameAs(name))
			return dev;
	}

	return NULL;
}

void CSoundCardInfo::freeAPIs()
{
	for (unsigned int i = 0; i < m_apis.size(); i++)
		delete m_apis.at(i);

	m_apis.clear();
}

void CSoundCardInfo::freeDevs()
{
	for (unsigned int i = 0; i < m_devs.size(); i++)
		delete m_devs.at(i);

	m_devs.clear();
}
