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

#include "DSPControl.h"
#include "UWSDRApp.h"

#include <wx/datetime.h>


const int TX_READER    = 44;
const int RX_READER    = 55;
const int CW_READER    = 66;
const int VOICE_READER = 77;

const unsigned int RINGBUFFER_SIZE = 100001;
const unsigned int BLOCK_SIZE      = 2048;		// XXXX

CDSPControl::CDSPControl(float sampleRate) :
wxThread(),
m_dttsp(NULL),
m_cwKeyer(NULL),
m_voiceKeyer(NULL),
m_sampleRate(sampleRate),
m_txReader(NULL),
m_txWriter(NULL),
m_rxReader(NULL),
m_rxWriter(NULL),
m_txInControl(NULL),
m_txInPin(IN_NONE),
m_keyInControl(NULL),
m_keyInPin(IN_NONE),
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
m_swap(false),
m_clockId(-1),
m_lastTXIn(false),
m_lastKeyIn(false),
m_rxUnderruns(0U),
m_rxOverruns(0U),
m_txUnderruns(0U),
m_txOverruns(0U)
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

void CDSPControl::setTXInControl(CSerialControl* control, INPIN pin)
{
	wxASSERT(control != NULL);
	wxASSERT(pin != IN_NONE);

	m_txInControl = control;
	m_txInPin     = pin;
}

void CDSPControl::setKeyInControl(CSerialControl* control, INPIN pin)
{
	wxASSERT(control != NULL);
	wxASSERT(pin != IN_NONE);

	m_keyInControl = control;
	m_keyInPin     = pin;
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

	m_rxWriter->enable();

	while (!TestDestroy()) {
		wxSemaError ret = m_waiting.WaitTimeout(500UL);

		if (ret == wxSEMA_NO_ERROR) {
			if (m_transmit) {
				unsigned int nSamples = m_txRingBuffer.getData(m_txBuffer, BLOCK_SIZE);
				if (nSamples != BLOCK_SIZE)
					m_txUnderruns++;

				if (nSamples > 0) {
					scaleBuffer(m_txBuffer, nSamples, m_power, m_swap);
					m_txWriter->write(m_txBuffer, nSamples);
				}
			}

			unsigned int nSamples = m_rxRingBuffer.getData(m_rxBuffer, BLOCK_SIZE);

			// Create silence on transmit if no sidetone is being transmitted
			if (nSamples == 0 && m_transmit) {
				::memset(m_rxBuffer, 0x00, BLOCK_SIZE * 2 * sizeof(float));
				nSamples = BLOCK_SIZE;
			}

			if (nSamples != BLOCK_SIZE)
				m_rxUnderruns++;

			scaleBuffer(m_rxBuffer, nSamples, m_afGain);
			m_rxWriter->write(m_rxBuffer, nSamples);

			// Don't record when transmitting
			if (m_record != NULL && !m_transmit)
				m_record->write(m_rxBuffer, nSamples);
		}
	}

	m_running = false;

	m_rxWriter->disable();
	m_txWriter->disable();

	::wxLogMessage(wxT("DSPControl: RX Overruns=%u RX Underruns=%u TX Overruns=%u TX Underruns=%u"), m_rxOverruns, m_rxUnderruns, m_txOverruns, m_txUnderruns);

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

	// Open the TX In port and set the relevant output pin high to be shorted to the relevant input pin
	if (m_txInControl != NULL) {
		ret = m_txInControl->open();

		if (!ret) {
			m_dttsp->close();
			m_cwKeyer->close();
			m_voiceKeyer->close();

			return false;
		}

		switch (m_txInPin) {
			case IN_RTS_CTS:
			case IN_RTS_DSR:
				m_txInControl->setRTS(true);
				break;
			case IN_DTR_DSR:
			case IN_DTR_CTS:
				m_txInControl->setDTR(true);
				break;
			default:
				break;
		}
	}

	// Open the Key In port and set the relevant output pin high to be shorted to the relevant input pin
	if (m_keyInControl != NULL) {
		ret = m_keyInControl->open();

		if (!ret) {
			m_dttsp->close();
			m_cwKeyer->close();
			m_voiceKeyer->close();

			return false;
		}

		switch (m_keyInPin) {
			case IN_RTS_CTS:
			case IN_RTS_DSR:
				m_keyInControl->setRTS(true);
				break;
			case IN_DTR_DSR:
			case IN_DTR_CTS:
				m_keyInControl->setDTR(true);
				break;
			default:
				break;
		}
	}

	if (m_rxReader->hasClock())
		m_clockId = RX_READER;

	if (m_clockId == -1) {
		m_dttsp->close();
		m_cwKeyer->close();
		m_voiceKeyer->close();

		::wxLogError(wxT("No reader can provide a suitable clock"));
		return false;
	}

	return true;
}

