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

#ifndef	GriffinPowerMate_H
#define	GriffinPowerMate_H

#include <wx/wx.h>

#include "usb.h"

#include "DialInterface.h"


class CGriffinPowerMate : public IDialInterface, public wxThread  {
    public:
	CGriffinPowerMate();

	virtual void setCallback(IDialCallback* callback, int id);

	virtual bool open();
	virtual void close();

	virtual void* Entry();

    protected:
	virtual ~CGriffinPowerMate();

    private:
	struct usb_dev_handle* m_handle;
	IDialCallback*         m_callback;
	int                    m_id;
	char*                  m_buffer;
	unsigned int           m_len;
	bool                   m_button;
	unsigned int           m_speed;

	struct usb_device* find(unsigned int vendor, unsigned int product) const;
};

#endif
