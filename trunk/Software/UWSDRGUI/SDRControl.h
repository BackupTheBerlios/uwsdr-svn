/*
 *   Copyright (C) 2006 by Jonathan Naylor G4KLX
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

#ifndef	SDRControl_H
#define	SDRControl_H

#include "Frequency.h"
#include "ControlInterface.h"

class ISDRControl {

    public:
	virtual void setCallback(IControlInterface* callback, int id) = 0;

	virtual bool open() = 0;

	virtual void enableTX(bool on) = 0;
	virtual void enableRX(bool on) = 0;
	virtual void setTXAndFreq(bool transmit, const CFrequency& freq) = 0;

	virtual void close() = 0;
};

#endif
