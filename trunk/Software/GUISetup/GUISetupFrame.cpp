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

#include "GUISetupFrame.h"

#include <wx/file.h>
#include <wx/config.h>

#if defined(__WXGTK__) || defined(__WXMAC__)
#include "GUISetup.xpm"
#endif

#if defined(__WINDOWS__)
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#endif

const int CREATE_BUTTON   = 27543;
const int BROWSE_BUTTON   = 27544;
const int API_COMBO       = 27545;
const int NAME_COMBO      = 27546;

const int BORDER_SIZE     = 5;
const int DATA_WIDTH      = 150;

BEGIN_EVENT_TABLE(CGUISetupFrame, wxFrame)
	EVT_CHOICE(API_COMBO, CGUISetupFrame::onAPI)
	EVT_COMBOBOX(NAME_COMBO, CGUISetupFrame::onName)
	EVT_BUTTON(CREATE_BUTTON, CGUISetupFrame::onCreate)
	EVT_BUTTON(BROWSE_BUTTON, CGUISetupFrame::onBrowse)
END_EVENT_TABLE()

CGUISetupFrame::CGUISetupFrame() :
wxFrame(NULL, -1, wxString(_("uWave SDR GUI Setup")), wxDefaultPosition, wxDefaultSize, wxMINIMIZE_BOX  | wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX | wxCLIP_CHILDREN),
m_name(NULL),
m_filename(NULL),
m_apiChoice(NULL),
m_devChoice(NULL),
m_address(NULL),
m_control(NULL),
m_data(NULL),
m_startMenu(NULL),
m_info()
{
	SetIcon(wxICON(GUISetup));

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	wxPanel* panel = new wxPanel(this, -1);

	wxFlexGridSizer* panelSizer = new wxFlexGridSizer(3);

	wxStaticText* label1 = new wxStaticText(panel, -1, _("Name:"));
	panelSizer->Add(label1, 0, wxALL, BORDER_SIZE);

	m_name = new wxComboBox(panel, NAME_COMBO, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_name, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy1 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy1, 0, wxALL, BORDER_SIZE);

	wxStaticText* label2 = new wxStaticText(panel, -1, _("SDR File Name:"));
	panelSizer->Add(label2, 0, wxALL, BORDER_SIZE);

	m_filename = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_filename, 0, wxALL, BORDER_SIZE);

	wxButton* browse = new wxButton(panel, BROWSE_BUTTON, _("Browse..."));
	panelSizer->Add(browse, 0, wxALL, BORDER_SIZE);

	wxStaticText* label3 = new wxStaticText(panel, -1, _("Audio API:"));
	panelSizer->Add(label3, 0, wxALL, BORDER_SIZE);

	m_apiChoice = new wxChoice(panel, API_COMBO, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_apiChoice, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy2 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy2, 0, wxALL, BORDER_SIZE);

	wxStaticText* label4 = new wxStaticText(panel, -1, _("Audio Device:"));
	panelSizer->Add(label4, 0, wxALL, BORDER_SIZE);

	m_devChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_devChoice, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy3 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy3, 0, wxALL, BORDER_SIZE);

	wxStaticText* label6 = new wxStaticText(panel, -1, _("SDR IP Address:"));
	panelSizer->Add(label6, 0, wxALL, BORDER_SIZE);

	m_address = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_address, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy5 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy5, 0, wxALL, BORDER_SIZE);

	wxStaticText* label7 = new wxStaticText(panel, -1, _("SDR Control Port:"));
	panelSizer->Add(label7, 0, wxALL, BORDER_SIZE);

	m_control = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_control, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy6 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy6, 0, wxALL, BORDER_SIZE);

	wxStaticText* label8 = new wxStaticText(panel, -1, _("SDR Data Port:"));
	panelSizer->Add(label8, 0, wxALL, BORDER_SIZE);

	m_data = new wxTextCtrl(panel, -1, wxEmptyString, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(m_data, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy7 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy7, 0, wxALL, BORDER_SIZE);

	wxStaticText* label9 = new wxStaticText(panel, -1, _("Create Start Menu entry:"));
	panelSizer->Add(label9, 0, wxALL, BORDER_SIZE);

	m_startMenu = new wxCheckBox(panel, -1, wxEmptyString);
	panelSizer->Add(m_startMenu, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy8 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy8, 0, wxALL, BORDER_SIZE);

#if defined(__WXMSW__)
	wxStaticText* label10 = new wxStaticText(panel, -1, _("Create Desktop icon:"));
	panelSizer->Add(label10, 0, wxALL, BORDER_SIZE);

	m_deskTop = new wxCheckBox(panel, -1, wxEmptyString);
	panelSizer->Add(m_deskTop, 0, wxALL, BORDER_SIZE);

	wxStaticText* dummy9 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy9, 0, wxALL, BORDER_SIZE);
