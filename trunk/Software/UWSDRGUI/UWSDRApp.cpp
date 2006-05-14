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

const wxString KEY_FILE_NAME       = wxT("FileName");
const wxString KEY_MAX_RX_FREQ     = wxT("MaxReceiveFreq");
const wxString KEY_MIN_RX_FREQ     = wxT("MinReceiveFreq");
const wxString KEY_MAX_TX_FREQ     = wxT("MaxTransmitFreq");
const wxString KEY_MIN_TX_FREQ     = wxT("MinTransmitFreq");
const wxString KEY_VFO_A           = wxT("VfoA");
const wxString KEY_VFO_B           = wxT("VfoB");
const wxString KEY_VFO_C           = wxT("VfoC");
const wxString KEY_VFO_D           = wxT("VfoD");
const wxString KEY_SHIFT           = wxT("Shift");
const wxString KEY_VFO_CHOICE      = wxT("VfoChoice");
const wxString KEY_VFO_SPLIT_SHIFT = wxT("VfoSplitShift");
const wxString KEY_VFO_SPEED_FM    = wxT("VfoSpeedFM");
const wxString KEY_VFO_SPEED_AM    = wxT("VfoSpeedAM");
const wxString KEY_VFO_SPEED_SSB   = wxT("VfoSpeedSSB");
const wxString KEY_VFO_SPEED_CWW   = wxT("VfoSpeedCWW");
const wxString KEY_VFO_SPEED_CWN   = wxT("VfoSpeedCWN");
const wxString KEY_STEP_VERY_FAST  = wxT("StepVeryFast");
const wxString KEY_STEP_FAST       = wxT("StepFast");
const wxString KEY_STEP_MEDIUM     = wxT("StepMedium");
const wxString KEY_STEP_SLOW       = wxT("StepSlow");
const wxString KEY_STEP_VERY_SLOW  = wxT("StepVerySlow");
const wxString KEY_MODE            = wxT("Mode");
const wxString KEY_AGC_AM          = wxT("AGCAM");
const wxString KEY_AGC_SSB         = wxT("AGCSSB");
const wxString KEY_AGC_CW          = wxT("AGCCW");
const wxString KEY_FILTER          = wxT("Filter");
const wxString KEY_FILTER_FM       = wxT("FilterFM");
const wxString KEY_FILTER_AM       = wxT("FilterAM");
const wxString KEY_FILTER_SSB      = wxT("FilterSSB");
const wxString KEY_FILTER_CWW      = wxT("FilterCWW");
const wxString KEY_FILTER_CWN      = wxT("FilterCWN");
const wxString KEY_IP_ADDRESS      = wxT("IPAddress");
const wxString KEY_CONTROL_PORT    = wxT("ControlPort");
const wxString KEY_DATA_PORT       = wxT("DataPort");
const wxString KEY_AUDIO_API       = wxT("AudioAPI");
const wxString KEY_AUDIO_IN_DEV    = wxT("AudioInDev");
const wxString KEY_AUDIO_OUT_DEV   = wxT("AudioOutDev");
const wxString KEY_RIT_CTRL        = wxT("RitCtrl");
const wxString KEY_RIT_VALUE       = wxT("RitFreq");
const wxString KEY_NB_CTRL         = wxT("NoiseBlankerCtrl");
const wxString KEY_NB_VALUE        = wxT("NoiseBlankerValue");
const wxString KEY_NB2_CTRL        = wxT("NoiseBlanker2Ctrl");
const wxString KEY_NB2_VALUE       = wxT("NoiseBlanker2Value");
const wxString KEY_SP_CTRL         = wxT("SpeechProcCtrl");
const wxString KEY_SP_VALUE        = wxT("SpeechprocValue");
const wxString KEY_RX_METER        = wxT("ReceiveMeter");
const wxString KEY_TX_METER        = wxT("TransmitMeter");
const wxString KEY_SPECTRUM_POS    = wxT("SpectrumPos");
const wxString KEY_SPECTRUM_TYPE   = wxT("SpectrumType");
const wxString KEY_SPECTRUM_SPEED  = wxT("SpectrumSpeed");
const wxString KEY_RX_IQ_PHASE     = wxT("ReceiveIQPhase");
const wxString KEY_RX_IQ_GAIN      = wxT("ReceiveIQGain");
const wxString KEY_TX_IQ_PHASE     = wxT("TransmitIQPhase");
const wxString KEY_TX_IQ_GAIN      = wxT("TransmitIQGain");
const wxString KEY_AF_GAIN         = wxT("AFGain");
const wxString KEY_SQUELCH         = wxT("Squelch");
const wxString KEY_MIC_GAIN        = wxT("MicGain");
const wxString KEY_POWER           = wxT("Power");

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

	SetVendorName(APPNAME);
	SetAppName(m_parameters->m_name);

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
	::wxLogMessage(_("Ending µWave SDR"));

	writeConfig();
	delete wxConfigBase::Set(NULL);

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
	wxConfigBase* profile = wxConfigBase::Get();

	wxASSERT(profile != NULL);

	profile->SetRecordDefaults(true);

	if (!profile->Read(KEY_FILE_NAME, &m_parameters->m_fileName)) {
		delete profile;
		return false;
	}

	wxString freq;
	profile->Read(KEY_MAX_RX_FREQ,     &freq, wxT("9999999.0"));
	m_parameters->m_maxReceiveFreq.setFrequency(freq);

	profile->Read(KEY_MIN_RX_FREQ,     &freq, wxT("0.0"));
	m_parameters->m_minReceiveFreq.setFrequency(freq);

	profile->Read(KEY_MAX_TX_FREQ,     &freq, wxT("9999999.0"));
	m_parameters->m_maxTransmitFreq.setFrequency(freq);

	profile->Read(KEY_MIN_TX_FREQ,     &freq, wxT("0.0"));
	m_parameters->m_minTransmitFreq.setFrequency(freq);

	profile->Read(KEY_VFO_A,           &freq, wxT("0.0"));
	m_parameters->m_vfoA.setFrequency(freq);

	profile->Read(KEY_VFO_B,           &freq, wxT("0.0"));
	m_parameters->m_vfoB.setFrequency(freq);

	profile->Read(KEY_VFO_C,           &freq, wxT("0.0"));
	m_parameters->m_vfoC.setFrequency(freq);

	profile->Read(KEY_VFO_D,           &freq, wxT("0.0"));
	m_parameters->m_vfoD.setFrequency(freq);

	int num;
	profile->Read(KEY_SHIFT,           &num, 0);
	m_parameters->m_shift = num;

	profile->Read(KEY_VFO_CHOICE,      &m_parameters->m_vfoChoice, VFO_A);

	profile->Read(KEY_VFO_SPLIT_SHIFT, &m_parameters->m_vfoSplitShift, VFO_NONE);

	profile->Read(KEY_VFO_SPEED_FM,    &m_parameters->m_vfoSpeedFM,  SPEED_VERYFAST);
	profile->Read(KEY_VFO_SPEED_AM,    &m_parameters->m_vfoSpeedAM,  SPEED_MEDIUM);
	profile->Read(KEY_VFO_SPEED_SSB,   &m_parameters->m_vfoSpeedSSB, SPEED_SLOW);
	profile->Read(KEY_VFO_SPEED_CWW,   &m_parameters->m_vfoSpeedCWW, SPEED_SLOW);
	profile->Read(KEY_VFO_SPEED_CWN,   &m_parameters->m_vfoSpeedCWN, SPEED_VERYSLOW);

	profile->Read(KEY_AGC_AM,          &m_parameters->m_agcAM,  AGC_SLOW);
	profile->Read(KEY_AGC_SSB,         &m_parameters->m_agcSSB, AGC_SLOW);
	profile->Read(KEY_AGC_CW,          &m_parameters->m_agcCW,  AGC_FAST);

	profile->Read(KEY_STEP_VERY_FAST,  &m_parameters->m_stepVeryFast, FREQ_VERY_FAST_STEP);
	profile->Read(KEY_STEP_FAST,       &m_parameters->m_stepFast,     FREQ_FAST_STEP);
	profile->Read(KEY_STEP_MEDIUM,     &m_parameters->m_stepMedium,   FREQ_MEDIUM_STEP);
	profile->Read(KEY_STEP_SLOW,       &m_parameters->m_stepSlow,     FREQ_SLOW_STEP);
	profile->Read(KEY_STEP_VERY_SLOW,  &m_parameters->m_stepVerySlow, FREQ_VERY_SLOW_STEP);

	profile->Read(KEY_MODE,            &m_parameters->m_mode, MODE_USB);

	profile->Read(KEY_FILTER,          &m_parameters->m_filter,    FILTER_AUTO);
	profile->Read(KEY_FILTER_FM,       &m_parameters->m_filterFM,  FILTER_10000);
	profile->Read(KEY_FILTER_AM,       &m_parameters->m_filterAM,  FILTER_6000);
	profile->Read(KEY_FILTER_SSB,      &m_parameters->m_filterSSB, FILTER_2100);
	profile->Read(KEY_FILTER_CWW,      &m_parameters->m_filterCWW, FILTER_2100);
	profile->Read(KEY_FILTER_CWN,      &m_parameters->m_filterCWN, FILTER_500);

	profile->Read(KEY_IP_ADDRESS,      &m_parameters->m_ipAddress);

	profile->Read(KEY_CONTROL_PORT,    &num);
	m_parameters->m_controlPort = num;

	profile->Read(KEY_DATA_PORT,       &num);
	m_parameters->m_dataPort = num;

	profile->Read(KEY_AUDIO_API,       &m_parameters->m_audioAPI);
	profile->Read(KEY_AUDIO_IN_DEV,    &m_parameters->m_audioInDev);
	profile->Read(KEY_AUDIO_OUT_DEV,   &m_parameters->m_audioOutDev);

	profile->Read(KEY_RIT_CTRL,        &m_parameters->m_ritOn,   false);
	profile->Read(KEY_RIT_VALUE,       &m_parameters->m_ritFreq, 0);

	profile->Read(KEY_NB_CTRL,         &m_parameters->m_nbOn,    false);
	profile->Read(KEY_NB_VALUE,        &num, 20);
	m_parameters->m_nbValue = num;

	profile->Read(KEY_NB2_CTRL,        &m_parameters->m_nb2On,   false);
	profile->Read(KEY_NB2_VALUE,       &num, 15);
	m_parameters->m_nb2Value = num;

    profile->Read(KEY_SP_CTRL,         &m_parameters->m_spOn,    false);
	profile->Read(KEY_SP_VALUE,        &num, 3);
	m_parameters->m_spValue = num;

	profile->Read(KEY_RX_METER,        &m_parameters->m_rxMeter, METER_SIGNAL);
	profile->Read(KEY_TX_METER,        &m_parameters->m_txMeter, METER_POWER);

	profile->Read(KEY_SPECTRUM_POS,    &m_parameters->m_spectrumPos,   SPECTRUM_PRE_FILT);
	profile->Read(KEY_SPECTRUM_TYPE,   &m_parameters->m_spectrumType,  SPECTRUM_PANADAPTER);
	profile->Read(KEY_SPECTRUM_SPEED,  &m_parameters->m_spectrumSpeed, SPECTRUM_100MS);

	profile->Read(KEY_RX_IQ_PHASE,     &m_parameters->m_rxIQphase, 0);
	profile->Read(KEY_RX_IQ_GAIN,      &m_parameters->m_rxIQgain,  0);
	profile->Read(KEY_TX_IQ_PHASE,     &m_parameters->m_txIQphase, 0);
	profile->Read(KEY_TX_IQ_GAIN,      &m_parameters->m_txIQgain,  0);

	profile->Read(KEY_AF_GAIN,         &num, 0);
	m_parameters->m_afGain = num;

	profile->Read(KEY_SQUELCH,         &num, 0);
	m_parameters->m_squelch = num;

	profile->Read(KEY_MIC_GAIN,        &num, 0);
	m_parameters->m_micGain = num;

	profile->Read(KEY_POWER,           &num, 0);
	m_parameters->m_power = num;

	profile->Flush();

	return true;
}

