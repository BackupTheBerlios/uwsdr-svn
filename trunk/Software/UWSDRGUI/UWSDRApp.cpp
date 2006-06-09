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

#include <wx/config.h>
#include <wx/cmdline.h>

#include "UWSDRApp.h"
#include "UWSDRDefs.h"
#include "SDRDescrFile.h"
#include "Log.h"

const wxString KEY_FILE_NAME       = wxT("/FileName");
const wxString KEY_MAX_RX_FREQ     = wxT("/MaxReceiveFreq");
const wxString KEY_MIN_RX_FREQ     = wxT("/MinReceiveFreq");
const wxString KEY_MAX_TX_FREQ     = wxT("/MaxTransmitFreq");
const wxString KEY_MIN_TX_FREQ     = wxT("/MinTransmitFreq");
const wxString KEY_VFO_A           = wxT("/VfoA");
const wxString KEY_VFO_B           = wxT("/VfoB");
const wxString KEY_VFO_C           = wxT("/VfoC");
const wxString KEY_VFO_D           = wxT("/VfoD");
const wxString KEY_SHIFT           = wxT("/Shift");
const wxString KEY_VFO_CHOICE      = wxT("/VfoChoice");
const wxString KEY_VFO_SPLIT_SHIFT = wxT("/VfoSplitShift");
const wxString KEY_VFO_SPEED_FM    = wxT("/VfoSpeedFM");
const wxString KEY_VFO_SPEED_AM    = wxT("/VfoSpeedAM");
const wxString KEY_VFO_SPEED_SSB   = wxT("/VfoSpeedSSB");
const wxString KEY_VFO_SPEED_CWW   = wxT("/VfoSpeedCWW");
const wxString KEY_VFO_SPEED_CWN   = wxT("/VfoSpeedCWN");
const wxString KEY_STEP_VERY_FAST  = wxT("/StepVeryFast");
const wxString KEY_STEP_FAST       = wxT("/StepFast");
const wxString KEY_STEP_MEDIUM     = wxT("/StepMedium");
const wxString KEY_STEP_SLOW       = wxT("/StepSlow");
const wxString KEY_STEP_VERY_SLOW  = wxT("/StepVerySlow");
const wxString KEY_MODE            = wxT("/Mode");
const wxString KEY_AGC_AM          = wxT("/AGCAM");
const wxString KEY_AGC_SSB         = wxT("/AGCSSB");
const wxString KEY_AGC_CW          = wxT("/AGCCW");
const wxString KEY_FILTER          = wxT("/Filter");
const wxString KEY_FILTER_FM       = wxT("/FilterFM");
const wxString KEY_FILTER_AM       = wxT("/FilterAM");
const wxString KEY_FILTER_SSB      = wxT("/FilterSSB");
const wxString KEY_FILTER_CWW      = wxT("/FilterCWW");
const wxString KEY_FILTER_CWN      = wxT("/FilterCWN");
const wxString KEY_IP_ADDRESS      = wxT("/IPAddress");
const wxString KEY_CONTROL_PORT    = wxT("/ControlPort");
const wxString KEY_DATA_PORT       = wxT("/DataPort");
const wxString KEY_AUDIO_API       = wxT("/AudioAPI");
const wxString KEY_AUDIO_IN_DEV    = wxT("/AudioInDev");
const wxString KEY_AUDIO_OUT_DEV   = wxT("/AudioOutDev");
const wxString KEY_RIT_CTRL        = wxT("/RitCtrl");
const wxString KEY_RIT_VALUE       = wxT("/RitFreq");
const wxString KEY_NB_CTRL         = wxT("/NoiseBlankerCtrl");
const wxString KEY_NB_VALUE        = wxT("/NoiseBlankerValue");
const wxString KEY_NB2_CTRL        = wxT("/NoiseBlanker2Ctrl");
const wxString KEY_NB2_VALUE       = wxT("/NoiseBlanker2Value");
const wxString KEY_SP_CTRL         = wxT("/SpeechProcCtrl");
const wxString KEY_SP_VALUE        = wxT("/SpeechprocValue");
const wxString KEY_RX_METER        = wxT("/ReceiveMeter");
const wxString KEY_TX_METER        = wxT("/TransmitMeter");
const wxString KEY_SPECTRUM_POS    = wxT("/SpectrumPos");
const wxString KEY_SPECTRUM_TYPE   = wxT("/SpectrumType");
const wxString KEY_SPECTRUM_SPEED  = wxT("/SpectrumSpeed");
const wxString KEY_RX_IQ_PHASE     = wxT("/ReceiveIQPhase");
const wxString KEY_RX_IQ_GAIN      = wxT("/ReceiveIQGain");
const wxString KEY_TX_IQ_PHASE     = wxT("/TransmitIQPhase");
const wxString KEY_TX_IQ_GAIN      = wxT("/TransmitIQGain");
const wxString KEY_AF_GAIN         = wxT("/AFGain");
const wxString KEY_SQUELCH         = wxT("/Squelch");
const wxString KEY_MIC_GAIN        = wxT("/MicGain");
const wxString KEY_POWER           = wxT("/Power");

