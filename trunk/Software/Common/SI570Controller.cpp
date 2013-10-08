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

const unsigned int REQUEST_GET_VERSION   = 0x00U;
const unsigned int REQUEST_SET_FREQUENCY = 0x32U;
const unsigned int REQUEST_SET_TRANSMIT  = 0x50U;

#if defined(WIN32)

#include <Setupapi.h>
#include "HID.h"

CSI570Controller::CSI570Controller() :
m_handle(INVALID_HANDLE_VALUE),
m_file(INVALID_HANDLE_VALUE),
m_callback(NULL),
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
	GUID guid;
	::HidD_GetHidGuid(&guid);

	HDEVINFO devInfo = ::SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (devInfo == INVALID_HANDLE_VALUE) {
		wxLogError(wxT("Error from SetupDiGetClassDevs: err=%lu"), ::GetLastError());
		return false;
	}

	SP_DEVICE_INTERFACE_DATA devInfoData;
	devInfoData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

	for (unsigned int index = 0U; ::SetupDiEnumDeviceInterfaces(devInfo, NULL, &guid, index, &devInfoData); index++) {
		DWORD length;
		::SetupDiGetDeviceInterfaceDetail(devInfo, &devInfoData, NULL, 0U, &length, NULL);

		PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = PSP_DEVICE_INTERFACE_DETAIL_DATA(::malloc(length));
		detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

		DWORD required;
		BOOL ret1 = ::SetupDiGetDeviceInterfaceDetail(devInfo, &devInfoData, detailData, length, &required, NULL);
		if (!ret1) {
			wxLogError(wxT("Error from SetupDiGetDeviceInterfaceDetail: err=%lu"), ::GetLastError());
			::SetupDiDestroyDeviceInfoList(devInfo);
			::free(detailData);
			return false;
		}

		// Get the handle for getting the attributes
		HANDLE handle = ::CreateFile(detailData->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if (handle == INVALID_HANDLE_VALUE) {
			wxLogError(wxT("Error from CreateFile: err=%u"), ::GetLastError());
			::SetupDiDestroyDeviceInfoList(devInfo);
			::free(detailData);
			return false;
		}

		HIDD_ATTRIBUTES attributes;
		attributes.Size = sizeof(HIDD_ATTRIBUTES);
		BOOL res = ::HidD_GetAttributes(handle, &attributes);
		if (!res) {
			wxLogError(wxT("Error from HidD_GetAttributes: err=%u"), ::GetLastError());
			::CloseHandle(handle);
			::SetupDiDestroyDeviceInfoList(devInfo);
			::free(detailData);
			return false;
		}

		::CloseHandle(handle);

		if (attributes.VendorID  == SI570_VID && attributes.ProductID == SI570_PID) {
			m_handle = ::CreateFile(detailData->DevicePath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (m_handle == INVALID_HANDLE_VALUE) {
				wxLogError(wxT("Error from CreateFile: err=%u"), ::GetLastError());
				::SetupDiDestroyDeviceInfoList(devInfo);
				::free(detailData);
				return false;
			}

			m_file = ::CreateFile(detailData->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
			if (m_file == INVALID_HANDLE_VALUE) {
				wxLogError(wxT("Error from CreateFile: err=%lu"), ::GetLastError());
				::SetupDiDestroyDeviceInfoList(devInfo);
				::free(detailData);
				return false;
			}

			ret1 = ::WinUsb_Initialize(m_file, &m_handle);
			if (!ret1) {
				wxLogError(wxT("Error from WinUsb_Initialize: err=%lu"), ::GetLastError());
				::SetupDiDestroyDeviceInfoList(devInfo);
				::CloseHandle(m_file);
				::free(detailData);
				return false;
			}

			::SetupDiDestroyDeviceInfoList(devInfo);
			::free(detailData);

			WINUSB_SETUP_PACKET packet;
			packet.RequestType = (BMREQUEST_DEVICE_TO_HOST << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_DEVICE;
			packet.Request = REQUEST_GET_VERSION;
			packet.Value = 0xE00U;
			packet.Index = 0U;
			packet.Length = 2U;

			ULONG sent = 0UL;
			unsigned char version[2U];
			BOOL result = ::WinUsb_ControlTransfer(m_handle, packet, version, 2UL, &sent, NULL);

			if (!result) {
				::wxLogError(wxT("Error from WinUsb_ControlTransfer: err=%lu"), ::GetLastError());
				::WinUsb_Free(m_handle);
				::CloseHandle(m_file);
				return false;
			} else if (sent == 2) {
				::wxLogMessage(wxT("SI570Controller version: %u.%u"), version[1U], version[0U]);
				return true;
			} else {
				::wxLogMessage(wxT("SI570Controller version: unknown"));
				return true;
			}
		}
	}

	::SetupDiDestroyDeviceInfoList(devInfo);

	::wxLogError(wxT("Could not find the SI570 USB device"));

	return false;
}

bool CSI570Controller::setFrequency(const CFrequency& freq)
{
	WINUSB_SETUP_PACKET packet;
	packet.RequestType = (BMREQUEST_HOST_TO_DEVICE << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_DEVICE;
	packet.Request = REQUEST_SET_FREQUENCY;
	packet.Value = 0U;
	packet.Index = 0U;
	packet.Length = 4U;

	double dFrequency = double(freq.get()) / 1000000.0;

	wxUint32 frequency = wxUint32(dFrequency * (1UL << 21));

	ULONG sent = 0UL;
	BOOL result = ::WinUsb_ControlTransfer(m_handle, packet, (UCHAR*)&frequency, 4UL, &sent, NULL);
	if (!result) {
		::wxLogError(wxT("Error from WinUsb_ControlTransfer: err=%lu"), ::GetLastError());
		return false;
	}

	return true;
}

bool CSI570Controller::setTransmit(bool tx)
{
	WINUSB_SETUP_PACKET packet;
	packet.RequestType = (BMREQUEST_DEVICE_TO_HOST << 7) | (BMREQUEST_VENDOR << 5) | BMREQUEST_TO_DEVICE;
	packet.Request = REQUEST_SET_TRANSMIT;
	packet.Value = tx ? 0x01U : 0x00U;
	packet.Index = 0U;
	packet.Length = 1U;

	ULONG sent = 0UL;
	unsigned char key;
	BOOL result = ::WinUsb_ControlTransfer(m_handle, packet, &key, 1UL, &sent, NULL);
	if (!result) {
		::wxLogError(wxT("Error from WinUsb_ControlTransfer: err=%lu"), ::GetLastError());
		return false;
	}

	return true;
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
m_callback(NULL),
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
	int n = ::libusb_control_transfer(m_device, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, REQUEST_GET_VERSION, 0xE00U, 0U, version, 2U, 500);

	if (n < 0) {
		::wxLogError(wxT("Error from libusb_control_transfer: err=%d"), n);
		::libusb_close(m_device);
		return false;
	} else if (n == 2) {
		::wxLogMessage(wxT("SI570Controller version: %u.%u"), version[1U], version[0U]);
		return true;
	} else {
		::wxLogMessage(wxT("SI570Controller version: unknown"));
		return true;
	}
}

bool CSI570Controller::setFrequency(const CFrequency& freq)
{
	double dFrequency = double(freq.get()) / 1000000.0;

	wxUint32 frequency = wxUint32(dFrequency * (1UL << 21));

	int n = ::libusb_control_transfer(m_device, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT, REQUEST_SET_FREQUENCY, 0U, 0U, (unsigned char*)&frequency, 4U, 500);
	if (n < 0) {
		::wxLogError(wxT("Error from libusb_control_transfer: err=%d"), n);
		return false;
	}

	return true;
}

bool CSI570Controller::setTransmit(bool tx)
{
	uint16_t value = tx ? 0x01U : 0x00U;

	unsigned char key;
	int n = ::libusb_control_transfer(m_device, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, REQUEST_SET_TRANSMIT, value, 0U, &key, 1U, 500);
	if (n < 0) {
		::wxLogError(wxT("Error from libusb_control_transfer: err=%d"), n);
		return false;
	}

	return true;
}

void CSI570Controller::close()
{
	::libusb_close(m_device);

	m_device = NULL;
}

#endif

void CSI570Controller::setCallback(IControlInterface* callback)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
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
	wxASSERT(m_callback != NULL);

	if (transmit && !m_txEnable)
		return;

	if (freq != m_frequency) {
		bool ok = setFrequency(freq);
		if (!ok) {
			m_callback->connectionLost();
			return;
		}

		m_frequency = freq;
	}

	if (transmit != m_tx) {
		bool ok = setTransmit(transmit);
		if (!ok) {
			m_callback->connectionLost();
			return;
		}

		m_tx = transmit;
	}
}

void CSI570Controller::sendCommand(const char* WXUNUSED(command))
{
}

void CSI570Controller::setClockTune(unsigned int WXUNUSED(clock))
{
}
