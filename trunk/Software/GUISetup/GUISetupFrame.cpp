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

#include "GUISetupFrame.h"

#include "EthernetDialog.h"
#include "SoundCardDialog.h"

#include <wx/config.h>
#include <wx/filename.h>
#include <wx/dir.h>

#if defined(__WXGTK__) || defined(__WXMAC__)
#include "GUISetup.xpm"
#endif

#if defined(__WXGTK__)
const wxString XDG_DATA_HOME_ENV = wxT("XDG_DATA_HOME");
const wxString HOME_ENV          = wxT("HOME");
const wxString DEFAULT_DIR       = wxT("/.local/share");
const wxString APPLICATION_DIR   = wxT("/applications");
const wxString UWSDR_FILE        = wxT("UWSDR.desktop");
const wxString NAME_TOKEN        = wxT("@NAME@");
#endif

#if defined(__WINDOWS__)
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#endif

const struct {
	bool userAudioButton;
	bool sdrAudioButton;
	bool ethernetButton;
	bool portButton;
	bool txOutData;
	bool audioCheck;
} featureList[] = {
	{true, true,  false, false, false, false},	// TYPE_AUDIORX
	{true, true,  false, true,  true,  true},	// TYPE_AUDIOTXRX
	{true, false, false, true,  false, false},	// TYPE_DEMO
	{true, false, true,  true,  false, false}	// TYPE_UWSDR1
};

const int CREATE_BUTTON     = 27543;
const int BROWSE_BUTTON     = 27544;
const int USER_AUDIO_BUTTON = 27545;
const int SDR_AUDIO_BUTTON  = 27546;
const int ETHERNET_BUTTON   = 27547;
const int PORT_BUTTON       = 27548;
const int NAME_COMBO        = 27549;


const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;


BEGIN_EVENT_TABLE(CGUISetupFrame, wxFrame)
	EVT_COMBOBOX(NAME_COMBO,      CGUISetupFrame::onName)
	EVT_TEXT_ENTER(NAME_COMBO,    CGUISetupFrame::onName)
	EVT_BUTTON(CREATE_BUTTON,     CGUISetupFrame::onCreate)
	EVT_BUTTON(BROWSE_BUTTON,     CGUISetupFrame::onBrowse)
	EVT_BUTTON(USER_AUDIO_BUTTON, CGUISetupFrame::onUserAudio)
	EVT_BUTTON(SDR_AUDIO_BUTTON,  CGUISetupFrame::onSDRAudio)
	EVT_BUTTON(ETHERNET_BUTTON,   CGUISetupFrame::onEthernet)
	EVT_BUTTON(PORT_BUTTON,       CGUISetupFrame::onPort)
END_EVENT_TABLE()


