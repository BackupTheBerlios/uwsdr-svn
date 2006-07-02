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
extern void process_samples(float* bufi, float* bufq, unsigned int size);
extern void setup_workspace(float rate,
			     unsigned int buflen,
			     SDRMODE mode,
			     unsigned int specsize, unsigned int cpdsize);
extern void destroy_workspace();


//========================================================================

static void gethold()
{
	if (top.jack.ring.o.i->freeSpace() < top.hold.size.frames) {
		// pathology
		::wxLogError(wxT("Not enough space in output ring buffer"));
	} else {
		top.jack.ring.o.i->addData(top.hold.buf.i, top.hold.size.frames);
		top.jack.ring.o.q->addData(top.hold.buf.q, top.hold.size.frames);
	}

	if (top.jack.ring.i.i->dataSpace() < top.hold.size.frames) {
		// pathology
		::memset(top.hold.buf.i, 0x00, top.hold.size.bytes);
		::memset(top.hold.buf.q, 0x00, top.hold.size.bytes);

		::wxLogError(wxT("Not enough data in input ring buffers"));
	} else {
		top.jack.ring.i.i->getData(top.hold.buf.i, top.hold.size.frames);
		top.jack.ring.i.q->getData(top.hold.buf.q, top.hold.size.frames);
    }
}

static bool canhold()
{
	wxASSERT(top.jack.ring.i.i != NULL);

	return top.jack.ring.i.i->dataSpace() >= top.hold.size.frames;
}


//------------------------------------------------------------------------

static void run_mute()
{
	::memset(top.hold.buf.i, 0x00, top.hold.size.bytes);
	::memset(top.hold.buf.q, 0x00, top.hold.size.bytes);

	uni.tick++;
}

static void run_pass()
{
	uni.tick++;
}

static void run_play()
{
	process_samples(top.hold.buf.i, top.hold.buf.q, top.hold.size.frames);
}

// NB do not set RUN_SWCH directly via setRunState;
// use setSWCH instead
static void run_swch()
{
	if (top.swch.bfct.have == 0) {
		// first time
		// apply ramp down
		unsigned int m = top.swch.fade;
		unsigned int n = top.swch.tail;
		for (unsigned int i = 0; i < m; i++) {
			float w = 1.0F - float(i) / float(m);
			top.hold.buf.i[i] *= w;
			top.hold.buf.q[i] *= w;
		}

		::memset(top.hold.buf.i + m, 0x00, n);
		::memset(top.hold.buf.q + m, 0x00, n);

		top.swch.bfct.have++;
	} else if (top.swch.bfct.have < top.swch.bfct.want) {
		// in medias res
		::memset(top.hold.buf.i, 0x00, top.hold.size.bytes);
		::memset(top.hold.buf.q, 0x00, top.hold.size.bytes);

		top.swch.bfct.have++;
	} else {
		// last time
		// apply ramp up
		unsigned int m = top.swch.fade;
		for (unsigned int i = 0; i < m; i++) {
			float w = float(i) / float(m);
			top.hold.buf.i[i] *= w;
			top.hold.buf.q[i] *= w;
		}

		uni.mode.trx = top.swch.trx.next;
		top.state = top.swch.run.last;
		top.swch.bfct.want = 0;
		top.swch.bfct.have = 0;
	}

	process_samples(top.hold.buf.i, top.hold.buf.q, top.hold.size.frames);
}

//========================================================================


void Audio_Callback(float* input_i, float* input_q, float* output_i, float* output_q, unsigned int nframes)
{
	wxASSERT(input_i != NULL);
	wxASSERT(input_q != NULL);
	wxASSERT(output_i != NULL);
	wxASSERT(output_q != NULL);

	if (top.susp) {
		::memset(output_i, 0x00, nframes * sizeof(float));
		::memset(output_q, 0x00, nframes * sizeof(float));
		return;
	}

	if (top.jack.ring.o.i->dataSpace() >= nframes && top.jack.ring.o.q->dataSpace() >= nframes) {
		top.jack.ring.o.i->getData(output_i, nframes);
		top.jack.ring.o.q->getData(output_q, nframes);
	} else {
		top.jack.ring.i.i->clear();
		top.jack.ring.i.q->clear();
		top.jack.ring.o.i->clear();
		top.jack.ring.o.q->clear();

		::memset(output_i, 0x00, nframes * sizeof(float));
		::memset(output_q, 0x00, nframes * sizeof(float));

		::wxLogError(wxT("Not enough space in the output ring buffers"));
	}

	// input: copy from port to ring
	if (top.jack.ring.i.i->freeSpace() >= nframes && top.jack.ring.i.q->freeSpace() >= nframes) {
		top.jack.ring.i.i->addData(input_i, nframes);
		top.jack.ring.i.q->addData(input_q, nframes);
	} else {
		top.jack.ring.i.i->clear();
		top.jack.ring.i.q->clear();
		top.jack.ring.o.i->clear();
		top.jack.ring.o.q->clear();

		::wxLogError(wxT("Not enough data in the input ring buffers"));
	}

	// if enough accumulated in ring, fire dsp
	if (top.jack.ring.i.i->dataSpace() >= top.hold.size.frames && top.jack.ring.i.q->dataSpace() >= top.hold.size.frames)
		top.sync.buf.sem->Post();
}

