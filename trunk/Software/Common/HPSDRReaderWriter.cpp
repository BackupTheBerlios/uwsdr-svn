/*
 *   Copyright (C) 2008 by Jonathan Naylor G4KLX
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

#include "HPSDRReaderWriter.h"

const unsigned int HPSDR_VENDOR_ID    = 0xFFFE;
const unsigned int HPSDR_PRODUCT_ID   = 0x0007;
const unsigned int HPSDR_IN_ENDPOINT  = 0x86;
const unsigned int HPSDR_OUT_ENDPOINT = 0x02;

const unsigned int HPSDR_RXBUFFER_SIZE = 512U;
const unsigned int HPSDR_TXBUFFER_SIZE = 512U;
const unsigned int HPSDR_RXMAX_SAMPLES = 63U;
const unsigned int HPSDR_TXMAX_SAMPLES = 63U;

const unsigned char HPSDR_SYNC = 0x7F;

const unsigned char HPSDR_MOX   = 0x01;

const unsigned char HPSDR_48KHZ = 0x00;

const unsigned char HPSDR_CTRL1 = 0x00;
const unsigned char HPSDR_CTRL2 = 0x02;

const unsigned char HPSDR_PTT = 0x01;
const unsigned char HPSDR_KEY = 0x02;

const unsigned char HPSDR_OVERFLOW = 0x01;

const unsigned int C0_POS = 3U;
const unsigned int C1_POS = 4U;
const unsigned int C2_POS = 5U;
const unsigned int C3_POS = 6U;
const unsigned int C4_POS = 7U;


CHPSDRReaderWriter::CHPSDRReaderWriter(unsigned int blockSize, int c0, int c1, int c2, int c3, int c4) :
m_usb(NULL),
m_blockSize(blockSize),
m_dataRingBuffer(NULL),
m_audioRingBuffer(NULL),
m_usbBuffer(NULL),
m_cbBuffer(NULL),
m_dataBuffer(NULL),
m_audioBuffer(NULL),
m_dataCallback(NULL),
m_audioCallback(NULL),
m_controlCallback(NULL),
m_dataId(0),
m_audioId(0),
m_controlId(0),
m_transmit(false),
m_frequency(0U),
m_robin(0U),
m_c0(c0),
m_c1(c1),
m_c2(c2),
m_c3(c3),
m_c4(c4),
m_ptt(false),
m_key(false)
{
	m_dataRingBuffer  = new CRingBuffer(2048U, 2U);
	m_audioRingBuffer = new CRingBuffer(2048U, 2U);

	m_usbBuffer   = new char[HPSDR_TXBUFFER_SIZE];
	m_cbBuffer    = new float[m_blockSize * 2U];
	m_dataBuffer  = new float[HPSDR_TXMAX_SAMPLES * 2U];
	m_audioBuffer = new float[HPSDR_TXMAX_SAMPLES * 2U];
}

CHPSDRReaderWriter::~CHPSDRReaderWriter()
{
	delete m_dataRingBuffer;
	delete m_audioRingBuffer;

	delete[] m_usbBuffer;
	delete[] m_cbBuffer;
	delete[] m_dataBuffer;
	delete[] m_audioBuffer;
}

bool CHPSDRReaderWriter::open()
{
	m_usb = new CUSBBulkReaderWriter();
	m_usb->setCallback(this, 0);

	return m_usb->open(HPSDR_VENDOR_ID, HPSDR_PRODUCT_ID, HPSDR_IN_ENDPOINT, HPSDR_OUT_ENDPOINT);
}

void CHPSDRReaderWriter::setCallback(IControlInterface* callback, int id)
{
	wxASSERT(callback != NULL);

	m_controlCallback = callback;
	m_controlId       = id;
}

void CHPSDRReaderWriter::setDataCallback(IDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_dataCallback = callback;
	m_dataId       = id;
}

void CHPSDRReaderWriter::setAudioCallback(IDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_audioCallback = callback;
	m_audioId       = id;
}

void CHPSDRReaderWriter::close()
{
	m_usb->close();
}

void CHPSDRReaderWriter::purgeAudio()
{
	m_audioRingBuffer->clear();
}

void CHPSDRReaderWriter::purgeData()
{
	m_dataRingBuffer->clear();
}

bool CHPSDRReaderWriter::callback(char* buffer, unsigned int len, int WXUNUSED(id))
{
	if (len != HPSDR_RXBUFFER_SIZE) {
		::wxLogWarning(wxT("Invalid buffer length received from HPSDR"));
		return true;
	}

	if (buffer[0] != HPSDR_SYNC || buffer[1] != HPSDR_SYNC || buffer[2] != HPSDR_SYNC) {
		::wxLogWarning(wxT("Invalid sync pattern received from HPSDR"));
		return true;
	}

	bool ptt = (buffer[C0_POS] & HPSDR_PTT) == HPSDR_PTT;
	bool key = (buffer[C0_POS] & HPSDR_KEY) == HPSDR_KEY;

	// bool overflow = (buffer[C1_POS] & HPSDR_OVERFLOW) == HPSDR_OVERFLOW;

	unsigned int pos = C4_POS + 1U;
	for (unsigned int i = 0U; i < HPSDR_RXMAX_SAMPLES; i++) {
		float f[2];

		unsigned int iData = (buffer[pos++] << 16) & 0xFF0000;
		iData |= (buffer[pos++] << 8) & 0xFF00;
		iData |= (buffer[pos++] << 0) & 0xFF;

		unsigned int qData = (buffer[pos++] << 16) & 0xFF0000;
		qData |= (buffer[pos++] << 8) & 0xFF00;
		qData |= (buffer[pos++] << 0) & 0xFF;

		f[0] = float(iData) / 8388607.0F - 1.0F;
		f[1] = float(qData) / 8388607.0F - 1.0F;
		m_dataRingBuffer->addData(f, 1U);

		unsigned int audio = (buffer[pos++] << 8) & 0xFF00;
		audio |= (buffer[pos++] << 0) & 0xFF;

		f[0] = f[1] = float(audio) / 32767.0F - 1.0F;
		m_audioRingBuffer->addData(f, 1U);
	}

	unsigned int n = m_dataRingBuffer->dataSpace();
	if (n >= m_blockSize && m_dataCallback != NULL) {
		m_dataRingBuffer->getData(m_cbBuffer, m_blockSize);
		m_dataCallback->callback(m_cbBuffer, m_blockSize, m_dataId);
	}

	n = m_audioRingBuffer->dataSpace();
	if (n >= m_blockSize && m_audioCallback != NULL) {
		m_audioRingBuffer->getData(m_cbBuffer, m_blockSize);
		m_audioCallback->callback(m_cbBuffer, m_blockSize, m_audioId);
	}

	if (ptt != m_ptt && m_controlCallback != NULL) {
		m_controlCallback->setTransmit(ptt);
		m_ptt = ptt;
	}

	if (key != m_key && m_controlCallback != NULL) {
		m_controlCallback->setKey(key);
		m_key = key;
	}

	return true;
}

void CHPSDRReaderWriter::writeData(const float* buffer, unsigned int nSamples)
{
	wxASSERT(buffer != NULL);

	unsigned int n = m_dataRingBuffer->addData(buffer, nSamples);
	if (n != nSamples)
		::wxLogWarning(wxT("Overrun in the HPSDR data ring buffer, wanted=%u have=%u"), nSamples, n);

	writeUSB();
}

void CHPSDRReaderWriter::writeAudio(const float* buffer, unsigned int nSamples)
{
	wxASSERT(buffer != NULL);

	unsigned int n = m_audioRingBuffer->addData(buffer, nSamples);
	if (n != nSamples)
		::wxLogWarning(wxT("Overrun in the HPSDR audio ring buffer, wanted=%u have=%u"), nSamples, n);

	writeUSB();
}

void CHPSDRReaderWriter::writeUSB()
{
	while (m_dataRingBuffer->dataSpace() >= HPSDR_TXMAX_SAMPLES &&
		   m_audioRingBuffer->dataSpace() >= HPSDR_TXMAX_SAMPLES) {
		m_usbBuffer[0] = HPSDR_SYNC;
		m_usbBuffer[1] = HPSDR_SYNC;
		m_usbBuffer[2] = HPSDR_SYNC;

		if (m_robin > 9U) {
			m_usbBuffer[C0_POS] = m_c0 & 0xFF;
			m_usbBuffer[C0_POS] |= HPSDR_CTRL1;
			if (m_transmit)
				m_usbBuffer[C0_POS] |= HPSDR_MOX;

			m_usbBuffer[C1_POS] = m_c1 & 0xFF;
			m_usbBuffer[C1_POS] |= HPSDR_48KHZ;

			m_usbBuffer[C2_POS] = m_c2 & 0xFF;

			m_usbBuffer[C3_POS] = m_c3 & 0xFF;

			m_usbBuffer[C4_POS] = m_c4 & 0xFF;

			m_robin = 0U;
		} else {
			m_usbBuffer[C0_POS] = HPSDR_CTRL2;
			if (m_transmit)
				m_usbBuffer[C0_POS] |= HPSDR_MOX;

			m_usbBuffer[C1_POS] = (m_frequency >>  0) & 0xFF;
			m_usbBuffer[C2_POS] = (m_frequency >>  8) & 0xFF;
			m_usbBuffer[C3_POS] = (m_frequency >> 16) & 0xFF;
			m_usbBuffer[C4_POS] = (m_frequency >> 24) & 0xFF;

			m_robin++;
		}

		m_dataRingBuffer->getData(m_dataBuffer, HPSDR_TXMAX_SAMPLES);
		m_audioRingBuffer->getData(m_audioBuffer, HPSDR_TXMAX_SAMPLES);

		unsigned int pos = C4_POS + 1U;
		for (unsigned int i = 0U; i < HPSDR_TXMAX_SAMPLES; i++) {
			unsigned int iData = (unsigned int)((m_audioBuffer[i * 2 + 0] + 1.0F) * 32767.0F + 0.5F);
			unsigned int qData = (unsigned int)((m_audioBuffer[i * 2 + 1] + 1.0F) * 32767.0F + 0.5F);

			m_usbBuffer[pos++] = (iData >> 8) & 0xFF;
			m_usbBuffer[pos++] = (iData >> 0) & 0xFF;

			m_usbBuffer[pos++] = (qData >> 8) & 0xFF;
			m_usbBuffer[pos++] = (qData >> 0) & 0xFF;

			iData = (unsigned int)((m_dataBuffer[i * 2 + 0] + 1.0F) * 32767.0F + 0.5F);
			qData = (unsigned int)((m_dataBuffer[i * 2 + 1] + 1.0F) * 32767.0F + 0.5F);

			m_usbBuffer[pos++] = (iData >> 8) & 0xFF;
			m_usbBuffer[pos++] = (iData >> 0) & 0xFF;

			m_usbBuffer[pos++] = (qData >> 8) & 0xFF;
			m_usbBuffer[pos++] = (qData >> 0) & 0xFF;
		}

		m_usb->write(m_usbBuffer, pos);
	}
}

void CHPSDRReaderWriter::enableTX(bool WXUNUSED(on))
{
}

void CHPSDRReaderWriter::enableRX(bool WXUNUSED(on))
{
}

void CHPSDRReaderWriter::setTXAndFreq(bool transmit, const CFrequency& freq)
{
	m_transmit  = transmit;
	m_frequency = freq.getMHz() * 1000000U + (unsigned int)(freq.getHz() + 0.5);
}

void CHPSDRReaderWriter::setClockTune(unsigned int WXUNUSED(clock))
{
}
