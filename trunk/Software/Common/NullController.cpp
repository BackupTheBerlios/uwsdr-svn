/*
 *   Copyright (C) 2006-2007 by Jonathan Naylor G4KLX
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

void CNullController::setCallback(IControlInterface* WXUNUSED(callback), int WXUNUSED(id))
{
}

bool CNullController::open()
{
	return true;
}

void CNullController::enableTX(bool WXUNUSED(on))
{
}

void CNullController::enableRX(bool WXUNUSED(on))
{
}

void CNullController::setTXAndFreq(bool WXUNUSED(transmit), const CFrequency& WXUNUSED(freq))
{
}

void CNullController::sendCommand(const char* WXUNUSED(command))
{
}

void CNullController::setClockTune(unsigned int WXUNUSED(clock))
{
}

void CNullController::close()
{
}
