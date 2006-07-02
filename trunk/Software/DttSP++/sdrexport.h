/* sdrexport.h

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

The authors can be reached by email at

ab2kt@arrl.net
or
rwmcgwier@comcast.net

or by paper mail at

The DTTS Microwave Society
6 Kathleen Place
Bridgewater, NJ 08807
*/

#ifndef _sdrexport_h
#define _sdrexport_h

#include "Defs.h"
#include "Meter.h"
#include "Spectrum.h"
#include "CXB.h"
#include "CorrectIQ.h"
#include "Oscillator.h"
#include "FIR.h"
#include "FilterOVSV.h"
#include "NoiseBlanker.h"
#include "BlockLMS.h"
#include "LMS.h"
#include "SpeechProc.h"
#include "AMDemod.h"
#include "AMMod.h"
#include "FMDemod.h"
#include "FMMod.h"
#include "SSBDemod.h"
#include "SSBMod.h"
#include "SpotTone.h"
#include "Compand.h"
#include "DCBlock.h"
#include "RingBuffer.h"
#include "GraphicEQ.h"
#include "AGC.h"
#include "Squelch.h"
#include "TX.h"

#include <wx/wx.h>

//------------------------------------------------------------------------
/* modulation types, modes */

//========================================================================
/* RX/TX both */
//------------------------------------------------------------------------
extern struct _uni
{
  float samplerate;
  unsigned int buflen;

  struct {
    SDRMODE sdr;
    TRXMODE trx;
  } mode;

  struct {
    CMeter* gen;
  } meter;

  struct {
	CSpectrum* gen;
	SPECTRUMtype type;
  } spec;

  struct {
    unsigned int bits;
  } wisdom;

  unsigned int cpdlen;
  unsigned long tick;
} uni;

//------------------------------------------------------------------------
/* RX */
//------------------------------------------------------------------------

extern struct _rx
{
  struct {
    CXB* i;
	CXB* o;
  } buf;

  CCorrectIQ* iqfix;

  struct {
    COscillator* gen;
  } osc;

  CFilterOVSV* filt;

  struct {
	COscillator* gen;
  } rit;

  struct {
    CNoiseBlanker* gen;
    bool flag;
  } nb;

  struct {
    CNoiseBlanker* gen;
    bool flag;
  } nb_sdrom;

  struct {
    CLMS* gen;
    bool flag;
  } anr, anf;

  struct {
    CBlockLMS* gen;
    bool flag;
  } banr, banf;

  struct {
    CAGC* gen;
    bool flag;
  } agc;

  CAMDemod*  am;
  CFMDemod*  fm;
  CSSBDemod* ssb;

  struct {
    bool flag;
    CSpotTone* gen;
  } spot;

  struct {
	CSquelch* gen;
  } squelch;

  struct {
    bool flag;
    CCompand* gen;
  } cpd;

  struct {
    CGraphicEQ* gen;
    bool flag;
  } grapheq;

  SDRMODE mode;

  struct {
    bool flag;
  } bin;

  float norm;
  COMPLEX azim;
  unsigned long tick;
} rx;


extern CTX* tx;
//------------------------------------------------------------------------

typedef enum _runmode {
   RUN_MUTE,
   RUN_PASS,
   RUN_PLAY,
   RUN_SWCH
} RUNMODE;

extern struct _top {
  bool running;
  RUNMODE state;

  // audio io
  struct {
    struct {
      float *i, *q;
    } buf;

    struct {
      unsigned int frames, bytes;
    } size;
  } hold;

  struct {
    struct {
      struct {
	     CRingBuffer *i, *q;
      } i, o;
    } ring;

    size_t size;
  } jack;

  // update io
  // multiprocessing & synchronization
  struct {
    struct {
      wxSemaphore* sem;
    } buf, upd;
  } sync;

  // TRX switching
  struct {
    struct {
      unsigned int want, have;
    } bfct;

    struct {
      TRXMODE next;
    } trx;

    struct {
      RUNMODE last;
    } run;

    unsigned int fade, tail;
  } swch;

  bool susp;
} top;

#endif
