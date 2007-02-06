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

#ifndef	SDRParameters_H
#define	SDRParameters_H

#include <wx/wx.h>

#include "UWSDRDefs.h"
#include "Frequency.h"
#include "SDRDescrFile.h"

class CSDRParameters {

    public:
	CSDRParameters();
	~CSDRParameters();

	wxString     m_hardwareName;
	int			 m_hardwareType;
	CFrequency   m_hardwareMaxFreq;
	CFrequency   m_hardwareMinFreq;
	float        m_hardwareStepSize;
	float        m_hardwareSampleRate;
	bool         m_hardwareReceiveOnly;

	wxString     m_fileName;
	wxString     m_name;
	CFrequency   m_maxReceiveFreq;
	CFrequency   m_minReceiveFreq;
	CFrequency   m_maxTransmitFreq;
	CFrequency   m_minTransmitFreq;

	CFrequency   m_vfoA;
	CFrequency   m_vfoB;
	CFrequency   m_vfoC;
	CFrequency   m_vfoD;
	unsigned int m_freqShift;
	double       m_freqOffset;
	int          m_vfoChoice;
	int          m_vfoSplitShift;

	int          m_vfoSpeedFM;
	int          m_vfoSpeedAM;
	int          m_vfoSpeedSSB;
	int          m_vfoSpeedCWW;
	int          m_vfoSpeedCWN;

	double       m_stepVeryFast;
	double       m_stepFast;
	double       m_stepMedium;
	double       m_stepSlow;
	double       m_stepVerySlow;

	int          m_mode;
	bool         m_zeroIF;
	bool         m_swapIQ;
	unsigned int m_clockTune;

	int          m_deviationFMW;
	int          m_deviationFMN;
	int          m_agcAM;
	int          m_agcSSB;
	int          m_agcCW;

	int          m_filter;
	int          m_filterFMW;
	int          m_filterFMN;
	int          m_filterAM;
	int          m_filterSSB;
	int          m_filterCWW;
	int          m_filterCWN;

	wxString     m_ipAddress;
	unsigned int m_controlPort;
	unsigned int m_dataPort;

	bool         m_txInEnable;
	wxString     m_txInDev;
	int          m_txInPin;

	bool         m_keyInEnable;
	wxString     m_keyInDev;
	int          m_keyInPin;

	wxString     m_txOutDev;
	int          m_txOutPin;

	int          m_userAudioAPI;
	int          m_userAudioInDev;
	int          m_userAudioOutDev;

	int          m_sdrAudioAPI;
	int          m_sdrAudioInDev;
	int          m_sdrAudioOutDev;

	bool         m_ritOn;
	int          m_ritFreq;

	bool         m_nbOn;
	unsigned int m_nbValue;
	bool         m_nb2On;
	unsigned int m_nb2Value;

	bool         m_spOn;
	unsigned int m_spValue;
	unsigned int m_carrierLevel;

	unsigned int m_alcAttack;
	unsigned int m_alcDecay;
	unsigned int m_alcHang;

	int          m_rxMeter;
	int          m_txMeter;

	int          m_spectrumPos;
	int          m_spectrumType;
	int          m_spectrumSpeed;
	int          m_spectrumDB;

	int          m_rxIQphase;
	int          m_rxIQgain;
	int          m_txIQphase;
	int          m_txIQgain;

	unsigned int m_afGain;
	unsigned int m_rfGain;
	unsigned int m_squelch;
	unsigned int m_micGain;
	unsigned int m_power;

	unsigned int m_cwSpeed;
	wxString     m_cwLocal;
	wxString     m_cwRemote;
	wxString     m_cwLocator;
	wxString     m_cwReport;
	wxString     m_cwSerial;
	wxString     m_cwMessage[CWKEYBOARD_COUNT];

	wxString     m_voiceDir;
	wxString     m_voiceFile[VOICEKEYER_COUNT];
};

#endif
