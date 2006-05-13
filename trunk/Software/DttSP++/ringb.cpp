/*
  Memory-mapped ringbuffer
  Derived from jack/ringbuffer.h

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
    
    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

    Original
    Copyright (C) 2000 Paul Davis
    Copyright (C) 2003 Rohan Drape

    Derived
    Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY
*/

#include "ringb.h"
#include "fromsys.h"

ringb_float_t* ringb_float_create(size_t sz2)
{
  ringb_float_t *rb = new ringb_float_t;
  rb->buf = new float[sz2];
  rb->size = sz2;		// power-of-2-sized
  rb->mask = rb->size - 1;
  rb->wptr = rb->rptr = 0;
  return rb;
}

void ringb_float_free(ringb_float_t* rb)
{
	if (rb != NULL) {
		delete[] rb->buf;
		delete rb;
	}
}

void ringb_float_reset(ringb_float_t* rb)
{
  // NB not thread-safe
  rb->rptr = 0;
  rb->wptr = 0;
}

void ringb_float_clear(ringb_float_t* rb, size_t nfloats)
{
  size_t i;
  float zero = 0;
  for (i = 0; i < nfloats; i++)
    ringb_float_write (rb, &zero, 1);
}

void ringb_float_restart(ringb_float_t* rb, size_t nfloats)
{
  ringb_float_reset (rb);
  ringb_float_clear (rb, nfloats);
}

size_t ringb_float_read_space(const ringb_float_t* rb)
{
  size_t w = rb->wptr, r = rb->rptr;
  if (w > r)
    return w - r;
  else
    return (w - r + rb->size) & rb->mask;
}

size_t ringb_float_write_space(const ringb_float_t* rb)
{
  size_t w = rb->wptr, r = rb->rptr;
  if (w > r)
    return ((r - w + rb->size) & rb->mask) - 1;
  else if (w < r)
    return r - w - 1;
  else
    return rb->size - 1;
}

size_t ringb_float_read(ringb_float_t* rb, float* dest, size_t cnt)
{
  size_t free_cnt, cnt2, to_read, n1, n2;
  if ((free_cnt = ringb_float_read_space (rb)) == 0)
    return 0;
  to_read = cnt > free_cnt ? free_cnt : cnt;
  if ((cnt2 = rb->rptr + to_read) > rb->size)
    n1 = rb->size - rb->rptr, n2 = cnt2 & rb->mask;
  else
    n1 = to_read, n2 = 0;
  memcpy (dest, &(rb->buf[rb->rptr]), n1 * sizeof (float));
  rb->rptr = (rb->rptr + n1) & rb->mask;
  if (n2)
    {
      memcpy (dest + n1, &(rb->buf[rb->rptr]), n2 * sizeof (float));
      rb->rptr = (rb->rptr + n2) & rb->mask;
    }
  return to_read;
}

size_t ringb_float_write(ringb_float_t* rb, const float* src, size_t cnt)
{
  size_t free_cnt, cnt2, to_write, n1, n2;
  if ((free_cnt = ringb_float_write_space (rb)) == 0)
    return 0;
  to_write = cnt > free_cnt ? free_cnt : cnt;
  if ((cnt2 = rb->wptr + to_write) > rb->size)
    n1 = rb->size - rb->wptr, n2 = cnt2 & rb->mask;
  else
    n1 = to_write, n2 = 0;
  memcpy (&(rb->buf[rb->wptr]), src, n1 * sizeof (float));
  rb->wptr = (rb->wptr + n1) & rb->mask;
  if (n2)
    {
      memcpy (&(rb->buf[rb->wptr]), src + n1, n2 * sizeof (float));
      rb->wptr = (rb->wptr + n2) & rb->mask;
    }
  return to_write;
}