IMPLEMENT_APP(CUWSDRApp)

CUWSDRApp::CUWSDRApp() :
wxApp(),
m_frame(NULL),
m_parameters(NULL)
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

	wxLog* logger = new CLog(m_parameters->m_name + ".log");
	wxLog::SetActiveTarget(logger);

	::wxLogMessage(_("Starting ") + VERSION);
	::wxLogMessage(_("Using configuration: ") + m_parameters->m_name);

	if (!readConfig()) {
		::wxLogError(_("Cannot open the SDR configuration - ") + m_parameters->m_name);
		::wxMessageBox(_("Cannot open the SDR configuration - ") + m_parameters->m_name);
		return false;
	}

	if (!readDescrFile()) {
		::wxLogError(_("Cannot open the SDR description file - ") + m_parameters->m_fileName);
		::wxMessageBox(_("Cannot open the SDR description file - ") + m_parameters->m_fileName);
		return false;
	}

	wxString title = VERSION + wxT(" - ") + m_parameters->m_name;

	if (m_parameters->m_hardwareReceiveOnly)
		title.Append(wxT(" - Receive Only"));

	m_frame = new CUWSDRFrame(title);
	m_frame->Show();

	SetTopWindow(m_frame);

	// Sanity checking for the frequencies
	if (m_parameters->m_maxReceiveFreq > m_parameters->m_maxHardwareFreq)
		m_parameters->m_maxReceiveFreq = m_parameters->m_maxHardwareFreq;
	if (m_parameters->m_minReceiveFreq < m_parameters->m_minHardwareFreq)
		m_parameters->m_minReceiveFreq = m_parameters->m_minHardwareFreq;
	if (m_parameters->m_maxTransmitFreq > m_parameters->m_maxReceiveFreq)
		m_parameters->m_maxTransmitFreq = m_parameters->m_maxReceiveFreq;
	if (m_parameters->m_minTransmitFreq < m_parameters->m_minReceiveFreq)
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
	parser.AddSwitch(wxT("s"), wxEmptyString, _("Switch SDR Control off"));

	parser.AddParam(_("SDR configuration name"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_OPTION_MANDATORY);

	wxApp::OnInitCmdLine(parser);
}

bool CUWSDRApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
	if (!wxApp::OnCmdLineParsed(parser))
		return false;

	m_parameters->m_name = parser.GetParam(0);

	m_parameters->m_sdrEnabled = !parser.Found(wxT("s"));

	return true;
}

int CUWSDRApp::OnExit()
{
	::wxLogMessage(_("Ending uWave SDR"));

	writeConfig();

	return 0;
}

bool CUWSDRApp::readDescrFile()
{
	wxASSERT(m_parameters != NULL);

	CSDRDescrFile descrFile(m_parameters->m_fileName);

	if (!descrFile.isValid())
		return false;

	m_parameters->m_hardwareName            = descrFile.getName();
	m_parameters->m_maxHardwareFreq         = descrFile.getMaxFreq();
	m_parameters->m_minHardwareFreq         = descrFile.getMinFreq();
	m_parameters->m_hardwareStepSize        = descrFile.getStepSize();
	m_parameters->m_hardwareSampleRate      = descrFile.getSampleRate();
	m_parameters->m_hardwareProtocolVersion = descrFile.getProtocolVersion();
	m_parameters->m_hardwareReceiveOnly     = descrFile.getReceiveOnly();

	return true;
}

