/*
 *   Copyright (C) 2002-2004,2007,2008 by Jonathan Naylor G4KLX
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

#if defined(__APPLE__) && defined(__MACH__)
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOTypes.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <sys/param.h>
#elif defined(__WINDOWS__)
#include <setupapi.h>
#include <winioctl.h>
#else
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#endif

wxMutex CSerialControl::s_mutex;

std::vector<SSerialList*> CSerialControl::s_serialList;

wxArrayString CSerialControl::getDevs()
{
	wxArrayString devices;

	devices.Alloc(10);

#if defined(__APPLE__) && defined(__MACH__)
	mach_port_t masterPort;
	kern_return_t ret = ::IOMasterPort(MACH_PORT_NULL, &masterPort);
	if (ret != KERN_SUCCESS) {
		::wxLogError(wxT("IOMasterPort() returned %d"), ret);
		return devices;
	}

	CFMutableDictionaryRef match = ::IOServiceMatching(kIOSerialBSDServiceValue);
	if (match == NULL)
		::wxLogWarning(wxT("IOServiceMatching() returned NULL"));
	else
		::CFDictionarySetValue(match, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDRS232Type));

	io_iterator_t services;
	ret = ::IOServiceGetMatchingServices(masterPort, match, &services);
	if (ret != KERN_SUCCESS) {
		::wxLogError(wxT("IOServiceGetMatchingServices() returned %d"), ret);
		return devices;
	}

	io_object_t modem;
	while ((modem = ::IOIteratorNext(services))) {
		CFTypeRef filePath = ::IORegistryEntryCreateCFProperty(modem, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
		if (filePath != NULL) {
			char port[MAXPATHLEN];
			Boolean result = ::CFStringGetCString((const __CFString*)filePath, port, MAXPATHLEN, kCFStringEncodingASCII);
			::CFRelease(filePath);

			if (result)
				devices.Add(port);

			::IOObjectRelease(modem);
		}
	}

	::IOObjectRelease(services);
#elif defined(__WINDOWS__)
	HDEVINFO devInfo = ::SetupDiGetClassDevs(&GUID_CLASS_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (devInfo == INVALID_HANDLE_VALUE) {
		::wxLogError(wxT("SetupDiGetClassDevs() failed, err=0x%lx"), ::GetLastError());
		return devices;
	}

	SP_DEVICE_INTERFACE_DATA ifcData;
	ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	DWORD detDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
	char* charData = new char[detDataSize];

	SP_DEVICE_INTERFACE_DETAIL_DATA* detData = (SP_DEVICE_INTERFACE_DETAIL_DATA *)charData;
	detData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	for (DWORD i = 0; ::SetupDiEnumDeviceInterfaces(devInfo, NULL, &GUID_CLASS_COMPORT, i, &ifcData); i++) {
		SP_DEVINFO_DATA devData;
		devData.cbSize = sizeof(SP_DEVINFO_DATA);

		BOOL ret = ::SetupDiGetDeviceInterfaceDetail(devInfo, &ifcData, detData, detDataSize, NULL, &devData);
		if (!ret) {
			::wxLogError(wxT("SetupDiGetDeviceInterfaceDetail() failed, err=0x%lx"), ::GetLastError());
			return devices;
		}

		devices.Add(detData->DevicePath);
	}

	if (::GetLastError() != NO_ERROR && ::GetLastError() != ERROR_NO_MORE_ITEMS) {
		::wxLogError(wxT("SetupDiEnumDeviceInterfaces() failed, err=0x%lx"), ::GetLastError());
		return devices;
	}

	delete[] charData;
	::SetupDiDestroyDeviceInfoList(devInfo);
#else
	devices.Add(wxT("/dev/ttyS0"));
	devices.Add(wxT("/dev/ttyS1"));
	devices.Add(wxT("/dev/ttyS2"));
	devices.Add(wxT("/dev/ttyS3"));
	devices.Add(wxT("/dev/ttyS4"));
	devices.Add(wxT("/dev/ttyUSB0"));
	devices.Add(wxT("/dev/ttyUSB1"));
	devices.Add(wxT("/dev/ttyUSB2"));
	devices.Add(wxT("/dev/ttyUSB3"));
	devices.Add(wxT("/dev/ttyUSB4"));
#endif

	return devices;
}

CSerialControl* CSerialControl::getInstance(const wxString& dev)
{
	wxMutexLocker lock(s_mutex);

	SSerialList* p = NULL;
	for (std::vector<SSerialList*>::iterator it = s_serialList.begin(); it != s_serialList.end(); it++) {
		if (dev.IsSameAs((*it)->name)) {
			p = *it;
			break;
		}
	}

	if (p == NULL) {
		p = new SSerialList;
		p->name = dev,
		p->ptr  = new CSerialControl(dev);
		s_serialList.push_back(p);
	}

	return p->ptr;
}

void CSerialControl::clock()
{
	m_run.Post();
}

void CSerialControl::close()
{
	if (m_count > 1) {
		m_count--;
		return;
	}

	m_exit = true;

	m_run.Post();
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

CSerialControl::CSerialControl(const wxString& dev) :
wxThread(),
m_dev(dev),
m_count(0),
m_rts(true),
m_dtr(true),
m_cts(false),
m_dsr(false),
m_mutex(),
m_run(),
m_exit(false),
m_handle(INVALID_HANDLE_VALUE)
{
}

CSerialControl::~CSerialControl()
{
}

bool CSerialControl::open()
{
	DWORD errCode;

	if (m_count > 0) {
		m_count++;
		return true;
	}

	m_handle = ::CreateFile(m_dev.mb_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_handle == INVALID_HANDLE_VALUE) {
		::wxLogError(wxT("SerialControl: cannot open the serial port - %ld"), ::GetLastError());
		return false;
	}

	DCB dcb;
	if (::GetCommState(m_handle, &dcb) == 0) {
		::ClearCommError(m_handle, &errCode, NULL);
		::CloseHandle(m_handle);
		::wxLogError(wxT("SerialControl: cannot get the serial port status - %ld"), ::GetLastError());
		return false;
	}

	dcb.fOutxCtsFlow = FALSE;
	dcb.fOutxDsrFlow = FALSE;
	dcb.fDtrControl  = DTR_CONTROL_DISABLE;
	dcb.fRtsControl  = RTS_CONTROL_DISABLE;

	if (::SetCommState(m_handle, &dcb) == 0) {
		::ClearCommError(m_handle, &errCode, NULL);
		::CloseHandle(m_handle);
		::wxLogError(wxT("SerialControl: cannot set the serial port status - %ld"), ::GetLastError());
		return false;
	}

	::ClearCommError(m_handle, &errCode, NULL);

	m_count++;

	Create();
	Run();

	return true;
}

void* CSerialControl::Entry()
{
	m_run.Wait();

	while (!m_exit) {
		m_mutex.Lock();

		DWORD status;
		DWORD errCode;
		if (::GetCommModemStatus(m_handle, &status) == 0) {
			::ClearCommError(m_handle, &errCode, NULL);
			::wxLogError(wxT("SerialControl: cannot get the serial port status - %ld"), ::GetLastError());
		} else {
			m_cts = (status & MS_CTS_ON) == MS_CTS_ON;
			m_dsr = (status & MS_DSR_ON) == MS_DSR_ON;
		}

		m_mutex.Unlock();

		m_run.Wait();
	}

	::CloseHandle(m_handle);

	return (void*)0;
}

bool CSerialControl::setRTS(bool set)
{
	wxMutexLocker lock(m_mutex);

	if (set == m_rts)
		return true;

	DWORD rts = (set) ? SETRTS : CLRRTS;
	DWORD errCode;

	if (::EscapeCommFunction(m_handle, rts) == 0) {
		::ClearCommError(m_handle, &errCode, NULL);
		::wxLogError(wxT("SerialControl: cannot set RTS - %ld"), ::GetLastError());
		return false;
	}

	m_rts = set;

	return true;
}

bool CSerialControl::setDTR(bool set)
{
	wxMutexLocker lock(m_mutex);

	if (set == m_dtr)
		return true;

	DWORD dtr = (set) ? SETDTR : CLRDTR;
	DWORD errCode;

	if (::EscapeCommFunction(m_handle, dtr) == 0) {
		::ClearCommError(m_handle, &errCode, NULL);
		::wxLogError(wxT("SerialControl: cannot set DTR - %ld"), ::GetLastError());
		return false;
	}

	m_dtr = set;

	return true;
}

#else

CSerialControl::CSerialControl(const wxString& dev) :
wxThread(),
m_dev(dev),
m_count(0),
m_rts(true),
m_dtr(true),
m_cts(false),
m_dsr(false),
m_mutex(),
m_run(),
m_exit(false),
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
		::wxLogError(wxT("SerialControl: cannot open the serial port - %d, %s"), errno, strerror(errno));
		return false;
	}

	if (::isatty(m_fd) == 0) {
		::close(m_fd);
		::wxLogError(wxT("SerialControl: %s is not a serial port"), m_dev.c_str());
		return false;
	}

	m_count++;

	Create();
	Run();

	return true;
}

void* CSerialControl::Entry()
{
	m_run.Wait();

	while (!m_exit) {
		m_mutex.Lock();

		unsigned int y;
		if (::ioctl(m_fd, TIOCMGET, &y) < 0) {
			::wxLogError(wxT("SerialControl: cannot get the serial port status - %d, %s"), errno, strerror(errno));
		} else {
			m_cts = (y & TIOCM_CTS) == TIOCM_CTS;
			m_dsr = (y & TIOCM_DSR) == TIOCM_DSR;
		}

		m_mutex.Unlock();

		m_run.Wait();
	}

	return (void*)0;
}

bool CSerialControl::setRTS(bool set)
{
	wxMutexLocker lock(m_mutex);

	if (set == m_rts)
		return true;

	unsigned int y;
	if (::ioctl(m_fd, TIOCMGET, &y) < 0) {
		::wxLogError(wxT("SerialControl: cannot get the serial port status - %d, %s"), errno, strerror(errno));
		return false;
	}

	if (set)
		y |= TIOCM_RTS;
	else
		y &= ~TIOCM_RTS;

	if (::ioctl(m_fd, TIOCMSET, &y) < 0) {
		::wxLogError(wxT("SerialControl: cannot set the serial port status - %d, %s"), errno, strerror(errno));
		return false;
	}

	m_rts = set;

	return true;
}

bool CSerialControl::setDTR(bool set)
{
	wxMutexLocker lock(m_mutex);

	if (set == m_dtr)
		return true;

	unsigned int y;
	if (::ioctl(m_fd, TIOCMGET, &y) < 0) {
		::wxLogError(wxT("SerialControl: cannot get the serial port status - %d, %s"), errno, strerror(errno));
		return false;
	}

	if (set)
		y |= TIOCM_DTR;
	else
		y &= ~TIOCM_DTR;

	if (::ioctl(m_fd, TIOCMSET, &y) < 0) {
		::wxLogError(wxT("SerialControl: cannot set the serial port status - %d, %s"), errno, strerror(errno));
		return false;
	}

	m_dtr = set;

	return true;
}

#endif