#endif

	wxStaticText* dummy10 = new wxStaticText(panel, -1, wxEmptyString);
	panelSizer->Add(dummy10, 0, wxALL, BORDER_SIZE);

	wxButton* create = new wxButton(panel, CREATE_BUTTON, _("Create"), wxDefaultPosition, wxSize(DATA_WIDTH, -1));
	panelSizer->Add(create, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);

	mainSizer->Add(panel);

	SetSizer(mainSizer);

	mainSizer->SetSizeHints(this);

	enumerateConfigs();
	enumerateAPI();
}

CGUISetupFrame::~CGUISetupFrame()
{
}

void CGUISetupFrame::onName(wxCommandEvent& event)
{
	wxString name = m_name->GetValue();

	readConfig(name);
}

void CGUISetupFrame::onAPI(wxCommandEvent& event)
{
	int n = event.GetSelection();

	CSoundCardAPI* api = m_info.getAPIs().at(n);

	enumerateAudio(*api);
}

void CGUISetupFrame::onBrowse(wxCommandEvent& event)
{
	wxFileDialog files(this, _("Choose an SDR Configuration File"), wxEmptyString, wxEmptyString, _("SDR files (*.sdr)|*.sdr"), wxFD_FILE_MUST_EXIST);
	int ret = files.ShowModal();

	if (ret == wxID_OK) {
		wxString fileName = files.GetPath();
		m_filename->SetValue(fileName);
	}
}

