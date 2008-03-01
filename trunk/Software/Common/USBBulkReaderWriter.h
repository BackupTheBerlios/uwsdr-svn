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

#ifndef	USBBulkReaderWriter_H
#define	USBBulkReaderWriter_H

#include <wx/wx.h>

#include "usb.h"

#include "SocketCallback.h"

class CUSBBulkReaderWriter : public wxThread {
    public:
	CUSBBulkReaderWriter();

	virtual void setCallback(ISocketCallback* callback, int id);

	virtual struct usb_device* find(unsigned int vendor, unsigned int product) const;

	virtual bool open(unsigned int vendor, unsigned int product, unsigned int inEndPoint, unsigned int outEndPoint);

	virtual bool write(const char* buffer, unsigned int len);

	virtual void close();

	virtual void* Entry();

    protected:
	virtual ~CUSBBulkReaderWriter();

    private:
	unsigned int           m_inEndPoint;
	unsigned int           m_outEndPoint;
	struct usb_dev_handle* m_handle;
	char*                  m_buffer;
	ISocketCallback*       m_callback;
	int                    m_id;
};

#endif
