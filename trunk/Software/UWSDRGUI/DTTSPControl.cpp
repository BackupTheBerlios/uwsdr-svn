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

#include "DTTSPControl.h"

#include "UWSDRDefs.h"
#include "DTTSPExports.h"

CDTTSPControl::CDTTSPControl() :
wxThread(),
m_sampleRate(0),
m_blockSize(0),
m_filter(-1),
m_mode(-1),
m_rxFreq(99999.9F),
m_txFreq(99999.9F),
m_transmit(false),
m_agc(-1),
m_nb(false),
m_nbValue(0),
m_nb2(false),
m_nb2Value(0),
m_sp(false),
m_spValue(0),
m_rxPhase(0),
m_rxGain(0),
m_txPhase(0),
m_txGain(0),
m_squelch(999999),
m_started(false)
{
}

CDTTSPControl::~CDTTSPControl()
{
}

void CDTTSPControl::open(unsigned int sampleRate, unsigned int blockSize)
{
	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	::Setup_SDR(REAL(sampleRate), blockSize);

	::Release_Update();

	::SetDCBlock(true);

	::SetSquelchState(true);

    ::SetWindow(HANN_WINDOW);

	Create();

	SetPriority(WXTHREAD_MAX_PRIORITY);

	Run();
}

void* CDTTSPControl::Entry()
{
	m_started = true;

	::process_samples_thread();

	m_started = false;

	return (void*)0;
}


void CDTTSPControl::setFilter(int filter)
{
	m_filter = filter;

	normaliseFilter();
}

void CDTTSPControl::setMode(int mode)
{
	if (mode == m_mode)
		return;

	switch (mode) {
		case MODE_FMW:
			::SetMode(FMN);      // XXX FIXME
         ::SetDeviation(5000.0F);
			break;
		case MODE_FMN:
			::SetMode(FMN);
         ::SetDeviation(2500.0F);
			break;
		case MODE_AM:
			::SetMode(AM);
			break;
		case MODE_USB:
			::SetMode(USB);
			break;
		case MODE_LSB:
			::SetMode(LSB);
			break;
		case MODE_CWW:
		case MODE_CWN:
			::SetMode(CWU);
			break;
		default:
			::wxLogError(_("Unknown mode value = %d"), mode);
			return;
	}

	m_mode = mode;

	normaliseFilter();
}

void CDTTSPControl::setTXAndFreq(bool transmit, float freq)
{
	if (transmit) {
		if (freq != m_txFreq)
			::SetTXOsc(freq);

		if (transmit != m_transmit)
			::SetTRX(TX);

		m_txFreq   = freq;
		m_transmit = true;
	} else {
		if (freq != m_rxFreq)
			::SetOsc(freq);

		if (transmit != m_transmit)
			::SetTRX(RX);

		m_rxFreq   = freq;
		m_transmit = false;
	}
}

void CDTTSPControl::setAGC(int agc)
{
	if (agc == m_agc)
		return;

	switch (agc) {
		case AGC_FAST:
			::SetRXAGC(agcFAST);
			break;
		case AGC_MEDIUM:
			::SetRXAGC(agcMED);
			break;
		case AGC_SLOW:
			::SetRXAGC(agcSLOW);
			break;
		case AGC_NONE:
			::SetRXAGC(agcOFF);
			break;
		default:
			::wxLogError(_("Unknown AGC value = %d"), agc);
			return;
	}

	m_agc = agc;
}

void CDTTSPControl::setNB(bool onOff)
{
	if (onOff == m_nb)
		return;

	::SetNB(onOff);

	m_nb = onOff;
}

void CDTTSPControl::setNBValue(unsigned int value)
{
	if (value == m_nbValue)
		return;

	::SetNBvals(REAL(value));

	m_nbValue = value;
}

void CDTTSPControl::setNB2(bool onOff)
{
	if (onOff == m_nb2)
		return;

	::SetSDROM(onOff);

	m_nb2 = onOff;
}

void CDTTSPControl::setNB2Value(unsigned int value)
{
	if (value == m_nb2Value)
		return;

	::SetSDROMvals(REAL(value));

	m_nb2Value = value;
}

void CDTTSPControl::setSP(bool onOff)
{
	if (onOff == m_sp)
		return;

	::SetTXAGCFF(onOff);

	m_sp = onOff;
}

void CDTTSPControl::setSPValue(unsigned int value)
{
	if (value == m_spValue)
		return;

	::SetTXAGCFFCompression(REAL(value));

	m_spValue = value;
}

void CDTTSPControl::setRXIAndQ(int phase, int gain)
{
	if (phase == m_rxPhase && gain == m_rxGain)
		return;

	::SetCorrectIQ(phase, gain);

	m_rxPhase = phase;
	m_rxGain  = gain;
}

void CDTTSPControl::setTXIAndQ(int phase, int gain)
{
	if (phase == m_txPhase && gain == m_txGain)
		return;

	::SetCorrectTXIQ(phase, gain);

	m_txPhase = phase;
	m_txGain  = gain;
}

void CDTTSPControl::setSquelch(unsigned int value)
{
	if (value == m_squelch)
		return;

	// Map 0 - 1000 to -200.0 - 0.0
	float sql = float(value) / 5.0F - 200.0F;

	::SetSquelchVal(sql);

	m_squelch = value;
}

