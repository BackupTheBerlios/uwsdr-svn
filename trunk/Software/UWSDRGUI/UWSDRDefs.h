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

#ifndef	UWSDRDefs_H
#define	UWSDRDefs_H

// For the GUI
const wxString APPNAME     = wxT("UWSDR");

const int BORDER_SIZE      = 5;

#if defined(__WXMSW__)
const int BUTTON_HEIGHT    = -1;

const int FREQDIAL_WIDTH   = 150;
const int FREQDIAL_HEIGHT  = 150;

const int VOLSQL_WIDTH     = 75;
const int VOLSQL_HEIGHT    = 75;

const int SPECTRUM_WIDTH   = 480;
const int SPECTRUM_HEIGHT  = 100;

const int FREQPAD_WIDTH    = 60;
const int FREQPAD_HEIGHT   = 30;

const int INFO_WIDTH       = 80;
const int INFO_HEIGHT      = 70;

const int FREQUENCY_WIDTH  = 400;
const int FREQUENCY_HEIGHT = 70;

const int CONTROL_WIDTH    = 100;

const int SMETER_WIDTH     = 195;
const int SMETER_HEIGHT    = 70;

#elif defined(__WXGTK__)
const int BUTTON_HEIGHT    = 22;

const int FREQDIAL_WIDTH   = 165;
const int FREQDIAL_HEIGHT  = 165;

const int VOLSQL_WIDTH     = 85;
const int VOLSQL_HEIGHT    = 85;

const int SPECTRUM_WIDTH   = 500;
const int SPECTRUM_HEIGHT  = 105;

const int FREQPAD_WIDTH    = 70;
const int FREQPAD_HEIGHT   = 25;

const int INFO_WIDTH       = 90;
const int INFO_HEIGHT      = 70;

const int FREQUENCY_WIDTH  = 410;
const int FREQUENCY_HEIGHT = 70;

const int CONTROL_WIDTH    = 105;

const int SMETER_WIDTH     = 210;
const int SMETER_HEIGHT    = 70;

#else
#error "Unknown platform"
#endif

enum {
	VFO_A,
	VFO_B,
	VFO_C,
	VFO_D,
	VFO_COUNT
};

enum {
	VFO_SPLIT,
	VFO_SHIFT_1,
	VFO_SHIFT_2,
	VFO_NONE
};

enum {
	SPEED_VERYFAST,
	SPEED_FAST,
	SPEED_MEDIUM,
	SPEED_SLOW,
	SPEED_VERYSLOW
};

enum {
	MODE_CWN,
	MODE_CWW,
	MODE_USB,
	MODE_LSB,
	MODE_AM,
	MODE_FMN,
	MODE_FMW
};

enum {
	FILTER_20000,
	FILTER_15000,
	FILTER_10000,
	FILTER_6000,
	FILTER_4000,
	FILTER_2600,
	FILTER_2100,
	FILTER_1000,
	FILTER_500,
	FILTER_250,
	FILTER_100,
	FILTER_50,
	FILTER_25,
	FILTER_AUTO
};

enum {
	DEVIATION_6000,
	DEVIATION_5000,
	DEVIATION_3000,
	DEVIATION_2500,
	DEVIATION_2000
};

enum {
	AGC_FAST,
	AGC_MEDIUM,
	AGC_SLOW,
	AGC_NONE
};

enum {
	METER_I_INPUT,
	METER_Q_INPUT,
	METER_SIGNAL,
	METER_AVG_SIGNAL,
	METER_AGC,
	METER_MICROPHONE,
	METER_POWER,
	METER_ALC
};

enum {
	SPECTRUM_PRE_FILT,
	SPECTRUM_POST_FILT,
	SPECTRUM_POST_AGC
};

enum {
	SPECTRUM_PANADAPTER1,
	SPECTRUM_PANADAPTER2,
	SPECTRUM_WATERFALL
};

enum {
	SPECTRUM_100MS,
	SPECTRUM_200MS,
	SPECTRUM_300MS,
	SPECTRUM_400MS,
	SPECTRUM_500MS,
	SPECTRUM_1000MS
};

const int CW_OFFSET           = 800;

const double FREQ_VERY_FAST_STEP = 500.0;
const double FREQ_FAST_STEP      = 300.0;
const double FREQ_MEDIUM_STEP    = 100.0;
const double FREQ_SLOW_STEP      = 10.0;
const double FREQ_VERY_SLOW_STEP = 3.0;

const int SPECTRUM_SIZE = 4096;

#endif
