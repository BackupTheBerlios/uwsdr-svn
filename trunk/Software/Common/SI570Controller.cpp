/*
 *   Copyright (C) 2013 by Jonathan Naylor G4KLX
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
#include "SI570Controller.h"

const unsigned int SI570_VID = 0x16C0;
const unsigned int SI570_PID = 0x05DC;

const unsigned int REQUEST_READ_VERSION = 0x00U;

#if defined(WIN32)

CSI570Controller::CSI570Controller() :
m_handle(INVALID_HANDLE_VALUE),
m_file(INVALID_HANDLE_VALUE),
m_frequency(),
m_txEnable(false),
m_tx(false)
{
}

CSI570Controller::~CSI570Controller()
{
}

bool CSI570Controller::open()
{
	WINUSB_SETUP_PACKET packet;
	packet.RequestType = (BMREQUEST_DEVICE_TO_HOST << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_DEVICE;
	packet.Request = REQUEST_READ_VERSION;
	packet.Value = 0xE00U;
	packet.Index = 0U;
	packet.Length = 2U;

	ULONG sent = 0UL;
	unsigned char version[2U];
	BOOL result = ::WinUsb_ControlTransfer(m_handle, packet, version, 2UL, &sent, NULL);

	if (!result) {
		::wxLogError(wxT("SI570Controller version: error: %lu"), ::GetLastError());
		::WinUsb_Free(m_handle);
		::CloseHandle(m_file);
		m_handle = INVALID_HANDLE_VALUE;
		m_file = INVALID_HANDLE_VALUE;
		return false;
	} else if (sent == 2) {
		::wxLogMessage(wxT("SI570Controller version: %u.%u"), version[1U], version[0U]);
		return true;
	} else {
		::wxLogMessage(wxT("SI570Controller version: unknown"));
		return true;
	}
}

void CSI570Controller::close()
{
	::WinUsb_Free(m_handle);
	::CloseHandle(m_file);

	m_handle = INVALID_HANDLE_VALUE;
	m_file = INVALID_HANDLE_VALUE;
}

#else

CSI570Controller::CSI570Controller() :
m_context(NULL),
m_device(NULL),
m_frequency(),
m_txEnable(false),
m_tx(false)
{
	::libusb_init(&m_context);
}

CSI570Controller::~CSI570Controller()
{
	::libusb_exit(m_context);
}

bool CSI570Controller::open()
{
	m_device = ::libusb_open_device_with_vid_pid(m_context, SI570_VID, SI570_PID);
	if (m_device == NULL) {
		::wxLogError(wxT("Could not find the SI570 USB device"));
		return false;
	}

	unsigned char version[2U];
	int n = ::libusb_control_msg(m_device, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, REQUEST_READ_VERSION, 0xE00U, 0U, (char*)version, 2U, 500);

	if (n < 0) {
		::wxLogError(wxT("SI570Controller version: error: %d"), n);
		::libusb_close(m_device);
		m_device = NULL;
		return false;
	} else if (n == 2) {
		::wxLogMessage(wxT("SI570Controller version: %u.%u"), version[1U], version[0U]);
		return true;
	} else {
		::wxLogMessage(wxT("SI570Controller version: unknown"));
		return true;
	}
}

void CSI570Controller::close()
{
	::libusb_close(m_device);

	m_device = NULL;
}

#endif

void CSI570Controller::setCallback(IControlInterface* WXUNUSED(callback))
{
}

void CSI570Controller::enableTX(bool on)
{
	m_txEnable = on;
}

void CSI570Controller::enableRX(bool WXUNUSED(on))
{
}

void CSI570Controller::setTXAndFreq(bool transmit, const CFrequency& freq)
{
	if (freq != m_frequency) {
		m_frequency = freq;
	}

	if (transmit && !m_tx) {
		m_tx = transmit;
	} else if (!transmit && m_tx) {
		m_tx = transmit;
	}
}

void CSI570Controller::sendCommand(const char* WXUNUSED(command))
{
}

void CSI570Controller::setClockTune(unsigned int WXUNUSED(clock))
{
}
