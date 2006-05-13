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
#include "FreqKeypad.h"

#include "Nullreader.h"
#include "NullWriter.h"
#include "SignalReader.h"
#include "SoundCardReader.h"
#include "SoundCardWriter.h"
#include "SoundFileReader.h"
#include "SoundFileWriter.h"

#include "UWSDR.xpm"

const int MENU_KEYPAD        = 36427;
const int MENU_PREFERENCES   = 36428;
const int MENU_CW_KEYBOARD   = 36429;
const int MENU_RECORD        = 36430;
const int MENU_HARDWARE_INFO = 36431;
const int MENU_ABOUT         = 36432;
const int MENU_EXIT          = 36433;

const int MENU_BUTTON       = 17856;
const int VFO_AB_BUTTON     = 17857;
const int VFO_CD_BUTTON     = 17858;
const int VFO_SWAP_BUTTON   = 17860;
const int VFO_SPLIT_BUTTON  = 17861;
const int VFO_SHIFT1_BUTTON = 17863;
const int VFO_SHIFT2_BUTTON = 17864;
const int FREQ_KNOB         = 17865;
const int FREQ_MHZ1_BUTTON  = 17866;
const int FREQ_MHZ2_BUTTON  = 17867;
const int FREQ_DISPLAY      = 17868;		// Not used
const int INFO_DISPLAY      = 17869;		// Not used
const int FREQ_SPECTRUM     = 17870;		// Not used
const int MODE_CHOICE       = 17871;
const int FILTER_CHOICE     = 17872;
const int RIT_BUTTON        = 17873;
const int MUTE_BUTTON       = 17874;
const int RIT_KNOB          = 17875;
const int TX_BUTTON         = 17876;
const int SMETER            = 17877;		// Not used
const int MIC_KNOB          = 17878;
const int POWER_KNOB        = 17879;
const int VOLUME_KNOB       = 17880;
const int SQUELCH_KNOB      = 17881;
const int DISPLAY_TIMER     = 17882;

BEGIN_EVENT_TABLE(CUWSDRFrame, wxFrame)
	EVT_BUTTON(MENU_BUTTON, CUWSDRFrame::onMenuButton)
	EVT_MENU(MENU_KEYPAD, CUWSDRFrame::onMenuSelection)
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
m_freqDisplay(NULL),
m_spectrumDisplay(NULL),
m_infoBox(NULL),
m_mode(NULL),
m_filter(NULL),
m_ritCtrl(NULL),
m_rit(NULL),
m_sMeter(NULL),
m_micGain(NULL),
m_power(NULL),
m_afGain(NULL),
m_squelch(NULL),
m_spectrum(NULL)
{
	SetIcon(wxIcon(UWSDR_xpm));

	m_sdr = new CSDRControl(this, -1);

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

//	wxBoxSizer* belowFreqSizer = new wxBoxSizer(wxHORIZONTAL);
	wxFlexGridSizer* belowFreqSizer = new wxFlexGridSizer(3);

	wxGridSizer* bandModeSizer = new wxGridSizer(2, 0, 8);
	bandModeSizer->Add(createModeChoice(panel));
	bandModeSizer->Add(createRITButton(panel), 0, wxEXPAND);
	bandModeSizer->Add(createFilterChoice(panel));
	bandModeSizer->Add(createMuteButton(panel), 0, wxEXPAND);

	belowFreqSizer->Add(bandModeSizer, 0, wxEXPAND |wxLEFT | wxRIGHT, BORDER_SIZE);
	belowFreqSizer->Add(createRIT(panel), 0, wxLEFT | wxRIGHT, BORDER_SIZE);
	belowFreqSizer->Add(createTransmitButton(panel), 0, wxTOP | wxLEFT, BORDER_SIZE);

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
}

CUWSDRFrame::~CUWSDRFrame()
{
	delete[] m_spectrum;
	delete   m_sdr;
	delete   m_menu;
}

