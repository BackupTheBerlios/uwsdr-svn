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

#include "HPSDRKeyInterface.h"

CHPSDRKeyInterface::CHPSDRKeyInterface(CHPSDRReaderWriter* hpsdr) :
m_hpsdr(hpsdr)
{
	wxASSERT(hpsdr != NULL);
}

CHPSDRKeyInterface::~CHPSDRKeyInterface()
{
}

bool CHPSDRKeyInterface::open()
{
	return true;
}

bool CHPSDRKeyInterface::getState()
{
	return m_hpsdr->getKey();
}

void CHPSDRKeyInterface::close()
{
}

void CHPSDRKeyInterface::clock()
{
}
