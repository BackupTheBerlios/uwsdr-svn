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

#include "DSPControl.h"

#include <wx/datetime.h>


const int TX_READER = 77;
const int RX_READER = 88;

const int RINGBUFFER_SIZE = 100001;
const int BLOCK_SIZE      = 2048;		// XXXX

CDSPControl::CDSPControl(unsigned int sampleRate, float centreFreq) :
wxThread(),
m_dttsp(NULL),
m_sampleRate(sampleRate),
m_centreFreq(centreFreq),
m_txReader(NULL),
m_txWriter(NULL),
m_rxReader(NULL),
m_rxWriter(NULL),
m_waiting(),
m_txRingBuffer(RINGBUFFER_SIZE),
m_rxRingBuffer(RINGBUFFER_SIZE),
m_txBuffer(NULL),
m_rxBuffer(NULL),
m_outBuffer(NULL),
m_record(NULL),
m_transmit(false),
m_running(false),
m_afGain(0.0),
m_micGain(0.0),
m_power(0.0)
{
	m_dttsp = new CDTTSPControl();
	m_dttsp->open(m_sampleRate, BLOCK_SIZE);

	m_txBuffer = new float[BLOCK_SIZE * 2];
	m_rxBuffer = new float[BLOCK_SIZE * 2];

	m_outBuffer = new float[BLOCK_SIZE * 2];
}

CDSPControl::~CDSPControl()
{
	delete[] m_txBuffer;
	delete[] m_rxBuffer;
	delete[] m_outBuffer;
}

void CDSPControl::setTXReader(IDataReader* reader)
{
	wxASSERT(reader != NULL);
	wxASSERT(m_txReader == NULL);

	m_txReader = reader;
}

void CDSPControl::setTXWriter(IDataWriter* writer)
{
	wxASSERT(writer != NULL);
	wxASSERT(m_txWriter == NULL);

	m_txWriter = writer;
}

void CDSPControl::setRXReader(IDataReader* reader)
{
	wxASSERT(reader != NULL);
	wxASSERT(m_rxReader == NULL);

	m_rxReader = reader;
}

void CDSPControl::setRXWriter(IDataWriter* writer)
{
	wxASSERT(writer != NULL);

	m_rxWriter = writer;
}

void* CDSPControl::Entry()
{
	wxASSERT(m_dttsp != NULL);

	bool ret = openIO();
	if (!ret) {
		m_dttsp->close();

		// We have a problem so wait for death
		while (!TestDestroy())
			Sleep(500);

		return (void*)1;
	}

	// Open for business
	m_running = true;

	while (!TestDestroy()) {
		m_waiting.Wait();

		if (m_transmit) {
			unsigned int nSamples = m_txRingBuffer.getData(m_txBuffer, BLOCK_SIZE);
			if (nSamples != BLOCK_SIZE)
				::wxLogError(_("Underrun in TX ring buffer, wanted %u available %u"), BLOCK_SIZE, nSamples);

			if (nSamples > 0) {
				scaleBuffer(m_txBuffer, nSamples, m_power);
				m_txWriter->write(m_txBuffer, nSamples);
			}
		} else {
			unsigned int nSamples = m_rxRingBuffer.getData(m_rxBuffer, BLOCK_SIZE);
			if (nSamples != BLOCK_SIZE)
				::wxLogError(_("Underrun in RX ring buffer, wanted %u available %u"), BLOCK_SIZE, nSamples);

			if (nSamples > 0) {
				scaleBuffer(m_rxBuffer, nSamples, m_afGain);
				m_rxWriter->write(m_rxBuffer, nSamples);

				if (m_record != NULL)
					m_record->write(m_rxBuffer, nSamples);
			}
		}
	}

	m_running = false;

	closeIO();

	m_dttsp->close();

	return (void*)0;
}

bool CDSPControl::openIO()
{
	wxASSERT(m_txReader != NULL);
	wxASSERT(m_rxReader != NULL);
	wxASSERT(m_txWriter != NULL);
	wxASSERT(m_rxWriter != NULL);

	// This should be done before opening
	m_txReader->setCallback(this, TX_READER);
	m_rxReader->setCallback(this, RX_READER);

	bool ret = m_txWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_rxWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		m_txWriter->close();
		return false;
	}

	ret = m_txReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		m_txWriter->close();
		m_rxWriter->close();
		return false;
	}

	ret = m_rxReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		m_txWriter->close();
		m_rxWriter->close();
		m_txReader->close();
		return false;
	}

	return true;
}

void CDSPControl::closeIO()
{
	wxASSERT(m_txReader != NULL);
	wxASSERT(m_rxReader != NULL);
	wxASSERT(m_txWriter != NULL);
	wxASSERT(m_rxWriter != NULL);

	setRecord(false);

	m_txReader->close();
	m_rxReader->close();
	m_txWriter->close();
	m_rxWriter->close();
}

