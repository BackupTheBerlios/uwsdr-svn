/* winmain.cpp

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006-5 by Frank Brickle, AB2KT and Bob McGwier, N4HY

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

#include "local.h"
#include "sdrexport.h"

#include <wx/wx.h>


// elementary defaults
struct _loc loc;

/////////////////////////////////////////////////////////////////////////
// most of what little we know here about the inner loop,
// functionally speaking

extern void reset_meters();
extern void reset_spectrum();
extern void reset_counters();
extern void process_samples(float *, float *, float *, float *, unsigned int);
extern void setup_workspace(REAL rate,
			     unsigned int buflen,
			     SDRMODE mode,
			     unsigned int specsize, unsigned int cpdsize);
extern void destroy_workspace();


//========================================================================

PRIVATE void gethold()
{
  if (ringb_float_write_space (top.jack.ring.o.l) < top.hold.size.frames)
    {
      // pathology
      top.jack.blow.rb.o++;
    }
  else
    {
      ringb_float_write (top.jack.ring.o.l, top.hold.buf.l,
			 top.hold.size.frames);
      ringb_float_write (top.jack.ring.o.r, top.hold.buf.r,
			 top.hold.size.frames);
#ifdef USE_AUXILIARY
      ringb_float_write (top.jack.auxr.o.l, top.hold.aux.l,
			 top.hold.size.frames);
      ringb_float_write (top.jack.auxr.o.r, top.hold.aux.r,
			 top.hold.size.frames);
#else
      ringb_float_write (top.jack.auxr.o.l, top.hold.buf.l,
			 top.hold.size.frames);
      ringb_float_write (top.jack.auxr.o.r, top.hold.buf.r,
			 top.hold.size.frames);
    }
#endif
  if (ringb_float_read_space (top.jack.ring.i.l) < top.hold.size.frames)
    {
      // pathology
      ::memset(top.hold.buf.l, 0, top.hold.size.bytes);
      ::memset(top.hold.buf.r, 0, top.hold.size.bytes);
      ::memset(top.hold.aux.l, 0, top.hold.size.bytes);
      ::memset(top.hold.aux.r, 0, top.hold.size.bytes);
      top.jack.blow.rb.i++;
    }
  else
    {
      ringb_float_read (top.jack.ring.i.l,
			top.hold.buf.l, top.hold.size.frames);
      ringb_float_read (top.jack.ring.i.r,
			top.hold.buf.r, top.hold.size.frames);
#ifdef USE_AUXILIARY
      ringb_float_read (top.jack.auxr.i.l,
			top.hold.aux.l, top.hold.size.frames);
      ringb_float_read (top.jack.auxr.i.r,
			top.hold.aux.r, top.hold.size.frames);
#else
      ringb_float_read (top.jack.auxr.i.l,
			top.hold.buf.l, top.hold.size.frames);
      ringb_float_read (top.jack.auxr.i.r,
			top.hold.buf.r, top.hold.size.frames);
#endif
    }
}

PRIVATE bool canhold()
{

  return (ringb_float_read_space (top.jack.ring.i.l) >=
	  (size_t) top.hold.size.frames);

}


//------------------------------------------------------------------------

PRIVATE void run_mute()
{
	::memset(top.hold.buf.l, 0, top.hold.size.bytes);
	::memset(top.hold.buf.r, 0, top.hold.size.bytes);
	::memset(top.hold.aux.l, 0, top.hold.size.bytes);
	::memset(top.hold.aux.r, 0, top.hold.size.bytes);
	uni.tick++;
}

PRIVATE void run_pass()
{
	uni.tick++;
}

PRIVATE void run_play()
{
	process_samples (top.hold.buf.l, top.hold.buf.r, top.hold.aux.l, top.hold.aux.r, top.hold.size.frames);
}

// NB do not set RUN_SWCH directly via setRunState;
// use setSWCH instead

PRIVATE void run_swch()
{
  if (top.swch.bfct.have == 0)
    {
      // first time
      // apply ramp down
      int i, m = top.swch.fade, n = top.swch.tail;
      for (i = 0; i < m; i++)
	{
	  float w = (float) 1.0 - (float) i / (float) m;
	  top.hold.buf.l[i] *= w, top.hold.buf.r[i] *= w;
	}
      ::memset((top.hold.buf.l + m), 0, n);
      ::memset((top.hold.buf.r + m), 0, n);
      top.swch.bfct.have++;
    }
  else if (top.swch.bfct.have < top.swch.bfct.want)
    {
      // in medias res
      ::memset(top.hold.buf.l, 0, top.hold.size.bytes);
      ::memset(top.hold.buf.r, 0, top.hold.size.bytes);
      top.swch.bfct.have++;
    }
  else
    {
      // last time
      // apply ramp up
      int i, m = top.swch.fade, n = top.swch.tail;
      for (i = 0; i < m; i++)
	{
	  float w = (float) i / m;
	  top.hold.buf.l[i] *= w, top.hold.buf.r[i] *= w;
	}
      uni.mode.trx = top.swch.trx.next;
      top.state = top.swch.run.last;
      top.swch.bfct.want = top.swch.bfct.have = 0;

    }
  process_samples (top.hold.buf.l, top.hold.buf.r,
		   top.hold.aux.l, top.hold.aux.r, top.hold.size.frames);
}

//========================================================================


void Audio_Callback (float *input_l, float *input_r, float *output_l,
		float *output_r, unsigned int nframes)
{
  if (top.susp)
    {
      ::memset(output_l, 0, nframes * sizeof(float));
      ::memset(output_r, 0, nframes * sizeof(float));
      return;
    }

  if ((ringb_float_read_space (top.jack.ring.o.l) >= nframes)
      && (ringb_float_read_space (top.jack.ring.o.r) >= nframes))
    {

      ringb_float_read (top.jack.auxr.o.l, output_l, nframes);
      ringb_float_read (top.jack.auxr.o.r, output_r, nframes);
      ringb_float_read (top.jack.ring.o.l, output_l, nframes);
      ringb_float_read (top.jack.ring.o.r, output_r, nframes);
    }
  else
    {				// rb pathology
      ringb_float_reset (top.jack.ring.i.l);
      ringb_float_reset (top.jack.ring.i.r);
      ringb_float_reset (top.jack.auxr.i.l);
      ringb_float_reset (top.jack.auxr.i.r);
      ringb_float_restart (top.jack.ring.o.r, nframes);
      ringb_float_restart (top.jack.ring.o.l, nframes);
      ringb_float_restart (top.jack.auxr.o.r, nframes);
      ringb_float_restart (top.jack.auxr.o.l, nframes);
      memset (output_l, 0, nframes * sizeof (float));
      memset (output_r, 0, nframes * sizeof (float));
      top.jack.blow.rb.o++;
    }

  // input: copy from port to ring
  if ((ringb_float_write_space (top.jack.ring.i.l) >= nframes)
      && (ringb_float_write_space (top.jack.ring.i.r) >= nframes))
    {
      ringb_float_write (top.jack.ring.i.l, (float *) input_l, nframes);
      ringb_float_write (top.jack.ring.i.r, (float *) input_r, nframes);
      ringb_float_write (top.jack.auxr.i.l, (float *) input_l, nframes);
      ringb_float_write (top.jack.auxr.i.r, (float *) input_r, nframes);
    }
  else
    {				// rb pathology
      ringb_float_reset (top.jack.ring.i.l);
      ringb_float_reset (top.jack.ring.i.r);
      ringb_float_reset (top.jack.auxr.i.l);
      ringb_float_reset (top.jack.auxr.i.r);
      ringb_float_restart (top.jack.ring.o.r, nframes);
      ringb_float_restart (top.jack.ring.o.l, nframes);
      ringb_float_restart (top.jack.auxr.o.r, nframes);
      ringb_float_restart (top.jack.auxr.o.l, nframes);
      top.jack.blow.rb.i++;
    }

  // if enough accumulated in ring, fire dsp
  if (ringb_float_read_space (top.jack.ring.i.l) !=
      ringb_float_read_space (top.jack.ring.i.r))
    {
      ringb_float_reset (top.jack.ring.i.l);
      ringb_float_reset (top.jack.ring.i.r);
      ringb_float_reset (top.jack.auxr.i.l);
      ringb_float_reset (top.jack.auxr.i.r);
      ringb_float_restart (top.jack.ring.o.r, nframes);
      ringb_float_restart (top.jack.ring.o.l, nframes);
      ringb_float_restart (top.jack.auxr.o.r, nframes);
      ringb_float_restart (top.jack.auxr.o.l, nframes);
      top.jack.blow.cb++;
    }

  if (ringb_float_read_space (top.jack.ring.i.l) >= top.hold.size.frames)
    top.sync.buf.sem->Post();

	// check for blowups
	if (top.jack.blow.cb > 0 || top.jack.blow.rb.i > 0 || top.jack.blow.rb.o > 0) {
      ::wxLogError(_("@@@ mon [%lu]: cb = %d rbi = %d rbo = %d xr = %d"),
	       uni.tick,
	       top.jack.blow.cb,
	       top.jack.blow.rb.i, top.jack.blow.rb.o, top.jack.blow.xr);
      ::memset(&top.jack.blow, 0, sizeof(top.jack.blow));
	}
}

void Audio_CallbackIL(float *input, float *output, unsigned int nframes)
{
  if (top.susp)
    {
      memset (output, 0, 2 * nframes * sizeof (float));
      return;
    }

  if ((ringb_float_read_space (top.jack.ring.o.l) >= nframes)
      && (ringb_float_read_space (top.jack.ring.o.r) >= nframes))
    {
      unsigned int i, j;
      /* The following code is broken up in this manner to minimize
         cache hits */

      for (i = 0, j = 0; i < nframes; i++, j += 2)
	ringb_float_read (top.jack.auxr.o.l, &output[j], 1);
      for (i = 0, j = 1; i < nframes; i++, j += 2)
	ringb_float_read (top.jack.auxr.o.r, &output[j], 1);
      for (i = 0, j = 0; i < nframes; i++, j += 2)
	ringb_float_read (top.jack.ring.o.l, &output[j], 1);
      for (i = 0, j = 1; i < nframes; i++, j += 2)
	ringb_float_read (top.jack.ring.o.r, &output[j], 1);
    }
  else
    {				// rb pathology
      ringb_float_reset (top.jack.ring.i.l);
      ringb_float_reset (top.jack.ring.i.r);
      ringb_float_reset (top.jack.auxr.i.l);
      ringb_float_reset (top.jack.auxr.i.r);
      ringb_float_restart (top.jack.ring.o.r, nframes);
      ringb_float_restart (top.jack.ring.o.l, nframes);
      ringb_float_restart (top.jack.auxr.o.r, nframes);
      ringb_float_restart (top.jack.auxr.o.l, nframes);
      memset (output, 0, 2 * nframes * sizeof (float));
      top.jack.blow.rb.o++;
    }

  // input: copy from port to ring
  if ((ringb_float_write_space (top.jack.ring.i.l) >= nframes)
      && (ringb_float_write_space (top.jack.ring.i.r) >= nframes))
    {
      unsigned int i, j;
      /* The following code is broken up in this manner to minimize
         cache hits */
      for (i = 0, j = 0; i < nframes; i++, j += 2)
	ringb_float_write (top.jack.auxr.i.l, &input[j], 1);
      for (i = 0, j = 1; i < nframes; i++, j += 2)
	ringb_float_write (top.jack.auxr.i.r, &input[j], 1);
      for (i = 0, j = 0; i < nframes; i++, j += 2)
	ringb_float_write (top.jack.ring.i.l, &input[j], 1);
      for (i = 0, j = 0; i < nframes; i++, j += 2)
	ringb_float_write (top.jack.ring.i.r, &input[j], 1);
    }
  else
    {				// rb pathology
      ringb_float_reset (top.jack.ring.i.l);
      ringb_float_reset (top.jack.ring.i.r);
      ringb_float_reset (top.jack.auxr.i.l);
      ringb_float_reset (top.jack.auxr.i.r);
      ringb_float_restart (top.jack.ring.o.r, nframes);
      ringb_float_restart (top.jack.ring.o.l, nframes);
      ringb_float_restart (top.jack.auxr.o.r, nframes);
      ringb_float_restart (top.jack.auxr.o.l, nframes);
      top.jack.blow.rb.i++;
    }

  // if enough accumulated in ring, fire dsp
  if (ringb_float_read_space (top.jack.ring.i.l) !=
      ringb_float_read_space (top.jack.ring.i.r))
    {
      ringb_float_reset (top.jack.ring.i.l);
      ringb_float_reset (top.jack.ring.i.r);
      ringb_float_reset (top.jack.auxr.i.l);
      ringb_float_reset (top.jack.auxr.i.r);
      ringb_float_restart (top.jack.ring.o.r, nframes);
      ringb_float_restart (top.jack.ring.o.l, nframes);
      ringb_float_restart (top.jack.auxr.o.r, nframes);
      ringb_float_restart (top.jack.auxr.o.l, nframes);
      top.jack.blow.cb++;
    }

  if (ringb_float_read_space (top.jack.ring.i.l) >= top.hold.size.frames)
    top.sync.buf.sem->Post();

	// check for blowups
	if (top.jack.blow.cb > 0 || top.jack.blow.rb.i > 0 || top.jack.blow.rb.o > 0) {
      ::wxLogError(_("@@@ mon [%lu]: cb = %d rbi = %d rbo = %d xr = %d"),
	       uni.tick,
	       top.jack.blow.cb,
	       top.jack.blow.rb.i, top.jack.blow.rb.o, top.jack.blow.xr);
      ::memset(&top.jack.blow, 0, sizeof(top.jack.blow));
	}
}

