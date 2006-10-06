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

#include "DataControl.h"

#include "SignalReader.h"
#include "SoundFileReader.h"
#include "SoundCardReader.h"
#include "SDRDataReader.h"
#include "NullWriter.h"
#include "SoundCardWriter.h"
#include "SDRDataWriter.h"


const int INTERNAL_READER  = 77;
const int SOUNDCARD_READER = 78;
const int SOUNDFILE_READER = 79;

const int SDRDATA_READER   = 88;

const unsigned int RINGBUFFER_SIZE = 100001;
const unsigned int BLOCK_SIZE      = 2048;		// XXXX

CDataControl::CDataControl(float sampleRate, const wxString& address, int port, int api, long inDev, long outDev) :
wxThread(),
m_sampleRate(sampleRate),
m_address(address),
m_port(port),
m_api(api),
m_inDev(inDev),
m_outDev(outDev),
m_internalReader(NULL),
m_soundCardReader(NULL),
m_soundFileReader(NULL),
m_rxWriter(NULL),
m_nullWriter(NULL),
m_soundCardWriter(NULL),
m_txReader(NULL),
m_waiting(),
m_txRingBuffer(RINGBUFFER_SIZE, 2),
m_rxRingBuffer(RINGBUFFER_SIZE, 2),
m_txBuffer(NULL),
m_rxBuffer(NULL),
m_source(SOURCE_INTERNAL),
m_transmit(false),
m_mute(true),
m_running(false)
{
	m_txBuffer = new float[BLOCK_SIZE * 2];
	m_rxBuffer = new float[BLOCK_SIZE * 2];
}

CDataControl::~CDataControl()
{
	delete[] m_txBuffer;
	delete[] m_rxBuffer;
}

void* CDataControl::Entry()
{
	bool ret = openIO();
	if (!ret) {
		closeIO();

		// We have a problem so wait for death
		while (!TestDestroy())
			Sleep(500);

		return (void*)1;
	}

	m_running = true;

	while (!TestDestroy()) {
		wxSemaError ret = m_waiting.WaitTimeout(500UL);

		if (ret == wxSEMA_NO_ERROR) {
			if (m_transmit) {
				unsigned int nSamples = m_txRingBuffer.getData(m_txBuffer, BLOCK_SIZE);

				if (nSamples > 0) {
					switch (m_source) {
						case SOURCE_INTERNAL:
						case SOURCE_SOUNDFILE:
							m_nullWriter->write(m_txBuffer, nSamples);
							break;
						case SOURCE_SOUNDCARD:
							m_soundCardWriter->write(m_txBuffer, nSamples);
							break;
					}
				}
			} else {
				unsigned int nSamples = m_rxRingBuffer.getData(m_rxBuffer, BLOCK_SIZE);
	
				if (nSamples > 0)
					m_rxWriter->write(m_rxBuffer, nSamples);
			}
		}
	}

	m_running = false;

	closeIO();

	return (void*)0;
}

bool CDataControl::setSoundFileReader(const wxString& fileName)
{
	if (m_soundFileReader != NULL) {
		m_soundFileReader->close();
		delete m_soundFileReader;
		m_soundFileReader = NULL;
	}

	m_soundFileReader = new CSoundFileReader(fileName);

	m_soundFileReader->setCallback(this, SOUNDFILE_READER);
	bool ret = m_soundFileReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		delete m_soundFileReader;
		m_soundFileReader = NULL;
		return false;
	}

	return true;
}

bool CDataControl::openIO()
{
	m_internalReader  = new CSignalReader(m_sampleRate / 4.0F + 1000.5F, 0.0008F, 0.001F);
	m_soundCardReader = new CSoundCardReader(m_api, m_inDev);
	m_rxWriter        = new CSDRDataWriter(m_address, m_port, 1);

	m_nullWriter      = new CNullWriter();
	m_soundCardWriter = new CSoundCardWriter(m_api, m_outDev);
	m_txReader        = new CSDRDataReader(m_address, m_port, 1);

	// This should be done before opening
	m_internalReader->setCallback(this,  INTERNAL_READER);
	m_soundCardReader->setCallback(this, SOUNDCARD_READER);
	m_txReader->setCallback(this,        SDRDATA_READER);

	bool ret = m_internalReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_soundCardReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_rxWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_nullWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_soundCardWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_txReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	return true;
}

void CDataControl::closeIO()
{
	if (m_internalReader != NULL) {
		m_internalReader->close();
		delete m_internalReader;
	}

	if (m_soundCardReader != NULL) {
		m_soundCardReader->close();
		delete m_soundCardReader;
	}

	if (m_rxWriter != NULL) {
		m_rxWriter->close();
		delete m_rxWriter;
	}

	if (m_soundFileReader != NULL) {
		m_soundFileReader->close();
		delete m_soundFileReader;
	}

	if (m_nullWriter != NULL) {
		m_nullWriter->close();
		delete m_nullWriter;
	}

	if (m_soundCardWriter != NULL) {
		m_soundCardWriter->close();
		delete m_soundCardWriter;
	}

	if (m_txReader != NULL) {
		m_txReader->close();
		delete m_txReader;
	}
}

void CDataControl::callback(float* inBuffer, unsigned int nSamples, int id)
{
	wxASSERT(inBuffer != NULL);
	wxASSERT(nSamples > 0);

	if (!m_running)
		return;

	switch (id) {
		case INTERNAL_READER: {
				if (m_transmit || m_source != SOURCE_INTERNAL || m_mute)
					return;

				unsigned int n = m_rxRingBuffer.addData(inBuffer, nSamples);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case SOUNDCARD_READER: {
				if (m_internalReader->needsClock())
					m_internalReader->clock();

				if (m_soundFileReader != NULL) {
					if (m_soundFileReader->needsClock())
						m_soundFileReader->clock();
				}

				if (m_txReader->needsClock())
					m_txReader->clock();

				if (m_transmit || m_source != SOURCE_SOUNDCARD || m_mute)
					return;

				unsigned int n = m_rxRingBuffer.addData(inBuffer, nSamples);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case SOUNDFILE_READER: {
				if (m_transmit || m_source != SOURCE_SOUNDFILE || m_mute)
					return;

				unsigned int n = m_rxRingBuffer.addData(inBuffer, nSamples);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case SDRDATA_READER: {
				if (!m_transmit)
					return;

				unsigned int n = m_txRingBuffer.addData(inBuffer, nSamples);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		default:
			break;
	}
}

void CDataControl::setTX(bool transmit)
{
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
}

void CDataControl::setMute(bool mute)
{
	// On a change to mute and we are receiving, empty the receive ring buffers and
	// drain the semaphore.
	if (!m_transmit) {
		wxSemaError status = m_waiting.TryWait();
		while (status != wxSEMA_BUSY) {
			m_waiting.Wait();
			status = m_waiting.TryWait();
		}

		m_rxRingBuffer.clear();
	}

	m_mute = mute;
}

void CDataControl::setSource(int source)
{
	if (m_source == SOURCE_SOUNDFILE) {
		m_soundFileReader->close();
		delete m_soundFileReader;
		m_soundFileReader = NULL;
	}

	m_source = source;
}
