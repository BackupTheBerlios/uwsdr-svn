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
m_sampleRate(0.0F),
m_blockSize(0),
m_filter(-1),
m_mode(-1),
m_rxFreq(99999.9F),
m_txFreq(99999.9F),
m_rit(99999.9F),
m_transmit(false),
m_deviation(-1),
m_agc(-1),
m_nb(false),
m_nbValue(0),
m_nb2(false),
m_nb2Value(0),
m_sp(false),
m_spValue(0),
m_attack(0),
m_decay(0),
m_hang(0),
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

void CDTTSPControl::open(float sampleRate, unsigned int blockSize)
{
	m_sampleRate = sampleRate;
	m_blockSize  = blockSize;

	::Setup_SDR(sampleRate, blockSize);

	::Release_Update();

	::SetDCBlock(true);

	::SetTXSquelchSt(false);

	::SetTXLevelerSt(false);

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
			::SetMode(FMN);
			break;
		case MODE_FMN:
			::SetMode(FMN);
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
			::wxLogError(wxT("Unknown mode value = %d"), mode);
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

void CDTTSPControl::setRIT(float freq)
{
	if (freq == m_rit)
		return;

	::SetRIT(freq);

	m_rit = freq;
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
			::wxLogError(wxT("Unknown AGC value = %d"), agc);
			return;
	}

	m_agc = agc;
}

void CDTTSPControl::setDeviation(int dev)
{
	if (dev == m_deviation)
		return;

	switch (dev) {
		case DEVIATION_6000:
			::SetDeviation(6000.0F);
			break;
		case DEVIATION_5000:
			::SetDeviation(5000.0F);
			break;
		case DEVIATION_3000:
			::SetDeviation(3000.0F);
			break;
		case DEVIATION_2500:
			::SetDeviation(2500.0F);
			break;
		case DEVIATION_2000:
			::SetDeviation(2000.0F);
			break;
		default:
			::wxLogError(wxT("Unknown Deviation value = %d"), dev);
			return;
	}

	m_deviation = dev;
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

	::SetNBvals(float(value));

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

	::SetSDROMvals(float(value));

	m_nb2Value = value;
}

void CDTTSPControl::setSP(bool onOff)
{
	if (onOff == m_sp)
		return;

	::SetTXCompressionSt(onOff);

	m_sp = onOff;
}

void CDTTSPControl::setSPValue(unsigned int value)
{
	if (value == m_spValue)
		return;

	::SetTXCompressionLevel(float(value));

	m_spValue = value;
}

