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
#include "portaudio.h"

#include <wx/file.h>
#include <wx/config.h>

#include "GUISetup.xpm"

#ifdef __WXMSW__
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
	EVT_COMBOBOX(API_COMBO, CGUISetupFrame::onAPI)
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
m_inDevs(),
m_outDevs(),
m_startMenu(NULL)
{
	SetIcon(wxIcon(GUISetup_xpm));

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

	m_apiChoice = new wxChoice(panel, -1, wxDefaultPosition, wxSize(DATA_WIDTH, -1));
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

#ifdef __WXMSW__
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
	int api = event.GetSelection();

	enumerateAudio(api);
}

void CGUISetupFrame::onBrowse(wxCommandEvent& event)
{
	wxFileDialog files(this, _("Choose an SDR Configuration File"), wxEmptyString, wxEmptyString, _("SDR files (*.sdr)|*.sdr"), wxFILE_MUST_EXIST);
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
		::wxMessageBox(_("The Name is not allowed to be empty"));
		return;
	}

	wxString fileName = m_filename->GetValue();
	if (fileName.IsEmpty()) {
		::wxMessageBox(_("The SDR File Name is not allowed to be empty"));
		return;
	}
	if (!wxFile::Exists(fileName.c_str())) {
		::wxMessageBox(_("The SDR File does not exist"));
		return;
	}

	long audioAPI = m_apiChoice->GetSelection();
	if (audioAPI == wxNOT_FOUND) {
		::wxMessageBox(_("The Audio API is not allowed to be empty"));
		return;
	}

	int devChoice = m_devChoice->GetSelection();
	if (devChoice == wxNOT_FOUND) {
		::wxMessageBox(_("The Audio Device is not allowed to be empty"));
		return;
	}
	long audioInDev  = m_inDevs.at(devChoice);
	long audioOutDev = m_outDevs.at(devChoice);

	wxString ipAddress = m_address->GetValue();
	if (ipAddress.IsEmpty()) {
		::wxMessageBox(_("The SDR IP Address is not allowed to be empty"));
		return;
	}

	wxString control = m_control->GetValue();
	if (control.IsEmpty()) {
		::wxMessageBox(_("The SDR Control Port is not allowed to be empty"));
		return;
	}
	int controlPort = ::atoi(control.c_str());
	if (controlPort < 1 || controlPort > 65536) {
		::wxMessageBox(_("The SDR Control Port must be between 1 and 65536"));
		return;
	}
	
	wxString data = m_data->GetValue();
	if (data.IsEmpty()) {
		::wxMessageBox(_("The SDR Data Port is not allowed to be empty"));
		return;
	}
	int dataPort = ::atoi(data.c_str());
	if (dataPort < 1 || dataPort > 65536) {
		::wxMessageBox(_("The SDR Data Port must be between 1 and 65536"));
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

	config->Write(fileNameKey,    fileName);
	config->Write(audioAPIKey,    audioAPI);
	config->Write(audioOutDevKey, audioOutDev);
	config->Write(audioInDevKey,  audioInDev);
	config->Write(ipAddressKey,   ipAddress);
	config->Write(controlPortKey, long(controlPort));
	config->Write(dataPortKey,    long(dataPort));
	config->Flush();

#ifdef __WXMSW__
	wxString instDirKey = wxT("/InstPath");

	wxString dir;
	bool found = config->Read(instDirKey, &dir);

	if (!found) {
		::wxMessageBox(_("Cannot find the registry key for the\ninstallation directory. Cannot create\nthe start menu entry."));
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

	unsigned int i;
	for (i = 0; i < m_outDevs.size(); i++)
		if (m_outDevs.at(i) == outDev)
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
	wxASSERT(m_apiChoice != NULL);

	PaError error = ::Pa_Initialize();
	if (error != paNoError) {
		::wxMessageBox(_("Cannot initialise the sound access system."));
		return;
	}

	PaHostApiIndex n = ::Pa_GetHostApiCount();

	if (n <= 0) {
		::wxMessageBox(_("There appear to be no audio APIs\navailable on your PC!"));
		::Pa_Terminate();
		return;
	}

	int defAPI = ::Pa_GetDefaultHostApi();

	for (PaHostApiIndex i = 0; i < n; i++) {
		const PaHostApiInfo* hostAPI = ::Pa_GetHostApiInfo(i);

		m_apiChoice->Append(hostAPI->name);
	}

	m_apiChoice->SetSelection(defAPI);

	::Pa_Terminate();

	enumerateAudio(defAPI);
}

void CGUISetupFrame::enumerateAudio(int api)
{
	wxASSERT(m_devChoice != NULL);

	PaError error = ::Pa_Initialize();
	if (error != paNoError) {
		::wxMessageBox(_("Cannot initialise the sound access system."));
		return;
	}

	PaDeviceIndex n = ::Pa_GetDeviceCount();

	if (n <= 0) {
		::wxMessageBox(_("There appear to be no audio devices\nattached to your PC!"));
		::Pa_Terminate();
		return;
	}

	for (PaDeviceIndex i = 0; i < n; i++) {
		const PaDeviceInfo* device = ::Pa_GetDeviceInfo(i);

		if (device->maxInputChannels > 0 && device->hostApi == api) {
			m_devChoice->Append(device->name);
			m_inDevs.push_back(i);
		}

		if (device->maxOutputChannels > 0 && device->hostApi == api)
			m_outDevs.push_back(i);
	}

	if (n > 0)
		m_devChoice->SetSelection(0);

	::Pa_Terminate();
}

#ifdef __WXMSW__
void CGUISetupFrame::writeStartMenu(const wxString& name, const wxString& dir)
{
	TCHAR folder[MAX_PATH];
	BOOL res = ::SHGetSpecialFolderPath(NULL, folder, CSIDL_PROGRAMS, FALSE);

	if (!res) {
		::wxMessageBox(_("Cannot get the Start Menu folder from Windows"));
		return;
	}

	wxString linkName = name + wxT(".lnk");
	wxString linkPath = wxString(folder) + wxT("\\UWSDR\\") + linkName;
	wxString iconPath = dir + wxT("\\UWSDR.ico");
	wxString exePath  = dir + wxT("\\UWSDR.exe");
	wxString args = wxT("-s ") + name;

	HRESULT hRes = ::CoInitialize(NULL);
	if (!SUCCEEDED(hRes)) {
		::wxMessageBox(_("Cannot initialise the COM interface"));
		return;
	}

	IShellLink* pShellLink;
	hRes = ::CoCreateInstance(CLSID_ShellLink, NULL,
								CLSCTX_INPROC_SERVER,
								IID_IShellLink,
								(void**)&pShellLink);

	if (!SUCCEEDED(hRes)) {
		::CoUninitialize();
		::wxMessageBox(_("Cannot create a COM interface"));
		return;
	}

	pShellLink->SetPath(exePath.c_str());
	pShellLink->SetArguments(args.c_str());
/*
	if (strlen(pszDescription) > 0)
		pShellLink->SetDescription(pszDescription);

	if (iShowmode > 0)
		pShellLink->SetShowCmd(iShowmode);
*/
	pShellLink->SetWorkingDirectory(dir.c_str());
	pShellLink->SetIconLocation(iconPath.c_str(), 0);

    WORD wszLinkfile[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, linkPath.c_str(), -1, wszLinkfile, MAX_PATH);

    IPersistFile* pPersistFile;
	hRes = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);

	if (!SUCCEEDED(hRes)) {
		pShellLink->Release();
		::CoUninitialize();
		::wxMessageBox(_("Cannot query the COM interface"));
		return;
	}

	hRes = pPersistFile->Save(wszLinkfile, TRUE);
	if (!SUCCEEDED(hRes))
		::wxMessageBox(_("Cannot save the shortcut file"));

	pPersistFile->Release();
	pShellLink->Release();

	::CoUninitialize();
}

