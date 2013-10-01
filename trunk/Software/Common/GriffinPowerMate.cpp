/*
 *   Copyright (C) 2008,2013 by Jonathan Naylor G4KLX
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

#include "GriffinPowerMate.h"

const unsigned int POWERMATE_VENDOR_ID    = 0x077D;
const unsigned int POWERMATE_PRODUCT_ID   = 0x0410;
const unsigned int POWERMATE_IN_ENDPOINT  = 0x81;
const unsigned int POWERMATE_OUT_ENDPOINT = 0x02;

const unsigned int POWERMATE_TIMEOUT      = 1000U;

#if defined(__WINDOWS__)

#include <Setupapi.h>
#include "HID.h"

CGriffinPowerMate::CGriffinPowerMate() :
wxThread(wxTHREAD_JOINABLE),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_len(0),
m_button(false),
m_speed(1U),
m_killed(false),
m_file(INVALID_HANDLE_VALUE),
m_handle(INVALID_HANDLE_VALUE)
{
}

CGriffinPowerMate::~CGriffinPowerMate()
{
}

bool CGriffinPowerMate::open()
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

		// Is this a Griffin PowerMate?
		if (attributes.VendorID  == POWERMATE_VENDOR_ID && attributes.ProductID == POWERMATE_PRODUCT_ID) {
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

			ULONG value;
			ULONG valueLength = sizeof(ULONG);
			ret1 = ::WinUsb_GetPipePolicy(m_handle, POWERMATE_IN_ENDPOINT, MAXIMUM_TRANSFER_SIZE, &valueLength, &value);
			if (!ret1) {
				wxLogError(wxT("Error from WinUsb_GetPipePolicy: err=%lu"), ::GetLastError());
				::CloseHandle(m_handle);
				::CloseHandle(m_file);
				return false;
			}

			m_len = value;

			m_buffer = new unsigned char[m_len];

			Create();
			Run();

			return true;
		}
	}

	::SetupDiDestroyDeviceInfoList(devInfo);

	return false;
}

void CGriffinPowerMate::setCallback(IDialCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

void* CGriffinPowerMate::Entry()
{
	wxASSERT(m_callback != NULL);
	wxASSERT(m_handle != INVALID_HANDLE_VALUE);

	while (!m_killed) {
		ULONG written;
		BOOL res = ::WinUsb_ReadPipe(m_handle, POWERMATE_IN_ENDPOINT, m_buffer, m_len, &written, NULL);
		if (res) {
			bool button = m_buffer[0] == 1U;

			int knob = 0;
			switch (m_buffer[1]) {
				case 0x01U: knob = 1;  break;
				case 0xFFU: knob = -1; break;
			}

			if (button && !m_button) {
				switch (m_speed) {
					case 1U:  m_speed = 16U; break;
					case 4U:  m_speed = 1U;  break;
					case 9U:  m_speed = 4U;  break;
					case 16U: m_speed = 9U;  break;
				}
			}

			if (knob != 0)
				m_callback->dialMoved(m_id, knob * m_speed);

			m_button = button;
		}
	}

	delete[] m_buffer;

	::WinUsb_Free(m_handle);
	::CloseHandle(m_file);

	return NULL;
}

void CGriffinPowerMate::close()
{
	m_killed = true;

	Wait();
}

#else

CGriffinPowerMate::CGriffinPowerMate() :
wxThread(wxTHREAD_JOINABLE),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_len(0),
m_button(false),
m_speed(1U),
m_killed(false),
m_context(NULL),
m_handle(NULL)
{
	::libusb_init(&m_context);
}

CGriffinPowerMate::~CGriffinPowerMate()
{
	wxASSERT(m_context != NULL);

	::libusb_exit(m_context);
}

bool CGriffinPowerMate::open()
{
	m_handle = ::libusb_open_device_with_vid_pid(m_context, POWERMATE_VENDOR_ID, POWERMATE_PRODUCT_ID);
	if (m_handle == NULL) {
		wxLogError(wxT("Could not open the Griffin Powermate"));
		return false;
	}

	int res = ::libusb_claim_interface(m_handle, 0);
	if (res != 0) {
		wxLogError(wxT("Error from libusb_claim_interface: err=%d"), res);
		::libusb_close(m_handle);
		m_handle = NULL;
		return false;
	}

	libusb_device* device = ::libusb_get_device(m_handle);
	if (device == NULL) {
		wxLogError(wxT("Error from libusb_get_device"));
		::libusb_close(m_handle);
		m_handle = NULL;
		return false;
	}

	m_len = ::libusb_get_max_packet_size(device, POWERMATE_IN_ENDPOINT);

	m_buffer = new unsigned char[m_len];

	Create();
	Run();

	return true;
}

void CGriffinPowerMate::setCallback(IDialCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

void* CGriffinPowerMate::Entry()
{
	wxASSERT(m_handle != NULL);
	wxASSERT(m_callback != NULL);

	while (!m_killed) {
		int written;
		int n = ::libusb_interrupt_transfer(m_handle, POWERMATE_IN_ENDPOINT, m_buffer, m_len, &written, POWERMATE_TIMEOUT);
		if (n > 0) {
			bool button = m_buffer[0] == 1U;

			int knob = 0;
			switch (m_buffer[1]) {
				case 0x01U: knob = 1;  break;
				case 0xFFU: knob = -1; break;
			}

			if (button && !m_button) {
				switch (m_speed) {
					case 1U:  m_speed = 16U; break;
					case 4U:  m_speed = 1U;  break;
					case 9U:  m_speed = 4U;  break;
					case 16U: m_speed = 9U;  break;
				}
			}

			if (knob != 0)
				m_callback->dialMoved(m_id, knob * m_speed);

			m_button = button;
		}
	}

	delete[] m_buffer;

	::libusb_release_interface(m_handle, 0);

	::libusb_close(m_handle);

	return NULL;
}

void CGriffinPowerMate::close()
{
	m_killed = true;

	Wait();
}

#endif