void CUWSDRFrame::setParameters(CSDRParameters* parameters)
{
	wxASSERT(parameters != NULL);
	wxASSERT(m_spectrumDisplay != NULL);
	wxASSERT(m_infoBox != NULL);
	wxASSERT(m_mode != NULL);
	wxASSERT(m_filter != NULL);
	wxASSERT(m_ritCtrl != NULL);
	wxASSERT(m_rit != NULL);
	wxASSERT(m_micGain != NULL);
	wxASSERT(m_power != NULL);
	wxASSERT(m_afGain != NULL);
	wxASSERT(m_squelch != NULL);
	wxASSERT(m_sMeter != NULL);
	wxASSERT(m_sdr != NULL);

	m_parameters = parameters;

	bool ret = m_sdr->open(m_parameters->m_ipAddress, m_parameters->m_controlPort, m_parameters->m_hardwareProtocolVersion, m_parameters->m_sdrEnabled);
	if (!ret) {
		::wxLogError(_("Problems communicating with the SDR"));
		::wxMessageBox(_("Problems communicating with the SDR"), _("�Wave SDR Error"), wxICON_ERROR);
		Close(true);
		return;
	}

	if (!m_parameters->m_hardwareReceiveOnly)
		m_sdr->enableTX(true);
	m_sdr->enableRX(true);

	m_spectrumDisplay->setSampleRate(m_parameters->m_hardwareSampleRate);
	m_spectrumDisplay->setType(m_parameters->m_spectrumType);
	m_spectrumDisplay->setSpeed(m_parameters->m_spectrumSpeed);

	// FIXME
	// (m_parameters->m_ipAddress, m_parameters->m_dataPort, m_parameters->m_hardwareProtocolVersion, m_parameters->m_sdrEnabled);

	m_dsp = new CDSPControl(m_parameters->m_hardwareSampleRate, CENTRE_FREQ);

	// m_dsp->setTXReader(new CNullReader());
	// m_dsp->setTXReader(new CSoundCardReader(m_parameters->m_audioAPI, m_parameters->m_audioInDev));
	m_dsp->setTXReader(new CSignalReader(750, 0.0F, 0.5F));
	m_dsp->setTXWriter(new CNullWriter());

	m_dsp->setRXReader(new CSignalReader(12000, 4.77E-7F, 5E-7F));
	// m_dsp->setRXWriter(new CSoundFileWriter("RX.wav", 16));
	// m_dsp->setRXWriter(new CSoundCardWriter(m_parameters->m_audioAPI, m_parameters->m_audioOutDev));
	m_dsp->setRXWriter(new CNullWriter());

	m_dsp->Create();
	m_dsp->Run();

	m_infoBox->setVFO(m_parameters->m_vfoChoice);
	m_infoBox->setSplitShift(m_parameters->m_vfoSplitShift);
	m_infoBox->setRIT(m_parameters->m_ritOn);

	m_mode->SetSelection(m_parameters->m_mode);

	m_filter->SetSelection(m_parameters->m_filter);

	m_ritCtrl->SetValue(m_parameters->m_ritOn);
	m_rit->setValue(m_parameters->m_ritFreq);

	m_micGain->setValue(m_parameters->m_micGain);
	m_dsp->setMicGain(m_parameters->m_micGain);

	m_power->setValue(m_parameters->m_power);
	m_dsp->setPower(m_parameters->m_power);

	m_afGain->setValue(m_parameters->m_afGain);
	m_dsp->setAFGain(m_parameters->m_afGain);

	m_squelch->setValue(m_parameters->m_squelch);
	m_dsp->setSquelch(m_parameters->m_squelch);

	m_dsp->setNB(m_parameters->m_nbOn);
	m_dsp->setNBValue(m_parameters->m_nbValue);
	m_dsp->setNB2(m_parameters->m_nb2On);
	m_dsp->setNB2Value(m_parameters->m_nb2Value);

	m_dsp->setSP(m_parameters->m_spOn);
	m_dsp->setSPValue(m_parameters->m_spValue);

	m_dsp->setRXIAndQ(m_parameters->m_rxIQphase, m_parameters->m_rxIQgain);
	m_dsp->setTXIAndQ(m_parameters->m_txIQphase, m_parameters->m_txIQgain);

	m_sMeter->setRXMeter(m_parameters->m_rxMeter);
	m_sMeter->setTXMeter(m_parameters->m_txMeter);

	m_timer.SetOwner(this, DISPLAY_TIMER);
	m_timer.Start(100);

	normaliseMode();

	normaliseFreq();
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
	m_menu->AppendCheckItem(MENU_RECORD, _("Record"));
	m_menu->AppendSeparator();
	m_menu->Append(wxID_HELP,            _("Help\tF1"));
	m_menu->Append(MENU_HARDWARE_INFO,   _("Hardware Info"));
	m_menu->Append(wxID_ABOUT,           _("About �Wave SDR"));
	m_menu->AppendSeparator();
	m_menu->Append(wxID_EXIT,            _("Exit\tALT-F4"));
}

wxButton* CUWSDRFrame::createMenuButton(wxWindow* window)
{
	return new wxButton(window, MENU_BUTTON, _("Menu"), wxDefaultPosition, wxSize(FREQDIAL_WIDTH, -1));
}

