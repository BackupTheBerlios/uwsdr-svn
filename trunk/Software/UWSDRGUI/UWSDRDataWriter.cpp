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

#include "UWSDRDataWriter.h"


const unsigned int HEADER_SIZE = 6;
const unsigned int SAMPLE_SIZE = 4;

// const unsigned int MAX_SAMPLES = 210;


CUWSDRDataWriter::CUWSDRDataWriter(CUDPDataWriter* writer, unsigned int version, unsigned int maxSamples, bool delay) :
wxThread(),
m_writer(writer),
m_version(version),
m_sequence(0U),
m_sockBuffer(NULL),
m_dataBuffer(NULL),
m_buffer(NULL),
m_delay(delay),
m_delayTime(0UL),
m_waiting(),
m_requests(0U),
m_overruns(0U),
m_packets(0U),
m_maxSamples(maxSamples),
m_packetRequests(0U),
m_enabled(false)
{
	wxASSERT(m_writer != NULL);
}

CUWSDRDataWriter::~CUWSDRDataWriter()
{
}

bool CUWSDRDataWriter::open(float sampleRate, unsigned int blockSize)
{
	m_enabled = false;

	bool ret = m_writer->open();
	if (!ret)
		return false;

	m_sockBuffer = new unsigned char[HEADER_SIZE + m_maxSamples * SAMPLE_SIZE];
	m_dataBuffer = new float[m_maxSamples * 2];

	m_buffer = new CRingBuffer(blockSize * 10, 2);

	if (m_delay) {
		m_delayTime = (500UL * m_maxSamples) / (unsigned long)sampleRate;
		::wxLogMessage(wxT("Delay is %lums"), m_delayTime);
	}

	Create();
	Run();

	return true;
}

/*
 * Put the data into the ring buffer, and use the event to send it out.
 */
void CUWSDRDataWriter::write(const float* buffer, unsigned int nSamples)
{
	wxASSERT(buffer != NULL);
	wxASSERT(nSamples > 0);

	if (!m_enabled)
		return;

	unsigned int n = m_buffer->addData(buffer, nSamples);

	m_requests++;

	if (n != nSamples)
		m_overruns++;

	if (n > 0)
		m_waiting.Post();
}

/*
 * Put the data into the ring buffer, and use the event to send it out.
 */
void* CUWSDRDataWriter::Entry()
{
	while (!TestDestroy()) {
		wxSemaError ret = m_waiting.WaitTimeout(500UL);

		if (ret == wxSEMA_NO_ERROR)
			writePacket();
	}

	m_writer->close();

	delete[] m_sockBuffer;
	delete[] m_dataBuffer;
	delete   m_buffer;

	::wxLogMessage(wxT("UWSDRDataWriter: %u max samples, %u overruns, %u requests, %u packet requests, %u packets"), m_maxSamples, m_overruns, m_requests, m_packetRequests, m_packets);

	return (void*)0;
}

void CUWSDRDataWriter::writePacket()
{
	unsigned int nSamples = m_buffer->getData(m_dataBuffer, m_maxSamples);

	m_packetRequests++;

	while (nSamples > 0 && !TestDestroy()) {
		m_packets++;

		m_sockBuffer[0] = 'D';
		m_sockBuffer[1] = 'T';

		m_sockBuffer[2] = (m_sequence >> 0) & 0xFF;
		m_sockBuffer[3] = (m_sequence >> 8) & 0xFF;

		m_sequence += 2;
		if (m_sequence > 0xFFFF) {
			if ((m_sequence % 2) == 0)
				m_sequence = 1;
			else
				m_sequence = 0;
		}

		m_sockBuffer[4] = (nSamples >> 0) & 0xFF;
		m_sockBuffer[5] = (nSamples >> 8) & 0xFF;

		unsigned int len = HEADER_SIZE;
		for (unsigned int i = 0; i < nSamples; i++) {
			unsigned int iData = (unsigned int)((m_dataBuffer[i * 2 + 0] + 1.0F) * 32767.0F + 0.5F);
			unsigned int qData = (unsigned int)((m_dataBuffer[i * 2 + 1] + 1.0F) * 32767.0F + 0.5F);

			m_sockBuffer[len++] = (iData >> 8) & 0xFF;
			m_sockBuffer[len++] = (iData >> 0) & 0xFF;

			m_sockBuffer[len++] = (qData >> 8) & 0xFF;
			m_sockBuffer[len++] = (qData >> 0) & 0xFF;
		}

		bool ret = m_writer->write(m_sockBuffer, len);
		if (!ret)
			return;

		if (m_delay)
			::wxMilliSleep(m_delayTime);

		nSamples = m_buffer->getData(m_dataBuffer, m_maxSamples);
	}
}

void CUWSDRDataWriter::close()
{
	m_enabled = false;

	Delete();
}

void CUWSDRDataWriter::enable(bool enable)
{
	m_buffer->clear();

	wxSemaError status = m_waiting.TryWait();
	while (status != wxSEMA_BUSY) {
		m_waiting.Wait();
		status = m_waiting.TryWait();
	}

	m_enabled = enable;
}

void CUWSDRDataWriter::disable()
{
	enable(false);
}