float CDTTSPControl::getMeter(int type)
{
	if (!m_started)
		return -200.0F;

	switch (type) {
		case METER_SIGNAL:
			return ::Calculate_Meters(SIGNAL_STRENGTH);
		case METER_AVG_SIGNAL:
			return ::Calculate_Meters(AVG_SIGNAL_STRENGTH);
		case METER_AGC:
			return ::Calculate_Meters(AGC_GAIN);
		case METER_MICROPHONE:
			return ::Calculate_Meters(MIC);
		case METER_POWER:
			return ::Calculate_Meters(PWR);
		case METER_ALC:
			return ::Calculate_Meters(ALC);
		default:
			::wxLogError(_("Unknown meter type = %d"), type);
			return -200.0F;
	}
}

void CDTTSPControl::getSpectrum(float* spectrum, int pos)
{
	wxASSERT(spectrum != NULL);

	if (!m_started)
		return;

	if (!m_transmit) {
		switch (pos) {
			case SPECTRUM_PRE_FILT:
				::Process_Panadapter(spectrum);
				break;
			case SPECTRUM_POST_FILT:
				::Process_Spectrum(spectrum);
				break;
			default:
				::wxLogError(_("Unknown spectrum position = %d"), pos);
				break;
		}
	} else {
		::Process_Spectrum(spectrum);
	}
}

void CDTTSPControl::dataIO(const float* input, float* output, unsigned int nSamples)
{
	if (m_started)
		::Audio_CallbackIL((float*)input, output, nSamples);
}

void CDTTSPControl::close()
{
	::Destroy_SDR();
}

void CDTTSPControl::normaliseFilter()
{
	if (m_filter == -1)
		return;

	int width = 0;
	switch (m_filter) {
		case FILTER_20000:
			width = 20000;
			break;
		case FILTER_15000:
			width = 15000;
			break;
		case FILTER_10000:
			width = 10000;
			break;
		case FILTER_6000:
			width = 6000;
			break;
		case FILTER_4000:
			width = 4000;
			break;
		case FILTER_2600:
			width = 2600;
			break;
		case FILTER_2100:
			width = 2100;
			break;
		case FILTER_1000:
			width = 1000;
			break;
		case FILTER_500:
			width = 500;
			break;
		case FILTER_250:
			width = 250;
			break;
		case FILTER_100:
			width = 100;
			break;
		case FILTER_50:
			width = 50;
			break;
		case FILTER_25:
			width = 25;
			break;
		default:
			::wxLogError(_("Unknown filter value = %d"), m_filter);
			return;
	}

	int low  = 0;
	int high = 0;
	switch (m_mode) {
		case MODE_FMW:
      case MODE_FMN:
		case MODE_AM:
			high = width / 2;
			low  = -width / 2;
			break;
		case MODE_USB:
			switch (m_filter) {
				case FILTER_20000:
				case FILTER_15000:
				case FILTER_10000:
				case FILTER_6000:
				case FILTER_4000:
					high = width + 100;
					low  = 100;
					break;
				case FILTER_2600:
				case FILTER_2100:
				case FILTER_1000:
					high = width + 200;
					low  = 200;
					break;
				case FILTER_500:
					high = 850;
					low  = 350;
					break;
				case FILTER_250:
					high = 725;
					low  = 475;
					break;
				case FILTER_100:
					high = 650;
					low  = 550;
					break;
				case FILTER_50:
					high = 625;
					low  = 575;
					break;
				case FILTER_25:
					high = 613;
					low  = 587;
					break;
			}
			break;
		case MODE_LSB:
			switch (m_filter) {
				case FILTER_20000:
				case FILTER_15000:
				case FILTER_10000:
				case FILTER_6000:
				case FILTER_4000:
					high = -100;
					low  = -width - 100;
					break;
				case FILTER_2600:
				case FILTER_2100:
				case FILTER_1000:
					high = -200;
					low  = -width - 200;
					break;
				case FILTER_500:
					high = -350;
					low  = -850;
					break;
				case FILTER_250:
					high = -475;
					low  = -725;
					break;
				case FILTER_100:
					high = -550;
					low  = -650;
					break;
				case FILTER_50:
					high = -575;
					low  = -625;
					break;
				case FILTER_25:
					high = -587;
					low  = -613;
					break;
			}
			break;
		case MODE_CWW:
		case MODE_CWN:
			switch (m_filter) {
				case FILTER_20000:
				case FILTER_15000:
				case FILTER_10000:
				case FILTER_6000:
				case FILTER_4000:
				case FILTER_2600:
				case FILTER_2100:
					high = width + 100;
					low  = 100;
					break;
				case FILTER_1000:
					high = 1300;
					low  = 300;
					break;
				case FILTER_500:
					high = 1050;
					low  = 550;
					break;
				case FILTER_250:
					high = 925;
					low  = 675;
					break;
				case FILTER_100:
					high = 850;
					low  = 750;
					break;
				case FILTER_50:
					high = 825;
					low  = 775;
					break;
				case FILTER_25:
					high = 813;
					low  = 788;
					break;
			}
			break;
	}

	::SetFilter(double(low), double(high), m_blockSize, RX);
	::SetFilter(double(low), double(high), m_blockSize, TX);	// FIXME
}
