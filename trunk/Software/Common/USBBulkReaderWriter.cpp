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

#include "USBBulkReaderWriter.h"

const unsigned int USB_BUFFER_SIZE = 5000U;
const unsigned int USB_TIMEOUT     = 1000U;

CUSBBulkReaderWriter::CUSBBulkReaderWriter() :
wxThread(),
m_inEndPoint(0U),
m_outEndPoint(0U),
m_handle(NULL),
m_buffer(NULL),
m_callback(NULL),
m_id(0)
{
	::usb_init();
	::usb_find_devices();
	::usb_find_busses();

	m_buffer = new char[USB_BUFFER_SIZE];
}

CUSBBulkReaderWriter::~CUSBBulkReaderWriter()
{
	delete[] m_buffer;
}

void CUSBBulkReaderWriter::setCallback(IRawDataCallback* callback, int id)
{
	wxASSERT(callback != NULL);

	m_callback = callback;
	m_id       = id;
}

struct usb_device *CUSBBulkReaderWriter::find(unsigned int vendor, unsigned int product) const
{
	for (struct usb_bus* bus = usb_get_busses(); bus != NULL; bus = bus->next)
		for (struct usb_device* dev = bus->devices; dev != NULL; dev = dev->next)
			if (dev->descriptor.idVendor == vendor && dev->descriptor.idProduct == product)
				return dev;

	return NULL;
}

bool CUSBBulkReaderWriter::open(unsigned int vendor, unsigned int product, unsigned int inEndPoint, unsigned int outEndPoint)
{
	struct usb_device* dev = find(vendor, product);
	if (dev == NULL) {
		::wxLogWarning(wxT("Cannot find the USB hardware with vendor: 0x%04X and product: 0x%04X"), vendor, product);
		return false;
	}

	m_handle = ::usb_open(dev);
	if (m_handle == NULL) {
		::wxLogWarning(wxT("Cannot open the USB device with vendor: 0x%04X and product: 0x%04X"), vendor, product);
		return false;
	}

	int rc = ::usb_set_configuration(m_handle, 1);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot set the USB configuration, rc: %d"), rc);
		::usb_close(m_handle);
		return false;
	} 

	rc = ::usb_claim_interface(m_handle, 0);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot claim the USB interface, rc: %d"), rc);
		::usb_close(m_handle);
		return false;
	}

	rc = ::usb_set_altinterface(m_handle, 0);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot set the alternate USB interface, rc: %d"), rc);
		::usb_close(m_handle);
		return false;
	}

	rc = ::usb_clear_halt(m_handle, inEndPoint);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot clear the USB halt on end point %u, rc: %d"), inEndPoint, rc);
		::usb_close(m_handle);
		return false;
	} 

	rc = ::usb_clear_halt(m_handle, outEndPoint); 
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot clear the USB halt on end point %u, rc: %d"), outEndPoint, rc);
		::usb_close(m_handle);
		return false;
	}

	m_inEndPoint  = inEndPoint;
	m_outEndPoint = outEndPoint;

	Create();
	Run();

	return true;
}

bool CUSBBulkReaderWriter::write(const char* buffer, unsigned int len)
{
	wxASSERT(m_handle != NULL);
	wxASSERT(buffer != NULL);

	unsigned int n = ::usb_bulk_write(m_handle, m_outEndPoint, (char *)buffer, len, USB_TIMEOUT);

	return n == len;
}

void CUSBBulkReaderWriter::close()
{
	Delete();
}

void* CUSBBulkReaderWriter::Entry()
{
	wxASSERT(m_handle != NULL);
	wxASSERT(m_callback != NULL);

	while (!TestDestroy()) {
		unsigned int n = ::usb_bulk_read(m_handle, m_inEndPoint, m_buffer, USB_BUFFER_SIZE, USB_TIMEOUT);

		if (n > 0U)
			m_callback->callback(m_buffer, n, m_id);
	}

	::usb_release_interface(m_handle, 0x0);
	::usb_close(m_handle);

	return NULL;
}
