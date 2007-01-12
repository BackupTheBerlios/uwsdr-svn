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

#include "UWSDRPreferences.h"
#include "UWSDRFrame.h"
#include "UWSDRApp.h"
#include "UWSDRDefs.h"
#include "NullController.h"
#include "UWSDRController.h"
#include "FreqKeypad.h"
#include "Version.h"
#include "NullReader.h"
#include "NullWriter.h"
#include "SignalReader.h"
#include "TwoToneReader.h"
#include "ThreeToneReader.h"
#include "SDRDataReader.h"
#include "SDRDataWriter.h"
#include "SoundCardReader.h"
#include "SoundCardWriter.h"
#include "SoundFileReader.h"
#include "SoundFileWriter.h"

#if defined(__WXGTK__) || defined(__WXMAC__)
#include "UWSDR.xpm"
#endif

#include <wx/aboutdlg.h>

enum {
	MENU_KEYPAD = 36427,
	MENU_PREFERENCES,
	MENU_CW_KEYBOARD,
	MENU_VOICE_KEYBOARD,
	MENU_RECORD,
	MENU_HARDWARE_INFO,
	MENU_ABOUT,
	MENU_EXIT
};

enum {
	MENU_BUTTON = 17856,
	VFO_AB_BUTTON,
	VFO_CD_BUTTON,
	VFO_SWAP_BUTTON,
	VFO_SPLIT_BUTTON,
	VFO_SHIFT1_BUTTON,
	VFO_SHIFT2_BUTTON,
	FREQ_KNOB,
	FREQ_MHZ1_BUTTON,
	FREQ_MHZ2_BUTTON,
	FREQ_DISPLAY,
	INFO_DISPLAY,
	FREQ_SPECTRUM,
	MODE_CHOICE,
	FILTER_CHOICE,
	RIT_BUTTON,
	MUTE_BUTTON,
	RIT_KNOB,
	TX_BUTTON,
	SMETER,
	MIC_KNOB,
	POWER_KNOB,
	VOLUME_KNOB,
	SQUELCH_KNOB,
	DISPLAY_TIMER
};

BEGIN_EVENT_TABLE(CUWSDRFrame, wxFrame)
	EVT_BUTTON(MENU_BUTTON, CUWSDRFrame::onMenuButton)
	EVT_MENU(MENU_KEYPAD, CUWSDRFrame::onMenuSelection)
	EVT_MENU(MENU_CW_KEYBOARD, CUWSDRFrame::onMenuSelection)
	EVT_MENU(MENU_VOICE_KEYBOARD, CUWSDRFrame::onMenuSelection)
	EVT_MENU(MENU_PREFERENCES, CUWSDRFrame::onMenuSelection)
	EVT_MENU(MENU_RECORD, CUWSDRFrame::onMenuSelection)
	EVT_MENU(wxID_HELP, CUWSDRFrame::onMenuSelection)
	EVT_MENU(MENU_HARDWARE_INFO, CUWSDRFrame::onMenuSelection)
	EVT_MENU(wxID_ABOUT, CUWSDRFrame::onMenuSelection)
	EVT_MENU(wxID_EXIT, CUWSDRFrame::onMenuSelection)
	EVT_BUTTON(VFO_AB_BUTTON, CUWSDRFrame::onVFOButton)
	EVT_BUTTON(VFO_CD_BUTTON, CUWSDRFrame::onVFOButton)
	EVT_BUTTON(VFO_SWAP_BUTTON, CUWSDRFrame::onVFOButton)
	EVT_BUTTON(VFO_SPLIT_BUTTON, CUWSDRFrame::onVFOButton)
	EVT_BUTTON(VFO_SHIFT1_BUTTON, CUWSDRFrame::onVFOButton)
	EVT_BUTTON(VFO_SHIFT2_BUTTON, CUWSDRFrame::onVFOButton)
	EVT_BUTTON(FREQ_MHZ1_BUTTON, CUWSDRFrame::onMHzButton)
	EVT_BUTTON(FREQ_MHZ2_BUTTON, CUWSDRFrame::onMHzButton)
	EVT_CHOICE(MODE_CHOICE, CUWSDRFrame::onModeChoice)
	EVT_CHOICE(FILTER_CHOICE, CUWSDRFrame::onFilterChoice)
	EVT_TOGGLEBUTTON(RIT_BUTTON, CUWSDRFrame::onRITButton)
	EVT_TOGGLEBUTTON(MUTE_BUTTON, CUWSDRFrame::onMuteButton)
	EVT_TOGGLEBUTTON(TX_BUTTON, CUWSDRFrame::onTXButton)
	EVT_MOUSEWHEEL(CUWSDRFrame::onMouseWheel)
	EVT_TIMER(DISPLAY_TIMER, CUWSDRFrame::onTimer)
	EVT_CLOSE(CUWSDRFrame::onClose)
END_EVENT_TABLE()