void CDSPControl::callback(float* inBuffer, unsigned int nSamples, int id)
{
	wxASSERT(m_dttsp != NULL);
	wxASSERT(inBuffer != NULL);
	wxASSERT(m_outBuffer != NULL);
	wxASSERT(nSamples > 0);

	// Don't process any data until the main thread is ready
	if (!m_running)
		return;

	switch (id) {
		case RX_READER: {
				if (m_transmit)
					return;

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_rxRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					::wxLogError(_("Overrun in RX ring buffer, needed %u available %u"), nSamples, n);
				else
					m_waiting.Post();
			}
			break;

		case TX_READER: {
				if (!m_transmit)
					return;

				scaleBuffer(inBuffer, nSamples, m_micGain);

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_txRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					::wxLogError(_("Overrun in TX ring buffer, needed %u available %u"), nSamples, n);
				else
					m_waiting.Post();
			}
			break;

		default:
			::wxLogError(_("callback() from unknown source = %d"), id);
			break;
	}
}

void CDSPControl::setMode(int mode)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setMode(mode);
}

void CDSPControl::setFilter(int filter)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setFilter(filter);
}

void CDSPControl::setAGC(int agc)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setAGC(agc);
}

void CDSPControl::setDeviation(int dev)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setDeviation(dev);
}

void CDSPControl::setTXAndFreq(bool transmit, float freq)
{
	wxASSERT(m_dttsp != NULL);

	// On a change from transmit to receive and vice versa we empty the ring buffers and
	// drain the semaphore.
	if (transmit != m_transmit) {
		wxSemaError status = m_waiting.TryWait();
		while (status != wxSEMA_BUSY) {
			m_waiting.Wait();
			status = m_waiting.TryWait();
		}

		m_txRingBuffer.clear();
		m_rxRingBuffer.clear();
	}

	m_transmit = transmit;

	if (transmit)
		m_dttsp->setTXAndFreq(true, freq + m_centreFreq);
	else
		m_dttsp->setTXAndFreq(false, -(freq + m_centreFreq));
}

void CDSPControl::setRIT(float freq)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setRIT(freq);
}

void CDSPControl::setNB(bool onOff)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setNB(onOff);
}

void CDSPControl::setNBValue(unsigned int value)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setNBValue(value);
}

void CDSPControl::setNB2(bool onOff)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setNB2(onOff);
}

void CDSPControl::setNB2Value(unsigned int value)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setNB2Value(value);
}

void CDSPControl::setSP(bool onOff)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setSP(onOff);
}

void CDSPControl::setSPValue(unsigned int value)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setSPValue(value);
}

void CDSPControl::setRXIAndQ(int phase, int gain)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setRXIAndQ(phase, gain);
}

void CDSPControl::setTXIAndQ(int phase, int gain)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setTXIAndQ(phase, gain);
}

float CDSPControl::getMeter(int type)
{
	wxASSERT(m_dttsp != NULL);

	return m_dttsp->getMeter(type);
}

void CDSPControl::getSpectrum(float* spectrum, int pos)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->getSpectrum(spectrum, pos);
}

void CDSPControl::setAFGain(unsigned int value)
{
	m_afGain = float(value) / 1000.0F;
}

void CDSPControl::setMicGain(unsigned int value)
{
	m_micGain = float(value) / 1000.0F;
}

void CDSPControl::setPower(unsigned int value)
{
	m_power = float(value) / 1000.0F;
}

void CDSPControl::setSquelch(unsigned int value)
{
	wxASSERT(m_dttsp != NULL);

	m_dttsp->setSquelch(value);
}

bool CDSPControl::setRecord(bool record)
{
	if (record && m_record == NULL) {
		wxDateTime now = wxDateTime::Now();
		wxString fileName = now.Format(wxT("%Y%m%d-%H%M%S.wav"));

		CSoundFileWriter* sdfw = new CSoundFileWriter(fileName);

		bool ret = sdfw->open(m_sampleRate, BLOCK_SIZE);
		if (!ret) {
			::wxLogError(_("Cannot open file %s for recording"), fileName.c_str());
			delete sdfw;
			return false;
		}

		m_record = sdfw;

		::wxLogMessage(_("Opened file %s for recording"), fileName.c_str());
	} else if (!record && m_record != NULL) {
		CSoundFileWriter* sdfw = m_record;
		m_record = NULL;

		sdfw->close();
		delete sdfw;

		::wxLogMessage(_("Closed sound file"));
	}

	return true;
}

void CDSPControl::scaleBuffer(float* buffer, unsigned int nSamples, float scale)
{
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);
	wxASSERT(scale >= 0.0 && scale <= 1.0);

	for (unsigned int i = 0; i < nSamples * 2; i++)
		buffer[i] *= scale;
}
