/*
 *   Copyright (C) 2006-2008 by Jonathan Naylor G4KLX
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

#include "SoundCardReaderWriter.h"

const int INTERNAL_READER_1 = 77;
const int INTERNAL_READER_2 = 78;
const int SOUNDCARD_READER  = 79;
const int SOUNDFILE_READER  = 80;

const unsigned int RINGBUFFER_SIZE = 100001;
const unsigned int BLOCK_SIZE      = 2048;		// XXXX

const unsigned int RXHEADER_SIZE = 8U;
const unsigned int RXSAMPLE_SIZE = 6U;

const unsigned int TXHEADER_SIZE = 6U;
const unsigned int TXSAMPLE_SIZE = 4U;


CDataControl::CDataControl(float sampleRate, const wxString& address, int port, int inDev, int outDev) :
wxThread(),
m_sampleRate(sampleRate),
m_address(address),
m_port(port),
m_inDev(inDev),
m_outDev(outDev),
m_internal1Reader(NULL),
m_internal2Reader(NULL),
m_soundCardReader(NULL),
m_soundFileReader(NULL),
m_rxWriter(NULL),
m_nullWriter(NULL),
m_soundCardWriter(NULL),
m_txReader(NULL),
m_waiting(),
m_txSequence(-1),
m_rxSequence(0U),
m_txRingBuffer(RINGBUFFER_SIZE, 2),
m_rxRingBuffer(RINGBUFFER_SIZE, 2),
m_txBuffer(NULL),
m_rxBuffer(NULL),
m_txSockBuffer(NULL),
m_rxSockBuffer(NULL),
m_source(SOURCE_INTERNAL_1),
m_transmit(false),
m_mute(true),
m_running(false)
{
	m_txBuffer = new float[BLOCK_SIZE * 2];
	m_rxBuffer = new float[BLOCK_SIZE * 2];

	m_rxSockBuffer = new unsigned char[BLOCK_SIZE * RXSAMPLE_SIZE + RXHEADER_SIZE];
	m_txSockBuffer = new float[BLOCK_SIZE * 2];
}

CDataControl::~CDataControl()
{
	delete[] m_txBuffer;
	delete[] m_rxBuffer;

	delete[] m_rxSockBuffer;
	delete[] m_txSockBuffer;
}

bool CDataControl::open()
{
	bool ret = openIO();
	if (!ret) {
		closeIO();
		return false;
	}

	Create();
	Run();

	return true;
}

void* CDataControl::Entry()
{
	m_running = true;

	while (!TestDestroy()) {
		wxSemaError ret = m_waiting.WaitTimeout(500UL);

		if (ret == wxSEMA_NO_ERROR) {
			if (m_transmit) {
				unsigned int nSamples = m_txRingBuffer.getData(m_txBuffer, BLOCK_SIZE);

				if (nSamples > 0) {
					switch (m_source) {
						case SOURCE_INTERNAL_1:
						case SOURCE_INTERNAL_2:
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
					sendData(m_rxBuffer, nSamples);
			}
		}
	}

	m_running = false;

	closeIO();

	return (void*)0;
}

void CDataControl::close()
{
	Delete();
}

bool CDataControl::setSoundFileReader(const wxString& fileName)
{
	if (m_soundFileReader != NULL)
		m_soundFileReader->close();

	m_soundFileReader = new CSoundFileReader(fileName);

	m_soundFileReader->setCallback(this, SOUNDFILE_READER);
	bool ret = m_soundFileReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		m_soundFileReader = NULL;
		return false;
	}

	m_source = SOURCE_SOUNDFILE;

	return true;
}

bool CDataControl::openIO()
{
	CSoundCardReaderWriter* scrw = new CSoundCardReaderWriter(m_inDev, m_outDev, 2U, 2U);
	m_soundCardReader = scrw;
	m_soundCardWriter = scrw;

	m_internal1Reader = new CSignalReader(1000.0F, 0.0003F, 0.0004F);
	m_internal2Reader = new CSignalReader(1000.0F, 0.0F, 0.001F);
	m_rxWriter        = new CUDPDataWriter(m_address, m_port);

	m_nullWriter      = new CNullWriter();
	m_txReader        = new CUDPDataReader(m_address, m_port);

	// This should be done before opening
	m_internal1Reader->setCallback(this, INTERNAL_READER_1);
	m_internal2Reader->setCallback(this, INTERNAL_READER_2);
	m_soundCardReader->setCallback(this, SOUNDCARD_READER);

	m_txReader->setCallback(this, 0);

	bool ret = m_internal1Reader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_internal2Reader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_soundCardReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_rxWriter->open();
	if (!ret)
		return false;

	ret = m_nullWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_soundCardWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_txReader->open();
	if (!ret)
		return false;

	return true;
}

void CDataControl::closeIO()
{
	m_internal1Reader->close();
	m_internal2Reader->close();
	m_soundCardReader->close();
	m_rxWriter->close();
	m_nullWriter->close();
	m_soundCardWriter->close();
	m_txReader->close();

	if (m_soundFileReader != NULL)
		m_soundFileReader->close();
}

void CDataControl::setCallback(IRawDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

void CDataControl::callback(float* inBuffer, unsigned int nSamples, int id)
{
	wxASSERT(inBuffer != NULL);

	if (!m_running)
		return;

	switch (id) {
		case INTERNAL_READER_1: {
				if (m_source != SOURCE_INTERNAL_1)
					return;

				unsigned int n = m_rxRingBuffer.addData(inBuffer, nSamples);

				if (n != nSamples)
					::wxLogMessage(wxT("Overflow in the RX ring buffer, wanted: %u have: %u"), nSamples, n);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case INTERNAL_READER_2: {
				if (m_source != SOURCE_INTERNAL_2)
					return;

				unsigned int n = m_rxRingBuffer.addData(inBuffer, nSamples);

				if (n != nSamples)
					::wxLogMessage(wxT("Overflow in the RX ring buffer, wanted: %u have: %u"), nSamples, n);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case SOUNDCARD_READER: {
				if (m_mute)
					return;

				if (m_source == SOURCE_INTERNAL_1) {
					m_internal1Reader->clock();
					return;
				}

				if (m_source == SOURCE_INTERNAL_2) {
					m_internal2Reader->clock();
					return;
				}

				if (m_source == SOURCE_SOUNDFILE) {
					if (m_soundFileReader != NULL)
						m_soundFileReader->clock();

					return;
				}

				unsigned int n = m_rxRingBuffer.addData(inBuffer, nSamples);

				if (n != nSamples)
					::wxLogMessage(wxT("Overflow in the RX ring buffer, wanted: %u have: %u"), nSamples, n);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case SOUNDFILE_READER: {
				if (m_source != SOURCE_SOUNDFILE)
					return;

				if (nSamples == 0) {
					m_soundFileReader->rewind();
					return;
				}

				unsigned int n = m_rxRingBuffer.addData(inBuffer, nSamples);

				if (n != nSamples)
					::wxLogMessage(wxT("Overflow in the RX ring buffer, wanted: %u have: %u"), nSamples, n);

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

		m_internal1Reader->purge();
		m_internal2Reader->purge();
		m_soundCardReader->purge();

		if (m_soundFileReader != NULL)
			m_soundFileReader->purge();
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

		m_internal1Reader->purge();
		m_internal2Reader->purge();
		m_soundCardReader->purge();

		if (m_soundFileReader != NULL)
			m_soundFileReader->purge();
	}

	m_mute = mute;
}

void CDataControl::setSource(int source)
{
	if (m_source == SOURCE_SOUNDFILE) {
		m_soundFileReader->close();
		m_soundFileReader = NULL;
	}

	m_source = source;
}

void CDataControl::sendACK(const wxString& command)
{
	wxASSERT(m_rxWriter != NULL);

	wxString text;
	text.Printf("AK%s;", command.c_str());

	m_rxWriter->write(text.c_str(), text.Length());
}

void CDataControl::sendNAK(const wxString& command)
{
	wxASSERT(m_rxWriter != NULL);

	wxString text;
	text.Printf("NK%s;", command.c_str());

	m_rxWriter->write(text.c_str(), text.Length());
}

void CDataControl::sendData(const float* buffer, unsigned int nSamples)
{
	wxASSERT(m_rxWriter != NULL);
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0U);

	m_rxSockBuffer[0] = 'D';
	m_rxSockBuffer[1] = 'R';

	m_rxSockBuffer[2] = (m_rxSequence >> 0) & 0xFF;
	m_rxSockBuffer[3] = (m_rxSequence >> 8) & 0xFF;

	m_rxSequence += 2;
	if (m_rxSequence > 0xFFFF) {
		if ((m_rxSequence % 2) == 0)
			m_rxSequence = 1;
		else
			m_rxSequence = 0;
	}

	m_rxSockBuffer[4] = (nSamples >> 0) & 0xFF;
	m_rxSockBuffer[5] = (nSamples >> 8) & 0xFF;

	m_rxSockBuffer[6] = 0x00;		// AGC value
	m_rxSockBuffer[7] = 0x00;		// Filler

	unsigned int len = RXHEADER_SIZE;
	for (unsigned int i = 0; i < nSamples; i++) {
		wxInt32 iData = wxInt32(buffer[i * 2 + 0] * float(0x7FFFFFFF));
		wxInt32 qData = wxInt32(buffer[i * 2 + 1] * float(0x7FFFFFFF));

		m_rxSockBuffer[len++] = (iData >> 16) & 0xFF;
		m_rxSockBuffer[len++] = (iData >> 8)  & 0xFF;
		m_rxSockBuffer[len++] = (iData >> 0)  & 0xFF;

		m_rxSockBuffer[len++] = (qData >> 16) & 0xFF;
		m_rxSockBuffer[len++] = (qData >> 8)  & 0xFF;
		m_rxSockBuffer[len++] = (qData >> 0)  & 0xFF;
	}

	m_rxWriter->write(m_rxSockBuffer, len);
}

bool CDataControl::callback(char* buffer, unsigned int len, int WXUNUSED(id))
{
	wxASSERT(m_callback != NULL);
	wxASSERT(buffer != NULL);
	wxASSERT(len > 0U);

	if (len < TXHEADER_SIZE || buffer[0] != 'D' || buffer[1] != 'T')
		return m_callback->callback(buffer, len, m_id);

	int seqNo = (wxUint8(buffer[3]) << 8) + wxUint8(buffer[2]);

	if (m_txSequence != -1 && seqNo != m_txSequence) {
		if (seqNo < m_txSequence && (seqNo % 2) == (m_txSequence % 2)) {
			::wxLogWarning(wxT("DataReader: Packet dropped at sequence no: %d, expected: %d"), seqNo, m_txSequence);
			return true;
		} else {
			::wxLogWarning(wxT("SDREmulatorReader: Packet missed at sequence no: %d, expected: %d"), seqNo, m_txSequence);
		}
	}

	m_txSequence = seqNo + 2;
	if (m_txSequence > 0xFFFF) {
		if ((m_txSequence % 2) == 0)
			m_txSequence = 1;
		else
			m_txSequence = 0;
	}

	if (!m_transmit)
		return true;

	unsigned int nSamples = (wxUint8(buffer[5]) << 8) + wxUint8(buffer[4]);

	unsigned int n = TXHEADER_SIZE;
	for (unsigned int i = 0; i < nSamples && n < len; n += TXSAMPLE_SIZE, i++) {
		wxInt16 iData = (buffer[n + 0] << 8) & 0xFF00 + (buffer[n + 1] << 0) & 0xFF;
		wxInt16 qData = (buffer[n + 2] << 8) & 0xFF00 + (buffer[n + 3] << 0) & 0xFF;

		m_txSockBuffer[i * 2 + 0] = float(iData) / float(0x7FFF);
		m_txSockBuffer[i * 2 + 1] = float(qData) / float(0x7FFF);
	}

	n = m_txRingBuffer.addData(m_txSockBuffer, nSamples);

	if (n != nSamples)
		::wxLogMessage(wxT("Overflow in the TX ring buffer, wanted: %u have: %u"), nSamples, n);

	if (n > 0)
		m_waiting.Post();

	return true;
}

#if defined(__WXDEBUG__)
void CDataControl::dumpBuffer(const wxString& title, float* buffer, unsigned int nSamples) const
{
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);

	::wxLogMessage(title);
	::wxLogMessage(wxT("Length: %05X"), nSamples);

	::wxLogMessage(wxT(":"));

	unsigned int n = 0;
	for (unsigned int i = 0; i < nSamples; i += 16) {
		wxString text;
		text.Printf(wxT("%05X:  "), i);

		for (unsigned int j = 0; j < 16; j++, n += 2) {
			wxString buf;
			buf.Printf(wxT("%f:%f "), buffer[n + 0], buffer[n + 1]);
			text.Append(buf);

			if ((i + j) >= nSamples)
				break;
		}

		::wxLogMessage(text);
	}
}
#endif