CGUISetupFrame::CGUISetupFrame() :
wxFrame(NULL, -1, wxString(_("uWave SDR GUI Setup")), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
m_name(NULL),
m_filenameText(NULL),
m_startMenu(NULL),
m_userAudio(NULL),
m_sdrAudio(NULL),
m_ethernet(NULL),
m_port(NULL),
m_filename(),
m_sdrType(TYPE_UWSDR1),
m_userAudioInDev(-1L),
m_userAudioOutDev(-1L),
m_sdrAudioInDev(-1L),
m_sdrAudioOutDev(-1L),
m_ipAddress(),
m_controlPort(-1L),
m_dataPort(-1L),
m_txInEnable(true),
m_txInDev(),
m_txInPin(IN_NONE),
m_keyInEnable(true),
m_keyInDev(),
m_keyInPin(IN_NONE),
m_txOutDev(),
m_txOutPin(OUT_NONE)
{
	SetIcon(wxICON(GUISetup));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(3);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Name:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_name = new wxComboBox(panel, NAME_COMBO, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1), 0, NULL, wxCB_DROPDOWN);
	panelSizer->Add(m_name, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy1 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy1, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("SDR File Name:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_filenameText = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1), wxTE_READONLY);
	panelSizer->Add(m_filenameText, 0, wxALL, BORDER_SIZE);

	wxButton* browse = new wxButton(panel, BROWSE_BUTTON, _("Browse..."));
	panelSizer->Add(browse, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("User Audio:"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_userAudio = new wxButton(panel, USER_AUDIO_BUTTON, _("Set"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_userAudio->Disable();
	panelSizer->Add(m_userAudio, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy2 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy2, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("SDR Audio:"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_sdrAudio = new wxButton(panel, SDR_AUDIO_BUTTON, _("Set"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_sdrAudio->Disable();
	panelSizer->Add(m_sdrAudio, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy3 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy3, 0, wxALL, BORDER_SIZE);

	wxStaticText* label5 = new wxStaticText(panel, -1, _("Ethernet:"));
	panelSizer->Add(label5, 0, wxALL, BORDER_SIZE);

	m_ethernet = new wxButton(panel, ETHERNET_BUTTON, _("Set"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_ethernet->Disable();
	panelSizer->Add(m_ethernet, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy4 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy4, 0, wxALL, BORDER_SIZE);

	wxStaticText* label6 = new wxStaticText(panel, -1, _("Control Port:"));
	panelSizer->Add(label6, 0, wxALL, BORDER_SIZE);

	m_port = new wxButton(panel, PORT_BUTTON, _("Set"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	m_port->Disable();
	panelSizer->Add(m_port, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy5 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy5, 0, wxALL, BORDER_SIZE);

#if defined(__WXGTK__)
	wxString dir;
	if (getDesktopDir(dir)) {
#endif
		wxStaticText* label7 = new wxStaticText(panel, -1, _("Create Menu entry:"));
		panelSizer->Add(label7, 0, wxALL, BORDER_SIZE);

		m_startMenu = new wxCheckBox(panel, -1, wxEmptyString);
		panelSizer->Add(m_startMenu, 0, wxALL, BORDER_SIZE);

		wxStaticText* dummy6 = new wxStaticText(panel, -1, wxEmptyString);
		panelSizer->Add(dummy6, 0, wxALL, BORDER_SIZE);
#if defined(__WXGTK__)
	}
#endif

#if defined(__WXMSW__)
	wxStaticText* label8 = new wxStaticText(panel, -1, _("Create Desktop icon:"));
	panelSizer->Add(label8, 0, wxALL, BORDER_SIZE);

	m_deskTop = new wxCheckBox(panel, -1, wxEmptyString);
	panelSizer->Add(m_deskTop, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy7 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy7, 0, wxALL, BORDER_SIZE);
#endif

	wxStaticText* dummy8 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy8, 0, wxALL, BORDER_SIZE);

	wxButton* create = new wxButton(panel, CREATE_BUTTON, _("Create"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(create, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	enumerateConfigs();
}

CGUISetupFrame::~CGUISetupFrame()
{
}

void CGUISetupFrame::onName(wxCommandEvent& WXUNUSED(event))
{
	wxString name = m_name->GetValue();

	readConfig(name);
}

void CGUISetupFrame::onBrowse(wxCommandEvent& WXUNUSED(event))
{
	// Pre-load the directory where the .sdr files are located
#if defined(__WXMSW__)
	wxConfig* config = new wxConfig(wxT("UWSDR"));

	wxString instDirKey = wxT("/InstPath");

	wxString sdrDir;
	bool found = config->Read(instDirKey, &sdrDir);

	delete config;

	if (!found) {
		::wxMessageBox(_("Cannot find the registry key for the\ninstallation directory."), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	sdrDir.Append(wxT("\\SDR Files"));
#elif defined(__WXGTK__)
	wxString sdrDir = DATA_DIR;
#endif

	wxFileDialog files(this, _("Choose an SDR Configuration File"), wxEmptyString, wxEmptyString, _("SDR files (*.sdr)|*.sdr"), wxFD_FILE_MUST_EXIST);
	files.SetDirectory(sdrDir);

	int ret = files.ShowModal();

	if (ret != wxID_OK)
		return;

	m_filename = files.GetPath();

	wxFileName filePath(m_filename);

	m_filenameText->SetValue(filePath.GetFullName());

	// Clear everything
	m_userAudio->Disable();
	m_sdrAudio->Disable();
	m_ethernet->Disable();
	m_port->Disable();

	CSDRDescrFile file(m_filename);
	if (!file.isValid()) {
		::wxMessageBox(_("Cannot open the SDR File"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	m_sdrType = file.getType();

	if (featureList[m_sdrType].userAudioButton)
		m_userAudio->Enable();
	if (featureList[m_sdrType].sdrAudioButton)
		m_sdrAudio->Enable();
	if (featureList[m_sdrType].ethernetButton)
		m_ethernet->Enable();
	if (featureList[m_sdrType].portButton)
		m_port->Enable();
}

void CGUISetupFrame::onCreate(wxCommandEvent& WXUNUSED(event))
{
	wxString name = m_name->GetValue();
	if (name.IsEmpty()) {
		::wxMessageBox(_("The Name is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	// Whitespace isn't allowed in the SDR name
	int pos1 = name.Find(wxT(' '));
	int pos2 = name.Find(wxT('\t'));
	if (pos1 != wxNOT_FOUND || pos2 != wxNOT_FOUND) {
		::wxMessageBox(_("The Name cannot contain white space (spaces, tabs, etc)"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	if (m_filename.IsEmpty()) {
		::wxMessageBox(_("The SDR File Name is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	CSDRDescrFile file(m_filename);
	if (!file.isValid()) {
		::wxMessageBox(_("The SDR File does not exist or is invalid"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	m_sdrType = file.getType();

	if (featureList[m_sdrType].userAudioButton) {
		if (m_userAudioInDev == -1L || m_userAudioOutDev == -1L) {
			::wxMessageBox(_("The User Audio has not been set"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (featureList[m_sdrType].sdrAudioButton) {
		if (m_sdrAudioInDev == -1L || m_sdrAudioOutDev == -1L) {
			::wxMessageBox(_("The SDR Audio has not been set"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (featureList[m_sdrType].audioCheck) {
		if (m_sdrAudioInDev == m_userAudioInDev || m_sdrAudioOutDev == m_userAudioOutDev) {
			::wxMessageBox(_("The SDR Audio cannot be the same as the User Audio"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (featureList[m_sdrType].ethernetButton) {
		if (m_ipAddress.IsEmpty() || m_controlPort == -1L || m_dataPort == -1L) {
			::wxMessageBox(_("The Ethernet has not been set"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (featureList[m_sdrType].txOutData) {
		if (m_txOutDev.IsEmpty() || m_txOutPin == -1) {
			::wxMessageBox(_("The Transmit Out Port has not been set"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	wxConfig* config = new wxConfig(wxT("UWSDR"));

	wxString fileNameKey        = wxT("/") + name + wxT("/FileName");
	wxString userAudioOutDevKey = wxT("/") + name + wxT("/UserAudioOutDev");
	wxString userAudioInDevKey  = wxT("/") + name + wxT("/UserAudioInDev");
	wxString sdrAudioOutDevKey  = wxT("/") + name + wxT("/SDRAudioOutDev");
	wxString sdrAudioInDevKey   = wxT("/") + name + wxT("/SDRAudioInDev");
	wxString ipAddressKey       = wxT("/") + name + wxT("/IPAddress");
	wxString controlPortKey     = wxT("/") + name + wxT("/ControlPort");
	wxString dataPortKey        = wxT("/") + name + wxT("/DataPort");
	wxString txInEnableKey      = wxT("/") + name + wxT("/TXInEnable");
	wxString txInDevKey         = wxT("/") + name + wxT("/TXInDev");
	wxString txInPinKey         = wxT("/") + name + wxT("/TXInPin");
	wxString keyInEnableKey     = wxT("/") + name + wxT("/KeyInEnable");
	wxString keyInDevKey        = wxT("/") + name + wxT("/KeyInDev");
	wxString keyInPinKey        = wxT("/") + name + wxT("/KeyInPin");
	wxString txOutDevKey        = wxT("/") + name + wxT("/TXOutDev");
	wxString txOutPinKey        = wxT("/") + name + wxT("/TXOutPin");

	wxString test;
	if (config->Read(fileNameKey, &test)) {
		int ret = ::wxMessageBox(_("An SDR with the same name already exists. Overwrite?"), _("Overwrite confirmation"), wxYES_NO | wxICON_QUESTION);
		if (ret == wxNO)
			return;
	}

	bool ret = config->Write(fileNameKey, m_filename);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - FileName"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	if (featureList[m_sdrType].userAudioButton) {
		ret = config->Write(userAudioOutDevKey, m_userAudioOutDev);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - UserAudioOutDev"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(userAudioInDevKey, m_userAudioInDev);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - UserAudioInDev"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (featureList[m_sdrType].sdrAudioButton) {
		ret = config->Write(sdrAudioOutDevKey, m_sdrAudioOutDev);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - SDRAudioOutDev"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(sdrAudioInDevKey, m_sdrAudioInDev);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - SDRAudioInDev"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (featureList[m_sdrType].ethernetButton) {
		bool ret = config->Write(ipAddressKey, m_ipAddress);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - IPAddress"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(controlPortKey, m_controlPort);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - ControlPort"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(dataPortKey, m_dataPort);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - DataPort"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}
	}

	if (featureList[m_sdrType].portButton) {
		bool ret = config->Write(txInEnableKey, m_txInEnable);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - TXInEnable"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(txInDevKey, m_txInDev);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - TXInDev"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(txInPinKey, m_txInPin);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - TXInPin"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(keyInEnableKey, m_keyInEnable);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - KeyInEnable"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(keyInDevKey, m_keyInDev);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - KeyInDev"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		ret = config->Write(keyInPinKey, m_keyInPin);
		if (!ret) {
			::wxMessageBox(_("Unable to write configuration data - KeyInPin"), _("GUISetup Error"), wxICON_ERROR);
			return;
		}

		if (featureList[m_sdrType].txOutData) {
			bool ret = config->Write(txOutDevKey, m_txOutDev);
			if (!ret) {
				::wxMessageBox(_("Unable to write configuration data - TXOutDev"), _("GUISetup Error"), wxICON_ERROR);
				return;
			}

			ret = config->Write(txOutPinKey, m_txOutPin);
			if (!ret) {
				::wxMessageBox(_("Unable to write configuration data - TXOutPin"), _("GUISetup Error"), wxICON_ERROR);
				return;
			}
		}
	}

	config->Flush();

#if defined(__WXMSW__)
	wxString instDirKey = wxT("/InstPath");

	wxString dir;
	bool found = config->Read(instDirKey, &dir);

	if (!found) {
		::wxMessageBox(_("Cannot find the registry key for the\ninstallation directory. Cannot create\nthe start menu entry."), _("GUISetup Error"), wxICON_ERROR);
	} else {
		bool create = m_startMenu->GetValue();
		if (create)
			writeStartMenu(name, dir);

		create = m_deskTop->GetValue();
		if (create)
			writeDeskTop(name, dir);
	}
#elif defined(__WXGTK__)
	bool create = m_startMenu->GetValue();
	if (create) {
		wxString dir;
		getDesktopDir(dir);

		writeStartMenu(name, dir);
	}
#else
	bool create = m_startMenu->GetValue();
	if (create)
		writeStartMenu(name, wxEmptyString);
#endif

	delete config;

	::wxMessageBox(_("uWave SDR configuration written."));

	Close(true);
}

void CGUISetupFrame::enumerateConfigs()
{
	wxConfig* config = new wxConfig(wxT("UWSDR"));

	wxString name;
	long n;
	bool ret = config->GetFirstGroup(name, n);

	while (ret) {
		m_name->Append(name);

		ret = config->GetNextGroup(name, n);
	}

	delete config;
}

void CGUISetupFrame::readConfig(const wxString& name)
{
	// Clear everything
	m_userAudio->Disable();
	m_sdrAudio->Disable();
	m_ethernet->Disable();
	m_port->Disable();

	m_filenameText->Clear();

	wxConfig* config = new wxConfig(wxT("UWSDR"));

	wxString fileNameKey        = wxT("/") + name + wxT("/FileName");
	wxString userAudioInDevKey  = wxT("/") + name + wxT("/UserAudioInDev");
	wxString userAudioOutDevKey = wxT("/") + name + wxT("/UserAudioOutDev");
	wxString sdrAudioInDevKey   = wxT("/") + name + wxT("/SDRAudioInDev");
	wxString sdrAudioOutDevKey  = wxT("/") + name + wxT("/SDRAudioOutDev");
	wxString ipAddressKey       = wxT("/") + name + wxT("/IPAddress");
	wxString controlPortKey     = wxT("/") + name + wxT("/ControlPort");
	wxString dataPortKey        = wxT("/") + name + wxT("/DataPort");
	wxString txInEnableKey      = wxT("/") + name + wxT("/TXInEnable");
	wxString txInDevKey         = wxT("/") + name + wxT("/TXInDev");
	wxString txInPinKey         = wxT("/") + name + wxT("/TXInPin");
	wxString keyInEnableKey     = wxT("/") + name + wxT("/KeyInEnable");
	wxString keyInDevKey        = wxT("/") + name + wxT("/KeyInDev");
	wxString keyInPinKey        = wxT("/") + name + wxT("/KeyInPin");
	wxString txOutDevKey        = wxT("/") + name + wxT("/TXOutDev");
	wxString txOutPinKey        = wxT("/") + name + wxT("/TXOutPin");

	bool ret = config->Read(fileNameKey, &m_filename);
	if (!ret)
		return;

	wxFileName filePath(m_filename);
	m_filenameText->SetValue(filePath.GetFullName());

	CSDRDescrFile file(m_filename);
	if (!file.isValid()) {
		::wxMessageBox(_("Cannot open the SDR File"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	config->Read(userAudioInDevKey,  &m_userAudioInDev);
	config->Read(userAudioOutDevKey, &m_userAudioOutDev);

	config->Read(sdrAudioInDevKey,  &m_sdrAudioInDev);
	config->Read(sdrAudioOutDevKey, &m_sdrAudioOutDev);

	config->Read(ipAddressKey,   &m_ipAddress);
	config->Read(controlPortKey, &m_controlPort);
	config->Read(dataPortKey,    &m_dataPort);

	config->Read(txInEnableKey,  &m_txInEnable);
	config->Read(txInDevKey,     &m_txInDev);
	config->Read(txInPinKey,     (long*)&m_txInPin);

	config->Read(keyInEnableKey, &m_keyInEnable);
	config->Read(keyInDevKey,    &m_keyInDev);
	config->Read(keyInPinKey,    (long*)&m_keyInPin);

	config->Read(txOutDevKey,    &m_txOutDev);
	config->Read(txOutPinKey,    (long*)&m_txOutPin);

	delete config;

	m_sdrType = file.getType();

	if (featureList[m_sdrType].userAudioButton)
		m_userAudio->Enable();
	if (featureList[m_sdrType].sdrAudioButton)
		m_sdrAudio->Enable();
	if (featureList[m_sdrType].ethernetButton)
		m_ethernet->Enable();
	if (featureList[m_sdrType].portButton)
		m_port->Enable();
}

void CGUISetupFrame::onUserAudio(wxCommandEvent& WXUNUSED(event))
{
	CSoundCardDialog dialog(this, _("User Audio Setup"), m_userAudioInDev, m_userAudioOutDev);

	int ret = dialog.ShowModal();
	if (ret == wxID_OK) {
		m_userAudioInDev  = dialog.getInDev();
		m_userAudioOutDev = dialog.getOutDev();
	}
}

void CGUISetupFrame::onSDRAudio(wxCommandEvent& WXUNUSED(event))
{
	CSoundCardDialog dialog(this, _("SDR Audio Setup"), m_sdrAudioInDev, m_sdrAudioOutDev);

	int ret = dialog.ShowModal();
	if (ret == wxID_OK) {
		m_sdrAudioInDev  = dialog.getInDev();
		m_sdrAudioOutDev = dialog.getOutDev();
	}
}

void CGUISetupFrame::onEthernet(wxCommandEvent& WXUNUSED(event))
{
	CEthernetDialog dialog(this, _("Ethernet Setup"), m_ipAddress, m_controlPort, m_dataPort);

	int ret = dialog.ShowModal();
	if (ret == wxID_OK) {
		m_ipAddress   = dialog.getIPAddress();
		m_controlPort = dialog.getControlPort();
		m_dataPort    = dialog.getDataPort();
	}
}

void CGUISetupFrame::onPort(wxCommandEvent& WXUNUSED(event))
{
	CPortDialog dialog(this, _("Control Port Setup"), featureList[m_sdrType].txOutData);

	dialog.setTXInEnable(m_txInEnable);
	dialog.setTXInDev(m_txInDev);
	dialog.setTXInPin(m_txInPin);

	dialog.setKeyInEnable(m_keyInEnable);
	dialog.setKeyInDev(m_keyInDev);
	dialog.setKeyInPin(m_keyInPin);

	dialog.setTXOutDev(m_txOutDev);
	dialog.setTXOutPin(m_txOutPin);

	int ret = dialog.ShowModal();
	if (ret != wxID_OK)
		return;

	m_txInEnable = dialog.getTXInEnable();
	m_txInDev    = dialog.getTXInDev();
	m_txInPin    = dialog.getTXInPin();

	m_keyInEnable = dialog.getKeyInEnable();
	m_keyInDev    = dialog.getKeyInDev();
	m_keyInPin    = dialog.getKeyInPin();

	m_txOutDev = dialog.getTXOutDev();
	m_txOutPin = dialog.getTXOutPin();
}

#if defined(__WXMSW__)

void CGUISetupFrame::writeStartMenu(const wxString& name, const wxString& dir)
{
	TCHAR folder[MAX_PATH];
	BOOL res = ::SHGetSpecialFolderPath(NULL, folder, CSIDL_PROGRAMS, FALSE);

	if (!res) {
		::wxMessageBox(_("Cannot get the Start Menu folder from Windows"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	wxString linkName = name + wxT(".lnk");
	wxString linkPath = wxString(folder) + wxT("\\UWSDR\\") + linkName;
	wxString exePath  = dir + wxT("\\UWSDR.exe");
	wxString args     = name;

	HRESULT hRes = ::CoInitialize(NULL);
	if (!SUCCEEDED(hRes)) {
		::wxMessageBox(_("Cannot initialise the COM interface"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	IShellLink* pShellLink;
	hRes = ::CoCreateInstance(CLSID_ShellLink, NULL,
								CLSCTX_INPROC_SERVER,
								IID_IShellLink,
								(void**)&pShellLink);

	if (!SUCCEEDED(hRes)) {
		::CoUninitialize();
		::wxMessageBox(_("Cannot create a COM interface"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	pShellLink->SetPath(exePath.c_str());
	pShellLink->SetArguments(args.c_str());
	pShellLink->SetWorkingDirectory(dir.c_str());

    WORD wszLinkfile[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, linkPath.c_str(), -1, LPWSTR(wszLinkfile), MAX_PATH);

    IPersistFile* pPersistFile;
	hRes = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);

	if (!SUCCEEDED(hRes)) {
		pShellLink->Release();
		::CoUninitialize();
		::wxMessageBox(_("Cannot query the COM interface"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	hRes = pPersistFile->Save(LPCOLESTR(wszLinkfile), TRUE);
	if (!SUCCEEDED(hRes))
		::wxMessageBox(_("Cannot save the shortcut file"), _("GUISetup Error"), wxICON_ERROR);

	pPersistFile->Release();
	pShellLink->Release();

	::CoUninitialize();
}

void CGUISetupFrame::writeDeskTop(const wxString& name, const wxString& dir)
{
	TCHAR folder[MAX_PATH];
	BOOL res = ::SHGetSpecialFolderPath(NULL, folder, CSIDL_DESKTOP, FALSE);

	if (!res) {
		::wxMessageBox(_("Cannot get the Desktop folder from Windows"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	wxString linkName = name + wxT(".lnk");
	wxString linkPath = wxString(folder) + wxT("\\") + linkName;
	wxString exePath  = dir + wxT("\\UWSDR.exe");
	wxString args     = name;

	HRESULT hRes = ::CoInitialize(NULL);
	if (!SUCCEEDED(hRes)) {
		::wxMessageBox(_("Cannot initialise the COM interface"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	IShellLink* pShellLink;
	hRes = ::CoCreateInstance(CLSID_ShellLink, NULL,
								CLSCTX_INPROC_SERVER,
								IID_IShellLink,
								(void**)&pShellLink);

	if (!SUCCEEDED(hRes)) {
		::CoUninitialize();
		::wxMessageBox(_("Cannot create a COM interface"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	pShellLink->SetPath(exePath.c_str());
	pShellLink->SetArguments(args.c_str());
	pShellLink->SetWorkingDirectory(dir.c_str());

    WORD wszLinkfile[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, linkPath.c_str(), -1, LPWSTR(wszLinkfile), MAX_PATH);

    IPersistFile* pPersistFile;
	hRes = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);

	if (!SUCCEEDED(hRes)) {
		pShellLink->Release();
		::CoUninitialize();
		::wxMessageBox(_("Cannot query the COM interface"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	hRes = pPersistFile->Save(LPCOLESTR(wszLinkfile), TRUE);
	if (!SUCCEEDED(hRes))
		::wxMessageBox(_("Cannot save the shortcut file"), _("GUISetup Error"), wxICON_ERROR);

	pPersistFile->Release();
	pShellLink->Release();

	::CoUninitialize();
}

#elif defined(__WXGTK__)

void CGUISetupFrame::writeStartMenu(const wxString& name, const wxString& dir)
{
	wxString fileName;
	fileName.Printf(wxT("%s/%s"), DATA_DIR, UWSDR_FILE.c_str());

	// Open the .desktop template file
	wxTextFile inFile;
	bool ret = inFile.Open(fileName);
	if (!ret) {
		::wxMessageBox(_("Cannot read file: ") + fileName, _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	fileName.Printf(wxT("%s/%s.desktop"), dir.c_str(), name.c_str());

	// Remove the old file so that Create will work below
	if (::wxFileExists(fileName))
		::wxRemoveFile(fileName);

	wxTextFile outFile;
	ret = outFile.Create(fileName);
	if (!ret) {
		::wxMessageBox(_("Cannot create file: ") + fileName, _("GUISetup Error"), wxICON_ERROR);
		inFile.Close();
		return;
	}

	wxString line = inFile.GetFirstLine();

	while (!inFile.Eof()) {
		line.Replace(NAME_TOKEN, name, true);

		outFile.AddLine(line);

		line = inFile.GetNextLine();
	}

	outFile.Write();
	outFile.Close();

	inFile.Close();
}

bool CGUISetupFrame::getDesktopDir(wxString& dir) const
{
	bool ret = ::wxGetEnv(XDG_DATA_HOME_ENV, &dir);

	if (!ret) {
		ret = ::wxGetEnv(wxT(HOME_ENV), &dir);
		if (!ret)
			return false;

		dir.Append(DEFAULT_DIR);
	}

	dir.Append(APPLICATION_DIR);

	return wxDir::Exists(dir);
}

#else

void CGUISetupFrame::writeStartMenu(const wxString& name, const wxString& dir)
{
   // To be filled in later
}

#endif
