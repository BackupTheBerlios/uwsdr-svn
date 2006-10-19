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

#include "SoundCardDialog.h"


const int API_COMBO       = 27545;
const int NAME_COMBO      = 27546;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;

BEGIN_EVENT_TABLE(CSoundCardDialog, wxDialog)
	EVT_CHOICE(API_COMBO, CSoundCardDialog::onAPI)
	EVT_BUTTON(wxID_OK,   CSoundCardDialog::onOK)
END_EVENT_TABLE()


CSoundCardDialog::CSoundCardDialog(wxWindow* parent, int id) :
wxDialog(parent, id, wxString(wxT("Sound Card Choice"))),
m_apiChoice(NULL),
m_devChoice(NULL),
m_info(),
m_api(-1),
m_inDev(-1),
m_outDev(-1)
{
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(2);

	wxStaticText* label3 = new wxStaticText(panel, -1, wxT("Audio API:"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_apiChoice = new wxChoice(panel, API_COMBO, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_apiChoice, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, wxT("Audio Device:"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_devChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_devChoice, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	mainSizer->Add(CreateButtonSizer(wxOK), 0, wxALL, BORDER_SIZE);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	enumerateAPI();
}

CSoundCardDialog::~CSoundCardDialog()
{
}

void CSoundCardDialog::onAPI(wxCommandEvent& event)
{
	long audioAPI = m_apiChoice->GetSelection();
	if (audioAPI == wxNOT_FOUND) {
		::wxMessageBox(wxT("The Audio API is not allowed to be empty"));
		return;
	}

	CSoundCardAPI* api = m_info.getAPIs().at(audioAPI);

	enumerateAudio(*api);
}

void CSoundCardDialog::onOK(wxCommandEvent& event)
{
	long audioAPI = m_apiChoice->GetSelection();
	if (audioAPI == wxNOT_FOUND) {
		::wxMessageBox(wxT("The Audio API is not allowed to be empty"));
		return;
	}

	m_api = m_info.getAPIs().at(audioAPI)->getAPI();

	int devChoice = m_devChoice->GetSelection();
	if (devChoice == wxNOT_FOUND) {
		::wxMessageBox(wxT("The Audio Device is not allowed to be empty"));
		return;
	}

	m_inDev  = m_info.getDevs().at(devChoice)->getInDev();
	m_outDev = m_info.getDevs().at(devChoice)->getOutDev();

	if (IsModal()) {
		EndModal(wxID_OK);
	} else {
		SetReturnCode(wxID_OK);
		Show(false);
	}
}

void CSoundCardDialog::enumerateAPI()
{
	bool ret = m_info.enumerateAPIs();

	if (!ret) {
		::wxMessageBox(wxT("Cannot access the sound access system."));
		return;
	}

	vector<CSoundCardAPI*>& apis = m_info.getAPIs();
	CSoundCardAPI* defAPI = NULL;

	for (unsigned int i = 0; i < apis.size(); i++) {
		CSoundCardAPI* api = apis.at(i);

		m_apiChoice->Append(api->getName());

		if (api->getDefault()) {
			m_apiChoice->SetSelection(i);
			defAPI = api;
		}
	}

	if (defAPI != NULL)
		enumerateAudio(*defAPI);
}

void CSoundCardDialog::enumerateAudio(const CSoundCardAPI& api)
{
	bool ret = m_info.enumerateDevs(api);

	if (!ret) {
		::wxMessageBox(wxT("Cannot access the sound access system."));
		return;
	}

	m_devChoice->Clear();

	vector<CSoundCardDev*>& devs = m_info.getDevs();

	for (unsigned int  i = 0; i < devs.size(); i++) {
		CSoundCardDev* dev = devs.at(i);

		m_devChoice->Append(dev->getName());

		if (dev->getInDefault())
			m_devChoice->SetSelection(i);
	}
}

int CSoundCardDialog::getAPI() const
{
	return m_api;
}

long CSoundCardDialog::getInDev() const
{
	return m_inDev;
}

long CSoundCardDialog::getOutDev() const
{
	return m_outDev;
}
