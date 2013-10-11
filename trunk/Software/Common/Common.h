/*
 *   Copyright (C) 2006-2008,2013 by Jonathan Naylor G4KLX
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

#ifndef	Version_H
#define	Version_H

#include <wx/wx.h>
#include <wx/datetime.h>

const wxString VERSION  = wxT("uWave SDR 0.9.1");

const wxDateTime::wxDateTime_t REL_DATE_DAY   = 11;
const wxDateTime::Month        REL_DATE_MONTH = wxDateTime::Oct;
const unsigned int             REL_DATE_YEAR  = 2013;

enum INPIN {
	IN_NONE = -1,
	IN_RTS_CTS,
	IN_RTS_DSR,
	IN_DTR_DSR,
	IN_DTR_CTS
};

enum OUTPIN {
	OUT_NONE = -1,
	OUT_RTS,
	OUT_DTR
};

enum TUNINGHW {
	TUNINGHW_NONE = 0,
	TUNINGHW_POWERMATE
};

const int JACK_API = -1;
const int JACK_DEV = -1;

#endif