CUWSDRFrame::CUWSDRFrame(const wxString& title) :
wxFrame(NULL, -1, title),
m_timer(),
m_parameters(NULL),
m_dsp(NULL),
m_sdr(NULL),
m_rxOn(true),
m_txOn(false),
m_stepSize(0.0),
m_record(false),
m_menu(NULL),
m_swap(NULL),
m_split(NULL),
m_shift1(NULL),
m_shift2(NULL),
m_mhzMinus(NULL),
m_mhzPlus(NULL),
m_freqDisplay(NULL),
m_spectrumDisplay(NULL),
m_infoBox(NULL),
m_mode(NULL),
m_filter(NULL),
m_ritCtrl(NULL),
m_rit(NULL),
m_mute(NULL),
m_transmit(NULL),
m_sMeter(NULL),
m_micGain(NULL),
m_power(NULL),
m_afGain(NULL),
m_squelch(NULL),
m_spectrum(NULL),
m_voiceKeyboard(NULL),
m_cwKeyboard(NULL)
{
	SetIcon(wxICON(UWSDR));

	m_spectrum = new float[SPECTRUM_SIZE];

	// The top level sizer, graph, list box and then controls
	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	createMenu();

	wxPanel* panel = new wxPanel(this);

	wxBoxSizer* panelSizer = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* freqDialSizer = new wxBoxSizer(wxVERTICAL);
	freqDialSizer->Add(createMenuButton(panel), 0, wxTOP | wxLEFT | wxRIGHT, BORDER_SIZE);
	freqDialSizer->Add(createVFOButtons(panel), 0,wxALL, BORDER_SIZE);
	freqDialSizer->Add(createFreqDial(panel), 0, wxALL, BORDER_SIZE);
	freqDialSizer->Add(createMHzButtons(panel), 0, wxALL, BORDER_SIZE);
	panelSizer->Add(freqDialSizer, 0, wxLEFT | wxRIGHT, BORDER_SIZE);

	wxBoxSizer* freqSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* freqInfoSizer = new wxBoxSizer(wxHORIZONTAL);
	freqInfoSizer->Add(createFreqDisplay(panel), 0, wxTOP | wxBOTTOM, BORDER_SIZE);
	freqInfoSizer->Add(createInfoBox(panel), 0, wxTOP | wxBOTTOM, BORDER_SIZE);
	freqSizer->Add(freqInfoSizer, 0, wxBOTTOM | wxRIGHT, BORDER_SIZE);

	freqSizer->Add(createSpectrumDisplay(panel), 0, wxBOTTOM | wxRIGHT, BORDER_SIZE);

	wxFlexGridSizer* belowFreqSizer = new wxFlexGridSizer(3);

	wxGridSizer* bandModeSizer = new wxGridSizer(2, 0, 8);
	bandModeSizer->Add(createModeChoice(panel));
	bandModeSizer->Add(createRITButton(panel), 0, wxEXPAND);
	bandModeSizer->Add(createFilterChoice(panel));
	bandModeSizer->Add(createMuteButton(panel), 0, wxEXPAND);

	belowFreqSizer->Add(bandModeSizer, 0, wxEXPAND |wxLEFT | wxRIGHT, BORDER_SIZE);
	belowFreqSizer->Add(createRIT(panel), 0, wxLEFT | wxRIGHT, BORDER_SIZE);
	belowFreqSizer->Add(createTransmitButton(panel), 0, wxLEFT, BORDER_SIZE);

	freqSizer->Add(belowFreqSizer, 0, wxRIGHT | wxBOTTOM, BORDER_SIZE);

	panelSizer->Add(freqSizer, 0, wxLEFT, BORDER_SIZE);

	wxBoxSizer* sMeterKnobSizer = new wxBoxSizer(wxVERTICAL);
	sMeterKnobSizer->Add(createSMeter(panel), 0, wxBOTTOM, BORDER_SIZE);

	wxGridSizer* knobSizer = new wxGridSizer(2, 0, 3);
	knobSizer->Add(createMicGain(panel));
	knobSizer->Add(createPowerControl(panel));
	knobSizer->Add(createAFGain(panel));
	knobSizer->Add(createSquelch(panel));

	sMeterKnobSizer->Add(knobSizer);

	panelSizer->Add(sMeterKnobSizer, 0, wxALL, BORDER_SIZE);

	panel->SetSizer(panelSizer);
	panelSizer->SetSizeHints(panel);

	mainSizer->Add(panel);

	SetSizer(mainSizer);
	mainSizer->SetSizeHints(this);

	m_voiceKeyboard = new CVoiceKeyboard(this, -1);
	m_cwKeyboard    = new CCWKeyboard(this, -1);
}

CUWSDRFrame::~CUWSDRFrame()
{
	delete[] m_spectrum;
	delete   m_menu;
	delete   m_voiceKeyboard;
	delete   m_cwKeyboard;
}

