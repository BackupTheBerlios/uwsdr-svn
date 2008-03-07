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

#include "SerialInterface.h"

CSerialInterface::CSerialInterface(const wxString& device, INPIN pin) :
m_control(NULL),
m_pin(pin)
{
	wxASSERT(pin != IN_NONE);

	m_control = CSerialControl::getInstance(device);
	wxASSERT(m_control != NULL);
}

CSerialInterface::~CSerialInterface()
{
}

bool CSerialInterface::open()
{
	bool ret = m_control->open();
	if (!ret)
		return false;

	switch (m_pin) {
		case IN_RTS_CTS:
		case IN_RTS_DSR:
			m_control->setRTS(true);
			break;

		case IN_DTR_DSR:
		case IN_DTR_CTS:
			m_control->setDTR(true);
			break;

		default:
			break;
	}

	return true;
}

bool CSerialInterface::getState()
{
	switch (m_pin) {
		case IN_RTS_CTS:
		case IN_DTR_CTS:
			return m_control->getCTS();

		case IN_DTR_DSR:
		case IN_RTS_DSR:
			return m_control->getDSR();

		default:
			return false;
	}
}

void CSerialInterface::close()
{
	switch (m_pin) {
		case IN_RTS_CTS:
		case IN_RTS_DSR:
			m_control->setRTS(false);
			break;

		case IN_DTR_DSR:
		case IN_DTR_CTS:
			m_control->setDTR(false);
			break;

		default:
			break;
	}

	m_control->close();
}

void CSerialInterface::clock()
{
	m_control->clock();
}
