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

#ifndef _ringb_h
#define _ringb_h

#include <sys/types.h>

typedef struct {
  float *buf;
  size_t wptr, rptr, size, mask;
} ringb_float_t;

/* Sets up a ringbuffer data structure of a specified size
 * and allocates the required memory.  
 * sz is requested ringbuffer size in bytes(floats),
 * MUST be a power of 2.
 * pre-allocated memory must be large enough to
 * accommodate (ringb header + stipulated memory size).
 * return a pointer to a new ringb_t, if successful,
 * 0 otherwise.
 * Freeing is done by calling ringb_free. */

extern ringb_float_t* ringb_float_create(size_t sz2);

/* free the ring buffers that are created without external
*  storage supplied */

extern void ringb_float_free(ringb_float_t* rb);

/*
 * Read data from the ringbuffer.
 * rb a pointer to the ringbuffer structure.
 * dest a pointer to a buffer where data read from the
 * ringbuffer will go.
 * cnt the number of bytes to read.
 *
 * return the number of bytes read, which may range from 0 to cnt. */

extern size_t ringb_float_read(ringb_float_t* rb, float* dest, size_t cnt);

/* Return the number of bytes available for reading.
 * rb a pointer to the ringbuffer structure.
 * return the number of bytes available to read. */

extern size_t ringb_float_read_space(const ringb_float_t* rb);

/* Reset the read and write pointers, making an empty buffer.
 * This is not thread safe. */

extern void ringb_float_reset(ringb_float_t* rb);

/* Write data into the ringbuffer.
 * rb a pointer to the ringbuffer structure.
 * src a pointer to the data to be written to the ringbuffer.
 * cnt the number of bytes(floats) to write.
 * return the number of bytes(floats) written, which may range from 0 to cnt */

extern size_t ringb_float_write(ringb_float_t* rb, const float* src, size_t cnt);

/* Return the number of bytes(floats) available for writing.
 * rb a pointer to the ringbuffer structure.
 * return the amount of free space (in bytes) available for writing. */

extern size_t ringb_float_write_space(const ringb_float_t* rb);

/* Fill the ring buffer for nbytes at the beginning with zeros 
 * rb a pointer to the ring buffer structure
 * nbytes the number of bytes to be written */

extern void ringb_float_clear(ringb_float_t* rb, size_t nfloats);

/* Reset the read and write pointers, making an empty buffer.
 * This is not thread safe. 
 * Fill the ring buffer for nbytes at the beginning with zeros 
 * rb a pointer to the ring buffer structure
 * nbytes the number of bytes to be written */

extern void ringb_float_restart(ringb_float_t* rb, size_t nfloats);

#endif