void CUWSDRFrame::setParameters(CSDRParameters* parameters)
{
	wxASSERT(parameters != NULL);

	m_parameters = parameters;

#if defined(TOBIAS) || defined(GRANT_TX)
	m_sdr = new CNullController();
#else
	switch (m_parameters->m_hardwareType) {
		case TYPE_UWSDR1:
			m_sdr = new CUWSDRController(m_parameters->m_ipAddress, m_parameters->m_controlPort, 1);
			break;
		default:
			m_sdr = new CNullController();
			break;
	}
#endif
	m_sdr->setCallback(this, -1);

	bool ret = m_sdr->open();
	if (!ret) {
		::wxLogError(wxT("Problems communicating with the SDR"));
		::wxMessageBox(_("Problems communicating with the SDR"), _("uWave SDR Error"), wxICON_ERROR);
		Close(true);
		return;
	}

	if (!m_parameters->m_hardwareReceiveOnly)
		m_sdr->enableTX(true);
	m_sdr->enableRX(true);

	m_spectrumDisplay->setSampleRate(m_parameters->m_hardwareSampleRate);
	m_spectrumDisplay->setPosition(m_parameters->m_spectrumPos);
	m_spectrumDisplay->setType(m_parameters->m_spectrumType);
	m_spectrumDisplay->setSpeed(m_parameters->m_spectrumSpeed);

	// Set the spectrum width depending on the step size and sample rate,
	float lowFreq = float(m_parameters->m_hardwareSampleRate) / 4.0F - m_parameters->m_hardwareStepSize / 2.0F;
	wxASSERT(lowFreq > 0.0F);

	if (lowFreq >= 20000.0F)
		m_spectrumDisplay->setBandwidth(40000.0F);
	else if (lowFreq >= 15000.0F)
		m_spectrumDisplay->setBandwidth(30000.0F);
	else if (lowFreq >= 12500.0F)
		m_spectrumDisplay->setBandwidth(25000.0F);
	else if (lowFreq >= 10000.0F)
		m_spectrumDisplay->setBandwidth(20000.0F);
	else if (lowFreq >= 7500.0F)
		m_spectrumDisplay->setBandwidth(15000.0F);
	else if (lowFreq >= 5000.0F)
		m_spectrumDisplay->setBandwidth(10000.0F);
	else
		m_spectrumDisplay->setBandwidth(5000.0F);

	m_dsp = new CDSPControl(m_parameters->m_hardwareSampleRate);

// FIXME
#if defined(GRANT_TX)
	// RX is disabled, TX is from audio card for signal output fed by a two-tone signal
	m_dsp->setTXReader(new CTwoToneReader(1000.0F, 1300.0F, 0.4F, new CSoundCardReader(m_parameters->m_userAudioAPI, m_parameters->m_userAudioInDev)));
	m_dsp->setTXWriter(new CSoundCardWriter(m_parameters->m_sdrAudioAPI, m_parameters->m_sdrAudioOutDev));
	m_dsp->setRXReader(new CNullReader());
	m_dsp->setRXWriter(new CNullWriter());
#elif defined(TOBIAS)
	// UDP in/out with audio on loudspeaker and two-tone audio on transmit
	m_dsp->setTXReader(new CTwoToneReader(1000.0F, 1300.0F, 0.4F, new CSoundCardReader(m_parameters->m_userAudioAPI, m_parameters->m_userAudioInDev)));
	m_dsp->setTXWriter(new CSDRDataWriter(m_parameters->m_ipAddress, m_parameters->m_dataPort, 1));
	m_dsp->setRXReader(new CSDRDataReader(m_parameters->m_ipAddress, m_parameters->m_dataPort, 1));
	m_dsp->setRXWriter(new CSoundCardWriter(m_parameters->m_userAudioAPI, m_parameters->m_userAudioOutDev));
#else
	switch (m_parameters->m_hardwareType) {
		case TYPE_AUDIORX:
			// TX is disabled, RX is from audio card for signal input and audio output
			m_dsp->setTXReader(new CNullReader());
			m_dsp->setTXWriter(new CNullWriter());
			m_dsp->setRXReader(new CSoundCardReader(m_parameters->m_sdrAudioAPI, m_parameters->m_sdrAudioInDev));
			// m_dsp->setRXReader(new CSignalReader(int(m_parameters->m_hardwareSampleRate / 4.0F + 1000.5F), 0.0003F, 0.0004F, new CSoundCardReader(m_parameters->m_sdrAudioAPI, m_parameters->m_sdrAudioInDev)));
			m_dsp->setRXWriter(new CSoundCardWriter(m_parameters->m_userAudioAPI, m_parameters->m_userAudioOutDev));
			break;

		case TYPE_DEMO:
			// A self contained variant for demo's and testing
			// m_dsp->setTXReader(new CTwoToneReader(1000.0F, 1300.0F, 0.4F, new CSoundCardReader(m_parameters->m_userAudioAPI, m_parameters->m_userAudioInDev)));
			m_dsp->setTXReader(new CThreeToneReader(500.0F, 1500.0F, 2000.0F, 0.25F, new CSoundCardReader(m_parameters->m_userAudioAPI, m_parameters->m_userAudioInDev)));
			m_dsp->setTXWriter(new CNullWriter());
			m_dsp->setRXReader(new CSignalReader(int(m_parameters->m_hardwareSampleRate / 4.0F + 1000.5F), 0.0003F, 0.0004F));
			m_dsp->setRXWriter(new CSoundCardWriter(m_parameters->m_userAudioAPI, m_parameters->m_userAudioOutDev));
			break;

		case TYPE_UWSDR1:
			// The standard configuration, UDP in/out and sound card for the user
			m_dsp->setTXReader(new CSoundCardReader(m_parameters->m_userAudioAPI, m_parameters->m_userAudioInDev));
			m_dsp->setTXWriter(new CSDRDataWriter(m_parameters->m_ipAddress, m_parameters->m_dataPort, 1));
			m_dsp->setRXReader(new CSDRDataReader(m_parameters->m_ipAddress, m_parameters->m_dataPort, 1));
			m_dsp->setRXWriter(new CSoundCardWriter(m_parameters->m_userAudioAPI, m_parameters->m_userAudioOutDev));
			break;
	}
#endif

	m_infoBox->setVFO(m_parameters->m_vfoChoice);
	m_infoBox->setSplitShift(m_parameters->m_vfoSplitShift);
	m_infoBox->setRIT(m_parameters->m_ritOn);

	m_mode->SetSelection(m_parameters->m_mode);

	m_filter->SetSelection(m_parameters->m_filter);

	m_ritCtrl->SetValue(m_parameters->m_ritOn);
	m_ritCtrl->SetLabel(m_parameters->m_ritOn ? _("Off") : _("On"));
	m_rit->setValue(m_parameters->m_ritFreq);

	m_micGain->setValue(m_parameters->m_micGain);
	m_dsp->setMicGain(m_parameters->m_micGain);

	m_power->setValue(m_parameters->m_power);
	m_dsp->setPower(m_parameters->m_power);

	m_afGain->setValue(m_parameters->m_afGain);
	m_dsp->setAFGain(m_parameters->m_afGain);
	m_dsp->setRFGain(m_parameters->m_rfGain);

	m_squelch->setValue(m_parameters->m_squelch);
	m_dsp->setSquelch(m_parameters->m_squelch);

	m_dsp->setNB(m_parameters->m_nbOn);
	m_dsp->setNBValue(m_parameters->m_nbValue);
	m_dsp->setNB2(m_parameters->m_nb2On);
	m_dsp->setNB2Value(m_parameters->m_nb2Value);

	m_dsp->setSP(m_parameters->m_spOn);
	m_dsp->setSPValue(m_parameters->m_spValue);

	m_dsp->setCarrierLevel(m_parameters->m_carrierLevel);

	m_dsp->setALCValue(m_parameters->m_alcAttack, m_parameters->m_alcDecay, m_parameters->m_alcHang);

	m_dsp->setRXIAndQ(m_parameters->m_rxIQphase, m_parameters->m_rxIQgain);
	m_dsp->setTXIAndQ(m_parameters->m_txIQphase, m_parameters->m_txIQgain);

	m_dsp->swapIQ(m_parameters->m_swapIQ);

	m_sMeter->setRXMeter(m_parameters->m_rxMeter);
	m_sMeter->setTXMeter(m_parameters->m_txMeter);

	m_cwKeyboard->setSpeed(m_parameters->m_cwSpeed);
	m_cwKeyboard->setLocal(m_parameters->m_cwLocal);
	m_cwKeyboard->setRemote(m_parameters->m_cwRemote);
	m_cwKeyboard->setLocator(m_parameters->m_cwLocator);
	m_cwKeyboard->setReport(m_parameters->m_cwReport);
	m_cwKeyboard->setSerial(m_parameters->m_cwSerial);
	for (int i = 0; i < CWKEYBOARD_COUNT; i++)
		m_cwKeyboard->setMessage(i, m_parameters->m_cwMessage[i]);

	m_voiceKeyboard->setDir(m_parameters->m_voiceDir);
	for (int j = 0; j < VOICEKEYER_COUNT; j++)
		m_voiceKeyboard->setFile(j, m_parameters->m_voiceFile[j]);

	m_timer.SetOwner(this, DISPLAY_TIMER);
	m_timer.Start(100);

	// If receive only disable/remove transmit possibilities
	if (m_parameters->m_hardwareReceiveOnly) {
		m_swap->Disable();
		m_split->Disable();
		m_shift1->Disable();
		m_shift2->Disable();
		m_mhzMinus->Disable();
		m_mhzPlus->Disable();
		m_ritCtrl->Disable();
		m_mute->Disable();
		m_rit->Disable();
		m_transmit->Disable();
		m_micGain->Disable();
		m_power->Disable();
		m_sMeter->setTXMenu(false);
		m_menu->Enable(MENU_VOICE_KEYBOARD, false);
		m_menu->Enable(MENU_CW_KEYBOARD, false);
	}

	normaliseMode();

	// Must be after normaliseMode
	m_dsp->setZeroIF(m_parameters->m_zeroIF);

	normaliseFreq();

	ret = m_dsp->open();
	if (!ret) {
		::wxLogError(wxT("Problems opening the I/O ports"));
		::wxMessageBox(_("Problems opening the input/output ports."), _("uWave SDR Error"), wxICON_ERROR);
		Close(true);
		return;
	}
}

CSDRParameters* CUWSDRFrame::getParameters()
{
	return m_parameters;
}

void CUWSDRFrame::createMenu()
{
	m_menu = new wxMenu();

	m_menu->Append(MENU_KEYPAD,          _("Frequency Keypad..."));
	m_menu->Append(MENU_PREFERENCES,     _("Preferences..."));
	m_menu->Append(MENU_CW_KEYBOARD,     _("CW Keyboard..."));
	m_menu->Append(MENU_VOICE_KEYBOARD,  _("Voice Keyer..."));
	m_menu->AppendCheckItem(MENU_RECORD, _("Record"));
	m_menu->AppendSeparator();
	m_menu->Append(wxID_HELP,            _("Help\tF1"));
	m_menu->Append(MENU_HARDWARE_INFO,   _("Hardware Info"));
	m_menu->Append(wxID_ABOUT,           _("About uWave SDR"));
	m_menu->AppendSeparator();
	m_menu->Append(wxID_EXIT,            _("Exit\tALT-F4"));
}

