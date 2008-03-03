/*
 *   Copyright (C) 2007,2008 by Jonathan Naylor G4KLX
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

#ifndef	SRTXRXController_H
#define	SRTXRXController_H

#include <wx/wx.h>

#include "SDRController.h"
#include "Frequency.h"
#include "ControlInterface.h"
#include "SerialControl.h"


class CSRTXRXController : public ISDRController {

    public:
	CSRTXRXController(const wxString& device, int pin);
	virtual ~CSRTXRXController();

	virtual void setCallback(IControlInterface* callback);

	virtual bool open();
	virtual void enableTX(bool on);
	virtual void enableRX(bool on);
	virtual void setTXAndFreq(bool transmit, const CFrequency& freq);
	virtual void sendCommand(const char* command);
	virtual void setClockTune(unsigned int clock);
	virtual void close();

    private:
	wxString        m_device;
	int             m_pin;
	bool            m_txEnable;
	CSerialControl* m_port;
};

#endif