void CDSPControl::closeIO()
{
	setRecord(false);

	m_rxReader->close();
	m_txReader->close();
	m_rxWriter->close();
	m_txWriter->close();

	if (m_txInControl != NULL) {
		m_txInControl->setRTS(false);
		m_txInControl->setDTR(false);

		m_txInControl->close();
	}

	if (m_keyInControl != NULL) {
		m_keyInControl->setRTS(false);
		m_keyInControl->setDTR(false);

		m_keyInControl->close();
	}

	m_dttsp->close();
	m_cwKeyer->close();
	m_voiceKeyer->close();

	if (m_record != NULL)
		m_record->close();
}

void CDSPControl::callback(float* inBuffer, unsigned int nSamples, int id)
{
	// Don't process any data until the main thread is ready
	if (!m_running)
		return;

	wxASSERT(inBuffer != NULL);
	wxASSERT(nSamples > 0);

	// Use whatever clock is available to run everything
	if (id == m_clockId) {
		if (m_transmit) {
			if (id != TX_READER)
				m_txReader->clock();

			if (m_cwKeyer->isActive() && (m_mode == MODE_CWUN || m_mode == MODE_CWUW || m_mode == MODE_CWLN || m_mode == MODE_CWLW))
				m_cwKeyer->clock();

			if (m_voiceKeyer->isActive() && m_mode != MODE_CWUN && m_mode != MODE_CWUW && m_mode != MODE_CWLN && m_mode != MODE_CWLW)
				m_voiceKeyer->clock();
		} else {
			if (id != RX_READER)
				m_rxReader->clock();
		}

		if (m_txInControl != NULL)
			m_txInControl->clock();

		if (m_keyInControl != NULL && m_keyInControl != m_txInControl)
			m_keyInControl->clock();
	}

	switch (id) {
		case RX_READER: {
				if (m_transmit)
					return;

				scaleBuffer(inBuffer, nSamples, m_rfGain, m_swap);

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_rxRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					m_rxOverruns++;

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case TX_READER: {
				if (!m_transmit)
					return;

				// If the voice or CW keyer are active, exit
				if (m_voiceKeyer->isActive() && m_mode != MODE_CWUN && m_mode != MODE_CWUW && m_mode != MODE_CWLN && m_mode != MODE_CWLW)
					return;
				if (m_cwKeyer->isActive() && (m_mode == MODE_CWUN || m_mode == MODE_CWUW || m_mode == MODE_CWLN || m_mode == MODE_CWLW))
					return;

				scaleBuffer(inBuffer, nSamples, m_micGain);

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_txRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					m_txOverruns++;

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case VOICE_READER: {
				if (!m_transmit)
					return;

				if (m_mode == MODE_CWUN || m_mode == MODE_CWUW || m_mode == MODE_CWLN || m_mode == MODE_CWLW)
					return;				

				scaleBuffer(inBuffer, nSamples, m_micGain);

				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n = m_txRingBuffer.addData(m_outBuffer, nSamples);
				if (n != nSamples)
					m_txOverruns++;

				if (n > 0)
					m_waiting.Post();
			}
			break;

		case CW_READER: {
				if (!m_transmit)
					return;

				if (m_mode != MODE_CWUN && m_mode != MODE_CWUW && m_mode != MODE_CWLN && m_mode != MODE_CWLW)
					return;

				scaleBuffer(inBuffer, nSamples, 0.9F);

				// Send the tone out for the side tone
				unsigned int n1 = m_rxRingBuffer.addData(inBuffer, nSamples);
				if (n1 != nSamples)
					m_rxOverruns++;

				// Now transmit it
				m_dttsp->dataIO(inBuffer, m_outBuffer, nSamples);

				unsigned int n2 = m_txRingBuffer.addData(m_outBuffer, nSamples);
				if (n2 != nSamples)
					m_txOverruns++;

				if (n1 > 0 || n2 > 0)
					m_waiting.Post();
			}
			break;

		default:
			::wxLogError(wxT("callback() from unknown source = %d"), id);
			break;
	}

	if (m_txInControl != NULL) {
		bool state;

		switch (m_txInPin) {
			case IN_RTS_CTS:
			case IN_DTR_CTS:
				state = m_txInControl->getCTS();
				break;
			case IN_DTR_DSR:
			case IN_RTS_DSR:
				state = m_txInControl->getDSR();
				break;
			default:
				state = m_transmit;
				break;
		}

		if (state != m_lastTXIn) {
			::wxGetApp().setTransmit(state);
			m_lastTXIn = state;
		}
	}

	// Only service the key input when in CW mode
	if (m_mode == MODE_CWUN || m_mode == MODE_CWUW || m_mode == MODE_CWLN || m_mode == MODE_CWLW) {
		if (m_keyInControl != NULL) {
			bool state;

			switch (m_keyInPin) {
				case IN_RTS_CTS:
				case IN_DTR_CTS:
					state = m_keyInControl->getCTS();
					break;
				case IN_DTR_DSR:
				case IN_RTS_DSR:
					state = m_keyInControl->getDSR();
					break;
				default:
					state = false;
					break;
			}

			if (state != m_lastKeyIn) {
				m_cwKeyer->key(state);
				m_lastKeyIn = state;
			}
		}
	}
}

void CDSPControl::setMode(UWSDRMODE mode)
{
	// We need a copy too ...
	m_mode = mode;

	m_dttsp->setMode(mode);
}

void CDSPControl::setZeroIF(bool onOff)
{
	m_dttsp->setZeroIF(onOff);
}

void CDSPControl::swapIQ(bool swap)
{
	m_swap = swap;
}

void CDSPControl::setFilter(FILTERWIDTH filter)
{
	m_dttsp->setFilter(filter);
}

void CDSPControl::setAGC(AGCSPEED agc)
{
	m_dttsp->setAGC(agc);
}

void CDSPControl::setDeviation(FMDEVIATION dev)
{
	m_dttsp->setDeviation(dev);
}

void CDSPControl::setTXAndFreq(bool transmit, float freq)
{
	// On a change from transmit to receive and vice versa we empty the ring buffers and
	// drain the semaphore. We mute the transmit writer when on receive.
	if (transmit != m_transmit) {
		if (transmit)
			m_txWriter->enable();
		else
			m_txWriter->disable();

		wxSemaError status = m_waiting.TryWait();
		while (status != wxSEMA_BUSY) {
			m_waiting.Wait();
			status = m_waiting.TryWait();
		}

		m_rxRingBuffer.clear();
		m_rxReader->purge();

		m_txRingBuffer.clear();
		m_txReader->purge();

		m_cwKeyer->purge();
		m_voiceKeyer->purge();
	}

	m_transmit = transmit;

	m_dttsp->setTXAndFreq(transmit, freq);
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

void CDSPControl::setCarrierLevel(unsigned int value)
{
	m_dttsp->setCarrierLevel(value);
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

float CDSPControl::getMeter(METERPOS type)
{
	return m_dttsp->getMeter(type);
}

float CDSPControl::getTXOffset()
{
	return m_dttsp->getTXOffset();
}

float CDSPControl::getRXOffset()
{
	return m_dttsp->getRXOffset();
}

void CDSPControl::getSpectrum(float* spectrum, SPECTRUMPOS pos)
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
			return false;
		}

		m_record = sdfw;

		::wxLogMessage(wxT("Opened file %s for recording"), fileName.c_str());
	} else if (!record && m_record != NULL) {
		CSoundFileWriter* sdfw = m_record;
		m_record = NULL;

		sdfw->close();

		::wxLogMessage(wxT("Closed sound file"));
	}

	return true;
}

void CDSPControl::scaleBuffer(float* buffer, unsigned int nSamples, float scale, bool swap)
{
	wxASSERT(buffer != NULL);
	wxASSERT(scale >= 0.0 && scale <= 1.0);

	if (swap) {
		for (unsigned int i = 0; i < nSamples; i++) {
			float val = buffer[i * 2 + 0];
			buffer[i * 2 + 0] = buffer[i * 2 + 1] * scale;
			buffer[i * 2 + 1] = val * scale;
		}
	} else {
		for (unsigned int i = 0; i < nSamples * 2; i++)
			buffer[i] *= scale;
	}
}

void CDSPControl::sendCW(unsigned int speed, const wxString& text)
{
	switch (speed) {
		case CW_END:
			break;
		case CW_ABORT:
			m_cwKeyer->abort();
			break;
		default:
			m_cwKeyer->send(speed, text);
			break;
	}
}

void CDSPControl::sendAudio(const wxString& fileName, VOICESTATUS state)
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
