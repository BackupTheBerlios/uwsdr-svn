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
const wxString VERSION     = wxT("uWave SDR 0.5.1");
const wxString REL_DATE    = wxT("10 June 2006");

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

const int VFO_A            = 0;
const int VFO_B            = 1;
const int VFO_C            = 2;
const int VFO_D            = 3;

const int VFO_SPLIT        = 0;
const int VFO_SHIFT_1      = 1;
const int VFO_SHIFT_2      = 2;
const int VFO_NONE         = 3;

const int SPEED_VERYFAST   = 0;
const int SPEED_FAST       = 1;
const int SPEED_MEDIUM     = 2;
const int SPEED_SLOW       = 3;
const int SPEED_VERYSLOW   = 4;

const int MODE_CWN         = 0;
const int MODE_CWW         = 1;
const int MODE_USB         = 2;
const int MODE_LSB         = 3;
const int MODE_AM          = 4;
const int MODE_FMN         = 5;
const int MODE_FMW         = 6;

const int FILTER_20000     = 0;
const int FILTER_15000     = 1;
const int FILTER_10000     = 2;
const int FILTER_6000      = 3;
const int FILTER_4000      = 4;
const int FILTER_2600      = 5;
const int FILTER_2100      = 6;
const int FILTER_1000      = 7;
const int FILTER_500       = 8;
const int FILTER_250       = 9;
const int FILTER_100       = 10;
const int FILTER_50        = 11;
const int FILTER_25        = 12;
const int FILTER_AUTO      = 13;

const int DEVIATION_6000   = 0;
const int DEVIATION_5000   = 1;
const int DEVIATION_3000   = 2;
const int DEVIATION_2500   = 3;
const int DEVIATION_2000   = 4;

const int AGC_FAST         = 0;
const int AGC_MEDIUM       = 1;
const int AGC_SLOW         = 2;
const int AGC_NONE         = 3;

const int METER_I_INPUT    = 0;
const int METER_Q_INPUT    = 1;
const int METER_SIGNAL     = 2;
const int METER_AVG_SIGNAL = 3;
const int METER_AGC        = 4;
const int METER_MICROPHONE = 5;
const int METER_POWER      = 6;
const int METER_ALC        = 7;

const int SPECTRUM_PRE_FILT   = 0;
const int SPECTRUM_POST_FILT  = 1;
const int SPECTRUM_POST_AGC   = 2;

const int SPECTRUM_PANADAPTER = 0;
const int SPECTRUM_WATERFALL  = 1;

const int SPECTRUM_100MS      = 0;
const int SPECTRUM_200MS      = 1;
const int SPECTRUM_300MS      = 2;
const int SPECTRUM_400MS      = 3;
const int SPECTRUM_500MS      = 4;
const int SPECTRUM_1000MS     = 5;

const double FREQ_VERY_FAST_STEP = 500.0;
const double FREQ_FAST_STEP      = 300.0;
const double FREQ_MEDIUM_STEP    = 100.0;
const double FREQ_SLOW_STEP      = 10.0;
const double FREQ_VERY_SLOW_STEP = 3.0;

const int SPECTRUM_SIZE = 4096;

#endif