void CUWSDRApp::writeConfig()
{
	wxConfigBase* profile = wxConfigBase::Get();

	wxASSERT(profile != NULL);

	profile->Write(KEY_MAX_RX_FREQ,     m_parameters->m_maxReceiveFreq.getString());
	profile->Write(KEY_MIN_RX_FREQ,     m_parameters->m_minReceiveFreq.getString());
	profile->Write(KEY_MAX_TX_FREQ,     m_parameters->m_maxTransmitFreq.getString());
	profile->Write(KEY_MIN_TX_FREQ,     m_parameters->m_minTransmitFreq.getString());
	profile->Write(KEY_VFO_A,           m_parameters->m_vfoA.getString());
	profile->Write(KEY_VFO_B,           m_parameters->m_vfoB.getString());
	profile->Write(KEY_VFO_C,           m_parameters->m_vfoC.getString());
	profile->Write(KEY_VFO_D,           m_parameters->m_vfoD.getString());
	profile->Write(KEY_SHIFT,           int(m_parameters->m_shift));
	profile->Write(KEY_VFO_CHOICE,      m_parameters->m_vfoChoice);
	profile->Write(KEY_VFO_SPLIT_SHIFT, m_parameters->m_vfoSplitShift);
	profile->Write(KEY_VFO_SPEED_FM,    m_parameters->m_vfoSpeedFM);
	profile->Write(KEY_VFO_SPEED_AM,    m_parameters->m_vfoSpeedAM);
	profile->Write(KEY_VFO_SPEED_SSB,   m_parameters->m_vfoSpeedSSB);
	profile->Write(KEY_VFO_SPEED_CWW,   m_parameters->m_vfoSpeedCWW);
	profile->Write(KEY_VFO_SPEED_CWN,   m_parameters->m_vfoSpeedCWN);
	profile->Write(KEY_AGC_AM,          m_parameters->m_agcAM);
	profile->Write(KEY_AGC_SSB,         m_parameters->m_agcSSB);
	profile->Write(KEY_AGC_CW,          m_parameters->m_agcCW);
	profile->Write(KEY_STEP_VERY_FAST,  m_parameters->m_stepVeryFast);
	profile->Write(KEY_STEP_FAST,       m_parameters->m_stepFast);
	profile->Write(KEY_STEP_MEDIUM,     m_parameters->m_stepMedium);
	profile->Write(KEY_STEP_SLOW,       m_parameters->m_stepSlow);
	profile->Write(KEY_STEP_VERY_SLOW,  m_parameters->m_stepVerySlow);
	profile->Write(KEY_MODE,            m_parameters->m_mode);
	profile->Write(KEY_FILTER,          m_parameters->m_filter);
	profile->Write(KEY_FILTER_FM,       m_parameters->m_filterFM);
	profile->Write(KEY_FILTER_AM,       m_parameters->m_filterAM);
	profile->Write(KEY_FILTER_SSB,      m_parameters->m_filterSSB);
	profile->Write(KEY_FILTER_CWW,      m_parameters->m_filterCWW);
	profile->Write(KEY_FILTER_CWN,      m_parameters->m_filterCWN);
	profile->Write(KEY_RIT_CTRL,        m_parameters->m_ritOn);
	profile->Write(KEY_RIT_VALUE,       m_parameters->m_ritFreq);
	profile->Write(KEY_NB_CTRL,         m_parameters->m_nbOn);
	profile->Write(KEY_NB_VALUE,        int(m_parameters->m_nbValue));
	profile->Write(KEY_NB2_CTRL,        m_parameters->m_nb2On);
	profile->Write(KEY_NB2_VALUE,       int(m_parameters->m_nb2Value));
	profile->Write(KEY_SP_CTRL,         m_parameters->m_spOn);
	profile->Write(KEY_SP_VALUE,        int(m_parameters->m_spValue));
	profile->Write(KEY_RX_METER,        m_parameters->m_rxMeter);
	profile->Write(KEY_TX_METER,        m_parameters->m_txMeter);
	profile->Write(KEY_SPECTRUM_POS,    m_parameters->m_spectrumPos);
	profile->Write(KEY_SPECTRUM_TYPE,   m_parameters->m_spectrumType);
	profile->Write(KEY_SPECTRUM_SPEED,  m_parameters->m_spectrumSpeed);
	profile->Write(KEY_RX_IQ_PHASE,     m_parameters->m_rxIQphase);
	profile->Write(KEY_RX_IQ_GAIN,      m_parameters->m_rxIQgain);
	profile->Write(KEY_TX_IQ_PHASE,     m_parameters->m_txIQphase);
	profile->Write(KEY_TX_IQ_GAIN,      m_parameters->m_txIQgain);
	profile->Write(KEY_AF_GAIN,         int(m_parameters->m_afGain));
	profile->Write(KEY_SQUELCH,         int(m_parameters->m_squelch));
	profile->Write(KEY_MIC_GAIN,        int(m_parameters->m_micGain));
	profile->Write(KEY_POWER,           int(m_parameters->m_power));

	profile->Flush();
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
