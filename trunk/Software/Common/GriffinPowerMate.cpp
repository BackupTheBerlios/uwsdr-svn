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

#if defined(WIN32)

CGriffinPowerMate::CGriffinPowerMate() :
wxThread(),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_len(0),
m_button(false),
m_speed(1U)
{
}

CGriffinPowerMate::~CGriffinPowerMate()
{
}

bool CGriffinPowerMate::open()
{
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
	wxASSERT(m_callback != NULL);

	while (!TestDestroy()) {
		Sleep(1000UL);
	}

	delete[] m_buffer;

	return NULL;
}

void CGriffinPowerMate::close()
{
	Delete();
}

#else

CGriffinPowerMate::CGriffinPowerMate() :
wxThread(),
m_handle(NULL),
m_callback(NULL),
m_id(0),
m_buffer(NULL),
m_len(0),
m_button(false),
m_speed(1U)
{
	::usb_init();
	::usb_find_devices();
	::usb_find_busses();
}

CGriffinPowerMate::~CGriffinPowerMate()
{
}

bool CGriffinPowerMate::open()
{
	struct usb_device* dev = find(POWERMATE_VENDOR_ID, POWERMATE_PRODUCT_ID);
	if (dev == NULL) {
		::wxLogWarning(wxT("Cannot find the USB hardware with vendor: 0x%04X and product: 0x%04X"), POWERMATE_VENDOR_ID, POWERMATE_PRODUCT_ID);
		return false;
	}

	m_handle = ::usb_open(dev);
	if (m_handle == NULL) {
		::wxLogWarning(wxT("Cannot open the USB device with vendor: 0x%04X and product: 0x%04X"), POWERMATE_VENDOR_ID, POWERMATE_PRODUCT_ID);
		return false;
	}

	int rc = ::usb_claim_interface(m_handle, 0);
	if (rc != 0) {
		::wxLogWarning(wxT("Cannot claim the USB interface, rc: %d"), rc);
		::usb_close(m_handle);
		return false;
	}

	m_len = dev->config->interface->altsetting->endpoint->wMaxPacketSize;

	m_buffer = new char[m_len];

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

	while (!TestDestroy()) {
		int n = ::usb_interrupt_read(m_handle, POWERMATE_IN_ENDPOINT, m_buffer, m_len, POWERMATE_TIMEOUT);
		if (n > 0) {
			bool button = m_buffer[0] == 1;

			int knob = 0;
			switch (m_buffer[1]) {
				case 0x01: knob = 1;  break;
				case 0xFF: knob = -1; break;
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

	::usb_release_interface(m_handle, 0);
	::usb_close(m_handle);

	return NULL;
}

void CGriffinPowerMate::close()
{
	Delete();
}

struct usb_device *CGriffinPowerMate::find(unsigned int vendor, unsigned int product) const
{
	for (struct usb_bus* bus = usb_get_busses(); bus != NULL; bus = bus->next)
		for (struct usb_device* dev = bus->devices; dev != NULL; dev = dev->next)
			if (dev->descriptor.idVendor == vendor && dev->descriptor.idProduct == product)
				return dev;

	return NULL;
}

#endif
