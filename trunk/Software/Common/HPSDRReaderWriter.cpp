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

const unsigned int HPSDR_TIMEOUT       = 1000U;

const unsigned char HPSDR_SYNC = 0x7F;

const unsigned char HPSDR_MOX   = 0x01;

const unsigned char HPSDR_48KHZ = 0x00;

const unsigned char HPSDR_CTRL1 = 0x00;
const unsigned char HPSDR_CTRL2 = 0x02;

const unsigned char HPSDR_PTT = 0x01;
const unsigned char HPSDR_KEY = 0x02;

const unsigned char HPSDR_OVERFLOW = 0x01;

const unsigned int SYNC0_POS = 0U;
const unsigned int SYNC1_POS = 1U;
const unsigned int SYNC2_POS = 2U;
const unsigned int C0_POS    = 3U;
const unsigned int C1_POS    = 4U;
const unsigned int C2_POS    = 5U;
const unsigned int C3_POS    = 6U;
const unsigned int C4_POS    = 7U;
const unsigned int DATA_POS  = 8U;


CHPSDRReaderWriter::CHPSDRReaderWriter(unsigned int blockSize, int c0, int c1, int c2, int c3, int c4) :
wxThread(),
m_handle(NULL),
m_blockSize(blockSize),
m_dataRingBuffer(NULL),
m_audioRingBuffer(NULL),
m_usbOutBuffer(NULL),
m_usbInBuffer(NULL),
m_cbBuffer(NULL),
m_dataBuffer(NULL),
m_audioBuffer(NULL),
m_dataCallback(NULL),
m_audioCallback(NULL),
m_controlCallback(NULL),
m_dataId(0),
m_audioId(0),
m_transmit(false),
m_frequency(0U),
m_robin(0U),
m_c0(c0),
m_c1(c1),
m_c2(c2),
m_c3(c3),
m_c4(c4),
m_offset(SYNC0_POS),
m_iData(0),
m_qData(0),
m_audio(0),
m_ptt(false),
m_key(false),
m_overflow(false),
m_space(512U)
{
	::usb_init();
	::usb_find_devices();
	::usb_find_busses();

	m_dataRingBuffer  = new CRingBuffer(2048U, 2U);
	m_audioRingBuffer = new CRingBuffer(2048U, 2U);

	m_usbOutBuffer = new char[HPSDR_TXBUFFER_SIZE];
	m_usbInBuffer  = new char[HPSDR_RXBUFFER_SIZE];

	m_cbBuffer    = new float[m_blockSize * 2U];
	m_dataBuffer  = new float[HPSDR_TXMAX_SAMPLES * 2U];
	m_audioBuffer = new float[HPSDR_TXMAX_SAMPLES * 2U];
}

CHPSDRReaderWriter::~CHPSDRReaderWriter()
{
	delete m_dataRingBuffer;
	delete m_audioRingBuffer;

	delete[] m_usbOutBuffer;
	delete[] m_usbInBuffer;

	delete[] m_cbBuffer;
	delete[] m_dataBuffer;
	delete[] m_audioBuffer;
}

bool CHPSDRReaderWriter::open()
{
	struct usb_device* dev = find(HPSDR_VENDOR_ID, HPSDR_PRODUCT_ID);
	if (dev == NULL) {
		::wxLogWarning(wxT("Cannot find the USB hardware with vendor: 0x%04X and product: 0x%04X"), HPSDR_VENDOR_ID, HPSDR_PRODUCT_ID);
		return false;
	}

	m_handle = ::usb_open(dev);
	if (m_handle == NULL) {
		::wxLogWarning(wxT("Cannot open the USB device with vendor: 0x%04X and product: 0x%04X"), HPSDR_VENDOR_ID, HPSDR_PRODUCT_ID);
		return false;
	}

	int rc = ::usb_set_configuration(m_handle, 1);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot set the USB configuration, rc: %d"), rc);
		::usb_close(m_handle);
		return false;
	} 

	rc = ::usb_claim_interface(m_handle, 0);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot claim the USB interface, rc: %d"), rc);
		::usb_close(m_handle);
		return false;
	}

	rc = ::usb_set_altinterface(m_handle, 0);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot set the alternate USB interface, rc: %d"), rc);
		::usb_close(m_handle);
		return false;
	}

	rc = ::usb_clear_halt(m_handle, HPSDR_IN_ENDPOINT);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot clear the USB halt on end point %u, rc: %d"), HPSDR_IN_ENDPOINT, rc);
		::usb_close(m_handle);
		return false;
	} 

	rc = ::usb_clear_halt(m_handle, HPSDR_OUT_ENDPOINT); 
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot clear the USB halt on end point %u, rc: %d"), HPSDR_OUT_ENDPOINT, rc);
		::usb_close(m_handle);
		return false;
	}

	Create();
	Run();

	return true;
}

