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

#include "SDRParameters.h"

CSDRParameters::CSDRParameters() :
m_hardwareName(),
m_maxHardwareFreq(),
m_minHardwareFreq(),
m_hardwareStepSize(0.0F),
m_hardwareSampleRate(0.0F),
m_hardwareProtocolVersion(0),
m_hardwareReceiveOnly(true),
m_fileName(),
m_name(),
m_maxReceiveFreq(),
m_minReceiveFreq(),
m_maxTransmitFreq(),
m_minTransmitFreq(),
m_vfoA(),
m_vfoB(),
m_vfoC(),
m_vfoD(),
m_shift(0),
m_vfoChoice(0),
m_vfoSplitShift(0),
m_vfoSpeedFM(0),
m_vfoSpeedAM(0),
m_vfoSpeedSSB(0),
m_vfoSpeedCWW(0),
m_vfoSpeedCWN(0),
m_stepVeryFast(0.0),
m_stepFast(0.0),
m_stepMedium(0.0),
m_stepSlow(0.0),
m_stepVerySlow(0.0),
m_mode(0),
m_deviationFMW(0),
m_deviationFMN(0),
m_agcAM(0),
m_agcSSB(0),
m_agcCW(0),
m_filter(0),
m_filterFMW(0),
m_filterFMN(0),
m_filterAM(0),
m_filterSSB(0),
m_filterCWW(0),
m_filterCWN(0),
m_ipAddress(),
m_controlPort(0),
m_dataPort(0),
m_audioAPI(0),
m_audioInDev(0),
m_audioOutDev(0),
m_ritOn(false),
m_ritFreq(0),
m_nbOn(false),
m_nbValue(0),
m_nb2On(false),
m_nb2Value(0),
m_spOn(false),
m_spValue(0),
m_rxMeter(0),
m_txMeter(0),
m_spectrumPos(0),
m_spectrumType(0),
m_spectrumSpeed(0), 
m_rxIQphase(0),
m_rxIQgain(0),
m_txIQphase(0),
m_txIQgain(0),
m_afGain(0),
m_rfGain(1000),
m_squelch(0),
m_micGain(0),
m_power(0),
m_cwSpeed(0),
m_cwLocal(),
m_cwRemote(),
m_cwLocator(),
m_cwReport(),
m_cwSerial(),
m_cwMessage(),
m_voiceDir(),
m_voiceFile()
{
}

CSDRParameters::~CSDRParameters()
{
}