void CGUISetupFrame::writeDeskTop(const wxString& name, const wxString& dir)
{
	TCHAR folder[MAX_PATH];
	BOOL res = ::SHGetSpecialFolderPath(NULL, folder, CSIDL_DESKTOP, FALSE);

	if (!res) {
		::wxMessageBox(_("Cannot get the Desktop folder from Windows"));
		return;
	}

	wxString linkName = name + wxT(".lnk");
	wxString linkPath = wxString(folder) + wxT("\\") + linkName;
	wxString iconPath = dir + wxT("\\UWSDR.ico");
	wxString exePath  = dir + wxT("\\UWSDR.exe");
	wxString args = wxT("-s ") + name;

	HRESULT hRes = ::CoInitialize(NULL);
	if (!SUCCEEDED(hRes)) {
		::wxMessageBox(_("Cannot initialise the COM interface"));
		return;
	}

	IShellLink* pShellLink;
	hRes = ::CoCreateInstance(CLSID_ShellLink, NULL,
								CLSCTX_INPROC_SERVER,
								IID_IShellLink,
								(void**)&pShellLink);

	if (!SUCCEEDED(hRes)) {
		::CoUninitialize();
		::wxMessageBox(_("Cannot create a COM interface"));
		return;
	}

	pShellLink->SetPath(exePath.c_str());
	pShellLink->SetArguments(args.c_str());
/*
	if (strlen(pszDescription) > 0)
		pShellLink->SetDescription(pszDescription);

	if (iShowmode > 0)
		pShellLink->SetShowCmd(iShowmode);
*/
	pShellLink->SetWorkingDirectory(dir.c_str());
	pShellLink->SetIconLocation(iconPath.c_str(), 0);

    WORD wszLinkfile[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, linkPath.c_str(), -1, wszLinkfile, MAX_PATH);

    IPersistFile* pPersistFile;
	hRes = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);

	if (!SUCCEEDED(hRes)) {
		pShellLink->Release();
		::CoUninitialize();
		::wxMessageBox(_("Cannot query the COM interface"));
		return;
	}

	hRes = pPersistFile->Save(wszLinkfile, TRUE);
	if (!SUCCEEDED(hRes))
		::wxMessageBox(_("Cannot save the shortcut file"));

	pPersistFile->Release();
	pShellLink->Release();

	::CoUninitialize();
}
#endif

#ifdef __WXGTK__
void CGUISetupFrame::writeStartMenu(const wxString& name, const wxString& dir)
{
	wxString fileName = "";

	wxFile file;
	bool ret = file.Open(fileName, wxFile::write);
	if (!ret) {
		::wxMessageBox(_("Cannot open file %s for writing"), fileName);
		return;
	}

	file.Write(wxT("[Desktop Entry]\n"));
	file.Write(wxT("Type=Application\n"));
	file.Write(wxT("Version=1.0\n"));
	file.Write(wxT("Encoding=UTF-8\n"));
	file.Write(wxT("Name=") + name + wxT("\n"));
	file.Write(wxT("Comment=\n"));
	file.Write(wxT("Icon=UWSDR.png\n"));
	file.Write(wxT("Exec=") + BIN_DIR + wxT("/UWSDR -s ") + name + wxT("\n"));
	file.Write(wxT("Path=.\n"));
	file.Write(wxT("Terminal=false\n"));

	file.Flush();
	file.Close();
}
#endif

