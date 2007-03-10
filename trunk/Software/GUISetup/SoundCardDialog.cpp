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

#include "SoundCardDialog.h"


const int API_COMBO       = 27545;
const int NAME_COMBO      = 27546;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;

BEGIN_EVENT_TABLE(CSoundCardDialog, wxDialog)
	EVT_CHOICE(API_COMBO, CSoundCardDialog::onAPI)
	EVT_BUTTON(wxID_OK,   CSoundCardDialog::onOK)
END_EVENT_TABLE()


CSoundCardDialog::CSoundCardDialog(wxWindow* parent, const wxString& title, int inDev, int outDev, unsigned int minIn, unsigned int minOut, int id) :
wxDialog(parent, id, title),
m_apiChoice(NULL),
m_devChoice(NULL),
m_info(),
m_type(),
m_inDev(inDev),
m_outDev(outDev),
m_minIn(minIn),
m_minOut(minOut)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Audio API:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_apiChoice = new wxChoice(panel, API_COMBO, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_apiChoice, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("Audio Device:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_devChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_devChoice, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	mainSizer->Add(CreateButtonSizer(wxOK | wxCANCEL), 0, wxALL, BORDER_SIZE);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	bool ret = m_info.enumerateAPIs();

	if (!ret) {
		::wxMessageBox(_("Cannot access the sound access system."), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	ret = m_info.enumerateDevs();

	if (!ret) {
		::wxMessageBox(_("Cannot access the sound access system."), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	enumerateAPI();
}

CSoundCardDialog::~CSoundCardDialog()
{
}

void CSoundCardDialog::onAPI(wxCommandEvent& WXUNUSED(event))
{
	long apiChoice = m_apiChoice->GetSelection();
	if (apiChoice == wxNOT_FOUND) {
		::wxMessageBox(_("The Audio API is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	CAudioDevAPI* api = (CAudioDevAPI*)m_apiChoice->GetClientData(apiChoice);

	m_type   = api->getType();
	m_inDev  = api->getInDefault();
	m_outDev = api->getOutDefault();

	enumerateAudio(*api);
}

void CSoundCardDialog::onOK(wxCommandEvent& WXUNUSED(event))
{
	int devChoice = m_devChoice->GetSelection();
	if (devChoice == wxNOT_FOUND) {
		::wxMessageBox(_("The Audio Device is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	CAudioDevDev* dev = (CAudioDevDev*)m_devChoice->GetClientData(devChoice);

	wxASSERT(dev != NULL);

	m_type   = dev->getType();
	m_inDev  = dev->getInDev();
	m_outDev = dev->getOutDev();

	if (IsModal()) {
		EndModal(wxID_OK);
	} else {
		SetReturnCode(wxID_OK);
		Show(false);
	}
}

void CSoundCardDialog::enumerateAPI()
{
	int defAPI = NO_API;

	if (m_inDev != NO_DEV) {
		for (unsigned int i = 0U; i < m_info.getDevs().size(); i++) {
			CAudioDevDev* dev = m_info.getDevs().at(i);

			if (m_inDev == dev->getInDev()) {
				defAPI = dev->getAPI();
				break;
			}
		}
	}

	CAudioDevAPI* chosen = NULL;

	for (unsigned int i = 0U; i < m_info.getAPIs().size(); i++) {
		CAudioDevAPI* api = m_info.getAPIs().at(i);

		m_apiChoice->Append(api->getName(), api);

		if (defAPI != NO_API && defAPI == api->getAPI()) {
			m_apiChoice->SetSelection(i);
			chosen = api;
		}

		if (defAPI == NO_API && api->getDefault()) {
			m_apiChoice->SetSelection(i);
			chosen = api;
		}
	}

	if (chosen != NULL)
		enumerateAudio(*chosen);
}

void CSoundCardDialog::enumerateAudio(const CAudioDevAPI& api)
{
	m_devChoice->Clear();

	unsigned int n = 0U;
	for (unsigned int i = 0U; i < m_info.getDevs().size(); i++) {
		CAudioDevDev* dev = m_info.getDevs().at(i);

		if (dev->getAPI() == api.getAPI() && dev->getInChannels() >= int(m_minIn) && dev->getOutChannels() >= int(m_minOut)) {
			m_devChoice->Append(dev->getName(), dev);

			if (m_inDev != NO_DEV && m_inDev == dev->getInDev()) {
				m_devChoice->SetSelection(n);
				m_outDev = dev->getOutDev();
			}

			if (m_inDev == NO_DEV && dev->getInDev() == api.getInDefault()) {
				m_devChoice->SetSelection(n);
				m_inDev  = dev->getInDev();
				m_outDev = dev->getOutDev();
			}

			n++;
		}
	}
}

SOUNDTYPE CSoundCardDialog::getType() const
{
	return m_type;
}

int CSoundCardDialog::getInDev() const
{
	return m_inDev;
}

int CSoundCardDialog::getOutDev() const
{
	return m_outDev;
}
