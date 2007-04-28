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

#include "UWSDRController.h"


CUWSDRController::CUWSDRController(CUDPDataReader* reader, CUDPDataWriter* writer, unsigned int version) :
m_reader(reader),
m_writer(writer),
m_id(0),
m_callback(NULL),
m_version(version),
m_txFreq(),
m_rxFreq(),
m_enableTX(false),
m_enableRX(false),
m_tx(false),
m_clock(99999)
{
	wxASSERT(m_reader != NULL);
	wxASSERT(m_writer != NULL);
}

CUWSDRController::~CUWSDRController()
{
}

void CUWSDRController::setCallback(IControlInterface* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

bool CUWSDRController::open()
{
	bool ret = m_writer->open();
	if (!ret)
		return false;

	ret = m_reader->open();
	if (!ret)
		return false;

	m_reader->setCallback(this, 0);

	return true;
}

void CUWSDRController::enableTX(bool on)
{
	if (m_enableTX == on)
		return;

	switch (m_version) {
		case 1:
			sendCommand(on ? "ET1;" : "ET0;");
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_enableTX = on;;
}

void CUWSDRController::enableRX(bool on)
{
	if (m_enableRX == on)
		return;

	switch (m_version) {
		case 1:
			sendCommand(on ? "ER1;" : "ER0;");
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_enableRX = on;
}

void CUWSDRController::setTXAndFreq(bool transmit, const CFrequency& freq)
{
	if (transmit && m_tx && freq == m_txFreq)
		return;

	if (!transmit && !m_tx && freq == m_rxFreq)
		return;

	if (transmit && freq != m_txFreq) {
		char command[25];

		switch (m_version) {
			case 1:
				::sprintf(command, "FT%s;", freq.getString().c_str());
				sendCommand(command);
				break;
			default:
				wxASSERT(false);
				break;
		}

		m_txFreq = freq;
	} else if (!transmit && freq != m_rxFreq) {
		char command[25];

		switch (m_version) {
			case 1:
				::sprintf(command, "FR%s;", freq.getString().c_str());
				sendCommand(command);
				break;
			default:
				wxASSERT(false);
				break;
		}

		m_rxFreq = freq;
	}

	if (transmit == m_tx)
		return;

	switch (m_version) {
		case 1:
			sendCommand(transmit ? "TX1;" : "TX0;");
			break;
		default:
			wxASSERT(false);
			break;
	}

	m_tx = transmit;
}

void CUWSDRController::setClockTune(unsigned int clock)
{
	clock += 32768;

	if (clock == m_clock)
		return;

	switch (m_version) {
		case 1: {
			char command[25];
			::sprintf(command, "CF%u;", clock);
			sendCommand(command);
			break;
		}
		default:
			wxASSERT(false);
			break;
	}

	m_clock = clock;
}

void CUWSDRController::sendCommand(const char* command)
{
	m_writer->write(command, ::strlen(command));
}

void CUWSDRController::close()
{
	m_reader->close();
	m_writer->close();
}

bool CUWSDRController::callback(char* buffer, unsigned int len, int id)
{
	wxASSERT(m_callback != NULL);

	switch (m_version) {
		case 1:
			if (::strncmp(buffer, "AK", 2) == 0)
				return true;
			if (::strncmp(buffer, "NK", 2) == 0) {
				buffer[len] = '\0';
				m_callback->sdrCommandNAK(buffer, m_id);
				return true;
			}
			return false;

		default:
			return false;
	}
}
