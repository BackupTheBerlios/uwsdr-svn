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

#include "UWSDRDataReader.h"

const unsigned int HEADER_SIZE = 8U;
const unsigned int SAMPLE_SIZE = 6U;


CUWSDRDataReader::CUWSDRDataReader(CUDPDataReader* reader, unsigned int version) :
m_reader(reader),
m_version(version),
m_blockSize(0),
m_size(0),
m_id(0),
m_callback(NULL),
m_sequence(-1),
m_ringBuffer(NULL),
m_sampBuffer(NULL),
m_missed(0),
m_requests(0),
m_underruns(0)
{
	wxASSERT(m_reader != NULL);
}

CUWSDRDataReader::~CUWSDRDataReader()
{
}

void CUWSDRDataReader::setCallback(IDataCallback* callback, int id)
{
	m_callback = callback;
	m_id       = id;
}

bool CUWSDRDataReader::open(float WXUNUSED(sampleRate), unsigned int blockSize)
{
	m_blockSize = blockSize;
	m_sequence  = -1;

	bool ret = m_reader->open();
	if (!ret)
		return false;

	m_reader->setCallback(this, 0);

	m_size = HEADER_SIZE + m_blockSize * SAMPLE_SIZE;

	m_ringBuffer = new CRingBuffer(m_blockSize * 10, 2);
	m_sampBuffer = new float[m_blockSize * 2];

	return true;
}

void CUWSDRDataReader::close()
{
	m_reader->close();

	delete   m_ringBuffer;
	delete[] m_sampBuffer;

	::wxLogMessage(wxT("UWSDRDataReader: %u missed frames, %u requests, %u underruns"), m_missed, m_requests, m_underruns);

	delete this;
}

void CUWSDRDataReader::clock()
{
	wxASSERT(m_callback != NULL);

	m_requests++;

	unsigned int len = m_ringBuffer->getData(m_sampBuffer, m_blockSize);

	if (len != m_blockSize)
		m_underruns++;

	if (len > 0)
		m_callback->callback(m_sampBuffer, len, m_id);
}

void CUWSDRDataReader::purge()
{
	m_ringBuffer->clear();
}

bool CUWSDRDataReader::hasClock()
{
	return false;
}

bool CUWSDRDataReader::callback(char* buffer, unsigned int len, int WXUNUSED(id))
{
	if (len < HEADER_SIZE || buffer[0] != 'D' || buffer[1] != 'R')
		return false;

	int seqNo = (wxUint8(buffer[3]) << 8) + wxUint8(buffer[2]);

	if (m_sequence != -1 && seqNo != m_sequence) {
		m_missed++;

		if (seqNo < m_sequence && (seqNo % 2) == (m_sequence % 2)) {
			::wxLogWarning(wxT("UWSDRDataReader: Packet dropped at sequence no: %d, expected: %d"), seqNo, m_sequence);
			return true;
		} else {
			::wxLogWarning(wxT("UWSDRDataReader: Packet missed at sequence no: %d, expected: %d"), seqNo, m_sequence);
		}
	}

	m_sequence = seqNo + 2;
	if (m_sequence > 0xFFFF) {
		if ((m_sequence % 2) == 0)
			m_sequence = 1;
		else
			m_sequence = 0;
	}

	unsigned int nSamples = (wxUint8(buffer[5]) << 8) + wxUint8(buffer[4]);

	unsigned int agc = buffer[6];

	unsigned int n = HEADER_SIZE;
	for (unsigned int i = 0; i < nSamples && n < len; n += SAMPLE_SIZE, i++) {
		wxInt32 iData = (buffer[n + 0] << 24) & 0xFF000000;
		iData |= (buffer[n + 1] << 16) & 0xFF0000;
		iData |= (buffer[n + 2] << 8) & 0xFF00;

		wxInt32 qData = (buffer[n + 3] << 24) & 0xFF000000;
		qData |= (buffer[n + 4] << 16) & 0xFF0000;
		qData |= (buffer[n + 5] << 8) & 0xFF00;

		float floatBuffer[2];
		floatBuffer[0] = float(iData) / float(0x7FFFFFFF);
		floatBuffer[1] = float(qData) / float(0x7FFFFFFF);

		if (agc > 1U) {
			floatBuffer[0] *= float(agc);
			floatBuffer[1] *= float(agc);
		}

		m_ringBuffer->addData(floatBuffer, 1);
	}

	return true;
}
