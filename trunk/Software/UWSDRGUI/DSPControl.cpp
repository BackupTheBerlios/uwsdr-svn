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

#include "UWSDRDefs.h"

#include <wx/datetime.h>


const int TX_READER    = 44;
const int RX_READER    = 55;
const int CW_READER    = 66;
const int VOICE_READER = 77;

const unsigned int RINGBUFFER_SIZE = 100001;
const unsigned int BLOCK_SIZE      = 2048;		// XXXX

CDSPControl::CDSPControl(float sampleRate, float centreFreq) :
wxThread(),
m_dttsp(NULL),
m_cwKeyer(NULL),
m_voiceKeyer(NULL),
m_sampleRate(sampleRate),
m_centreFreq(centreFreq),
m_txReader(NULL),
m_txWriter(NULL),
m_rxReader(NULL),
m_rxWriter(NULL),
m_waiting(),
m_txRingBuffer(RINGBUFFER_SIZE, 2),
m_rxRingBuffer(RINGBUFFER_SIZE, 2),
m_txBuffer(NULL),
m_rxBuffer(NULL),
m_outBuffer(NULL),
m_record(NULL),
m_transmit(false),
m_running(false),
m_afGain(0.0F),
m_rfGain(0.0F),
m_micGain(0.0F),
m_power(0.0F),
m_mode(MODE_USB),
m_clockId(-1)
{
	m_dttsp = new CDTTSPControl();
	m_dttsp->open(m_sampleRate, BLOCK_SIZE);

	m_cwKeyer = new CCWKeyer();
	m_cwKeyer->setCallback(this, CW_READER);
	m_cwKeyer->open(m_sampleRate, BLOCK_SIZE);

	m_voiceKeyer = new CVoiceKeyer();
	m_voiceKeyer->setCallback(this, VOICE_READER);
	m_voiceKeyer->open(m_sampleRate, BLOCK_SIZE);

	m_txBuffer = new float[BLOCK_SIZE * 2];
	m_rxBuffer = new float[BLOCK_SIZE * 2];

	m_outBuffer = new float[BLOCK_SIZE * 2];
}

CDSPControl::~CDSPControl()
{
	delete[] m_txBuffer;
	delete[] m_rxBuffer;
	delete[] m_outBuffer;
	delete   m_cwKeyer;
	delete   m_voiceKeyer;
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

bool CDSPControl::open()
{
	bool ret = openIO();
	if (!ret)
		return false;

	Create();
	Run();

	return true;
}

void* CDSPControl::Entry()
{
	// Open for business
	m_running = true;

	while (!TestDestroy()) {
		wxSemaError ret = m_waiting.WaitTimeout(500UL);

		if (ret == wxSEMA_NO_ERROR) {
			if (m_transmit) {
				unsigned int nSamples = m_txRingBuffer.getData(m_txBuffer, BLOCK_SIZE);
				if (nSamples != BLOCK_SIZE)
					::wxLogError(wxT("Underrun in TX ring buffer, wanted %u available %u"), BLOCK_SIZE, nSamples);

				if (nSamples > 0) {
					scaleBuffer(m_txBuffer, nSamples, m_power);
					m_txWriter->write(m_txBuffer, nSamples);
				}
			}

			unsigned int nSamples = m_rxRingBuffer.getData(m_rxBuffer, BLOCK_SIZE);

			// Create silence on transmit
			if (nSamples == 0 && m_transmit) {
				::memset(m_rxBuffer, 0x00, BLOCK_SIZE * 2 * sizeof(float));
				nSamples = BLOCK_SIZE;
			}

			if (nSamples != BLOCK_SIZE)
				::wxLogError(wxT("Underrun in RX ring buffer, wanted %u available %u"), BLOCK_SIZE, nSamples);

			scaleBuffer(m_rxBuffer, nSamples, m_afGain);
			m_rxWriter->write(m_rxBuffer, nSamples);

			// Don't record when transmitting
			if (m_record != NULL && !m_transmit)
				m_record->write(m_rxBuffer, nSamples);
		}
	}

	m_running = false;

	closeIO();

	return (void*)0;
}

void CDSPControl::close()
{
	Delete();
}

bool CDSPControl::openIO()
{
	// This should be done before opening
	m_txReader->setCallback(this, TX_READER);
	m_rxReader->setCallback(this, RX_READER);

	bool ret = m_txWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret)
		return false;

	ret = m_txReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		m_dttsp->close();
		m_cwKeyer->close();
		m_voiceKeyer->close();

		return false;
	}

	if (m_txReader->hasClock())
		m_clockId = TX_READER;

	ret = m_rxWriter->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		m_dttsp->close();
		m_cwKeyer->close();
		m_voiceKeyer->close();

		return false;
	}

	ret = m_rxReader->open(m_sampleRate, BLOCK_SIZE);
	if (!ret) {
		m_dttsp->close();
		m_cwKeyer->close();
		m_voiceKeyer->close();

		return false;
	}

	if (m_rxReader->hasClock())
		m_clockId = RX_READER;

