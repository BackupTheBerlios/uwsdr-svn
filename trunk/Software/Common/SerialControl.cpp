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

wxMutex CSerialControl::s_mutex;

wxArrayString CSerialControl::getDevs()
{
	wxArrayString devices;

	devices.Alloc(10);

	for (unsigned int i = 0; s_serialList[i].name != NULL; i++)
		devices.Add(s_serialList[i].name);

	return devices;
}

CSerialControl* CSerialControl::getInstance(const wxString& dev)
{
	wxMutexLocker lock(s_mutex);

	SSerialList* p = NULL;
	for (unsigned int i = 0; s_serialList[i].name != NULL; i++) {
		if (dev.IsSameAs(s_serialList[i].name)) {
			p = &s_serialList[i];
			break;
		}
	}

	wxASSERT(p != NULL);

	if (p->ptr == NULL)
		p->ptr = new CSerialControl(dev);

	return p->ptr;
}

void CSerialControl::setRTS(bool set)
{
	m_rts = set;
}

void CSerialControl::setDTR(bool set)
{
	m_dtr = set;
}

bool CSerialControl::getCTS() const
{
	return m_cts;
}

bool CSerialControl::getDSR() const
{
	return m_dsr;
}

#if defined(__WINDOWS__)

SSerialList CSerialControl::s_serialList[] = {
	{wxT("\\\\.\\COM1"), NULL},
	{wxT("\\\\.\\COM2"), NULL},
	{wxT("\\\\.\\COM3"), NULL},
	{wxT("\\\\.\\COM4"), NULL},
	{wxT("\\\\.\\COM5"), NULL},
	{wxT("\\\\.\\COM6"), NULL},
	{wxT("\\\\.\\COM7"), NULL},
	{wxT("\\\\.\\COM8"), NULL},
	{NULL,               NULL}
};

#include <winioctl.h>

CSerialControl::CSerialControl(const wxString& dev) :
m_dev(dev),
m_count(0),
m_rts(false),
m_dtr(false),
m_cts(false),
m_dsr(false),
m_lastRTS(false),
m_lastDTR(false),
m_handle(INVALID_HANDLE_VALUE)
{
}

CSerialControl::~CSerialControl()
{
}

bool CSerialControl::open()
{
	if (m_count > 0) {
		m_count++;
		return true;
	}

	m_handle = ::CreateFile(m_dev.mb_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_handle == INVALID_HANDLE_VALUE) {
		::wxLogError(wxT("Cannot open the serial port - 0x%lX"), ::GetLastError());
		return false;
	}

	DCB dcb;
	if (::GetCommState(m_handle, &dcb) == 0) {
		close();
		::wxLogError(wxT("%s is not a serial port"), m_dev.c_str());
		return false;
	}

	clock();

	m_count++;

	return true;
}

void CSerialControl::clock()
{
	wxASSERT(m_handle != INVALID_HANDLE_VALUE);

	DWORD status;
	if (::GetCommModemStatus(m_handle, &status) == 0) {
		::wxLogError(wxT("Cannot get the serial port status - 0x%lX"), ::GetLastError());
		return;
	}

	m_cts = bool(status & MS_CTS_ON);

	m_dsr = bool(status & MS_DSR_ON);

	if (m_lastRTS != m_rts) {
		int rts = (m_rts) ? SETRTS : CLRRTS;

		if (::EscapeCommFunction(m_handle, rts) == 0) {
			::wxLogError(wxT("Cannot set RTS - 0x%lX"), ::GetLastError());
			return;
		}

		m_lastRTS = m_rts;
	}

	if (m_lastDTR != m_dtr) {
		int dtr = (m_dtr) ? SETDTR : CLRDTR;

		if (::EscapeCommFunction(m_handle, dtr) == 0) {
			::wxLogError(wxT("Cannot set DTR - 0x%lX"), ::GetLastError());
			return;
		}

		m_lastDTR = m_dtr;
	}
}

void CSerialControl::close()
{
	if (m_count > 1) {
		m_count--;
		return;
	}

	::CloseHandle(m_handle);

	delete this;
}

#else

#include <sys/ioctl.h>
#include <sys/stat.h>

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

SSerialList CSerialControl::s_serialList[] = {
	{wxT("/dev/ttyS0"), NULL},
	{wxT("/dev/ttyS1"), NULL},
	{wxT("/dev/ttyS2"), NULL},
	{wxT("/dev/ttyS3"), NULL},
	{wxT("/dev/ttyS4"), NULL},
	{wxT("/dev/ttyS5"), NULL},
	{wxT("/dev/ttyS6"), NULL},
	{wxT("/dev/ttyS7"), NULL},
	{NULL,              NULL}
};

CSerialControl::CSerialControl(const wxString& dev) :
m_dev(dev),
m_count(0),
m_rts(false),
m_dtr(false),
m_cts(false),
m_dsr(false),
m_lastRTS(false),
m_lastDTR(false),
m_fd(-1)
{
}

CSerialControl::~CSerialControl()
{
}

bool CSerialControl::open()
{
	if (m_count > 0) {
		m_count++;
		return true;
	}

	m_fd = ::open(m_dev.mb_str(), O_RDWR, 0);
	if (m_fd < 0) {
		::wxLogError(wxT("Cannot open the serial port - %d, %s"), errno, strerror(errno));
		return false;
	}

	if (::isatty(m_fd) == 0) {
		close();
		::wxLogError(wxT("%s is not a serial port"), m_dev.c_str());
		return false;
	}

	clock();

	m_count++;

	return true;
}

void CSerialControl::clock()
{
	wxASSERT(m_fd != -1);

	unsigned int y;
	if (::ioctl(m_fd, TIOCMGET, &y) < 0) {
		::wxLogError(wxT("Cannot get the serial port status - %d, %s"), errno, strerror(errno));
		return;
	}

	m_cts = y & TIOCM_CTS;

	m_dsr = y & TIOCM_DSR;

	if (m_rts == m_lastRTS && m_dtr == m_lastDTR)
		return;

	if (m_rts)
		y |= TIOCM_RTS;
	else
		y &= ~TIOCM_RTS;

	if (m_dtr)
		y |= TIOCM_DTR;
	else
		y &= ~TIOCM_DTR;

	if (::ioctl(m_fd, TIOCMSET, &y) < 0) {
		::wxLogError(wxT("Cannot set the serial port status - %d, %s"), errno, strerror(errno));
		return;
	}

	m_lastRTS = m_rts;
	m_lastDTR = m_dtr;
}

void CSerialControl::close()
{
	if (m_count > 1) {
		m_count--;
		return;
	}

	::close(m_fd);

	delete this;
}

#endif