void CDTTSPControl::setALCValue(unsigned int attack, unsigned int decay, unsigned int hang)
{
	if (attack == m_attack && decay == m_decay && hang == m_hang)
		return;

	::SetTXALCAttack(float(attack));
	::SetTXALCDecay(float(decay));
	::SetTXALCHang(float(hang));

	m_attack = attack;
	m_decay  = decay;
	m_hang   = hang;
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

	float val = -200.0F;

	switch (type) {
		case METER_I_INPUT:
			val = ::Calculate_Meters(ADC_REAL);
			if (val != -200.0F)
				val += 55.0F;
			break;
		case METER_Q_INPUT:
			val = ::Calculate_Meters(ADC_IMAG);
			if (val != -200.0F)
				val += 55.0F;
			break;
		case METER_SIGNAL:
			val = ::Calculate_Meters(SIGNAL_STRENGTH);
			if (val != -200.0F)
				val += 45.0F;
			break;
		case METER_AVG_SIGNAL:
			val = ::Calculate_Meters(AVG_SIGNAL_STRENGTH);
			if (val != -200.0F)
				val += 45.0F;
			break;
		case METER_AGC:
			val = ::Calculate_Meters(AGC_GAIN);
			if (val != -200.0F)
				val -= 62.0F;
			break;
		case METER_MICROPHONE:
			val = ::Calculate_Meters(MIC);
			if (val != -200.0F)
				val -= 10.0F;
			break;
		case METER_POWER:
			val = ::Calculate_Meters(PWR);
			if (val != -200.0F)
				val *= 94.0F;
			break;
		case METER_ALC:
			val = ::Calculate_Meters(ALC);
			if (val != -200.0F)
				val -= 10.0F;
			break;
		default:
			::wxLogError(wxT("Unknown meter type = %d"), type);
			break;
	}

	return val;
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
			case SPECTRUM_POST_AGC:
				::Process_Scope(spectrum, SPECTRUM_SIZE);
				break;
			default:
				::wxLogError(wxT("Unknown spectrum position = %d"), pos);
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

	double width = 0.0;
	switch (m_filter) {
		case FILTER_20000:
			width = 20000.0;
			break;
		case FILTER_15000:
			width = 15000.0;
			break;
		case FILTER_10000:
			width = 10000.0;
			break;
		case FILTER_6000:
			width = 6000.0;
			break;
		case FILTER_4000:
			width = 4000.0;
			break;
		case FILTER_2600:
			width = 2600.0;
			break;
		case FILTER_2100:
			width = 2100.0;
			break;
		case FILTER_1000:
			width = 1000.0;
			break;
		case FILTER_500:
			width = 500.0;
			break;
		case FILTER_250:
			width = 250.0;
			break;
		case FILTER_100:
			width = 100.0;
			break;
		case FILTER_50:
			width = 50.0;
			break;
		case FILTER_25:
			width = 25.0;
			break;
		default:
			::wxLogError(wxT("Unknown filter value = %d"), m_filter);
			return;
	}

	double rxLow = 0.0, rxHigh = 0.0;
	double txLow = 0.0, txHigh = 0.0;
	switch (m_mode) {
		case MODE_FMW:
		case MODE_FMN:
		case MODE_AM:
			txHigh = rxHigh =  width / 2.0;
			txLow  = rxLow  = -width / 2.0;
			break;
		case MODE_USB:
			switch (m_filter) {
				case FILTER_20000:
				case FILTER_15000:
				case FILTER_10000:
				case FILTER_6000:
				case FILTER_4000:
					txHigh = rxHigh = width + 100.0;
					txLow  = rxLow  = 100.0;
					break;
				case FILTER_2600:
				case FILTER_2100:
				case FILTER_1000:
					txHigh = rxHigh = width + 200.0;
					txLow  = rxLow  = 200.0;
					break;
				case FILTER_500:
					txHigh = rxHigh = 850.0;
					txLow  = rxLow  = 350.0;
					break;
				case FILTER_250:
					txHigh = rxHigh = 725.0;
					txLow  = rxLow  = 475.0;
					break;
				case FILTER_100:
					txHigh = rxHigh = 650.0;
					txLow  = rxLow  = 550.0;
					break;
				case FILTER_50:
					txHigh = rxHigh = 625.0;
					txLow  = rxLow  = 575.0;
					break;
				case FILTER_25:
					txHigh = rxHigh = 613.0;
					txLow  = rxLow  = 587.0;
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
					txHigh = rxHigh = -100.0;
					txLow  = rxLow  = -width - 100.0;
					break;
				case FILTER_2600:
				case FILTER_2100:
				case FILTER_1000:
					txHigh = rxHigh = -200.0;
					txLow  = rxLow  = -width - 200.0;
					break;
				case FILTER_500:
					txHigh = rxHigh = -350.0;
					txLow  = rxLow  = -850.0;
					break;
				case FILTER_250:
					txHigh = rxHigh = -475.0;
					txLow  = rxLow  = -725.0;
					break;
				case FILTER_100:
					txHigh = rxHigh = -550.0;
					txLow  = rxLow  = -650.0;
					break;
				case FILTER_50:
					txHigh = rxHigh = -575.0;
					txLow  = rxLow  = -625.0;
					break;
				case FILTER_25:
					txHigh = rxHigh = -587.0;
					txLow  = rxLow  = -613.0;
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
					rxHigh = width + 100.0;
					rxLow  = 100.0;
					txHigh = CW_OFFSET + 200.0;
					txLow  = CW_OFFSET - 200.0;
					break;
				case FILTER_1000:
					rxHigh = CW_OFFSET + 500.0;
					rxLow  = CW_OFFSET - 500.0;
					txHigh = CW_OFFSET + 200.0;
					txLow  = CW_OFFSET - 200.0;
					break;
				case FILTER_500:
					rxHigh = CW_OFFSET + 250.0;
					rxLow  = CW_OFFSET - 250.0;
					txHigh = CW_OFFSET + 200.0;
					txLow  = CW_OFFSET - 200.0;
					break;
				case FILTER_250:
					rxHigh = CW_OFFSET + 125.0;
					rxLow  = CW_OFFSET - 125.0;
					txHigh = CW_OFFSET + 200.0;
					txLow  = CW_OFFSET - 200.0;
					break;
				case FILTER_100:
					rxHigh = CW_OFFSET + 50.0;
					rxLow  = CW_OFFSET - 50.0;
					txHigh = CW_OFFSET + 200.0;
					txLow  = CW_OFFSET - 200.0;
					break;
				case FILTER_50:
					rxHigh = CW_OFFSET + 25.0;
					rxLow  = CW_OFFSET - 25.0;
					txHigh = CW_OFFSET + 200.0;
					txLow  = CW_OFFSET - 200.0;
					break;
				case FILTER_25:
					rxHigh = CW_OFFSET + 13.0;
					rxLow  = CW_OFFSET - 12.0;
					txHigh = CW_OFFSET + 200.0;
					txLow  = CW_OFFSET - 200.0;
					break;
			}
			break;
		default:
			::wxLogError(wxT("Unknown mode value = %d"), m_mode);
			return;
	}

	::SetFilter(rxLow, rxHigh, m_blockSize, RX);
	::SetFilter(txLow, txHigh, m_blockSize, TX);
}
