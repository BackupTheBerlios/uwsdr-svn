/*
 *   Copyright (C) 2002-2004,2007 by Jonathan Naylor G4KLX
 *   Copyright (C) 1999-2001 by Thomas Sailor HB9JNX
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

#include "SerialControl.h"

#include <sys/types.h>


#if defined(__WINDOWS__)

#include <winioctl.h>

CSerialControl::CSerialControl(const wxString& device, int pin) :
m_device(device),
m_pin(pin),
m_handle(INVALID_HANDLE_VALUE)
{
}

CSerialControl::~CSerialControl()
{
}

bool CSerialControl::open()
{
	wxASSERT(m_handle == INVALID_HANDLE_VALUE);

	m_handle = ::CreateFile(m_device.mb_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_handle == INVALID_HANDLE_VALUE) {
		::wxLogError(wxT("Cannot open the serial port - 0x%lX"), ::GetLastError());
		return false;
	}

	DCB dcb;
	if (::GetCommState(m_handle, &dcb) == 0) {
		close();
		::wxLogError(wxT("%s is not a serial port"), m_device.c_str());
		return false;
	}

	return unkeyTX();
}

bool CSerialControl::keyTX()
{
	wxASSERT(m_handle != INVALID_HANDLE_VALUE);

	switch (m_pin) {
		case PIN_RTS:
			if (::EscapeCommFunction(m_handle, SETRTS) == 0) {
				::wxLogError(wxT("Cannot key the transmitter - 0x%lX"), ::GetLastError());
				return false;
			}
			break;

		case PIN_DTR:
			if (::EscapeCommFunction(m_handle, SETDTR) == 0) {
				::wxLogError(wxT("Cannot key the transmitter - 0x%lX"), ::GetLastError());
				return false;
			}
			break;
	}

	return true;
}

bool CSerialControl::unkeyTX()
{
	wxASSERT(m_handle != INVALID_HANDLE_VALUE);

	switch (m_pin) {
		case PIN_RTS:
			if (::EscapeCommFunction(m_handle, CLRRTS) == 0) {
				::wxLogError(wxT("Cannot key the transmitter - 0x%lX"), ::GetLastError());
				return false;
			}
			break;

		case PIN_DTR:
			if (::EscapeCommFunction(m_handle, CLRDTR) == 0) {
				::wxLogError(wxT("Cannot key the transmitter - 0x%lX"), ::GetLastError());
				return false;
			}
			break;
	}

	return true;
}

void CSerialControl::close()
{
	wxASSERT(m_handle != INVALID_HANDLE_VALUE);

	unkeyTX();

	::CloseHandle(m_handle);
	m_handle = INVALID_HANDLE_VALUE;
}

wxArrayString CSerialControl::getDevices()
{
	wxArrayString devices;

	devices.Alloc(8);

	devices.Add(wxT("\\\\.\\COM1"));
	devices.Add(wxT("\\\\.\\COM2"));
	devices.Add(wxT("\\\\.\\COM3"));
	devices.Add(wxT("\\\\.\\COM4"));
	devices.Add(wxT("\\\\.\\COM5"));
	devices.Add(wxT("\\\\.\\COM6"));
	devices.Add(wxT("\\\\.\\COM7"));
	devices.Add(wxT("\\\\.\\COM8"));

	return devices;
}

#else

#include <sys/ioctl.h>
#include <sys/stat.h>

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

CSerialControl::CSerialControl(const wxString& device, int pin) :
m_device(device),
m_pin(pin),
m_fd(-1)
{
}

CSerialControl::~CSerialControl()
{
}

bool CSerialControl::open()
{
	wxASSERT(m_fd == -1);

	m_fd = ::open(m_device.mb_str(), O_RDWR, 0);
	if (m_fd < 0) {
		::wxLogError(wxT("Cannot open the serial port - %d, %s"), errno, strerror(errno));
		return false;
	}

	if (::isatty(m_fd) == 0) {
		close();
		::wxLogError(wxT("%s is not a serial port"), m_device.c_str());
		return false
	}

	return unkeyTX();
}

bool CSerialControl::keyTX()
{
	wxASSERT(m_fd != -1);

	unsigned int y;
	if (::ioctl(m_fd, TIOCMGET, &y) < 0) {
		::wxLogError(wxT("Cannot key the transmitter - %d, %s"), errno, strerror(errno));
		return false;
	}

	switch (m_pin) {
		case PIN_RTS:
			y |= TIOCM_RTS;
			break;

		case PIN_DTR:
			y |= TIOCM_DTR;
			break;
	}

	if (::ioctl(m_fd, TIOCMSET, &y) < 0) {
		::wxLogError(wxT("Cannot key the transmitter - %d, %s"), errno, strerror(errno));
		return false;
	}

	return true;
}

bool CSerialControl::unkeyTX()
{
	wxASSERT(m_fd != -1);

	unsigned int y;
	if (::ioctl(m_fd, TIOCMGET, &y) < 0) {
		::wxLogError(wxT("Cannot unkey the transmitter - %d, %s"), errno, strerror(errno));
		return false;
	}

	switch (m_pin) {
		case PIN_RTS:
			y &= ~TIOCM_RTS;
			break;

		case PIN_DTR:
			y &= ~TIOCM_DTR;
			break;
	}

	if (::ioctl(m_fd, TIOCMSET, &y) < 0) {
		::wxLogError(wxT("Cannot unkey the transmitter - %d, %s"), errno, strerror(errno));
		return false;
	}

	return true;
}

void CSerialControl::close()
{
	wxASSERT(m_fd != -1);

	unkeyTX();

	::close(m_fd);
	m_fd = -1;
}

wxArrayString CSerialControl::getDevices()
{
	wxArrayString devices;

	devices.Alloc(8);

	devices.Add(wxT("/dev/ttyS0"));
	devices.Add(wxT("/dev/ttyS1"));
	devices.Add(wxT("/dev/ttyS2"));
	devices.Add(wxT("/dev/ttyS3"));
	devices.Add(wxT("/dev/ttyS4"));
	devices.Add(wxT("/dev/ttyS5"));
	devices.Add(wxT("/dev/ttyS6"));
	devices.Add(wxT("/dev/ttyS7"));

	return devices;
}

#endif