void CGUISetupFrame::onCreate(wxCommandEvent& event)
{
	wxString name = m_name->GetValue();
	if (name.IsEmpty()) {
		::wxMessageBox(_("The Name is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	wxString fileName = m_filename->GetValue();
	if (fileName.IsEmpty()) {
		::wxMessageBox(_("The SDR File Name is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}
	if (!wxFile::Exists(fileName.c_str())) {
		::wxMessageBox(_("The SDR File does not exist"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	long audioAPI = m_apiChoice->GetSelection();
	if (audioAPI == wxNOT_FOUND) {
		::wxMessageBox(_("The Audio API is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	int devChoice = m_devChoice->GetSelection();
	if (devChoice == wxNOT_FOUND) {
		::wxMessageBox(_("The Audio Device is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	long audioInDev  = m_info.getDevs().at(devChoice)->getInDev();
	long audioOutDev = m_info.getDevs().at(devChoice)->getOutDev();

	wxString ipAddress = m_address->GetValue();
	if (ipAddress.IsEmpty()) {
		::wxMessageBox(_("The SDR IP Address is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	wxString control = m_control->GetValue();
	if (control.IsEmpty()) {
		::wxMessageBox(_("The SDR Control Port is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	long controlPort;
	control.ToLong(&controlPort);
	if (controlPort < 1L || controlPort > 65536L) {
		::wxMessageBox(_("The SDR Control Port must be between 1 and 65536"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}
	
	wxString data = m_data->GetValue();
	if (data.IsEmpty()) {
		::wxMessageBox(_("The SDR Data Port is not allowed to be empty"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	long dataPort;
	data.ToLong(&dataPort);
	if (dataPort < 1L || dataPort > 65536L) {
		::wxMessageBox(_("The SDR Data Port must be between 1 and 65536"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	wxConfig* config = new wxConfig(wxT("UWSDR"));

	wxString fileNameKey    = wxT("/") + name + wxT("/FileName");
	wxString audioAPIKey    = wxT("/") + name + wxT("/AudioAPI");
	wxString audioOutDevKey = wxT("/") + name + wxT("/AudioOutDev");
	wxString audioInDevKey  = wxT("/") + name + wxT("/AudioInDev");
	wxString ipAddressKey   = wxT("/") + name + wxT("/IPAddress");
	wxString controlPortKey = wxT("/") + name + wxT("/ControlPort");
	wxString dataPortKey    = wxT("/") + name + wxT("/DataPort");

	wxString test;
	if (config->Read(fileNameKey, &test)) {
		int ret = ::wxMessageBox(_("An SDR with the same name already exists. Overwrite?"), _("Overwrite confirmation"), wxYES_NO | wxICON_QUESTION);
		if (ret == wxNO)
			return;
	}

	bool ret = config->Write(fileNameKey, fileName);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - FileName"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	ret = config->Write(audioAPIKey, audioAPI);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - AudioAPI"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	ret = config->Write(audioOutDevKey, audioOutDev);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - AudioOutDev"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	ret = config->Write(audioInDevKey, audioInDev);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - AudioInDev"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	ret = config->Write(ipAddressKey, ipAddress);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - IPAddress"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	ret = config->Write(controlPortKey, controlPort);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - ControlPort"), _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	ret = config->Write(dataPortKey, dataPort);
	if (!ret) {
		::wxMessageBox(_("Unable to write configuration data - DataPort"), _("GUISetup Error"), wxICON_ERROR);
		return;
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
	wxConfig* config = new wxConfig(wxT("UWSDR"));

	wxString fileNameKey    = wxT("/") + name + wxT("/FileName");
	wxString audioAPIKey    = wxT("/") + name + wxT("/AudioAPI");
	wxString audioOutDevKey = wxT("/") + name + wxT("/AudioOutDev");
	wxString ipAddressKey   = wxT("/") + name + wxT("/IPAddress");
	wxString controlPortKey = wxT("/") + name + wxT("/ControlPort");
	wxString dataPortKey    = wxT("/") + name + wxT("/DataPort");

	wxString text;
	config->Read(fileNameKey, &text);
	m_filename->SetValue(text);

	long api;
	config->Read(audioAPIKey, &api);
	m_apiChoice->SetSelection(api);

	long outDev;
	config->Read(audioOutDevKey, &outDev);

	vector<CSoundCardDev*>& devs = m_info.getDevs();

	for (unsigned int i = 0; i < devs.size(); i++)
		if (devs.at(i)->getOutDev() == outDev)
			m_devChoice->SetSelection(i);

	config->Read(ipAddressKey, &text);
	m_address->SetValue(text);

	long port;
	config->Read(controlPortKey, &port);
	text.Printf(wxT("%ld"), port);
	m_control->SetValue(text);

	config->Read(dataPortKey, &port);
	text.Printf(wxT("%ld"), port);
	m_data->SetValue(text);

	delete config;
}

void CGUISetupFrame::enumerateAPI()
{
	bool ret = m_info.enumerateAPIs();

	if (!ret) {
		::wxMessageBox(_("Cannot access the sound access system."), _("GUISetup Error"), wxICON_ERROR);
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

void CGUISetupFrame::enumerateAudio(const CSoundCardAPI& api)
{
	bool ret = m_info.enumerateDevs(api);

	if (!ret) {
		::wxMessageBox(_("Cannot access the sound access system."), _("GUISetup Error"), wxICON_ERROR);
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
#endif

#if defined(__WXGTK__)
void CGUISetupFrame::writeStartMenu(const wxString& name, const wxString& dir)
{
	wxString homeDir;
	bool ret = wxGetEnv(wxT("HOME"), &homeDir);
	if (!ret)
		return;

	wxString fileName;
	fileName.Printf(wxT("%s/.local/applications/%s.desktop"), homeDir.c_str(), name.c_str());

	wxFile file;
	ret = file.Open(fileName, wxFile::write);
	if (!ret) {
		::wxMessageBox(_("Cannot open file: ") + fileName, _("GUISetup Error"), wxICON_ERROR);
		return;
	}

	file.Write(wxT("[Desktop Entry]\n"));
	file.Write(wxT("Type=Application\n"));
	file.Write(wxT("Version=1.0\n"));
	file.Write(wxT("Encoding=UTF-8\n"));
	file.Write(wxT("Name=") + name + wxT("\n"));
	file.Write(wxT("Categories=Network;HamRadio\n"));
	file.Write(wxT("Comment=\n"));
	file.Write(wxT("Icon=UWSDR.png\n"));
	file.Write(wxT("Exec=") + wxString(BIN_DIR) + wxT("/UWSDR ") + name + wxT("\n"));
	file.Write(wxT("Path=.\n"));
	file.Write(wxT("Terminal=false\n"));

	file.Flush();
	file.Close();
}
#endif

#if defined(__WXMAC__) || defined(__WXCOCOA__)
void CGUISetupFrame::writeStartMenu(const wxString& name, const wxString& dir)
{
   // To be filled in later
}
#endif
