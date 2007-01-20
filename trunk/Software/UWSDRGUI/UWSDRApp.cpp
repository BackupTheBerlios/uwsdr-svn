/*
 *   Copyright (C) 2006,7 by Jonathan Naylor G4KLX
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

#include <wx/config.h>
#include <wx/cmdline.h>
#include <wx/fs_zip.h>
#include <wx/image.h>

#include "UWSDRApp.h"
#include "UWSDRDefs.h"
#include "SDRDescrFile.h"
#include "Version.h"
#include "Log.h"

const wxString KEY_FILE_NAME          = wxT("/FileName");
const wxString KEY_MAX_RX_FREQ        = wxT("/MaxReceiveFreq");
const wxString KEY_MIN_RX_FREQ        = wxT("/MinReceiveFreq");
const wxString KEY_MAX_TX_FREQ        = wxT("/MaxTransmitFreq");
const wxString KEY_MIN_TX_FREQ        = wxT("/MinTransmitFreq");
const wxString KEY_VFO_A              = wxT("/VfoA");
const wxString KEY_VFO_B              = wxT("/VfoB");
const wxString KEY_VFO_C              = wxT("/VfoC");
const wxString KEY_VFO_D              = wxT("/VfoD");
const wxString KEY_FREQ_SHIFT         = wxT("/FreqShift");
const wxString KEY_FREQ_OFFSET        = wxT("/FreqOffset");
const wxString KEY_VFO_CHOICE         = wxT("/VfoChoice");
const wxString KEY_VFO_SPLIT_SHIFT    = wxT("/VfoSplitShift");
const wxString KEY_VFO_SPEED_FM       = wxT("/VfoSpeedFM");
const wxString KEY_VFO_SPEED_AM       = wxT("/VfoSpeedAM");
const wxString KEY_VFO_SPEED_SSB      = wxT("/VfoSpeedSSB");
const wxString KEY_VFO_SPEED_CWW      = wxT("/VfoSpeedCWW");
const wxString KEY_VFO_SPEED_CWN      = wxT("/VfoSpeedCWN");
const wxString KEY_STEP_VERY_FAST     = wxT("/StepVeryFast");
const wxString KEY_STEP_FAST          = wxT("/StepFast");
const wxString KEY_STEP_MEDIUM        = wxT("/StepMedium");
const wxString KEY_STEP_SLOW          = wxT("/StepSlow");
const wxString KEY_STEP_VERY_SLOW     = wxT("/StepVerySlow");
const wxString KEY_MODE               = wxT("/Mode");
const wxString KEY_ZERO_IF            = wxT("/ZeroIF");
const wxString KEY_SWAP_IQ            = wxT("/SwapIQ");
const wxString KEY_CLOCK_TUNE         = wxT("/ClockTune");
const wxString KEY_DEV_FMW            = wxT("/DeviationFMW");
const wxString KEY_DEV_FMN            = wxT("/DeviationFMN");
const wxString KEY_AGC_AM             = wxT("/AGCAM");
const wxString KEY_AGC_SSB            = wxT("/AGCSSB");
const wxString KEY_AGC_CW             = wxT("/AGCCW");
const wxString KEY_FILTER             = wxT("/Filter");
const wxString KEY_FILTER_FMW         = wxT("/FilterFMW");
const wxString KEY_FILTER_FMN         = wxT("/FilterFMN");
const wxString KEY_FILTER_AM          = wxT("/FilterAM");
const wxString KEY_FILTER_SSB         = wxT("/FilterSSB");
const wxString KEY_FILTER_CWW         = wxT("/FilterCWW");
const wxString KEY_FILTER_CWN         = wxT("/FilterCWN");
const wxString KEY_IP_ADDRESS         = wxT("/IPAddress");
const wxString KEY_CONTROL_PORT       = wxT("/ControlPort");
const wxString KEY_DATA_PORT          = wxT("/DataPort");
const wxString KEY_CONTROL_DEVICE     = wxT("/ControlDevice");
const wxString KEY_CONTROL_PIN        = wxT("/ControlPin");
const wxString KEY_USER_AUDIO_API     = wxT("/UserAudioAPI");
const wxString KEY_USER_AUDIO_IN_DEV  = wxT("/UserAudioInDev");
const wxString KEY_USER_AUDIO_OUT_DEV = wxT("/UserAudioOutDev");
const wxString KEY_SDR_AUDIO_API      = wxT("/SDRAudioAPI");
const wxString KEY_SDR_AUDIO_IN_DEV   = wxT("/SDRAudioInDev");
const wxString KEY_SDR_AUDIO_OUT_DEV  = wxT("/SDRAudioOutDev");
const wxString KEY_RIT_CTRL           = wxT("/RitCtrl");
const wxString KEY_RIT_VALUE          = wxT("/RitFreq");
const wxString KEY_NB_CTRL            = wxT("/NoiseBlankerCtrl");
const wxString KEY_NB_VALUE           = wxT("/NoiseBlankerValue");
const wxString KEY_NB2_CTRL           = wxT("/NoiseBlanker2Ctrl");
const wxString KEY_NB2_VALUE          = wxT("/NoiseBlanker2Value");
const wxString KEY_SP_CTRL            = wxT("/SpeechProcCtrl");
const wxString KEY_SP_VALUE           = wxT("/SpeechprocValue");
const wxString KEY_CARRIER_LEVEL      = wxT("/CarrierLevel");
const wxString KEY_ALC_ATTACK         = wxT("/ALCAttackValue");
const wxString KEY_ALC_DECAY          = wxT("/ALCDecayValue");
const wxString KEY_ALC_HANG           = wxT("/ALCHangValue");
const wxString KEY_RX_METER           = wxT("/ReceiveMeter");
const wxString KEY_TX_METER           = wxT("/TransmitMeter");
const wxString KEY_SPECTRUM_POS       = wxT("/SpectrumPos");
const wxString KEY_SPECTRUM_TYPE      = wxT("/SpectrumType");
const wxString KEY_SPECTRUM_SPEED     = wxT("/SpectrumSpeed");
const wxString KEY_SPECTRUM_DB        = wxT("/SpectrumDB");
const wxString KEY_RX_IQ_PHASE        = wxT("/ReceiveIQPhase");
const wxString KEY_RX_IQ_GAIN         = wxT("/ReceiveIQGain");
const wxString KEY_TX_IQ_PHASE        = wxT("/TransmitIQPhase");
const wxString KEY_TX_IQ_GAIN         = wxT("/TransmitIQGain");
const wxString KEY_AF_GAIN            = wxT("/AFGain");
const wxString KEY_RF_GAIN            = wxT("/RFGain");
const wxString KEY_SQUELCH            = wxT("/Squelch");
const wxString KEY_MIC_GAIN           = wxT("/MicGain");
const wxString KEY_POWER              = wxT("/Power");
const wxString KEY_CW_SPEED           = wxT("/CWSpeed");
const wxString KEY_CW_LOCAL           = wxT("/CWLocalCallsign");
const wxString KEY_CW_REMOTE          = wxT("/CWRemoteCallsign");
const wxString KEY_CW_LOCATOR         = wxT("/CWLocator");
const wxString KEY_CW_REPORT          = wxT("/CWReport");
const wxString KEY_CW_SERIAL          = wxT("/CWSerialNumber");
const wxString KEY_CW_MESSAGE         = wxT("/CWMessage");
const wxString KEY_VOICE_DIR          = wxT("/VoiceDir");
const wxString KEY_VOICE_FILE         = wxT("/VoiceFile");


IMPLEMENT_APP(CUWSDRApp)

CUWSDRApp::CUWSDRApp() :
wxApp(),
m_frame(NULL),
m_parameters(NULL),
m_help(NULL)
{
	m_parameters = new CSDRParameters();
}

CUWSDRApp::~CUWSDRApp()
{
	delete m_parameters;
}

bool CUWSDRApp::OnInit()
{
	if (!wxApp::OnInit())
		return false;

	wxLog* logger = new CLog(m_parameters->m_name + wxT(".log"));
	wxLog::SetActiveTarget(logger);

	::wxInitAllImageHandlers();

	wxFileSystem::AddHandler(new wxZipFSHandler);
	m_help = new wxHtmlHelpController();
	m_help->SetTitleFormat(_("uWave SDR Help: %s"));

	wxFileName fileName;
	fileName.AssignDir(getHelpDir());
	fileName.SetFullName(wxT("UWSDR.zip"));
	m_help->AddBook(fileName);

	::wxLogMessage(wxT("Starting ") + VERSION);
	::wxLogMessage(wxT("Using configuration: ") + m_parameters->m_name);

	if (!readConfig()) {
		::wxLogError(wxT("Cannot open the SDR configuration - ") + m_parameters->m_name);
		::wxMessageBox(_("Cannot open the SDR configuration - ") + m_parameters->m_name, _("uWave SDR Error"), wxICON_ERROR);
		return false;
	}

	if (!readDescrFile()) {
		::wxLogError(wxT("Cannot open the SDR description file - ") + m_parameters->m_fileName);
		::wxMessageBox(_("Cannot open the SDR description file - ") + m_parameters->m_fileName, _("uWave SDR Error"), wxICON_ERROR);
		return false;
	}

	if (m_parameters->m_hardwareStepSize > 100.0F      ||
	    m_parameters->m_hardwareType == TYPE_AUDIORX   ||
	    m_parameters->m_hardwareType == TYPE_AUDIOTXRX ||
	    m_parameters->m_hardwareType == TYPE_DEMO)
		m_parameters->m_zeroIF = false;

	wxString title = VERSION + wxT(" - ") + m_parameters->m_name;

	m_frame = new CUWSDRFrame(title);
	m_frame->Show();

	SetTopWindow(m_frame);

	// Sanity checking for the frequencies
	if (m_parameters->m_maxReceiveFreq > m_parameters->m_hardwareMaxFreq ||
	    m_parameters->m_maxReceiveFreq < m_parameters->m_hardwareMinFreq)
		m_parameters->m_maxReceiveFreq = m_parameters->m_hardwareMaxFreq;

	if (m_parameters->m_minReceiveFreq < m_parameters->m_hardwareMinFreq ||
	    m_parameters->m_minReceiveFreq > m_parameters->m_hardwareMaxFreq)
		m_parameters->m_minReceiveFreq = m_parameters->m_hardwareMinFreq;

	if (m_parameters->m_maxTransmitFreq > m_parameters->m_maxReceiveFreq ||
	    m_parameters->m_maxTransmitFreq < m_parameters->m_minReceiveFreq)
		m_parameters->m_maxTransmitFreq = m_parameters->m_maxReceiveFreq;

	if (m_parameters->m_minTransmitFreq < m_parameters->m_minReceiveFreq ||
	    m_parameters->m_minTransmitFreq > m_parameters->m_maxReceiveFreq)
		m_parameters->m_minTransmitFreq = m_parameters->m_minReceiveFreq;

	if (m_parameters->m_vfoA > m_parameters->m_maxReceiveFreq)
		m_parameters->m_vfoA = m_parameters->m_maxReceiveFreq;

	if (m_parameters->m_vfoA < m_parameters->m_minReceiveFreq)
		m_parameters->m_vfoA = m_parameters->m_minReceiveFreq;

	if (m_parameters->m_vfoB > m_parameters->m_maxReceiveFreq)
		m_parameters->m_vfoB = m_parameters->m_maxReceiveFreq;

	if (m_parameters->m_vfoB < m_parameters->m_minReceiveFreq)
		m_parameters->m_vfoB = m_parameters->m_minReceiveFreq;

	if (m_parameters->m_vfoC > m_parameters->m_maxReceiveFreq)
		m_parameters->m_vfoC = m_parameters->m_maxReceiveFreq;

	if (m_parameters->m_vfoC < m_parameters->m_minReceiveFreq)
		m_parameters->m_vfoC = m_parameters->m_minReceiveFreq;

	if (m_parameters->m_vfoD > m_parameters->m_maxReceiveFreq)
		m_parameters->m_vfoD = m_parameters->m_maxReceiveFreq;

	if (m_parameters->m_vfoD < m_parameters->m_minReceiveFreq)
		m_parameters->m_vfoD = m_parameters->m_minReceiveFreq;

	m_frame->setParameters(m_parameters);

	return true;
}

void CUWSDRApp::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.AddParam(_("SDR configuration name"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);

	wxApp::OnInitCmdLine(parser);
}

bool CUWSDRApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!wxApp::OnCmdLineParsed(parser))
		return false;

	m_parameters->m_name = parser.GetParam(0);

	return true;
}

int CUWSDRApp::OnExit()
{
	::wxLogMessage(wxT("Ending uWave SDR"));

	writeConfig();

	delete m_help;

	return 0;
}

bool CUWSDRApp::readDescrFile()
{
	CSDRDescrFile descrFile(m_parameters->m_fileName);

	if (!descrFile.isValid())
		return false;

	m_parameters->m_hardwareName        = descrFile.getName();
	m_parameters->m_hardwareType        = descrFile.getType();
	m_parameters->m_hardwareMaxFreq     = descrFile.getMaxFreq();
	m_parameters->m_hardwareMinFreq     = descrFile.getMinFreq();
	m_parameters->m_hardwareStepSize    = descrFile.getStepSize();
	m_parameters->m_hardwareSampleRate  = descrFile.getSampleRate();
	m_parameters->m_hardwareReceiveOnly = descrFile.getReceiveOnly();

	// Change the hardware frequency limits for SoftRock type radios
	if (m_parameters->m_hardwareType == TYPE_AUDIORX || m_parameters->m_hardwareType == TYPE_AUDIOTXRX) {
		m_parameters->m_hardwareMaxFreq += m_parameters->m_hardwareSampleRate / 4.0F;
		m_parameters->m_hardwareMinFreq -= m_parameters->m_hardwareSampleRate / 4.0F;
	}

	return true;
}

bool CUWSDRApp::readConfig()
{
	wxString keyFileName        = wxT("/") + m_parameters->m_name + KEY_FILE_NAME;
	wxString keyMaxRxFreq       = wxT("/") + m_parameters->m_name + KEY_MAX_RX_FREQ;
	wxString keyMinRxFreq       = wxT("/") + m_parameters->m_name + KEY_MIN_RX_FREQ;
	wxString keyMaxTxFreq       = wxT("/") + m_parameters->m_name + KEY_MAX_TX_FREQ;
	wxString keyMinTxFreq       = wxT("/") + m_parameters->m_name + KEY_MIN_TX_FREQ;
	wxString keyVfoA            = wxT("/") + m_parameters->m_name + KEY_VFO_A;
	wxString keyVfoB            = wxT("/") + m_parameters->m_name + KEY_VFO_B;
	wxString keyVfoC            = wxT("/") + m_parameters->m_name + KEY_VFO_C;
	wxString keyVfoD            = wxT("/") + m_parameters->m_name + KEY_VFO_D;
	wxString keyFreqShift       = wxT("/") + m_parameters->m_name + KEY_FREQ_SHIFT;
	wxString keyFreqOffset      = wxT("/") + m_parameters->m_name + KEY_FREQ_OFFSET;
	wxString keyVfoChoice       = wxT("/") + m_parameters->m_name + KEY_VFO_CHOICE;
	wxString keyVfoSplitShift   = wxT("/") + m_parameters->m_name + KEY_VFO_SPLIT_SHIFT;
	wxString keyVfoSpeedFM      = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_FM;
	wxString keyVfoSpeedAM      = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_AM;
	wxString keyVfoSpeedSSB     = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_SSB;
	wxString keyVfoSpeedCWW     = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWW;
	wxString keyVfoSpeedCWN     = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWN;
	wxString keyDevFMW          = wxT("/") + m_parameters->m_name + KEY_DEV_FMW;
	wxString keyDevFMN          = wxT("/") + m_parameters->m_name + KEY_DEV_FMN;
	wxString keyAgcAM           = wxT("/") + m_parameters->m_name + KEY_AGC_AM;
	wxString keyAgcSSB          = wxT("/") + m_parameters->m_name + KEY_AGC_SSB;
	wxString keyAgcCW           = wxT("/") + m_parameters->m_name + KEY_AGC_CW;
	wxString keyStepVeryFast    = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_FAST;
	wxString keyStepFast        = wxT("/") + m_parameters->m_name + KEY_STEP_FAST;
	wxString keyStepMedium      = wxT("/") + m_parameters->m_name + KEY_STEP_MEDIUM;
	wxString keyStepSlow        = wxT("/") + m_parameters->m_name + KEY_STEP_SLOW;
	wxString keyStepVerySlow    = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_SLOW;
	wxString keyMode            = wxT("/") + m_parameters->m_name + KEY_MODE;
	wxString keyZeroIF          = wxT("/") + m_parameters->m_name + KEY_ZERO_IF;
	wxString keySwapIQ          = wxT("/") + m_parameters->m_name + KEY_SWAP_IQ;
	wxString keyClockTune       = wxT("/") + m_parameters->m_name + KEY_CLOCK_TUNE;
	wxString keyFilter          = wxT("/") + m_parameters->m_name + KEY_FILTER;
	wxString keyFilterFMW       = wxT("/") + m_parameters->m_name + KEY_FILTER_FMW;
	wxString keyFilterFMN       = wxT("/") + m_parameters->m_name + KEY_FILTER_FMN;
	wxString keyFilterAM        = wxT("/") + m_parameters->m_name + KEY_FILTER_AM;
	wxString keyFilterSSB       = wxT("/") + m_parameters->m_name + KEY_FILTER_SSB;
	wxString keyFilterCWW       = wxT("/") + m_parameters->m_name + KEY_FILTER_CWW;
	wxString keyFilterCWN       = wxT("/") + m_parameters->m_name + KEY_FILTER_CWN;
	wxString keyIpAddress       = wxT("/") + m_parameters->m_name + KEY_IP_ADDRESS;
	wxString keyControlPort     = wxT("/") + m_parameters->m_name + KEY_CONTROL_PORT;
	wxString keyDataPort        = wxT("/") + m_parameters->m_name + KEY_DATA_PORT;
	wxString keyControlDevice   = wxT("/") + m_parameters->m_name + KEY_CONTROL_DEVICE;
	wxString keyControlPin      = wxT("/") + m_parameters->m_name + KEY_CONTROL_PIN;
	wxString keyUserAudioAPI    = wxT("/") + m_parameters->m_name + KEY_USER_AUDIO_API;
	wxString keyUserAudioInDev  = wxT("/") + m_parameters->m_name + KEY_USER_AUDIO_IN_DEV;
	wxString keyUserAudioOutDev = wxT("/") + m_parameters->m_name + KEY_USER_AUDIO_OUT_DEV;
	wxString keySDRAudioAPI     = wxT("/") + m_parameters->m_name + KEY_SDR_AUDIO_API;
	wxString keySDRAudioInDev   = wxT("/") + m_parameters->m_name + KEY_SDR_AUDIO_IN_DEV;
	wxString keySDRAudioOutDev  = wxT("/") + m_parameters->m_name + KEY_SDR_AUDIO_OUT_DEV;
	wxString keyRitCtrl         = wxT("/") + m_parameters->m_name + KEY_RIT_CTRL;
	wxString keyRitValue        = wxT("/") + m_parameters->m_name + KEY_RIT_VALUE;
	wxString keyNbCtrl          = wxT("/") + m_parameters->m_name + KEY_NB_CTRL;
	wxString keyNbValue         = wxT("/") + m_parameters->m_name + KEY_NB_VALUE;
	wxString keyNb2Ctrl         = wxT("/") + m_parameters->m_name + KEY_NB2_CTRL;
	wxString keyNb2Value        = wxT("/") + m_parameters->m_name + KEY_NB2_VALUE;
	wxString keySpCtrl          = wxT("/") + m_parameters->m_name + KEY_SP_CTRL;
	wxString keySpValue         = wxT("/") + m_parameters->m_name + KEY_SP_VALUE;
	wxString keyCarrierLevel    = wxT("/") + m_parameters->m_name + KEY_CARRIER_LEVEL;
	wxString keyAlcAttack       = wxT("/") + m_parameters->m_name + KEY_ALC_ATTACK;
	wxString keyAlcDecay        = wxT("/") + m_parameters->m_name + KEY_ALC_DECAY;
	wxString keyAlcHang         = wxT("/") + m_parameters->m_name + KEY_ALC_HANG;
	wxString keyRxMeter         = wxT("/") + m_parameters->m_name + KEY_RX_METER;
	wxString keyTxMeter         = wxT("/") + m_parameters->m_name + KEY_TX_METER;
	wxString keySpectrumPos     = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_POS;
	wxString keySpectrumType    = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_TYPE;
	wxString keySpectrumSpeed   = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_SPEED;
	wxString keySpectrumDB      = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_DB;
	wxString keyRxIqPhase       = wxT("/") + m_parameters->m_name + KEY_RX_IQ_PHASE;
	wxString keyRxIqGain        = wxT("/") + m_parameters->m_name + KEY_RX_IQ_GAIN;
	wxString keyTxIqPhase       = wxT("/") + m_parameters->m_name + KEY_TX_IQ_PHASE;
	wxString keyTxIqGain        = wxT("/") + m_parameters->m_name + KEY_TX_IQ_GAIN;
	wxString keyAfGain          = wxT("/") + m_parameters->m_name + KEY_AF_GAIN;
	wxString keyRfGain          = wxT("/") + m_parameters->m_name + KEY_RF_GAIN;
	wxString keySquelch         = wxT("/") + m_parameters->m_name + KEY_SQUELCH;
	wxString keyMicGain         = wxT("/") + m_parameters->m_name + KEY_MIC_GAIN;
	wxString keyPower           = wxT("/") + m_parameters->m_name + KEY_POWER;
	wxString keyCwSpeed         = wxT("/") + m_parameters->m_name + KEY_CW_SPEED;
	wxString keyCwLocal         = wxT("/") + m_parameters->m_name + KEY_CW_LOCAL;
	wxString keyCwRemote        = wxT("/") + m_parameters->m_name + KEY_CW_REMOTE;
	wxString keyCwLocator       = wxT("/") + m_parameters->m_name + KEY_CW_LOCATOR;
	wxString keyCwReport        = wxT("/") + m_parameters->m_name + KEY_CW_REPORT;
	wxString keyCwSerial        = wxT("/") + m_parameters->m_name + KEY_CW_SERIAL;
	wxString keyVoiceDir        = wxT("/") + m_parameters->m_name + KEY_VOICE_DIR;

	wxString keyCwMessage[CWKEYBOARD_COUNT];
	for (int i = 0; i < CWKEYBOARD_COUNT; i++) {
		wxString number;
		number.Printf(wxT("%d"), i);

		keyCwMessage[i] = wxT("/") + m_parameters->m_name + KEY_CW_MESSAGE;
		keyCwMessage[i].Append(number);
	}

	wxString keyVoiceFile[VOICEKEYER_COUNT];
	for (int j = 0; j < VOICEKEYER_COUNT; j++) {
		wxString number;
		number.Printf(wxT("%d"), j);

		keyVoiceFile[j] = wxT("/") + m_parameters->m_name + KEY_VOICE_FILE;
		keyVoiceFile[j].Append(number);
	}

	wxConfig* profile = new wxConfig(APPNAME);
	wxASSERT(profile != NULL);

	profile->SetRecordDefaults(true);

	if (!profile->Read(keyFileName, &m_parameters->m_fileName)) {
		delete profile;
		return false;
	}

	wxString freq;
	profile->Read(keyMaxRxFreq,        &freq, wxT("9999999.0"));
	m_parameters->m_maxReceiveFreq.setFrequency(freq);

	profile->Read(keyMinRxFreq,        &freq, wxT("0.0"));
	m_parameters->m_minReceiveFreq.setFrequency(freq);

	profile->Read(keyMaxTxFreq,        &freq, wxT("9999999.0"));
	m_parameters->m_maxTransmitFreq.setFrequency(freq);

	profile->Read(keyMinTxFreq,        &freq, wxT("0.0"));
	m_parameters->m_minTransmitFreq.setFrequency(freq);

	profile->Read(keyVfoA,             &freq, wxT("0.0"));
	m_parameters->m_vfoA.setFrequency(freq);

	profile->Read(keyVfoB,             &freq, wxT("0.0"));
	m_parameters->m_vfoB.setFrequency(freq);

	profile->Read(keyVfoC,             &freq, wxT("0.0"));
	m_parameters->m_vfoC.setFrequency(freq);

	profile->Read(keyVfoD,             &freq, wxT("0.0"));
	m_parameters->m_vfoD.setFrequency(freq);

	int num;
	profile->Read(keyFreqShift,        &num, 0);
	m_parameters->m_freqShift = num;

	profile->Read(keyFreqOffset,       &m_parameters->m_freqOffset, 0.0);

	profile->Read(keyVfoChoice,        &m_parameters->m_vfoChoice, VFO_A);

	profile->Read(keyVfoSplitShift,    &m_parameters->m_vfoSplitShift, VFO_NONE);

	profile->Read(keyVfoSpeedFM,       &m_parameters->m_vfoSpeedFM,  SPEED_VERYFAST);
	profile->Read(keyVfoSpeedAM,       &m_parameters->m_vfoSpeedAM,  SPEED_MEDIUM);
	profile->Read(keyVfoSpeedSSB,      &m_parameters->m_vfoSpeedSSB, SPEED_SLOW);
	profile->Read(keyVfoSpeedCWW,      &m_parameters->m_vfoSpeedCWW, SPEED_SLOW);
	profile->Read(keyVfoSpeedCWN,      &m_parameters->m_vfoSpeedCWN, SPEED_VERYSLOW);

	profile->Read(keyDevFMW,           &m_parameters->m_deviationFMW, DEVIATION_5000);
	profile->Read(keyDevFMN,           &m_parameters->m_deviationFMN, DEVIATION_2500);

	profile->Read(keyAgcAM,            &m_parameters->m_agcAM,  AGC_SLOW);
	profile->Read(keyAgcSSB,           &m_parameters->m_agcSSB, AGC_SLOW);
	profile->Read(keyAgcCW,            &m_parameters->m_agcCW,  AGC_FAST);

	profile->Read(keyStepVeryFast,     &m_parameters->m_stepVeryFast, FREQ_VERY_FAST_STEP);
	profile->Read(keyStepFast,         &m_parameters->m_stepFast,     FREQ_FAST_STEP);
	profile->Read(keyStepMedium,       &m_parameters->m_stepMedium,   FREQ_MEDIUM_STEP);
	profile->Read(keyStepSlow,         &m_parameters->m_stepSlow,     FREQ_SLOW_STEP);
	profile->Read(keyStepVerySlow,     &m_parameters->m_stepVerySlow, FREQ_VERY_SLOW_STEP);

	profile->Read(keyMode,             &m_parameters->m_mode, MODE_USB);
	profile->Read(keyZeroIF,           &m_parameters->m_zeroIF, true);
	profile->Read(keySwapIQ,           &m_parameters->m_swapIQ, false);
	profile->Read(keyClockTune,        &num, 0L);
	m_parameters->m_clockTune = num;

	profile->Read(keyFilter,           &m_parameters->m_filter,    FILTER_AUTO);
	profile->Read(keyFilterFMW,        &m_parameters->m_filterFMW, FILTER_20000);
	profile->Read(keyFilterFMN,        &m_parameters->m_filterFMN, FILTER_10000);
	profile->Read(keyFilterAM,         &m_parameters->m_filterAM,  FILTER_6000);
	profile->Read(keyFilterSSB,        &m_parameters->m_filterSSB, FILTER_2100);
	profile->Read(keyFilterCWW,        &m_parameters->m_filterCWW, FILTER_2100);
	profile->Read(keyFilterCWN,        &m_parameters->m_filterCWN, FILTER_500);

	profile->Read(keyIpAddress,        &m_parameters->m_ipAddress);

	profile->Read(keyControlPort,      &num);
	m_parameters->m_controlPort = num;

	profile->Read(keyDataPort,         &num);
	m_parameters->m_dataPort = num;

	profile->Read(keyControlDevice,    &m_parameters->m_controlDevice);
	profile->Read(keyControlPin,       &m_parameters->m_controlPin);

	profile->Read(keyUserAudioAPI,     &m_parameters->m_userAudioAPI);
	profile->Read(keyUserAudioInDev,   &m_parameters->m_userAudioInDev);
	profile->Read(keyUserAudioOutDev,  &m_parameters->m_userAudioOutDev);

	profile->Read(keySDRAudioAPI,      &m_parameters->m_sdrAudioAPI);
	profile->Read(keySDRAudioInDev,    &m_parameters->m_sdrAudioInDev);
	profile->Read(keySDRAudioOutDev,   &m_parameters->m_sdrAudioOutDev);

	profile->Read(keyRitCtrl,          &m_parameters->m_ritOn,   false);
	profile->Read(keyRitValue,         &m_parameters->m_ritFreq, 0);

	profile->Read(keyNbCtrl,           &m_parameters->m_nbOn,    false);
	profile->Read(keyNbValue,          &num, 20);
	m_parameters->m_nbValue = num;

	profile->Read(keyNb2Ctrl,          &m_parameters->m_nb2On,   false);
	profile->Read(keyNb2Value,         &num, 15);
	m_parameters->m_nb2Value = num;

	profile->Read(keySpCtrl,           &m_parameters->m_spOn,    false);
	profile->Read(keySpValue,          &num, 3);
	m_parameters->m_spValue = num;

	profile->Read(keyCarrierLevel,     &num, 100);
	m_parameters->m_carrierLevel = num;

	profile->Read(keyAlcAttack,        &num, 2);
	m_parameters->m_alcAttack = num;

	profile->Read(keyAlcDecay,         &num, 10);
	m_parameters->m_alcDecay = num;

	profile->Read(keyAlcHang,          &num, 500);
	m_parameters->m_alcHang = num;

	profile->Read(keyRxMeter,          &m_parameters->m_rxMeter, METER_SIGNAL);
	profile->Read(keyTxMeter,          &m_parameters->m_txMeter, METER_POWER);

	profile->Read(keySpectrumPos,      &m_parameters->m_spectrumPos,   SPECTRUM_PRE_FILT);
	profile->Read(keySpectrumType,     &m_parameters->m_spectrumType,  SPECTRUM_PANADAPTER1);
	profile->Read(keySpectrumSpeed,    &m_parameters->m_spectrumSpeed, SPECTRUM_100MS);
	profile->Read(keySpectrumDB,       &m_parameters->m_spectrumDB,    SPECTRUM_40DB);

	profile->Read(keyRxIqPhase,        &m_parameters->m_rxIQphase, 0);
	profile->Read(keyRxIqGain,         &m_parameters->m_rxIQgain,  0);
	profile->Read(keyTxIqPhase,        &m_parameters->m_txIQphase, 0);
	profile->Read(keyTxIqGain,         &m_parameters->m_txIQgain,  0);

	profile->Read(keyAfGain,           &num, 0);
	m_parameters->m_afGain = num;

	profile->Read(keyRfGain,           &num, 1000);
	m_parameters->m_rfGain = num;

	profile->Read(keySquelch,          &num, 0);
	m_parameters->m_squelch = num;

	profile->Read(keyMicGain,          &num, 0);
	m_parameters->m_micGain = num;

	profile->Read(keyPower,            &num, 0);
	m_parameters->m_power = num;

	profile->Read(keyCwSpeed,          &num, KEYER_SPEED);
	m_parameters->m_cwSpeed = num;

	profile->Read(keyCwLocal,          &m_parameters->m_cwLocal,   wxEmptyString);
	profile->Read(keyCwRemote,         &m_parameters->m_cwRemote,  wxEmptyString);
	profile->Read(keyCwLocator,        &m_parameters->m_cwLocator, wxEmptyString);
	profile->Read(keyCwReport,         &m_parameters->m_cwReport,  KEYER_REPORT);
	profile->Read(keyCwSerial,         &m_parameters->m_cwSerial,  KEYER_SERIAL);

	for (int n = 0; n < CWKEYBOARD_COUNT; n++)
		profile->Read(keyCwMessage[n], &m_parameters->m_cwMessage[n], KEYER_MESSAGE[n]);

	profile->Read(keyVoiceDir,         &m_parameters->m_voiceDir,  wxEmptyString);

	for (int m = 0; m < VOICEKEYER_COUNT; m++)
		profile->Read(keyVoiceFile[m], &m_parameters->m_voiceFile[m], wxEmptyString);

	profile->Flush();

	delete profile;

	return true;
}

void CUWSDRApp::writeConfig()
{
	wxString keyMaxRxFreq     = wxT("/") + m_parameters->m_name + KEY_MAX_RX_FREQ;
	wxString keyMinRxFreq     = wxT("/") + m_parameters->m_name + KEY_MIN_RX_FREQ;
	wxString keyMaxTxFreq     = wxT("/") + m_parameters->m_name + KEY_MAX_TX_FREQ;
	wxString keyMinTxFreq     = wxT("/") + m_parameters->m_name + KEY_MIN_TX_FREQ;
	wxString keyVfoA          = wxT("/") + m_parameters->m_name + KEY_VFO_A;
	wxString keyVfoB          = wxT("/") + m_parameters->m_name + KEY_VFO_B;
	wxString keyVfoC          = wxT("/") + m_parameters->m_name + KEY_VFO_C;
	wxString keyVfoD          = wxT("/") + m_parameters->m_name + KEY_VFO_D;
	wxString keyFreqShift     = wxT("/") + m_parameters->m_name + KEY_FREQ_SHIFT;
	wxString keyFreqOffset    = wxT("/") + m_parameters->m_name + KEY_FREQ_OFFSET;
	wxString keyVfoChoice     = wxT("/") + m_parameters->m_name + KEY_VFO_CHOICE;
	wxString keyVfoSplitShift = wxT("/") + m_parameters->m_name + KEY_VFO_SPLIT_SHIFT;
	wxString keyVfoSpeedFM    = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_FM;
	wxString keyVfoSpeedAM    = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_AM;
	wxString keyVfoSpeedSSB   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_SSB;
	wxString keyVfoSpeedCWW   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWW;
	wxString keyVfoSpeedCWN   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWN;
	wxString keyDevFMW        = wxT("/") + m_parameters->m_name + KEY_DEV_FMW;
	wxString keyDevFMN        = wxT("/") + m_parameters->m_name + KEY_DEV_FMN;
	wxString keyAgcAM         = wxT("/") + m_parameters->m_name + KEY_AGC_AM;
	wxString keyAgcSSB        = wxT("/") + m_parameters->m_name + KEY_AGC_SSB;
	wxString keyAgcCW         = wxT("/") + m_parameters->m_name + KEY_AGC_CW;
	wxString keyStepVeryFast  = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_FAST;
	wxString keyStepFast      = wxT("/") + m_parameters->m_name + KEY_STEP_FAST;
	wxString keyStepMedium    = wxT("/") + m_parameters->m_name + KEY_STEP_MEDIUM;
	wxString keyStepSlow      = wxT("/") + m_parameters->m_name + KEY_STEP_SLOW;
	wxString keyStepVerySlow  = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_SLOW;
	wxString keyMode          = wxT("/") + m_parameters->m_name + KEY_MODE;
	wxString keyZeroIF        = wxT("/") + m_parameters->m_name + KEY_ZERO_IF;
	wxString keySwapIQ        = wxT("/") + m_parameters->m_name + KEY_SWAP_IQ;
	wxString keyClockTune     = wxT("/") + m_parameters->m_name + KEY_CLOCK_TUNE;
	wxString keyFilter        = wxT("/") + m_parameters->m_name + KEY_FILTER;
	wxString keyFilterFMW     = wxT("/") + m_parameters->m_name + KEY_FILTER_FMW;
	wxString keyFilterFMN     = wxT("/") + m_parameters->m_name + KEY_FILTER_FMN;
	wxString keyFilterAM      = wxT("/") + m_parameters->m_name + KEY_FILTER_AM;
	wxString keyFilterSSB     = wxT("/") + m_parameters->m_name + KEY_FILTER_SSB;
	wxString keyFilterCWW     = wxT("/") + m_parameters->m_name + KEY_FILTER_CWW;
	wxString keyFilterCWN     = wxT("/") + m_parameters->m_name + KEY_FILTER_CWN;
	wxString keyRitCtrl       = wxT("/") + m_parameters->m_name + KEY_RIT_CTRL;
	wxString keyRitValue      = wxT("/") + m_parameters->m_name + KEY_RIT_VALUE;
	wxString keyNbCtrl        = wxT("/") + m_parameters->m_name + KEY_NB_CTRL;
	wxString keyNbValue       = wxT("/") + m_parameters->m_name + KEY_NB_VALUE;
	wxString keyNb2Ctrl       = wxT("/") + m_parameters->m_name + KEY_NB2_CTRL;
	wxString keyNb2Value      = wxT("/") + m_parameters->m_name + KEY_NB2_VALUE;
	wxString keySpCtrl        = wxT("/") + m_parameters->m_name + KEY_SP_CTRL;
	wxString keySpValue       = wxT("/") + m_parameters->m_name + KEY_SP_VALUE;
	wxString keyCarrierLevel  = wxT("/") + m_parameters->m_name + KEY_CARRIER_LEVEL;
	wxString keyAlcAttack     = wxT("/") + m_parameters->m_name + KEY_ALC_ATTACK;
	wxString keyAlcDecay      = wxT("/") + m_parameters->m_name + KEY_ALC_DECAY;
	wxString keyAlcHang       = wxT("/") + m_parameters->m_name + KEY_ALC_HANG;
	wxString keyRxMeter       = wxT("/") + m_parameters->m_name + KEY_RX_METER;
	wxString keyTxMeter       = wxT("/") + m_parameters->m_name + KEY_TX_METER;
	wxString keySpectrumPos   = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_POS;
	wxString keySpectrumType  = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_TYPE;
	wxString keySpectrumSpeed = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_SPEED;
	wxString keySpectrumDB    = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_DB;
	wxString keyRxIqPhase     = wxT("/") + m_parameters->m_name + KEY_RX_IQ_PHASE;
	wxString keyRxIqGain      = wxT("/") + m_parameters->m_name + KEY_RX_IQ_GAIN;
	wxString keyTxIqPhase     = wxT("/") + m_parameters->m_name + KEY_TX_IQ_PHASE;
	wxString keyTxIqGain      = wxT("/") + m_parameters->m_name + KEY_TX_IQ_GAIN;
	wxString keyAfGain        = wxT("/") + m_parameters->m_name + KEY_AF_GAIN;
	wxString keyRfGain        = wxT("/") + m_parameters->m_name + KEY_RF_GAIN;
	wxString keySquelch       = wxT("/") + m_parameters->m_name + KEY_SQUELCH;
	wxString keyMicGain       = wxT("/") + m_parameters->m_name + KEY_MIC_GAIN;
	wxString keyPower         = wxT("/") + m_parameters->m_name + KEY_POWER;
	wxString keyCwSpeed       = wxT("/") + m_parameters->m_name + KEY_CW_SPEED;
	wxString keyCwLocal       = wxT("/") + m_parameters->m_name + KEY_CW_LOCAL;
	wxString keyCwRemote      = wxT("/") + m_parameters->m_name + KEY_CW_REMOTE;
	wxString keyCwLocator     = wxT("/") + m_parameters->m_name + KEY_CW_LOCATOR;
	wxString keyCwReport      = wxT("/") + m_parameters->m_name + KEY_CW_REPORT;
	wxString keyCwSerial      = wxT("/") + m_parameters->m_name + KEY_CW_SERIAL;
	wxString keyVoiceDir      = wxT("/") + m_parameters->m_name + KEY_VOICE_DIR;

	wxString keyCwMessage[CWKEYBOARD_COUNT];
	for (int i = 0; i < CWKEYBOARD_COUNT; i++) {
		wxString number;
		number.Printf(wxT("%d"), i);

		keyCwMessage[i] = wxT("/") + m_parameters->m_name + KEY_CW_MESSAGE;
		keyCwMessage[i].Append(number);
	}

	wxString keyVoiceFile[VOICEKEYER_COUNT];
	for (int j = 0; j < VOICEKEYER_COUNT; j++) {
		wxString number;
		number.Printf(wxT("%d"), j);

		keyVoiceFile[j] = wxT("/") + m_parameters->m_name + KEY_VOICE_FILE;
		keyVoiceFile[j].Append(number);
	}

	wxConfig* profile = new wxConfig(APPNAME);
	wxASSERT(profile != NULL);

	profile->Write(keyMaxRxFreq,        m_parameters->m_maxReceiveFreq.getString());
	profile->Write(keyMinRxFreq,        m_parameters->m_minReceiveFreq.getString());
	profile->Write(keyMaxTxFreq,        m_parameters->m_maxTransmitFreq.getString());
	profile->Write(keyMinTxFreq,        m_parameters->m_minTransmitFreq.getString());
	profile->Write(keyVfoA,             m_parameters->m_vfoA.getString());
	profile->Write(keyVfoB,             m_parameters->m_vfoB.getString());
	profile->Write(keyVfoC,             m_parameters->m_vfoC.getString());
	profile->Write(keyVfoD,             m_parameters->m_vfoD.getString());
	profile->Write(keyFreqShift,        int(m_parameters->m_freqShift));
	profile->Write(keyFreqOffset,       m_parameters->m_freqOffset);
	profile->Write(keyVfoChoice,        m_parameters->m_vfoChoice);
	profile->Write(keyVfoSplitShift,    m_parameters->m_vfoSplitShift);
	profile->Write(keyVfoSpeedFM,       m_parameters->m_vfoSpeedFM);
	profile->Write(keyVfoSpeedAM,       m_parameters->m_vfoSpeedAM);
	profile->Write(keyVfoSpeedSSB,      m_parameters->m_vfoSpeedSSB);
	profile->Write(keyVfoSpeedCWW,      m_parameters->m_vfoSpeedCWW);
	profile->Write(keyVfoSpeedCWN,      m_parameters->m_vfoSpeedCWN);
	profile->Write(keyDevFMW,           m_parameters->m_deviationFMW);
	profile->Write(keyDevFMN,           m_parameters->m_deviationFMN);
	profile->Write(keyAgcAM,            m_parameters->m_agcAM);
	profile->Write(keyAgcSSB,           m_parameters->m_agcSSB);
	profile->Write(keyAgcCW,            m_parameters->m_agcCW);
	profile->Write(keyStepVeryFast,     m_parameters->m_stepVeryFast);
	profile->Write(keyStepFast,         m_parameters->m_stepFast);
	profile->Write(keyStepMedium,       m_parameters->m_stepMedium);
	profile->Write(keyStepSlow,         m_parameters->m_stepSlow);
	profile->Write(keyStepVerySlow,     m_parameters->m_stepVerySlow);
	profile->Write(keyMode,             m_parameters->m_mode);
	profile->Write(keyZeroIF,           m_parameters->m_zeroIF);
	profile->Write(keySwapIQ,           m_parameters->m_swapIQ);
	profile->Write(keyClockTune,        int(m_parameters->m_clockTune));
	profile->Write(keyFilter,           m_parameters->m_filter);
	profile->Write(keyFilterFMW,        m_parameters->m_filterFMW);
	profile->Write(keyFilterFMN,        m_parameters->m_filterFMN);
	profile->Write(keyFilterAM,         m_parameters->m_filterAM);
	profile->Write(keyFilterSSB,        m_parameters->m_filterSSB);
	profile->Write(keyFilterCWW,        m_parameters->m_filterCWW);
	profile->Write(keyFilterCWN,        m_parameters->m_filterCWN);
	profile->Write(keyRitCtrl,          m_parameters->m_ritOn);
	profile->Write(keyRitValue,         m_parameters->m_ritFreq);
	profile->Write(keyNbCtrl,           m_parameters->m_nbOn);
	profile->Write(keyNbValue,          int(m_parameters->m_nbValue));
	profile->Write(keyNb2Ctrl,          m_parameters->m_nb2On);
	profile->Write(keyNb2Value,         int(m_parameters->m_nb2Value));
	profile->Write(keySpCtrl,           m_parameters->m_spOn);
	profile->Write(keySpValue,          int(m_parameters->m_spValue));
	profile->Write(keyCarrierLevel,     int(m_parameters->m_carrierLevel));
	profile->Write(keyAlcAttack,        int(m_parameters->m_alcAttack));
	profile->Write(keyAlcDecay,         int(m_parameters->m_alcDecay));
	profile->Write(keyAlcHang,          int(m_parameters->m_alcHang));
	profile->Write(keyRxMeter,          m_parameters->m_rxMeter);
	profile->Write(keyTxMeter,          m_parameters->m_txMeter);
	profile->Write(keySpectrumPos,      m_parameters->m_spectrumPos);
	profile->Write(keySpectrumType,     m_parameters->m_spectrumType);
	profile->Write(keySpectrumSpeed,    m_parameters->m_spectrumSpeed);
	profile->Write(keySpectrumDB,       m_parameters->m_spectrumDB);
	profile->Write(keyRxIqPhase,        m_parameters->m_rxIQphase);
	profile->Write(keyRxIqGain,         m_parameters->m_rxIQgain);
	profile->Write(keyTxIqPhase,        m_parameters->m_txIQphase);
	profile->Write(keyTxIqGain,         m_parameters->m_txIQgain);
	profile->Write(keyAfGain,           int(m_parameters->m_afGain));
	profile->Write(keyRfGain,           int(m_parameters->m_rfGain));
	profile->Write(keySquelch,          int(m_parameters->m_squelch));
	profile->Write(keyMicGain,          int(m_parameters->m_micGain));
	profile->Write(keyPower,            int(m_parameters->m_power));
	profile->Write(keyCwSpeed,          int(m_parameters->m_cwSpeed));
	profile->Write(keyCwLocal,          m_parameters->m_cwLocal);
	profile->Write(keyCwRemote,         m_parameters->m_cwRemote);
	profile->Write(keyCwLocator,        m_parameters->m_cwLocator);
	profile->Write(keyCwReport,         m_parameters->m_cwReport);
	profile->Write(keyCwSerial,         m_parameters->m_cwSerial);

	for (int n = 0; n < CWKEYBOARD_COUNT; n++)
		profile->Write(keyCwMessage[n], m_parameters->m_cwMessage[n]);

	profile->Write(keyVoiceDir,         m_parameters->m_voiceDir);

	for (int m = 0; m < CWKEYBOARD_COUNT; m++)
		profile->Write(keyVoiceFile[m], m_parameters->m_voiceFile[m]);

	profile->Flush();

	delete profile;
}

void CUWSDRApp::showHelp(int id)
{
	m_help->Display(id);
}

void CUWSDRApp::sendCW(unsigned int speed, const wxString& text)
{
	m_frame->sendCW(speed, text);
}

void CUWSDRApp::sendAudio(const wxString& fileName, int state)
{
	m_frame->sendAudio(fileName, state);
}

wxString CUWSDRApp::getHelpDir()
{
#if defined(__WXMSW__)
	wxConfig* config = new wxConfig(APPNAME);
	wxASSERT(config != NULL);

	wxString dir;
	bool ret = config->Read(wxT("/InstPath"), &dir);

	if (!ret) {
		delete config;
		return wxEmptyString;
	}

	delete config;

	return dir;
#elif defined(__WXGTK__)
	return DATA_DIR;
#elif defined(__WXMAC__)
	return wxEmptyString;
#else
#error "Unknown platform"
#endif
}

#if defined(__WXDEBUG__)
void CUWSDRApp::OnAssertFailure(const wxChar* file, int line, const wxChar* func, const wxChar* cond, const wxChar* msg)
{
	::wxLogFatalError(wxT("Assertion failed on line %d of %s in file %s: %s %s"), line, func, file, cond, msg);
}
#endif
