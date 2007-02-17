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

#include "SDRParameters.h"

CSDRParameters::CSDRParameters() :
m_hardwareName(),
m_hardwareType(TYPE_UWSDR1),
m_hardwareMaxFreq(),
m_hardwareMinFreq(),
m_hardwareStepSize(0.0F),
m_hardwareSampleRate(0.0F),
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
m_freqShift(0),
m_freqOffset(0.0),
m_vfoChoice(VFO_A),
m_vfoSplitShift(VFO_NONE),
m_vfoSpeedFM(SPEED_VERYFAST),
m_vfoSpeedAM(SPEED_MEDIUM),
m_vfoSpeedSSB(SPEED_SLOW),
m_vfoSpeedCWW(SPEED_SLOW),
m_vfoSpeedCWN(SPEED_VERYSLOW),
m_stepVeryFast(0.0),
m_stepFast(0.0),
m_stepMedium(0.0),
m_stepSlow(0.0),
m_stepVerySlow(0.0),
m_mode(MODE_USB),
m_zeroIF(false),
m_swapIQ(false),
m_clockTune(99999),
m_deviationFMW(DEVIATION_5000),
m_deviationFMN(DEVIATION_2500),
m_agcAM(AGC_SLOW),
m_agcSSB(AGC_SLOW),
m_agcCW(AGC_FAST),
m_filter(FILTER_AUTO),
m_filterFMW(FILTER_20000),
m_filterFMN(FILTER_10000),
m_filterAM(FILTER_6000),
m_filterSSB(FILTER_2100),
m_filterCWW(FILTER_2100),
m_filterCWN(FILTER_500),
m_ipAddress(),
m_controlPort(0),
m_dataPort(0),
m_txInEnable(false),
m_txInDev(),
m_txInPin(IN_NONE),
m_keyInEnable(false),
m_keyInDev(),
m_keyInPin(IN_NONE),
m_txOutDev(),
m_txOutPin(OUT_NONE),
m_userAudioInDev(-1),
m_userAudioOutDev(-1),
m_sdrAudioInDev(-1),
m_sdrAudioOutDev(-1),
m_ritOn(false),
m_ritFreq(0),
m_nbOn(false),
m_nbValue(0),
m_nb2On(false),
m_nb2Value(0),
m_spOn(false),
m_spValue(0),
m_carrierLevel(100),
m_alcAttack(0),
m_alcDecay(0),
m_alcHang(0),
m_rxMeter(METER_SIGNAL),
m_txMeter(METER_POWER),
m_spectrumPos(SPECTRUM_PRE_FILTER),
m_spectrumType(SPECTRUM_PANADAPTER1),
m_spectrumSpeed(SPECTRUM_100MS),
m_spectrumDB(SPECTRUM_40DB),
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