/*
	if (m_clockId == -1) {
		m_dttsp->close();
		m_cwKeyer->close();
		m_voiceKeyer->close();

		::wxLogError(wxT("No reader can provide a suitable clock"));
		return false;
	}
*/

	return true;
}

void CDSPControl::closeIO()
{
	setRecord(false);

	m_txReader->close();
	m_rxReader->close();
	m_txWriter->close();
	m_rxWriter->close();

	m_dttsp->close();
	m_cwKeyer->close();
	m_voiceKeyer->close();

	if (m_record != NULL) {
		m_record->close();
		delete m_record;
	}
}

void CDSPControl::callback(float* inBuffer, unsigned int nSamples, int id)
{
	wxASSERT(inBuffer != NULL);
	wxASSERT(nSamples > 0);

	// Don't process any data until the main thread is ready
	if (!m_running)
		return;

	// Use whatever clock is available to run everything
	if (id == m_clockId) {
		if (m_transmit) {
			m_txReader->clock();

			if (m_cwKeyer->isActive() && (m_mode == MODE_CWN || m_mode == MODE_CWW))
				m_cwKeyer->clock();

			if (m_voiceKeyer->isActive() && m_mode != MODE_CWN && m_mode != MODE_CWW)
				m_voiceKeyer->clock();
		} else {
			m_rxReader->clock();
		}
	}

	switch (id) {
		case RX_READER: {
				if (m_transmit)
					return;

				scaleBuffer(inBuffer, nSamples, m_rfGain);

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_rxRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					::wxLogError(wxT("Overrun in RX ring buffer, needed %u available %u"), nSamples, n);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case TX_READER: {
				if (!m_transmit)
					return;

				// If the voice or CW keyer are active, exit
				if (m_voiceKeyer->isActive() || m_cwKeyer->isActive())
					return;

				scaleBuffer(inBuffer, nSamples, m_micGain);

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_txRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					::wxLogError(wxT("Overrun in TX ring buffer, needed %u available %u"), nSamples, n);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case VOICE_READER: {
				if (!m_transmit)
					return;

				if (m_mode == MODE_CWN || m_mode == MODE_CWW)
					return;				

				scaleBuffer(inBuffer, nSamples, m_micGain);

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_txRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					::wxLogError(wxT("Overrun in TX ring buffer, needed %u available %u"), nSamples, n);

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case CW_READER: {
				if (!m_transmit)
					return;

				if (m_mode != MODE_CWN && m_mode != MODE_CWW)
					return;

				scaleBuffer(inBuffer, nSamples, 0.9F);

				// Send the tone out for the side tone
				unsigned int n1 = m_rxRingBuffer.addData(inBuffer, nSamples);
				if (n1 != nSamples)
					::wxLogError(wxT("Overrun in RX ring buffer, needed %u available %u"), nSamples, n1);

				// Now transmit it
				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n2 = m_txRingBuffer.addData(m_outBuffer, nSamples);
				if (n2 != nSamples)
					::wxLogError(wxT("Overrun in TX ring buffer, needed %u available %u"), nSamples, n2);

				if (n1 > 0 || n2 > 0)
					m_waiting.Post();
			}
			break;

		default:
			::wxLogError(wxT("callback() from unknown source = %d"), id);
			break;
	}
}

void CDSPControl::setMode(int mode)
{
	// We need a copy too ...
	m_mode = mode;

	m_dttsp->setMode(mode);
}

void CDSPControl::setFilter(int filter)
{
	m_dttsp->setFilter(filter);
}

void CDSPControl::setAGC(int agc)
{
	m_dttsp->setAGC(agc);
}

void CDSPControl::setDeviation(int dev)
{
	m_dttsp->setDeviation(dev);
}

void CDSPControl::setTXAndFreq(bool transmit, float freq)
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

		m_txReader->purge();
		m_txWriter->purge();

		m_rxReader->purge();
		m_rxWriter->purge();

		m_cwKeyer->purge();
		m_voiceKeyer->purge();
	}

	m_transmit = transmit;

	if (transmit)
		m_dttsp->setTXAndFreq(true, freq + m_centreFreq);
	else
		m_dttsp->setTXAndFreq(false, -(freq + m_centreFreq));
}

void CDSPControl::setRIT(float freq)
{
	m_dttsp->setRIT(freq);
}

void CDSPControl::setNB(bool onOff)
{
	m_dttsp->setNB(onOff);
}

void CDSPControl::setNBValue(unsigned int value)
{
	m_dttsp->setNBValue(value);
}

void CDSPControl::setNB2(bool onOff)
{
	m_dttsp->setNB2(onOff);
}

void CDSPControl::setNB2Value(unsigned int value)
{
	m_dttsp->setNB2Value(value);
}

void CDSPControl::setSP(bool onOff)
{
	m_dttsp->setSP(onOff);
}

void CDSPControl::setSPValue(unsigned int value)
{
	m_dttsp->setSPValue(value);
}

void CDSPControl::setALCValue(unsigned int attack, unsigned int decay, unsigned int hang)
{
	m_dttsp->setALCValue(attack, decay, hang);
}

void CDSPControl::setRXIAndQ(int phase, int gain)
{
	m_dttsp->setRXIAndQ(phase, gain);
}

void CDSPControl::setTXIAndQ(int phase, int gain)
{
	m_dttsp->setTXIAndQ(phase, gain);
}

float CDSPControl::getMeter(int type)
{
	return m_dttsp->getMeter(type);
}

void CDSPControl::getSpectrum(float* spectrum, int pos)
{
	m_dttsp->getSpectrum(spectrum, pos);
}

void CDSPControl::setAFGain(unsigned int value)
{
	m_afGain = float(value) / 1000.0F;
}

void CDSPControl::setRFGain(unsigned int value)
{
	m_rfGain = float(value) / 1000.0F;
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
			::wxLogError(wxT("Cannot open file %s for recording"), fileName.c_str());
			delete sdfw;
			return false;
		}

		m_record = sdfw;

		::wxLogMessage(wxT("Opened file %s for recording"), fileName.c_str());
	} else if (!record && m_record != NULL) {
		CSoundFileWriter* sdfw = m_record;
		m_record = NULL;

		sdfw->close();
		delete sdfw;

		::wxLogMessage(wxT("Closed sound file"));
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

void CDSPControl::sendCW(unsigned int speed, const wxString& text)
{
	if (speed == 0)
		m_cwKeyer->abort();
	else
		m_cwKeyer->send(speed, text);
}

void CDSPControl::sendAudio(const wxString& fileName, int state)
{
	if (state == VOICE_STOPPED)
		m_voiceKeyer->abort();
	else
		m_voiceKeyer->send(fileName, state);
}

#if defined(__WXDEBUG__)
void CDSPControl::dumpBuffer(const wxString& title, float* buffer, unsigned int nSamples) const
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
