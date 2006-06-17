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

#include <fromsys.h>
#include <defs.h>
#include <banal.h>
#include <splitfields.h>
#include <datatypes.h>
#include <bufvec.h>
#include <cxops.h>
#include <ringb.h>
#include <lmadf.h>
#include <fftw3.h>
#include <ovsv.h>
#include <filter.h>
#include <oscillator.h>
#include <dttspagc.h>
#include <am_demod.h>
#include <fm_demod.h>
#include <noiseblanker.h>
#include <correctIQ.h>
#include <speechproc.h>
#include <spottone.h>
#include <update.h>
#include <local.h>
#include <meter.h>
#include <spectrum.h>
//------------------------------------------------------------------------
// max no. simultaneous receivers
#ifndef MAXRX
#define MAXRX (4)
#endif
//------------------------------------------------------------------------
/* modulation types, modes */

//========================================================================
/* RX/TX both */
//------------------------------------------------------------------------
extern struct _uni
{
  REAL samplerate;
  int buflen;

  struct
  {
    SDRMODE sdr;
    TRXMODE trx;
  } mode;

  METERBlock meter;
  SpecBlock spec;

  struct
  {
    splitfld splt;
  } update;

  struct
  {
    int bits;
  } wisdom;

  struct
  {
    bool act[MAXRX];
    int lis, nac, nrx;
  } multirx;

  struct
  {
    struct
    {
      bool flag;
      REAL gain;
    } rx, tx;
  } mix;
  int cpdlen;
  long tick;

} uni;

//------------------------------------------------------------------------
/* RX */
//------------------------------------------------------------------------

extern struct _rx
{
  struct
  {
    CXB i, o;
  } buf;
  IQ iqfix;
  struct
  {
    double freq, phase;
    OSC gen;
  } osc;
  struct
  {
    double freq, phase;
    OSC gen;
  } rit;
  struct
  {
    ComplexFIR coef;
    FiltOvSv ovsv;
    COMPLEX *save;
  } filt;
  struct
  {
    REAL thresh;
    NB gen;
    bool flag;
  } nb;
  struct
  {
    REAL thresh;
    NB gen;
    bool flag;
  } nb_sdrom;
  struct
  {
    LMSR gen;
    bool flag;
  } anr, anf;
  struct
  {
    BLMS gen;
    bool flag;
  } banr, banf;
  struct
  {
    DTTSPAGC gen;
    bool flag;
  } dttspagc;
  struct
  {
    AMD gen;
  } am;
  struct
  {
    FMD gen;
  } fm;
  struct
  {
    bool flag;
    SpotToneGen gen;
  } spot;
  struct
  {
    REAL thresh, power;
    bool flag, running, set;
    int num;
  } squelch;

  struct
  {
    bool flag;
    WSCompander gen;
  } cpd;

  struct
  {
    EQ gen;
    bool flag;
  } grapheq;

  SDRMODE mode;
  struct
  {
    bool flag;
  } bin;
  REAL norm;
  COMPLEX azim;
  long tick;
} rx[MAXRX];

//------------------------------------------------------------------------
/* TX */
//------------------------------------------------------------------------
extern struct _tx
{
  struct
  {
    CXB i, o;
  } buf;
  IQ iqfix;

  struct
  {
    bool flag;
    DCBlocker gen;
  } dcb;

  struct
  {
    double freq, phase;
    OSC gen;
  } osc;
  struct
  {
    ComplexFIR coef;
    FiltOvSv ovsv;
    COMPLEX *save;
  } filt;

  struct
  {
    REAL carrier_level;
  } am;

  struct
  {
    REAL cvtmod2freq;
  } fm;

  struct
  {
    REAL thresh, power;
    bool flag, running, set;
    int num;
  } squelch;

  struct
  {
    DTTSPAGC gen;
    bool flag;
  } leveler, alc;

  struct
  {
    EQ gen;
    bool flag;
  } grapheq;


  struct
  {
    SpeechProc gen;
    bool flag;
  } spr;


  struct
  {
    bool flag;
    WSCompander gen;
  } cpd;

  SDRMODE mode;

  long tick;
  REAL norm;
} tx;

//------------------------------------------------------------------------

typedef enum _runmode
{
  RUN_MUTE, RUN_PASS, RUN_PLAY, RUN_SWCH
} RUNMODE;

extern struct _top
{
  bool running, verbose;
  RUNMODE state;

  // audio io
  struct
  {
    struct
    {
      float *l, *r;
    } aux, buf;
    struct
    {
      unsigned int frames, bytes;
    } size;
  } hold;

  struct
  {
    struct
    {
      struct
      {
	ringb_float_t *l, *r;
      } i, o;
    } ring;

    struct
    {
      struct
      {
	ringb_float_t *l, *r;
      } i, o;
    } auxr;

    size_t reset_size;

    size_t size;

    struct
    {
      int cb;
      struct
      {
	int i, o;
      } rb;
      int xr;
    } blow;
  } jack;

  // update io
  // multiprocessing & synchronization
  struct
  {
    struct
    {
      wxSemaphore* sem;
    } buf, upd;
  } sync;

  // TRX switching
  struct
  {
    struct
    {
      int want, have;
    } bfct;
    struct
    {
      TRXMODE next;
    } trx;
    struct
    {
      RUNMODE last;
    } run;
    int fade, tail;
  } swch;

  bool susp;

} top;

#endif