//========================================================================


void process_samples_thread()
{
	wxASSERT(top.sync.buf.sem != NULL);
	wxASSERT(top.sync.upd.sem != NULL);

	while (top.running) {
		top.sync.buf.sem->Wait();

		do {
			gethold ();

			top.sync.upd.sem->Wait();

			switch (top.state) {
				case RUN_MUTE:
					run_mute();
					break;

				case RUN_PASS:
					run_pass();
					break;

				case RUN_PLAY:
					run_play();
					break;

				case RUN_SWCH:
					run_swch();
					break;
			}

			top.sync.upd.sem->Post();
		} while (canhold());
	}
}


void closeup()
{
	top.running = false;
	top.susp = true;

	::wxMilliSleep(96);

	ringb_float_free(top.jack.auxr.i.l);
	ringb_float_free(top.jack.auxr.i.r);
	ringb_float_free(top.jack.auxr.o.l);
	ringb_float_free(top.jack.auxr.o.r);

	ringb_float_free(top.jack.ring.o.r);
	ringb_float_free(top.jack.ring.o.l);
	ringb_float_free(top.jack.ring.i.r);
	ringb_float_free(top.jack.ring.i.l);

	delete[] top.hold.buf.l;
	delete[] top.hold.buf.r;
	delete[] top.hold.aux.l;
	delete[] top.hold.aux.r;

	destroy_workspace();

	delete top.sync.upd.sem;
	delete top.sync.buf.sem;

	::wxLogMessage(_("DttSP finished"));
}

