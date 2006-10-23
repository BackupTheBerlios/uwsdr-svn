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
#include "NullController.h"


CNullController::CNullController()
{
}

CNullController::~CNullController()
{
}

void CNullController::setCallback(IControlInterface* callback, int id)
{
}

bool CNullController::open()
{
	return true;
}

void CNullController::enableTX(bool on)
{
}

void CNullController::enableRX(bool on)
{
}

void CNullController::setTXAndFreq(bool transmit, const CFrequency& freq)
{
}

void CNullController::sendCommand(const char* command)
{
}

void CNullController::close()
{
}