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

enum {
	RTS,
	DTR
};

CSRTXRXController::CSRTXRXController(const wxString& device, int pin) :
m_device(device),
m_pin(pin),
m_txEnable(false),
m_port(NULL)
{
	wxASSERT(pin == RTS || pin == DTR);

	m_port = CSerialControl::getInstance(device);

	switch (pin) {
		case RTS:
			m_port->setRTS(false);
			break;
		case DTR:
			m_port->setDTR(false);
			break;
	}
}

CSRTXRXController::~CSRTXRXController()
{
}

void CSRTXRXController::setCallback(IControlInterface* WXUNUSED(callback), int WXUNUSED(id))
{
}

bool CSRTXRXController::open()
{
	if (m_port == NULL)
		return false;

	::wxLogMessage(wxT("SRXTXController: started on device %s"), m_device.c_str());

	return m_port->open();
}

void CSRTXRXController::enableTX(bool on)
{
	m_txEnable = on;
}

void CSRTXRXController::enableRX(bool WXUNUSED(on))
{
}

void CSRTXRXController::setTXAndFreq(bool transmit, const CFrequency& WXUNUSED(freq))
{
	if (transmit && !m_txEnable)
		return;

	switch (m_pin) {
		case RTS:
			m_port->setRTS(transmit);
			break;
		case DTR:
			m_port->setDTR(transmit);
			break;
	}
}

void CSRTXRXController::sendCommand(const char* WXUNUSED(command))
{
}

void CSRTXRXController::setClockTune(unsigned int WXUNUSED(clock))
{
}

void CSRTXRXController::close()
{
	m_port->setRTS(false);
	m_port->setDTR(false);

	m_port->close();
}