wxSizer* CUWSDRFrame::createVFOButtons(wxWindow* window)
{
	wxSizer* grid = new wxGridSizer(2);

	wxButton* vfoA = new wxButton(window, VFO_AB_BUTTON, _("VFO A/B"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(vfoA);

	wxButton* vfoB = new wxButton(window, VFO_CD_BUTTON, _("VFO C/D"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(vfoB);

	wxButton* swap = new wxButton(window, VFO_SWAP_BUTTON, _("SWAP"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(swap);

	wxButton* split = new wxButton(window, VFO_SPLIT_BUTTON, _("SPLIT"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(split);

	wxButton* shift1 = new wxButton(window, VFO_SHIFT1_BUTTON, _("SHIFT -"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(shift1);

	wxButton* shift2 = new wxButton(window, VFO_SHIFT2_BUTTON, _("SHIFT +"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(shift2);

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

	wxButton* mhzMinus = new wxButton(window, FREQ_MHZ1_BUTTON, _("MHz -"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(mhzMinus);

	wxButton* mhzPlus = new wxButton(window, FREQ_MHZ2_BUTTON, _("MHz +"), wxDefaultPosition, wxSize(-1, -1));
	grid->Add(mhzPlus);

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
	m_mode->Append(_("FM"));

	sizer->Add(m_mode, 0, wxALL, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createFilterChoice(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Filter")), wxVERTICAL);

	m_filter = new wxChoice(window, FILTER_CHOICE, wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

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

	m_ritCtrl = new wxToggleButton(window, RIT_BUTTON, _("On/Off"), wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	sizer->Add(m_ritCtrl, 0, wxLEFT | wxRIGHT | wxBOTTOM, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxSizer* CUWSDRFrame::createMuteButton(wxWindow* window)
{
	wxStaticBoxSizer* sizer = new wxStaticBoxSizer(new wxStaticBox(window, -1, _("Mute")), wxVERTICAL);

	wxToggleButton* button = new wxToggleButton(window, MUTE_BUTTON, _("On/Off"), wxDefaultPosition, wxSize(CONTROL_WIDTH, -1));

	sizer->Add(button, 0, wxLEFT | wxRIGHT | wxBOTTOM, BORDER_SIZE);
	sizer->SetSizeHints(window);

	return sizer;
}

wxToggleButton* CUWSDRFrame::createTransmitButton(wxWindow* window)
{
	return new wxToggleButton(window, TX_BUTTON, _("TRANSMIT"), wxDefaultPosition, wxSize(CONTROL_WIDTH, CONTROL_WIDTH));
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
	wxASSERT(m_parameters != NULL);
	wxASSERT(m_dsp != NULL);

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
	wxASSERT(m_parameters != NULL);

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
			freq = (freq + m_parameters->m_minReceiveFreq) - m_parameters->m_maxReceiveFreq;

		if (freq < m_parameters->m_minReceiveFreq)
			freq = (freq + m_parameters->m_maxReceiveFreq) - m_parameters->m_minReceiveFreq;
	} else {
		if (freq >= m_parameters->m_maxTransmitFreq)
			freq = (freq + m_parameters->m_minTransmitFreq) - m_parameters->m_maxTransmitFreq;

		if (freq < m_parameters->m_minTransmitFreq)
			freq = (freq + m_parameters->m_maxTransmitFreq) - m_parameters->m_minTransmitFreq;
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
	wxASSERT(m_menu != NULL);

	m_menu->Check(MENU_RECORD, m_record);

	PopupMenu(m_menu, 0, 30);
}

void CUWSDRFrame::onVFOButton(wxCommandEvent& event)
{
	wxASSERT(m_parameters != NULL);
	wxASSERT(m_infoBox != NULL);

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
	wxASSERT(m_parameters != NULL);

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
	wxASSERT(m_parameters != NULL);

	m_parameters->m_mode = int(event.GetSelection());

	normaliseMode();
}

void CUWSDRFrame::onFilterChoice(wxCommandEvent& event)
{
	wxASSERT(m_parameters != NULL);

	m_parameters->m_filter = int(event.GetSelection());

	normaliseMode();
}

void CUWSDRFrame::onRITButton(wxCommandEvent& event)
{
	wxASSERT(m_infoBox != NULL);
	wxASSERT(m_parameters != NULL);

	m_parameters->m_ritOn = !m_parameters->m_ritOn;

	m_infoBox->setRIT(m_parameters->m_ritOn);

	if (m_txOn)
		return;

	normaliseFreq();
}

void CUWSDRFrame::onMuteButton(wxCommandEvent& event)
{
	wxASSERT(m_sdr != NULL);

	m_rxOn = !m_rxOn;

	m_sdr->enableRX(m_rxOn);
}

void CUWSDRFrame::onTXButton(wxCommandEvent& event)
{
	wxASSERT(m_infoBox != NULL);
	wxASSERT(m_parameters != NULL);

	if (!m_txOn && m_parameters->m_hardwareReceiveOnly) {
		::wxMessageBox(_("This SDR is only a receiver!"), _("�Wave SDR Error"), wxICON_ERROR);
		return;
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
			freq = m_parameters->m_vfoA - m_parameters->m_shift;
		else if (m_parameters->m_vfoChoice == VFO_A && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoA + m_parameters->m_shift;
		else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
			freq = m_parameters->m_vfoB - m_parameters->m_shift;
		else if (m_parameters->m_vfoChoice == VFO_B && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoB + m_parameters->m_shift;
		else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
			freq = m_parameters->m_vfoC - m_parameters->m_shift;
		else if (m_parameters->m_vfoChoice == VFO_C && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoC + m_parameters->m_shift;
		else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
			freq = m_parameters->m_vfoD - m_parameters->m_shift;
		else if (m_parameters->m_vfoChoice == VFO_D && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
			freq = m_parameters->m_vfoD + m_parameters->m_shift;

		if (freq >= m_parameters->m_maxTransmitFreq || freq < m_parameters->m_minTransmitFreq) {
			::wxBell();
			return;
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
}

void CUWSDRFrame::normaliseFreq()
{
	// We can be called too early ...
	if (m_parameters == NULL)
		return;

	wxASSERT(m_freqDisplay != NULL);
	wxASSERT(m_sdr != NULL);
	wxASSERT(m_dsp != NULL);

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

	if (m_parameters->m_ritOn && !m_txOn)
		freq += m_parameters->m_ritFreq;

	if (m_txOn && m_parameters->m_vfoSplitShift == VFO_SHIFT_1)
		freq -= m_parameters->m_shift;
	if (m_txOn && m_parameters->m_vfoSplitShift == VFO_SHIFT_2)
		freq += m_parameters->m_shift;

	m_freqDisplay->setFrequency(freq);

	// Subtract the IF frequency
	freq -= CENTRE_FREQ;

	// Now take into account the frequency steps of the SDR ...
	double offset = 0.0;
	if (m_parameters->m_hardwareStepSize > 1) {
		double stepSize = double(m_parameters->m_hardwareStepSize);
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

void CUWSDRFrame::normaliseMode()
{
	// We can be called too early
	if (m_parameters == NULL)
		return;

	wxASSERT(m_dsp != NULL);

	m_dsp->setMode(m_parameters->m_mode);

	int speed  = -1;
	int filter = m_parameters->m_filter;

	switch (m_parameters->m_mode) {
		case MODE_FM:
			if (filter == FILTER_AUTO)
				filter = m_parameters->m_filterFM;
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
	wxASSERT(m_parameters != NULL);
	wxASSERT(m_dsp != NULL);

	switch (event.GetId()) {
		case MENU_PREFERENCES: {
				CUWSDRPreferences preferences(this, -1, m_parameters);
				int reply = preferences.ShowModal();
				if (reply == wxID_OK) {
					normaliseMode();

					m_dsp->setNB(m_parameters->m_nbOn);
					m_dsp->setNBValue(m_parameters->m_nbValue);
					m_dsp->setNB2(m_parameters->m_nb2On);
					m_dsp->setNB2Value(m_parameters->m_nb2Value);

					m_dsp->setSP(m_parameters->m_spOn);
					m_dsp->setSPValue(m_parameters->m_spValue);

					m_dsp->setRXIAndQ(m_parameters->m_rxIQphase, m_parameters->m_rxIQgain);
					m_dsp->setTXIAndQ(m_parameters->m_txIQphase, m_parameters->m_txIQgain);
				}
			}
			break;
		case MENU_KEYPAD: {
				if (m_txOn)
					return;
				CFreqKeypad keypad(this, -1, m_parameters->m_minReceiveFreq, m_parameters->m_maxReceiveFreq);
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
		case MENU_CW_KEYBOARD:
			break;
		case MENU_RECORD: {
				m_record = !m_record;
				bool ret = m_dsp->setRecord(m_record);
				if (!ret) {
					m_record = false;
					::wxMessageBox(_("Cannot open the sound file for recording"), _("�Wave SDR Error"), wxICON_ERROR);
				}
			}
			break;
		case wxID_HELP:
			::wxGetApp().showHelp(_("UWSDR"));
			break;
		case MENU_HARDWARE_INFO: {
				wxString stepSize;
				stepSize.Printf(wxT("%u"), m_parameters->m_hardwareStepSize / 1000);

				wxString sampleRate;
				sampleRate.Printf(wxT("%u"), m_parameters->m_hardwareSampleRate);

				wxString protocolVersion;
				protocolVersion.Printf(wxT("%u"), m_parameters->m_hardwareProtocolVersion);

				wxString transmit = (m_parameters->m_hardwareReceiveOnly) ? _("No") : _("Yes");

				::wxMessageBox(_("The hardware parameters are:\n\n"
					"Name:\t\t") + m_parameters->m_hardwareName + _("\n"
					"Max. Freq:\t") + m_parameters->m_maxHardwareFreq.getString() + _(" MHz\n"
					"Min. Freq:\t") + m_parameters->m_minHardwareFreq.getString() + _(" MHz\n"
					"Step Size:\t") + stepSize + _(" kHz\n"
					"Sample Rate:\t") + sampleRate + _(" samples/sec\n"
					"Protocol Version:\t") + protocolVersion + _("\n"
					"Transmit:\t\t") + transmit,
					_("SDR Hardware Information"),
					wxICON_INFORMATION);
			}
			break;
		case wxID_ABOUT:
			::wxMessageBox(VERSION + REL_DATE + _("\n\n"
				"A Software Define Radio for Microwaves\n\n"
				"Hardware:\tChris Bartram, GW4DGU\n"
				"Firmware:\tTobias Weber, DG3YEV\n"
				"GUI:\t\tJonathan Naylor, ON/G4KLX\n"
				"DTTSP:\t\tBob McGwier, N4HY\n"
				"\t\tFrank Brickle, AB2KT"),
				_("About �Wave SDR"),
				wxICON_INFORMATION);
			break;
 		case wxID_EXIT:
			if (m_txOn)
				return;
			Close(false);
			break;
	}
}

void CUWSDRFrame::sdrCommandNAK(int id)
{
	::wxLogError(_("Received a NAK from the SDR"));
	::wxMessageBox(_("Received a NAK from the SDR"), _("�Wave SDR Error"), wxICON_ERROR);
}

void CUWSDRFrame::sdrConnectionLost(int id)
{
	::wxLogError(_("Connection to the SDR lost"));
	::wxMessageBox(_("Connection to the SDR lost"), _("�Wave SDR Error"), wxICON_ERROR);

	Close(true);
}

void CUWSDRFrame::onTimer(wxTimerEvent& event)
{
	wxASSERT(m_dsp != NULL);
	wxASSERT(m_sMeter != NULL);
	wxASSERT(m_spectrumDisplay != NULL);

	if (m_txOn) {
		int meter = m_sMeter->getTXMeter();
		m_parameters->m_txMeter = meter;

		float val = m_dsp->getMeter(meter);

		if (val != -200.0F) {
			wxLogMessage("TX Meter = %f", val);

			m_sMeter->setLevel(val * 94.0);

			m_dsp->getSpectrum(m_spectrum);
			m_spectrumDisplay->showSpectrum(m_spectrum, 0.25);
		}

		m_parameters->m_spectrumType  = m_spectrumDisplay->getType();
		m_parameters->m_spectrumSpeed = m_spectrumDisplay->getSpeed();
	} else {
		int meter = m_sMeter->getRXMeter();
		m_parameters->m_rxMeter = meter;

		float val = m_dsp->getMeter(meter);

		if (val != -200.0F) {
			m_sMeter->setLevel(val + 110.0);

			m_dsp->getSpectrum(m_spectrum);
			m_spectrumDisplay->showSpectrum(m_spectrum);
		}

		m_parameters->m_spectrumType  = m_spectrumDisplay->getType();
		m_parameters->m_spectrumSpeed = m_spectrumDisplay->getSpeed();
	}
}

void CUWSDRFrame::onClose(wxCloseEvent& event)
{
	wxASSERT(m_sdr != NULL);
	wxASSERT(m_dsp != NULL);

	if (!event.CanVeto()) {
		Destroy();
		return;
	}

	if (m_txOn) {
		event.Veto();
		return;
	}

	int reply = ::wxMessageBox(_("Do you want to exit �Wave SDR"),
		_("Exit �W SDR"),
		wxOK | wxCANCEL | wxICON_QUESTION);

	if (reply == wxOK) {
		m_timer.Stop();
		if (!m_parameters->m_hardwareReceiveOnly)
			m_sdr->enableTX(false);
		m_sdr->enableRX(false);
		m_sdr->close();
		m_dsp->Delete();
		Destroy();
	} else {
		event.Veto();
	}
}