void Audio_CallbackIL(float* input, float* output, unsigned int nframes)
{
	wxASSERT(input != NULL);
	wxASSERT(output != NULL);

	if (top.susp) {
		::memset(output, 0x00, 2 * nframes * sizeof (float));
		return;
	}

	if (top.jack.ring.o.i->dataSpace() >= nframes && top.jack.ring.o.q->dataSpace() >= nframes) {
		unsigned int i, j;
		/* The following code is broken up in this manner to minimize
		   cache hits */
		for (i = 0, j = 0; i < nframes; i++, j += 2)
			top.jack.ring.o.i->getData(&output[j], 1);
		for (i = 0, j = 1; i < nframes; i++, j += 2)
			top.jack.ring.o.q->getData(&output[j], 1);
	} else {
		top.jack.ring.i.i->clear();
		top.jack.ring.i.q->clear();
		top.jack.ring.o.i->clear();
		top.jack.ring.o.q->clear();

		::memset(output, 0x00, 2 * nframes * sizeof(float));

		::wxLogError(wxT("Not enough data in the output ring buffers"));
	}

	// input: copy from port to ring
	if (top.jack.ring.i.i->freeSpace() >= nframes && top.jack.ring.i.q->freeSpace() >= nframes) {
		unsigned int i, j;
		/* The following code is broken up in this manner to minimize
		   cache hits */
		for (i = 0, j = 0; i < nframes; i++, j += 2)
			top.jack.ring.i.i->addData(&input[j], 1);
		for (i = 0, j = 0; i < nframes; i++, j += 2)
			top.jack.ring.i.q->addData(&input[j], 1);
	} else {
		top.jack.ring.i.i->clear();
		top.jack.ring.i.q->clear();
		top.jack.ring.o.i->clear();
		top.jack.ring.o.q->clear();

		::wxLogError(wxT("Not enough space in the input ring buffers"));
	}

	// if enough accumulated in ring, fire dsp
	if (top.jack.ring.i.i->dataSpace() >= top.hold.size.frames && top.jack.ring.i.q->dataSpace() >= top.hold.size.frames)
		top.sync.buf.sem->Post();
}

//========================================================================


void process_samples_thread()
{
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

	delete top.jack.ring.o.i;
	delete top.jack.ring.o.q;
	delete top.jack.ring.i.i;
	delete top.jack.ring.i.q;

	delete[] top.hold.buf.i;
	delete[] top.hold.buf.q;

	destroy_workspace();

	delete top.sync.upd.sem;
	delete top.sync.buf.sem;

	::wxLogMessage(wxT("DttSP finished"));
}

//========================================================================
void setup(float sampleRate, unsigned int audioSize)
{
	top.running = true;
	top.state = RUN_PLAY;

	loc.def.rate  = sampleRate;
	loc.def.size  = audioSize;
	loc.def.mode  = DEFMODE;
	loc.def.spec  = DEFSPEC;
	loc.mult.ring = RINGMULT;
	loc.def.comp  = DEFCOMP;

	setup_workspace(loc.def.rate, loc.def.size, loc.def.mode, loc.def.spec, loc.def.comp);

	top.hold.size.frames = uni.buflen;
	top.hold.size.bytes  = top.hold.size.frames * sizeof(float);
	top.hold.buf.i = new float[top.hold.size.frames];
	top.hold.buf.q = new float[top.hold.size.frames];
	::memset(top.hold.buf.i, 0x00, top.hold.size.frames * sizeof(float));
	::memset(top.hold.buf.q, 0x00, top.hold.size.frames * sizeof(float));

	top.jack.size = 2048;
	top.jack.ring.i.i = new CRingBuffer(top.jack.size * loc.mult.ring, 1);
	top.jack.ring.i.q = new CRingBuffer(top.jack.size * loc.mult.ring, 1);
	top.jack.ring.o.i = new CRingBuffer(top.jack.size * loc.mult.ring, 1);
	top.jack.ring.o.q = new CRingBuffer(top.jack.size * loc.mult.ring, 1);

	top.susp = false;
	top.sync.upd.sem = new wxSemaphore();
	top.sync.buf.sem = new wxSemaphore();

	top.swch.fade = (unsigned int)(0.2F * float(uni.buflen) + 0.5F);
	top.swch.tail = top.hold.size.frames - top.swch.fade;

	reset_meters();
	reset_spectrum();
	reset_counters();

	::wxLogMessage(wxT("DttSP initialised"));
}

