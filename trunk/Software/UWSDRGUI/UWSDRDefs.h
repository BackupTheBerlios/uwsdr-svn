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
const wxString VERSION     = wxT("µWave SDR 0.5");
const wxString REL_DATE    = wxT("1 May 2006");

const wxString APPNAME     = wxT("UWSDR");

const int BORDER_SIZE      = 5;

const int FREQUENCY_WIDTH  = 400;
const int FREQUENCY_HEIGHT = 70;

const int SPECTRUM_WIDTH   = 480;
const int SPECTRUM_HEIGHT  = 100;

const int INFO_WIDTH       = 80;
const int INFO_HEIGHT      = 70;

const int SMETER_WIDTH     = 195;
const int SMETER_HEIGHT    = 70;

const int VOLSQL_WIDTH     = 75;
const int VOLSQL_HEIGHT    = 75;

const int FREQDIAL_WIDTH   = 150;
const int FREQDIAL_HEIGHT  = 150;

const int FREQPAD_WIDTH    = 60;
const int FREQPAD_HEIGHT   = 30;

const int CONTROL_WIDTH    = 100;

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
const int MODE_FM          = 5;

const int FILTER_10000     = 0;
const int FILTER_6000      = 1;
const int FILTER_4000      = 2;
const int FILTER_2600      = 3;
const int FILTER_2100      = 4;
const int FILTER_1000      = 5;
const int FILTER_500       = 6;
const int FILTER_250       = 7;
const int FILTER_100       = 8;
const int FILTER_50        = 9;
const int FILTER_25        = 10;
const int FILTER_AUTO      = 11;

const int AGC_FAST         = 0;
const int AGC_MEDIUM       = 1;
const int AGC_SLOW         = 2;
const int AGC_NONE         = 3;

const int METER_SIGNAL     = 0;
const int METER_AVG_SIGNAL = 1;
const int METER_AGC        = 2;
const int METER_MICROPHONE = 3;
const int METER_POWER      = 4;
const int METER_ALC        = 5;

const int SPECTRUM_PRE_FILT   = 0;
const int SPECTRUM_POST_FILT  = 1;

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

const double CENTRE_FREQ  = 11025.0;

const int SPECTRUM_SIZE = 4096;

#endif