bool CUWSDRApp::readConfig()
{
	wxString keyFileName      = wxT("/") + m_parameters->m_name + KEY_FILE_NAME;
	wxString keyMaxRxFreq     = wxT("/") + m_parameters->m_name + KEY_MAX_RX_FREQ;
	wxString keyMinRxFreq     = wxT("/") + m_parameters->m_name + KEY_MIN_RX_FREQ;
	wxString keyMaxTxFreq     = wxT("/") + m_parameters->m_name + KEY_MAX_TX_FREQ;
	wxString keyMinTxFreq     = wxT("/") + m_parameters->m_name + KEY_MIN_TX_FREQ;
	wxString keyVfoA          = wxT("/") + m_parameters->m_name + KEY_VFO_A;
	wxString keyVfoB          = wxT("/") + m_parameters->m_name + KEY_VFO_B;
	wxString keyVfoC          = wxT("/") + m_parameters->m_name + KEY_VFO_C;
	wxString keyVfoD          = wxT("/") + m_parameters->m_name + KEY_VFO_D;
	wxString keyShift         = wxT("/") + m_parameters->m_name + KEY_SHIFT;
	wxString keyVfoChoice     = wxT("/") + m_parameters->m_name + KEY_VFO_CHOICE;
	wxString keyVfoSplitShift = wxT("/") + m_parameters->m_name + KEY_VFO_SPLIT_SHIFT;
	wxString keyVfoSpeedFM    = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_FM;
	wxString keyVfoSpeedAM    = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_AM;
	wxString keyVfoSpeedSSB   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_SSB;
	wxString keyVfoSpeedCWW   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWW;
	wxString keyVfoSpeedCWN   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWN;
	wxString keyAgcAM         = wxT("/") + m_parameters->m_name + KEY_AGC_AM;
	wxString keyAgcSSB        = wxT("/") + m_parameters->m_name + KEY_AGC_SSB;
	wxString keyAgcCW         = wxT("/") + m_parameters->m_name + KEY_AGC_CW;
	wxString keyStepVeryFast  = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_FAST;
	wxString keyStepFast      = wxT("/") + m_parameters->m_name + KEY_STEP_FAST;
	wxString keyStepMedium    = wxT("/") + m_parameters->m_name + KEY_STEP_MEDIUM;
	wxString keyStepSlow      = wxT("/") + m_parameters->m_name + KEY_STEP_SLOW;
	wxString keyStepVerySlow  = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_SLOW;
	wxString keyMode          = wxT("/") + m_parameters->m_name + KEY_MODE;
	wxString keyFilter        = wxT("/") + m_parameters->m_name + KEY_FILTER;
	wxString keyFilterFM      = wxT("/") + m_parameters->m_name + KEY_FILTER_FM;
	wxString keyFilterAM      = wxT("/") + m_parameters->m_name + KEY_FILTER_AM;
	wxString keyFilterSSB     = wxT("/") + m_parameters->m_name + KEY_FILTER_SSB;
	wxString keyFilterCWW     = wxT("/") + m_parameters->m_name + KEY_FILTER_CWW;
	wxString keyFilterCWN     = wxT("/") + m_parameters->m_name + KEY_FILTER_CWN;
	wxString keyIpAddress     = wxT("/") + m_parameters->m_name + KEY_IP_ADDRESS;
	wxString keyControlPort   = wxT("/") + m_parameters->m_name + KEY_CONTROL_PORT;
	wxString keyDataPort      = wxT("/") + m_parameters->m_name + KEY_DATA_PORT;
	wxString keyAudioAPI      = wxT("/") + m_parameters->m_name + KEY_AUDIO_API;
	wxString keyAudioInDev    = wxT("/") + m_parameters->m_name + KEY_AUDIO_IN_DEV;
	wxString keyAudioOutDev   = wxT("/") + m_parameters->m_name + KEY_AUDIO_OUT_DEV;
	wxString keyRitCtrl       = wxT("/") + m_parameters->m_name + KEY_RIT_CTRL;
	wxString keyRitValue      = wxT("/") + m_parameters->m_name + KEY_RIT_VALUE;
	wxString keyNbCtrl        = wxT("/") + m_parameters->m_name + KEY_NB_CTRL;
	wxString keyNbValue       = wxT("/") + m_parameters->m_name + KEY_NB_VALUE;
	wxString keyNb2Ctrl       = wxT("/") + m_parameters->m_name + KEY_NB2_CTRL;
	wxString keyNb2Value      = wxT("/") + m_parameters->m_name + KEY_NB2_VALUE;
	wxString keySpCtrl        = wxT("/") + m_parameters->m_name + KEY_SP_CTRL;
	wxString keySpValue       = wxT("/") + m_parameters->m_name + KEY_SP_VALUE;
	wxString keyRxMeter       = wxT("/") + m_parameters->m_name + KEY_RX_METER;
	wxString keyTxMeter       = wxT("/") + m_parameters->m_name + KEY_TX_METER;
	wxString keySpectrumPos   = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_POS;
	wxString keySpectrumType  = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_TYPE;
	wxString keySpectrumSpeed = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_SPEED;
	wxString keyRxIqPhase     = wxT("/") + m_parameters->m_name + KEY_RX_IQ_PHASE;
	wxString keyRxIqGain      = wxT("/") + m_parameters->m_name + KEY_RX_IQ_GAIN;
	wxString keyTxIqPhase     = wxT("/") + m_parameters->m_name + KEY_TX_IQ_PHASE;
	wxString keyTxIqGain      = wxT("/") + m_parameters->m_name + KEY_TX_IQ_GAIN;
	wxString keyAfGain        = wxT("/") + m_parameters->m_name + KEY_AF_GAIN;
	wxString keySquelch       = wxT("/") + m_parameters->m_name + KEY_SQUELCH;
	wxString keyMicGain       = wxT("/") + m_parameters->m_name + KEY_MIC_GAIN;
	wxString keyPower         = wxT("/") + m_parameters->m_name + KEY_POWER;

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
	profile->Read(keyShift,            &num, 0);
	m_parameters->m_shift = num;

	profile->Read(keyVfoChoice,        &m_parameters->m_vfoChoice, VFO_A);

	profile->Read(keyVfoSplitShift,    &m_parameters->m_vfoSplitShift, VFO_NONE);

	profile->Read(keyVfoSpeedFM,       &m_parameters->m_vfoSpeedFM,  SPEED_VERYFAST);
	profile->Read(keyVfoSpeedAM,       &m_parameters->m_vfoSpeedAM,  SPEED_MEDIUM);
	profile->Read(keyVfoSpeedSSB,      &m_parameters->m_vfoSpeedSSB, SPEED_SLOW);
	profile->Read(keyVfoSpeedCWW,      &m_parameters->m_vfoSpeedCWW, SPEED_SLOW);
	profile->Read(keyVfoSpeedCWN,      &m_parameters->m_vfoSpeedCWN, SPEED_VERYSLOW);

	profile->Read(keyAgcAM,            &m_parameters->m_agcAM,  AGC_SLOW);
	profile->Read(keyAgcSSB,           &m_parameters->m_agcSSB, AGC_SLOW);
	profile->Read(keyAgcCW,            &m_parameters->m_agcCW,  AGC_FAST);

	profile->Read(keyStepVeryFast,     &m_parameters->m_stepVeryFast, FREQ_VERY_FAST_STEP);
	profile->Read(keyStepFast,         &m_parameters->m_stepFast,     FREQ_FAST_STEP);
	profile->Read(keyStepMedium,       &m_parameters->m_stepMedium,   FREQ_MEDIUM_STEP);
	profile->Read(keyStepSlow,         &m_parameters->m_stepSlow,     FREQ_SLOW_STEP);
	profile->Read(keyStepVerySlow,     &m_parameters->m_stepVerySlow, FREQ_VERY_SLOW_STEP);

	profile->Read(keyMode,             &m_parameters->m_mode, MODE_USB);

	profile->Read(keyFilter,           &m_parameters->m_filter,    FILTER_AUTO);
	profile->Read(keyFilterFM,         &m_parameters->m_filterFM,  FILTER_10000);
	profile->Read(keyFilterAM,         &m_parameters->m_filterAM,  FILTER_6000);
	profile->Read(keyFilterSSB,        &m_parameters->m_filterSSB, FILTER_2100);
	profile->Read(keyFilterCWW,        &m_parameters->m_filterCWW, FILTER_2100);
	profile->Read(keyFilterCWN,        &m_parameters->m_filterCWN, FILTER_500);

	profile->Read(keyIpAddress,        &m_parameters->m_ipAddress);

	profile->Read(keyControlPort,      &num);
	m_parameters->m_controlPort = num;

	profile->Read(keyDataPort,         &num);
	m_parameters->m_dataPort = num;

	profile->Read(keyAudioAPI,         &m_parameters->m_audioAPI);
	profile->Read(keyAudioInDev,       &m_parameters->m_audioInDev);
	profile->Read(keyAudioOutDev,      &m_parameters->m_audioOutDev);

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

	profile->Read(keyRxMeter,          &m_parameters->m_rxMeter, METER_SIGNAL);
	profile->Read(keyTxMeter,          &m_parameters->m_txMeter, METER_POWER);

	profile->Read(keySpectrumPos,      &m_parameters->m_spectrumPos,   SPECTRUM_PRE_FILT);
	profile->Read(keySpectrumType,     &m_parameters->m_spectrumType,  SPECTRUM_PANADAPTER);
	profile->Read(keySpectrumSpeed,    &m_parameters->m_spectrumSpeed, SPECTRUM_100MS);

	profile->Read(keyRxIqPhase,        &m_parameters->m_rxIQphase, 0);
	profile->Read(keyRxIqGain,         &m_parameters->m_rxIQgain,  0);
	profile->Read(keyTxIqPhase,        &m_parameters->m_txIQphase, 0);
	profile->Read(keyTxIqGain,         &m_parameters->m_txIQgain,  0);

	profile->Read(keyAfGain,           &num, 0);
	m_parameters->m_afGain = num;

	profile->Read(keySquelch,          &num, 0);
	m_parameters->m_squelch = num;

	profile->Read(keyMicGain,          &num, 0);
	m_parameters->m_micGain = num;

	profile->Read(keyPower,            &num, 0);
	m_parameters->m_power = num;

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
	wxString keyShift         = wxT("/") + m_parameters->m_name + KEY_SHIFT;
	wxString keyVfoChoice     = wxT("/") + m_parameters->m_name + KEY_VFO_CHOICE;
	wxString keyVfoSplitShift = wxT("/") + m_parameters->m_name + KEY_VFO_SPLIT_SHIFT;
	wxString keyVfoSpeedFM    = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_FM;
	wxString keyVfoSpeedAM    = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_AM;
	wxString keyVfoSpeedSSB   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_SSB;
	wxString keyVfoSpeedCWW   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWW;
	wxString keyVfoSpeedCWN   = wxT("/") + m_parameters->m_name + KEY_VFO_SPEED_CWN;
	wxString keyAgcAM         = wxT("/") + m_parameters->m_name + KEY_AGC_AM;
	wxString keyAgcSSB        = wxT("/") + m_parameters->m_name + KEY_AGC_SSB;
	wxString keyAgcCW         = wxT("/") + m_parameters->m_name + KEY_AGC_CW;
	wxString keyStepVeryFast  = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_FAST;
	wxString keyStepFast      = wxT("/") + m_parameters->m_name + KEY_STEP_FAST;
	wxString keyStepMedium    = wxT("/") + m_parameters->m_name + KEY_STEP_MEDIUM;
	wxString keyStepSlow      = wxT("/") + m_parameters->m_name + KEY_STEP_SLOW;
	wxString keyStepVerySlow  = wxT("/") + m_parameters->m_name + KEY_STEP_VERY_SLOW;
	wxString keyMode          = wxT("/") + m_parameters->m_name + KEY_MODE;
	wxString keyFilter        = wxT("/") + m_parameters->m_name + KEY_FILTER;
	wxString keyFilterFM      = wxT("/") + m_parameters->m_name + KEY_FILTER_FM;
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
	wxString keyRxMeter       = wxT("/") + m_parameters->m_name + KEY_RX_METER;
	wxString keyTxMeter       = wxT("/") + m_parameters->m_name + KEY_TX_METER;
	wxString keySpectrumPos   = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_POS;
	wxString keySpectrumType  = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_TYPE;
	wxString keySpectrumSpeed = wxT("/") + m_parameters->m_name + KEY_SPECTRUM_SPEED;
	wxString keyRxIqPhase     = wxT("/") + m_parameters->m_name + KEY_RX_IQ_PHASE;
	wxString keyRxIqGain      = wxT("/") + m_parameters->m_name + KEY_RX_IQ_GAIN;
	wxString keyTxIqPhase     = wxT("/") + m_parameters->m_name + KEY_TX_IQ_PHASE;
	wxString keyTxIqGain      = wxT("/") + m_parameters->m_name + KEY_TX_IQ_GAIN;
	wxString keyAfGain        = wxT("/") + m_parameters->m_name + KEY_AF_GAIN;
	wxString keySquelch       = wxT("/") + m_parameters->m_name + KEY_SQUELCH;
	wxString keyMicGain       = wxT("/") + m_parameters->m_name + KEY_MIC_GAIN;
	wxString keyPower         = wxT("/") + m_parameters->m_name + KEY_POWER;

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
	profile->Write(keyShift,            int(m_parameters->m_shift));
	profile->Write(keyVfoChoice,        m_parameters->m_vfoChoice);
	profile->Write(keyVfoSplitShift,    m_parameters->m_vfoSplitShift);
	profile->Write(keyVfoSpeedFM,       m_parameters->m_vfoSpeedFM);
	profile->Write(keyVfoSpeedAM,       m_parameters->m_vfoSpeedAM);
	profile->Write(keyVfoSpeedSSB,      m_parameters->m_vfoSpeedSSB);
	profile->Write(keyVfoSpeedCWW,      m_parameters->m_vfoSpeedCWW);
	profile->Write(keyVfoSpeedCWN,      m_parameters->m_vfoSpeedCWN);
	profile->Write(keyAgcAM,            m_parameters->m_agcAM);
	profile->Write(keyAgcSSB,           m_parameters->m_agcSSB);
	profile->Write(keyAgcCW,            m_parameters->m_agcCW);
	profile->Write(keyStepVeryFast,     m_parameters->m_stepVeryFast);
	profile->Write(keyStepFast,         m_parameters->m_stepFast);
	profile->Write(keyStepMedium,       m_parameters->m_stepMedium);
	profile->Write(keyStepSlow,         m_parameters->m_stepSlow);
	profile->Write(keyStepVerySlow,     m_parameters->m_stepVerySlow);
	profile->Write(keyMode,             m_parameters->m_mode);
	profile->Write(keyFilter,           m_parameters->m_filter);
	profile->Write(keyFilterFM,         m_parameters->m_filterFM);
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
	profile->Write(keyRxMeter,          m_parameters->m_rxMeter);
	profile->Write(keyTxMeter,          m_parameters->m_txMeter);
	profile->Write(keySpectrumPos,      m_parameters->m_spectrumPos);
	profile->Write(keySpectrumType,     m_parameters->m_spectrumType);
	profile->Write(keySpectrumSpeed,    m_parameters->m_spectrumSpeed);
	profile->Write(keyRxIqPhase,        m_parameters->m_rxIQphase);
	profile->Write(keyRxIqGain,         m_parameters->m_rxIQgain);
	profile->Write(keyTxIqPhase,        m_parameters->m_txIQphase);
	profile->Write(keyTxIqGain,         m_parameters->m_txIQgain);
	profile->Write(keyAfGain,           int(m_parameters->m_afGain));
	profile->Write(keySquelch,          int(m_parameters->m_squelch));
	profile->Write(keyMicGain,          int(m_parameters->m_micGain));
	profile->Write(keyPower,            int(m_parameters->m_power));

	profile->Flush();

	delete profile;
}

void CUWSDRApp::showHelp(const wxString& chapter)
{
}

#ifdef __WXDEBUG__
void CUWSDRApp::OnAssert(const wxChar* file, int line, const wxChar* cond, const wxChar* msg)
{
	::wxLogFatalError(_("Assertion failed on line %d in file %s: %s %s"), line, file, cond, msg);
}
#endif