wxButton* CUWSDRFrame::createMenuButton(wxWindow* window)
{
	return new wxButton(window, MENU_BUTTON, _("Menu"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH, BUTTON_HEIGHT));
}

wxSizer* CUWSDRFrame::createVFOButtons(wxWindow* window)
{
	wxSizer* grid = new wxGridSizer(2);

	wxButton* vfoA = new wxButton(window, VFO_AB_BUTTON, _("VFO A/B"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(vfoA);

	wxButton* vfoB = new wxButton(window, VFO_CD_BUTTON, _("VFO C/D"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(vfoB);

	m_swap = new wxButton(window, VFO_SWAP_BUTTON, _("SWAP"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(m_swap);

	m_split = new wxButton(window, VFO_SPLIT_BUTTON, _("SPLIT"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(m_split);

	m_shift1 = new wxButton(window, VFO_SHIFT1_BUTTON, _("SHIFT -"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(m_shift1);

	m_shift2 = new wxButton(window, VFO_SHIFT2_BUTTON, _("SHIFT +"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(m_shift2);

	grid->SetSizeHints(window);

	return grid;
}

wxPanel* CUWSDRFrame::createFreqDial(wxWindow* window)
{
	CFreqDial* freqDial = new CFreqDial(window, FREQ_KNOB, this, wxDefaultPosition, wxSize(FREQDIAL_WIDTH, FREQDIAL_HEIGHT));

	return freqDial;
}

wxSizer* CUWSDRFrame::createMHzButtons(wxWindow* window)
{
	wxSizer* grid = new wxGridSizer(2);

	m_mhzMinus = new wxButton(window, FREQ_MHZ1_BUTTON, _("MHz -"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(m_mhzMinus);

	m_mhzPlus = new wxButton(window, FREQ_MHZ2_BUTTON, _("MHz +"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH / 2, BUTTON_HEIGHT));
	grid->Add(m_mhzPlus);

	grid->SetSizeHints(window);

	return grid;
}

wxPanel* CUWSDRFrame::createFreqDisplay(wxWindow* window)
{
	m_freqDisplay = new CFreqDisplay(window, FREQ_DISPLAY, wxDefaultPosition, wxSize(FREQUENCY_WIDTH, FREQUENCY_HEIGHT));

	normaliseFreq();

	return m_freqDisplay;
}

wxPanel* CUWSDRFrame::createSpectrumDisplay(wxWindow* window)
{
	m_spectrumDisplay = new CSpectrumDisplay(window, FREQ_SPECTRUM, wxDefaultPosition, wxSize(SPECTRUM_WIDTH, SPECTRUM_HEIGHT));

	return m_spectrumDisplay;
}

wxSizer* CUWSDRFrame::createModeChoice(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Mode")), wxVERTICAL);

	m_mode = new wxChoice(window, MODE_CHOICE, wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	m_mode->Append(_("CW Narrow"));
	m_mode->Append(_("CW Wide"));
	m_mode->Append(_("USB"));
	m_mode->Append(_("LSB"));
	m_mode->Append(_("AM"));
	m_mode->Append(_("FM Narrow"));
	m_mode->Append(_("FM Wide"));

	sizer->Add(m_mode, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createFilterChoice(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Filter")), wxVERTICAL);

	m_filter = new wxChoice(window, FILTER_CHOICE, wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	m_filter->Append(wxT("20.0 kHz"));
	m_filter->Append(wxT("15.0 kHz"));
	m_filter->Append(wxT("10.0 kHz"));
	m_filter->Append(wxT("6.0 kHz"));
	m_filter->Append(wxT("4.0 kHz"));
	m_filter->Append(wxT("2.6 kHz"));
	m_filter->Append(wxT("2.1 kHz"));
	m_filter->Append(wxT("1.0 kHz"));
	m_filter->Append(wxT("500 Hz"));
	m_filter->Append(wxT("250 Hz"));
	m_filter->Append(wxT("100 Hz"));
	m_filter->Append(wxT("50 Hz"));
	m_filter->Append(wxT("25 Hz"));
	m_filter->Append(_("Auto"));

	sizer->Add(m_filter, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createRITButton(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("RIT")), wxVERTICAL);

	m_ritCtrl = new wxToggleButton(window, RIT_BUTTON, _("On"), wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	sizer->Add(m_ritCtrl, 0, wxLEFT | wxRIGHT | wxBOTTOM, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createMuteButton(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Mute")), wxVERTICAL);

	m_mute = new wxToggleButton(window, MUTE_BUTTON, _("On"), wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	sizer->Add(m_mute, 0, wxLEFT | wxRIGHT | wxBOTTOM, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createTransmitButton(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Transmit")), wxVERTICAL);

	m_transmit = new wxToggleButton(window, TX_BUTTON, _("On"), wxDefaultPosition, wxSize(TXBUTTON_WIDTH, TXBUTTON_HEIGHT));

	sizer->Add(m_transmit, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createRIT(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("RIT")), wxVERTICAL);

	m_rit = new CVolumeDial(window, RIT_KNOB, -5000, 5000, 0, this, wxDefaultPosition, wxSize(VOLSQL_WIDTH, VOLSQL_HEIGHT));

	sizer->Add(m_rit, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxPanel* CUWSDRFrame::createInfoBox(wxWindow* window)
{
	m_infoBox = new CInfoBox(window, INFO_DISPLAY, wxDefaultPosition, wxSize(INFO_WIDTH, INFO_HEIGHT));
	
	return m_infoBox;
}

wxPanel* CUWSDRFrame::createSMeter(wxWindow* window)
{
	m_sMeter = new CSMeter(window, SMETER, wxDefaultPosition, wxSize(SMETER_WIDTH, SMETER_HEIGHT));
	m_sMeter->setLevel(0);
	
	return m_sMeter;
}

wxSizer* CUWSDRFrame::createMicGain(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Mic Gain")), wxVERTICAL);

	m_micGain = new CVolumeDial(window, MIC_KNOB, 0, 1000, 0, this, wxDefaultPosition, wxSize(VOLSQL_WIDTH, VOLSQL_HEIGHT));

	sizer->Add(m_micGain, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createPowerControl(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Power")), wxVERTICAL);

	m_power = new CVolumeDial(window, POWER_KNOB, 0, 1000, 0, this, wxDefaultPosition, wxSize(VOLSQL_WIDTH, VOLSQL_HEIGHT));

	sizer->Add(m_power, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createAFGain(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("AF Gain")), wxVERTICAL);

	m_afGain = new CVolumeDial(window, VOLUME_KNOB, 0, 1000, 0, this, wxDefaultPosition, wxSize(VOLSQL_WIDTH, VOLSQL_HEIGHT));

	sizer->Add(m_afGain, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createSquelch(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Squelch")), wxVERTICAL);

	m_squelch = new CVolumeDial(window, SQUELCH_KNOB, 0, 1000, 0, this, wxDefaultPosition, wxSize(VOLSQL_WIDTH, VOLSQL_HEIGHT));

	sizer->Add(m_squelch, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

void CUWSDRFrame::onMouseWheel(wxMouseEvent& event)
{
	int movement = event.GetWheelRotation();

	if (movement > 0)
		freqChange(2.0);
	else
		freqChange(-2.0);
}

void CUWSDRFrame::dialMoved(int id, int value)
{
	switch (id) {
		case FREQ_KNOB:
			freqChange(double(value) * m_stepSize);
			break;
		case RIT_KNOB:
			m_parameters->m_ritFreq = value;
			normaliseFreq();
			break;
		case MIC_KNOB:
			m_parameters->m_micGain = value;
			m_dsp->setMicGain(value);
			break;
		case POWER_KNOB:
			m_parameters->m_power = value;
			m_dsp->setPower(value);
			break;
		case VOLUME_KNOB:
			m_parameters->m_afGain = value;
			m_dsp->setAFGain(value);
			break;
		case SQUELCH_KNOB:
			m_parameters->m_squelch = value;
			m_dsp->setSquelch(value);
			break;
	}
}

void CUWSDRFrame::freqChange(double value)
{
	CFrequency freq;

	if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoB + value;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoA + value;
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoD + value;
	else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoC + value;

	else if (m_parameters->m_vfoChoice == VFO_A)
		freq = m_parameters->m_vfoA + value;
	else if (m_parameters->m_vfoChoice == VFO_B)
		freq = m_parameters->m_vfoB + value;
	else if (m_parameters->m_vfoChoice == VFO_C)
		freq = m_parameters->m_vfoC + value;
	else if (m_parameters->m_vfoChoice == VFO_D)
		freq = m_parameters->m_vfoD + value;

	if (!m_txOn) {
		if (freq >= m_parameters->m_maxReceiveFreq)
			freq = (freq - m_parameters->m_maxReceiveFreq) + m_parameters->m_minReceiveFreq;

		if (freq < m_parameters->m_minReceiveFreq)
			freq = (freq - m_parameters->m_minReceiveFreq) + m_parameters->m_maxReceiveFreq;
	} else {
		if (freq >= m_parameters->m_maxTransmitFreq)
			freq = (freq - m_parameters->m_maxTransmitFreq) + m_parameters->m_minTransmitFreq;

		if (freq < m_parameters->m_minTransmitFreq)
			freq = (freq - m_parameters->m_minTransmitFreq) + m_parameters->m_maxTransmitFreq;
	}

	if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_parameters->m_vfoB = freq;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_parameters->m_vfoA = freq;
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_parameters->m_vfoD = freq;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_parameters->m_vfoC = freq;

	else if (m_parameters->m_vfoChoice == VFO_A)
		m_parameters->m_vfoA = freq;
	else if (m_parameters->m_vfoChoice == VFO_B)
		m_parameters->m_vfoB = freq;
	else if (m_parameters->m_vfoChoice == VFO_C)
		m_parameters->m_vfoC = freq;
	else if (m_parameters->m_vfoChoice == VFO_D)
		m_parameters->m_vfoD = freq;

	normaliseFreq();
}

void CUWSDRFrame::onMenuButton(wxCommandEvent& event)
{
	m_menu->Check(MENU_RECORD, m_record);

	PopupMenu(m_menu, 0, 30);
}

void CUWSDRFrame::onVFOButton(wxCommandEvent& event)
{
	if (m_txOn)
		return;

	switch (event.GetId()) {
		case VFO_AB_BUTTON:
			if (m_parameters->m_vfoChoice == VFO_A) {
				m_parameters->m_vfoChoice = VFO_B;
				m_infoBox->setVFO(VFO_B);
			} else {
				m_parameters->m_vfoChoice = VFO_A;
				m_infoBox->setVFO(VFO_A);
			}
			normaliseFreq();
			break;
		case VFO_CD_BUTTON:
			if (m_parameters->m_vfoChoice == VFO_C) {
				m_parameters->m_vfoChoice = VFO_D;
				m_infoBox->setVFO(VFO_D);
			} else {
				m_parameters->m_vfoChoice = VFO_C;
				m_infoBox->setVFO(VFO_C);
			}
			normaliseFreq();
			break;
		case VFO_SWAP_BUTTON:
			if (m_parameters->m_vfoChoice == VFO_A ||
				m_parameters->m_vfoChoice == VFO_B) {
				CFrequency temp = m_parameters->m_vfoA;
				m_parameters->m_vfoA = m_parameters->m_vfoB;
				m_parameters->m_vfoB = temp;
			} else {
				CFrequency temp = m_parameters->m_vfoC;
				m_parameters->m_vfoC = m_parameters->m_vfoD;
				m_parameters->m_vfoD = temp;
			}
			normaliseFreq();
			break;
		case VFO_SPLIT_BUTTON:
			if (m_parameters->m_vfoSplitShift == VFO_SPLIT)
				m_parameters->m_vfoSplitShift = VFO_NONE;
			else
				m_parameters->m_vfoSplitShift = VFO_SPLIT;
			m_infoBox->setSplitShift(m_parameters->m_vfoSplitShift);
			break;
		case VFO_SHIFT1_BUTTON:
			if (m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
				m_parameters->m_vfoSplitShift = VFO_NONE;
			else
				m_parameters->m_vfoSplitShift = VFO_SHIFT_1;
			m_infoBox->setSplitShift(m_parameters->m_vfoSplitShift);
			break;
		case VFO_SHIFT2_BUTTON:
			if (m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
				m_parameters->m_vfoSplitShift = VFO_NONE;
			else
				m_parameters->m_vfoSplitShift = VFO_SHIFT_2;
			m_infoBox->setSplitShift(m_parameters->m_vfoSplitShift);
			break;
	}
}

void CUWSDRFrame::onMHzButton(wxCommandEvent& event)
{
	if (m_txOn)
		return;

	switch (event.GetId()) {
		case FREQ_MHZ1_BUTTON:
			freqChange(-1000000.0);
			break;
		case FREQ_MHZ2_BUTTON:
			freqChange(1000000.0);
			break;
	}
}

void CUWSDRFrame::onModeChoice(wxCommandEvent& event)
{
	m_parameters->m_mode = int(event.GetSelection());

	normaliseMode();

	normaliseFreq();
}

void CUWSDRFrame::onFilterChoice(wxCommandEvent& event)
{
	m_parameters->m_filter = int(event.GetSelection());

	normaliseMode();

	normaliseFreq();
}

void CUWSDRFrame::onRITButton(wxCommandEvent& event)
{
	m_parameters->m_ritOn = !m_parameters->m_ritOn;

	m_infoBox->setRIT(m_parameters->m_ritOn);

	m_ritCtrl->SetLabel(m_parameters->m_ritOn ? _("Off") : _("On"));

	if (m_txOn)
		return;

	normaliseFreq();
}

void CUWSDRFrame::onMuteButton(wxCommandEvent& event)
{
	m_rxOn = !m_rxOn;

	m_mute->SetLabel(m_rxOn ? _("On") : _("Off"));

	m_sdr->enableRX(m_rxOn);
}

void CUWSDRFrame::onTXButton(wxCommandEvent& event)
{
	// If we're not in CW mode, go to TX/RX
	if (m_parameters->m_mode != MODE_CWW && m_parameters->m_mode != MODE_CWN) {
		setTransmit();
	} else {
		// Else is in CW mode we can use the main Transmit button to abort a CW
		// transmission, but not start one.
		if (m_txOn)
			::wxGetApp().sendCW(0, wxEmptyString);
	}

	m_transmit->SetValue(m_txOn);
	m_transmit->SetLabel(m_txOn ? _("Off") : _("On"));
}

bool CUWSDRFrame::setTransmit()
{
	if (!m_txOn && m_parameters->m_hardwareReceiveOnly) {
		::wxMessageBox(_("This SDR is only a receiver!"), _("uWave SDR Error"), wxICON_ERROR);
		return false;
	}

	// Sanity check on the transmit frequency
	if (!m_txOn) {
		CFrequency freq;
		if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_NONE)
			freq = m_parameters->m_vfoA;
		else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_NONE)
			freq = m_parameters->m_vfoB;
		else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_NONE)
			freq = m_parameters->m_vfoC;
		else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_NONE)
			freq = m_parameters->m_vfoD;

		else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SPLIT)
			freq = m_parameters->m_vfoB;
		else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SPLIT)
			freq = m_parameters->m_vfoA;
		else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SPLIT)
			freq = m_parameters->m_vfoD;
		else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SPLIT)
			freq = m_parameters->m_vfoC;

		else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
			freq = m_parameters->m_vfoA - m_parameters->m_freqShift;
		else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoA + m_parameters->m_freqShift;
		else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
			freq = m_parameters->m_vfoB - m_parameters->m_freqShift;
		else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoB + m_parameters->m_freqShift;
		else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
			freq = m_parameters->m_vfoC - m_parameters->m_freqShift;
		else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoC + m_parameters->m_freqShift;
		else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
			freq = m_parameters->m_vfoD - m_parameters->m_freqShift;
		else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoD + m_parameters->m_freqShift;

		if (freq >= m_parameters->m_maxTransmitFreq || freq < m_parameters->m_minTransmitFreq) {
			::wxBell();
			return false;
		}
	}

	m_txOn = !m_txOn;

	if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_infoBox->setVFO(VFO_B);
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_infoBox->setVFO(VFO_A);
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_infoBox->setVFO(VFO_D);
	else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		m_infoBox->setVFO(VFO_C);
	else
		m_infoBox->setVFO(m_parameters->m_vfoChoice);

	m_infoBox->setTX(m_txOn);

	normaliseFreq();

	return true;
}

void CUWSDRFrame::normaliseFreq()
{
	// We can be called too early ...
	if (m_parameters == NULL)
		return;

	CFrequency freq;

	if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_NONE)
		freq = m_parameters->m_vfoA;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_NONE)
		freq = m_parameters->m_vfoB;
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_NONE)
		freq = m_parameters->m_vfoC;
	else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_NONE)
		freq = m_parameters->m_vfoD;

	else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
		freq = m_parameters->m_vfoA;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
		freq = m_parameters->m_vfoB;
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
		freq = m_parameters->m_vfoC;
	else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
		freq = m_parameters->m_vfoD;

	else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
		freq = m_parameters->m_vfoA;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
		freq = m_parameters->m_vfoB;
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
		freq = m_parameters->m_vfoC;
	else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
		freq = m_parameters->m_vfoD;

	else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SPLIT && !m_txOn)
		freq = m_parameters->m_vfoA;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SPLIT && !m_txOn)
		freq = m_parameters->m_vfoB;
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SPLIT && !m_txOn)
		freq = m_parameters->m_vfoC;
	else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SPLIT && !m_txOn)
		freq = m_parameters->m_vfoD;

	else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoB;
	else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoA;
	else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoD;
	else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SPLIT && m_txOn)
		freq = m_parameters->m_vfoC;

	if (m_txOn && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
		freq -= m_parameters->m_freqShift;
	if (m_txOn && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
		freq += m_parameters->m_freqShift;

	// Set the RIT
	if (m_parameters->m_ritOn && !m_txOn)
		freq += double(m_parameters->m_ritFreq);

	CFrequency dispFreq = freq;

	// Adjust the display ONLY frequency
	if (m_parameters->m_mode == MODE_CWW || m_parameters->m_mode == MODE_CWN)
		dispFreq += CW_OFFSET;

	if (m_parameters->m_freqOffset != 0.0)
		dispFreq += m_parameters->m_freqOffset;

	m_freqDisplay->setFrequency(dispFreq);

	// Subtract the IF frequency
	float dspOffset;
	if (m_parameters->m_zeroIF) {
		if (m_txOn)
			dspOffset = m_dsp->getTXOffset();
		else
			dspOffset = m_dsp->getRXOffset();
	} else {
		dspOffset = m_parameters->m_hardwareSampleRate / 4.0F;
	}

	freq -= dspOffset;

	if (m_parameters->m_hardwareType == TYPE_AUDIORX) {
		CFrequency diff = freq - m_parameters->m_hardwareMinFreq;

		// Finally go to TX or RX
		m_dsp->setTXAndFreq(m_txOn, diff.getHz());
	} else {
		// Take into account the frequency steps of the SDR ...
		double offset = 0.0;
		if (m_parameters->m_hardwareStepSize > 1.0F) {		// FIXME XXX
			double stepSize = m_parameters->m_hardwareStepSize;
			double hz       = freq.getHz();

			offset = ::fmod(hz, stepSize);

			double base = hz - offset;

			if (offset >= stepSize / 2.0) {
				offset -= stepSize;
				base   += stepSize;
			}

			freq.setHz(base);
		}

		// Finally go to TX or RX
		m_sdr->setTXAndFreq(m_txOn, freq);
		m_dsp->setTXAndFreq(m_txOn, offset);
	}
}

void CUWSDRFrame::normaliseMode()
{
	// We can be called too early
	if (m_parameters == NULL)
		return;

	m_dsp->setMode(m_parameters->m_mode);

	int speed  = -1;
	int filter = m_parameters->m_filter;

	switch (m_parameters->m_mode) {
		case MODE_FMW:
			if (filter == FILTER_AUTO)
				filter = m_parameters->m_filterFMW;
			m_dsp->setDeviation(m_parameters->m_deviationFMW);
			speed = m_parameters->m_vfoSpeedFM;
			break;
		case MODE_FMN:
			if (filter == FILTER_AUTO)
				filter = m_parameters->m_filterFMN;
			m_dsp->setDeviation(m_parameters->m_deviationFMN);
			speed = m_parameters->m_vfoSpeedFM;
			break;
		case MODE_AM:
			if (filter == FILTER_AUTO)
				filter = m_parameters->m_filterAM;
			m_dsp->setAGC(m_parameters->m_agcAM);
			speed = m_parameters->m_vfoSpeedAM;
			break;
		case MODE_USB:
		case MODE_LSB:
			if (filter == FILTER_AUTO)
				filter = m_parameters->m_filterSSB;
			m_dsp->setAGC(m_parameters->m_agcSSB);
			speed = m_parameters->m_vfoSpeedSSB;
			break;
		case MODE_CWW:
			if (filter == FILTER_AUTO)
				filter = m_parameters->m_filterCWW;
			m_dsp->setAGC(m_parameters->m_agcCW);
			speed = m_parameters->m_vfoSpeedCWW;
			break;
		case MODE_CWN:
			if (filter == FILTER_AUTO)
				filter = m_parameters->m_filterCWN;
			m_dsp->setAGC(m_parameters->m_agcCW);
			speed = m_parameters->m_vfoSpeedCWN;
			break;
		default:
			break;
	}

	m_dsp->setFilter(filter);

	switch (speed) {
		case SPEED_VERYFAST:
			m_stepSize = m_parameters->m_stepVeryFast;
			break;
		case SPEED_FAST:
			m_stepSize = m_parameters->m_stepFast;
			break;
		case SPEED_MEDIUM:
			m_stepSize = m_parameters->m_stepMedium;
			break;
		case SPEED_SLOW:
			m_stepSize = m_parameters->m_stepSlow;
			break;
		case SPEED_VERYSLOW:
			m_stepSize = m_parameters->m_stepVerySlow;
			break;
	}
}

void CUWSDRFrame::onMenuSelection(wxCommandEvent& event)
{
	switch (event.GetId()) {
		case MENU_PREFERENCES: {
				CUWSDRPreferences preferences(this, -1, m_parameters, m_dsp);
				int reply = preferences.ShowModal();
				if (reply == wxID_OK) {
					normaliseMode();

					m_dsp->setNB(m_parameters->m_nbOn);
					m_dsp->setNBValue(m_parameters->m_nbValue);
					m_dsp->setNB2(m_parameters->m_nb2On);
					m_dsp->setNB2Value(m_parameters->m_nb2Value);

					m_dsp->setSP(m_parameters->m_spOn);
					m_dsp->setSPValue(m_parameters->m_spValue);
					m_dsp->setCarrierLevel(m_parameters->m_carrierLevel);

					m_dsp->setALCValue(m_parameters->m_alcAttack, m_parameters->m_alcDecay, m_parameters->m_alcHang);

					m_dsp->setZeroIF(m_parameters->m_zeroIF);
					m_dsp->swapIQ(m_parameters->m_swapIQ);

					normaliseFreq();
				}

				// These may have been set in the preferences and then cancel pressed
				// so reset them to the saved values
				m_dsp->setRXIAndQ(m_parameters->m_rxIQphase, m_parameters->m_rxIQgain);
				m_dsp->setTXIAndQ(m_parameters->m_txIQphase, m_parameters->m_txIQgain);
				m_dsp->setRFGain(m_parameters->m_rfGain);
			}
			break;
		case MENU_KEYPAD: {
				if (m_txOn)
					return;

				CFrequency freq;
				if (m_parameters->m_vfoChoice == VFO_A)
					freq = m_parameters->m_vfoA;
				else if (m_parameters->m_vfoChoice == VFO_B)
					freq = m_parameters->m_vfoB;
				else if (m_parameters->m_vfoChoice == VFO_C)
					freq = m_parameters->m_vfoC;
				else if (m_parameters->m_vfoChoice == VFO_D)
					freq = m_parameters->m_vfoD;

				CFreqKeypad keypad(this, -1, freq, m_parameters->m_minReceiveFreq, m_parameters->m_maxReceiveFreq);
				int reply = keypad.ShowModal();

				if (reply == wxID_OK) {
					CFrequency freq = keypad.getFrequency();

					if (m_parameters->m_vfoChoice == VFO_A)
						m_parameters->m_vfoA = freq;
					else if (m_parameters->m_vfoChoice == VFO_B)
						m_parameters->m_vfoB = freq;
					else if (m_parameters->m_vfoChoice == VFO_C)
						m_parameters->m_vfoC = freq;
					else if (m_parameters->m_vfoChoice == VFO_D)
						m_parameters->m_vfoD = freq;

					normaliseFreq();
				}
			}
			break;
		case MENU_VOICE_KEYBOARD:
			m_voiceKeyboard->Show(true);
			break;
		case MENU_CW_KEYBOARD:
			m_cwKeyboard->Show(true);
			break;
		case MENU_RECORD: {
				m_record = !m_record;
				bool ret = m_dsp->setRecord(m_record);
				if (!ret) {
					m_record = false;
					::wxMessageBox(_("Cannot open the sound file for recording"), _("uWave SDR Error"), wxICON_ERROR);
				}
			}
			break;
		case wxID_HELP:
			::wxGetApp().showHelp(1);
			break;
		case MENU_HARDWARE_INFO: {
				wxString stepSize;
				stepSize.Printf(wxT("%.1f"), m_parameters->m_hardwareStepSize);

				wxString sampleRate;
				sampleRate.Printf(wxT("%.0f"), m_parameters->m_hardwareSampleRate);

				wxString type;
				switch (m_parameters->m_hardwareType) {
					case TYPE_AUDIORX:
						type = wxT("Audio RX");
						break;
					case TYPE_DEMO:
						type = wxT("Demo");
						break;
					case TYPE_UWSDR1:
						type = wxT("UWSDR v1.0");
						break;
				}

				wxString transmit = (m_parameters->m_hardwareReceiveOnly) ? _("No") : _("Yes");

				::wxMessageBox(_("The hardware parameters are:\n\n"
					"Name:\t\t") + m_parameters->m_hardwareName + _("\n"
					"Max. Freq:\t") + m_parameters->m_hardwareMaxFreq.getString(3) + _(" MHz\n"
					"Min. Freq:\t") + m_parameters->m_hardwareMinFreq.getString(3) + _(" MHz\n"
					"Step Size:\t") + stepSize + _(" Hz\n"
					"Sample Rate:\t") + sampleRate + _(" samples/sec\n"
					"Type:\t\t") + type + _("\n"
					"Transmit:\t\t") + transmit,
					_("SDR Hardware Information"),
					wxICON_INFORMATION);
			}
			break;
		case wxID_ABOUT: {
			// The wxABoutBox under Windows looks horrible, so use a home made one
#if defined(__WXMSW__)
			::wxMessageBox(VERSION + wxT(" - ") + REL_DATE + _("\n\n"
				"A Software Define Radio for Microwaves\n\n"
				"Hardware:\tChris Bartram, GW4DGU\n"
				"\t\tGrant Hodgson, G8UBN\n"
				"\t\tNeil Whiting, G4BRK\n"
				"Firmware:\tTobias Weber, DG3YEV\n"
				"Software:\tJonathan Naylor, ON/G4KLX\n"
				"\t\tMichael White, G3WOE\n"
				"DTTSP:\t\tBob McGwier, N4HY\n"
				"\t\tFrank Brickle, AB2KT"),
				_("About uWave SDR"),
				wxICON_INFORMATION);
#else
				wxAboutDialogInfo info;

				info.SetName(wxT("UWSDR"));
				info.SetVersion(VERSION + wxT(" - ") + REL_DATE);
				info.SetDescription(_("A software defined radio for microwaves"));

				info.AddDeveloper(wxT("Jonathan Naylor, ON/G4KLX"));
				info.AddDeveloper(wxT("Michael White, G3WOE"));
				info.AddDeveloper(wxT("Chris Bartram, GW4DGU"));
				info.AddDeveloper(wxT("Grant Hodgson, G8UBN"));
				info.AddDeveloper(wxT("Tobias Weber, DG3YEV"));
				info.AddDeveloper(wxT("Neil Whiting, G4BRK"));
				info.AddDeveloper(wxT("Bob McGwier, N4HY"));
				info.AddDeveloper(wxT("Frank Brickle, AB2KT"));

				::wxAboutBox(info);
#endif
			}
			break;
 		case wxID_EXIT:
			if (m_txOn)
				return;
			Close(false);
			break;
	}
}

void CUWSDRFrame::sdrCommandNAK(const wxString& message, int id)
{
	::wxLogError(wxT("Received a NAK from the SDR: ") + message);
	::wxMessageBox(_("Received a NAK from the SDR\n") + message, _("uWave SDR Error"), wxICON_ERROR);
}

void CUWSDRFrame::sdrConnectionLost(int id)
{
	::wxLogError(wxT("Connection to the SDR lost"));
	::wxMessageBox(_("Connection to the SDR lost"), _("uWave SDR Error"), wxICON_ERROR);

	Close(true);
}

void CUWSDRFrame::onTimer(wxTimerEvent& event)
{
	m_parameters->m_spectrumPos   = m_spectrumDisplay->getPosition();
	m_parameters->m_spectrumType  = m_spectrumDisplay->getType();
	m_parameters->m_spectrumSpeed = m_spectrumDisplay->getSpeed();

	if (m_txOn) {
		int meter = m_sMeter->getTXMeter();
		m_parameters->m_txMeter = meter;

		float val = m_dsp->getMeter(meter);

		if (val != -200.0F) {
			m_sMeter->setLevel(val);

			float offset = 0.0F;
			if (m_parameters->m_zeroIF)
				offset = m_dsp->getTXOffset();

			m_dsp->getSpectrum(m_spectrum, m_parameters->m_spectrumPos);

			m_spectrumDisplay->showSpectrum(m_spectrum, 0.0F, offset);
			m_spectrumDisplay->getFreqPick();
		}
	} else {
		int meter = m_sMeter->getRXMeter();
		m_parameters->m_rxMeter = meter;

		float val = m_dsp->getMeter(meter);

		if (val != -200.0F) {
			m_sMeter->setLevel(val);

			float offset = 0.0F;
			if (m_parameters->m_zeroIF)
				offset = m_dsp->getRXOffset();

			m_dsp->getSpectrum(m_spectrum, m_parameters->m_spectrumPos);

			m_spectrumDisplay->showSpectrum(m_spectrum, -35.0F, offset);

			float freq = m_spectrumDisplay->getFreqPick();
			if (freq != 0.0F)
				freqChange(freq);
		}
	}
}

void CUWSDRFrame::onClose(wxCloseEvent& event)
{
	if (!event.CanVeto()) {
		Destroy();
		return;
	}

	if (m_txOn) {
		event.Veto();
		return;
	}

	int reply = ::wxMessageBox(_("Do you want to exit uWave SDR"),
		_("Exit uWSDR"),
		wxOK | wxCANCEL | wxICON_QUESTION);

	if (reply == wxOK) {
		m_timer.Stop();
		if (!m_parameters->m_hardwareReceiveOnly)
			m_sdr->enableTX(false);
		m_sdr->enableRX(false);
		m_sdr->close();
		m_dsp->close();

		// Grab the parameters from the CW keyboard
		m_parameters->m_cwSpeed   = m_cwKeyboard->getSpeed();
		m_parameters->m_cwLocal   = m_cwKeyboard->getLocal();
		m_parameters->m_cwRemote  = m_cwKeyboard->getRemote();
		m_parameters->m_cwLocator = m_cwKeyboard->getLocator();
		m_parameters->m_cwReport  = m_cwKeyboard->getReport();
		m_parameters->m_cwSerial  = m_cwKeyboard->getSerial();
		for (int i = 0; i < CWKEYBOARD_COUNT; i++)
			m_parameters->m_cwMessage[i] = m_cwKeyboard->getMessage(i);

		// Grab the parameters from the voice keyer
		m_parameters->m_voiceDir = m_voiceKeyboard->getDir();
		for (int j = 0; j < VOICEKEYER_COUNT; j++)
			m_parameters->m_voiceFile[j] = m_voiceKeyboard->getFile(j);

		Destroy();
	} else {
		event.Veto();
	}
}

void CUWSDRFrame::sendCW(unsigned int speed, const wxString& text)
{
	// If we're not in CW mode; show an error
	if (m_parameters->m_mode != MODE_CWW && m_parameters->m_mode != MODE_CWN) {
		::wxMessageBox(_("Not in a CW mode!"), _("uWave SDR Error"), wxICON_ERROR);
		return;
	}

	if (speed == 0) {
		if (!m_txOn)
			return;
	} else {
		if (m_txOn) {
			::wxMessageBox(_("Cannot send as already transmitting"), _("uWave SDR Error"), wxICON_ERROR);
			return;
		}
	}

	bool ret = setTransmit();

	// Going to TX (or RX) has failed
	if (!ret)
		return;

	m_dsp->sendCW(speed, text);
}

void CUWSDRFrame::sendAudio(const wxString& fileName, int state)
{
	// If we're in CW mode; show an error
	if (m_parameters->m_mode == MODE_CWW || m_parameters->m_mode == MODE_CWN) {
		::wxMessageBox(_("Not in a voice mode!"), _("uWave SDR Error"), wxICON_ERROR);
		return;
	}

	if (state == VOICE_STOPPED) {
		if (!m_txOn)
			return;
	} else {
		if (m_txOn) {
			::wxMessageBox(_("Cannot send as already transmitting"), _("uWave SDR Error"), wxICON_ERROR);
			return;
		}
	}

	bool ret = setTransmit();

	// Going to TX (or RX) has failed
	if (!ret)
		return;

	m_dsp->sendAudio(fileName, state);
}
