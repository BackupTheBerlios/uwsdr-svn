/*
 *   Copyright (C) 2007 by Jonathan Naylor G4KLX
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
#include "SRTXRXController.h"

#include "SerialControl.h"


CSRTXRXController::CSRTXRXController(const wxString& device, int pin) :
m_txEnable(false),
m_port(NULL)
{
	switch (pin) {
		case PIN_RTS:
		case PIN_DTR:
			m_port = new CSerialControl(device, pin);
			break;

		default:
			::wxLogError(wxT("Unknown control port %s"), device.c_str());
			break;
	}
}

CSRTXRXController::~CSRTXRXController()
{
	delete m_port;
}

void CSRTXRXController::setCallback(IControlInterface* callback, int id)
{
}

bool CSRTXRXController::open()
{
	if (m_port == NULL)
		return false;

	return m_port->open();
}

void CSRTXRXController::enableTX(bool on)
{
	m_txEnable = on;
}

void CSRTXRXController::enableRX(bool on)
{
}

void CSRTXRXController::setTXAndFreq(bool transmit, const CFrequency& freq)
{
	if (transmit && !m_txEnable)
		return;

	if (transmit)
		m_port->keyTX();
	else
		m_port->unkeyTX();
}

void CSRTXRXController::sendCommand(const char* command)
{
}

void CSRTXRXController::setClockTune(unsigned int clock)
{
}

void CSRTXRXController::close()
{
	m_port->unkeyTX();
	m_port->close();
}