//........................................................................

PRIVATE void setup_switching()
{
	top.swch.fade = (unsigned int)(0.2F * REAL(uni.buflen) + 0.5F);
	top.swch.tail = top.hold.size.frames - top.swch.fade;
}

PRIVATE void setup_local_audio()
{
	top.hold.size.frames = uni.buflen;
	top.hold.size.bytes = top.hold.size.frames * sizeof(float);

	top.hold.buf.l = new float[top.hold.size.frames];
	top.hold.buf.r = new float[top.hold.size.frames];
	top.hold.aux.l = new float[top.hold.size.frames];
	top.hold.aux.r = new float[top.hold.size.frames];

	::memset(top.hold.buf.l, 0, top.hold.size.frames * sizeof(float));
	::memset(top.hold.buf.r, 0, top.hold.size.frames * sizeof(float));
	::memset(top.hold.aux.l, 0, top.hold.size.frames * sizeof(float));
	::memset(top.hold.aux.r, 0, top.hold.size.frames * sizeof(float));
}

PRIVATE void setup_system_audio()
{
	top.jack.size = 2048;

	::memset(&top.jack.blow, 0, sizeof (top.jack.blow));

	top.jack.ring.i.l = ringb_float_create(top.jack.size * loc.mult.ring);
	top.jack.ring.i.r = ringb_float_create(top.jack.size * loc.mult.ring);
	top.jack.ring.o.l = ringb_float_create(top.jack.size * loc.mult.ring);
	top.jack.ring.o.r = ringb_float_create(top.jack.size * loc.mult.ring);

	top.jack.auxr.i.l = ringb_float_create(top.jack.size * loc.mult.ring);
	top.jack.auxr.i.r = ringb_float_create(top.jack.size * loc.mult.ring);
	top.jack.auxr.o.l = ringb_float_create(top.jack.size * loc.mult.ring);
	top.jack.auxr.o.r = ringb_float_create(top.jack.size * loc.mult.ring);

	ringb_float_clear(top.jack.ring.o.l, top.jack.size);
	ringb_float_clear(top.jack.ring.o.r, top.jack.size);
}

PRIVATE void setup_threading()
{
	top.susp = false;

	top.sync.upd.sem = new wxSemaphore(0, 1);
	top.sync.buf.sem = new wxSemaphore(0, 0);
}

//========================================================================
// hard defaults, then environment

PRIVATE void setup_defaults(REAL sampleRate, unsigned int audioSize)
{
	loc.def.rate = (sampleRate == 0.0F) ? DEFRATE : sampleRate;
	loc.def.size = (audioSize == 0) ? DEFSIZE : audioSize;
	loc.def.mode = DEFMODE;
	loc.def.spec = DEFSPEC;
	loc.mult.ring = RINGMULT;
	loc.def.comp = DEFCOMP;
}

//========================================================================
void setup(REAL sampleRate, unsigned int audioSize)
{
	top.running = true;
	top.state = RUN_PLAY;
	top.jack.reset_size = 1024;

	setup_defaults(sampleRate, audioSize);

	setup_workspace(loc.def.rate, loc.def.size, loc.def.mode, loc.def.spec, loc.def.comp);

	setup_local_audio();
	setup_system_audio();

	setup_threading();
	setup_switching();

	reset_meters();
	reset_spectrum();
	reset_counters();

	::wxLogMessage(_("DttSP initialised"));
}