void CHPSDRReaderWriter::setCallback(IControlInterface* callback)
{
	wxASSERT(callback != NULL);

	m_controlCallback = callback;
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

void* CHPSDRReaderWriter::Entry()
{
	wxASSERT(m_handle != NULL);
	wxASSERT(m_dataCallback != NULL);
	wxASSERT(m_audioCallback != NULL);
	wxASSERT(m_controlCallback != NULL);

	while (!TestDestroy()) {
		unsigned int n = ::usb_bulk_read(m_handle, HPSDR_IN_ENDPOINT, m_usbInBuffer, HPSDR_RXBUFFER_SIZE, HPSDR_TIMEOUT);

		if (n > 0U)
			processData(m_usbInBuffer, n);
	}

	::usb_release_interface(m_handle, 0x0);
	::usb_close(m_handle);

	return NULL;
}

void CHPSDRReaderWriter::close()
{
	Delete();
}

void CHPSDRReaderWriter::purgeAudio()
{
	m_audioRingBuffer->clear();
}

void CHPSDRReaderWriter::purgeData()
{
	m_dataRingBuffer->clear();
}

void CHPSDRReaderWriter::processData(char* buffer, unsigned int len)
{
	bool ptt, key, overflow;
	unsigned int dataOffset;
	unsigned int n;
	float f[2];

	for (unsigned int i = 0U; i < len; i++) {
		switch (m_offset) {
			case SYNC0_POS:
			case SYNC1_POS:
			case SYNC2_POS:
				if (buffer[i] == HPSDR_SYNC)
					m_offset++;
				else
					m_offset = SYNC0_POS;
				break;

			case C0_POS:
				ptt = (buffer[i] & HPSDR_PTT) == HPSDR_PTT;
				key = (buffer[i] & HPSDR_KEY) == HPSDR_KEY;

				if (ptt != m_ptt && m_controlCallback != NULL) {
					m_controlCallback->setTransmit(ptt);
					m_ptt = ptt;
				}

				if (key != m_key && m_controlCallback != NULL) {
					m_controlCallback->setKey(key);
					m_key = key;
				}

				m_offset++;
				break;

			case C1_POS:
				overflow = (buffer[i] & HPSDR_OVERFLOW) == HPSDR_OVERFLOW;

				if (overflow && !m_overflow && m_controlCallback != NULL) {
					m_controlCallback->commandError(_("Data overflow in HPSDR"));
					m_overflow = overflow;
				}

				m_offset++;
				break;

			case C2_POS:
				m_offset++;
				break;

			case C3_POS:
				m_space = buffer[i] * 16U;		// Check XXX
				m_offset++;
				break;

			case C4_POS:
				m_offset++;
				break;

			default:
				dataOffset = i - DATA_POS;

				switch (dataOffset % 8U) {
					case 0U:
						m_iData =  (buffer[i] << 24) & 0xFF000000;
						break;
					case 1U:
						m_iData |= (buffer[i] << 16) & 0xFF0000;
						break;
					case 2U:
						m_iData |= (buffer[i] << 8) & 0xFF00;
						break;
					case 3U:
						m_qData =  (buffer[i] << 24) & 0xFF000000;
						break;
					case 4U:
						m_qData |= (buffer[i] << 16) & 0xFF0000;
						break;
					case 5U:
						m_qData |= (buffer[i] << 8) & 0xFF00;
						break;
					case 6U:
						m_audio =  (buffer[i] << 8) & 0xFF00;
						break;
					case 7U:
						m_audio |= (buffer[i] << 0) & 0xFF;

						// The sample data is complete
						f[0] = float(m_iData) / float(0x7FFFFFFF);
						f[1] = float(m_qData) / float(0x7FFFFFFF);
						m_dataRingBuffer->addData(f, 1U);

						f[0] = f[1] = float(m_audio) / float(0x7FFF);
						m_audioRingBuffer->addData(f, 1U);

						n = m_dataRingBuffer->dataSpace();
						if (n >= m_blockSize && m_dataCallback != NULL) {
							m_dataRingBuffer->getData(m_cbBuffer, m_blockSize);
							m_dataCallback->callback(m_cbBuffer, m_blockSize, m_dataId);
						}

						n = m_audioRingBuffer->dataSpace();
						if (n >= m_blockSize && m_audioCallback != NULL) {
							m_audioRingBuffer->getData(m_cbBuffer, m_blockSize);
							m_audioCallback->callback(m_cbBuffer, m_blockSize, m_audioId);
						}
						break;
				}

				m_offset++;
				break;
		}

		if (m_offset >= HPSDR_RXBUFFER_SIZE)
			m_offset = SYNC0_POS;
	}
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
	if (m_space < HPSDR_TXBUFFER_SIZE)
		return;

	while (m_dataRingBuffer->dataSpace() >= HPSDR_TXMAX_SAMPLES &&
		   m_audioRingBuffer->dataSpace() >= HPSDR_TXMAX_SAMPLES) {
		m_usbOutBuffer[0] = HPSDR_SYNC;
		m_usbOutBuffer[1] = HPSDR_SYNC;
		m_usbOutBuffer[2] = HPSDR_SYNC;

		if (m_robin > 9U) {
			m_usbOutBuffer[C0_POS] = m_c0 & 0xFF;
			m_usbOutBuffer[C0_POS] |= HPSDR_CTRL1;
			if (m_transmit)
				m_usbOutBuffer[C0_POS] |= HPSDR_MOX;

			m_usbOutBuffer[C1_POS] = m_c1 & 0xFF;
			m_usbOutBuffer[C1_POS] |= HPSDR_48KHZ;

			m_usbOutBuffer[C2_POS] = m_c2 & 0xFF;

			m_usbOutBuffer[C3_POS] = m_c3 & 0xFF;

			m_usbOutBuffer[C4_POS] = m_c4 & 0xFF;

			m_robin = 0U;
		} else {
			m_usbOutBuffer[C0_POS] = HPSDR_CTRL2;
			if (m_transmit)
				m_usbOutBuffer[C0_POS] |= HPSDR_MOX;

			m_usbOutBuffer[C1_POS] = (m_frequency >>  0) & 0xFF;
			m_usbOutBuffer[C2_POS] = (m_frequency >>  8) & 0xFF;
			m_usbOutBuffer[C3_POS] = (m_frequency >> 16) & 0xFF;
			m_usbOutBuffer[C4_POS] = (m_frequency >> 24) & 0xFF;

			m_robin++;
		}

		m_dataRingBuffer->getData(m_dataBuffer, HPSDR_TXMAX_SAMPLES);
		m_audioRingBuffer->getData(m_audioBuffer, HPSDR_TXMAX_SAMPLES);

		unsigned int pos = C4_POS + 1U;
		for (unsigned int i = 0U; i < HPSDR_TXMAX_SAMPLES; i++) {
			wxInt16 iData = wxInt16(m_audioBuffer[i * 2 + 0] * 32767.0F);
			wxInt16 qData = wxInt16(m_audioBuffer[i * 2 + 1] * 32767.0F);

			m_usbOutBuffer[pos++] = (iData >> 8) & 0xFF;
			m_usbOutBuffer[pos++] = (iData >> 0) & 0xFF;

			m_usbOutBuffer[pos++] = (qData >> 8) & 0xFF;
			m_usbOutBuffer[pos++] = (qData >> 0) & 0xFF;

			iData = wxInt16(m_dataBuffer[i * 2 + 0] * 32767.0F);
			qData = wxInt16(m_dataBuffer[i * 2 + 1] * 32767.0F);

			m_usbOutBuffer[pos++] = (iData >> 8) & 0xFF;
			m_usbOutBuffer[pos++] = (iData >> 0) & 0xFF;

			m_usbOutBuffer[pos++] = (qData >> 8) & 0xFF;
			m_usbOutBuffer[pos++] = (qData >> 0) & 0xFF;
		}

		::usb_bulk_write(m_handle, HPSDR_OUT_ENDPOINT, (char *)m_usbOutBuffer, pos, HPSDR_TIMEOUT);
	}
}

void CHPSDRReaderWriter::enableTX(bool WXUNUSED(on))
{
}

bool CHPSDRReaderWriter::getPTT() const
{
	return m_ptt;
}

bool CHPSDRReaderWriter::getKey() const
{
	return m_key;
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

struct usb_device *CHPSDRReaderWriter::find(unsigned int vendor, unsigned int product) const
{
	for (struct usb_bus* bus = usb_get_busses(); bus != NULL; bus = bus->next)
		for (struct usb_device* dev = bus->devices; dev != NULL; dev = dev->next)
			if (dev->descriptor.idVendor == vendor && dev->descriptor.idProduct == product)
				return dev;

	return NULL;
}
