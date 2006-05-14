/* bufvec.c
   creation, deletion, management for vectors and buffers 
   
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

#include <bufvec.h>

#include <wx/wx.h>

/*------------------------------------------------------------------------*/
/* wrapper around calloc */
#include <fftw3.h>

void *
safealloc (int count, int nbytes, char *tag)
{
  void *p = fftw_malloc (count * nbytes);
  if (!p)
    {
      if (tag && *tag)
	fprintf (stderr, "safealloc: %s\n", tag);
      else
	perror ("safealloc");
      exit (1);
    }
  memset(p,0,(size_t)(count*nbytes));
  return p;
}

void
safefree (void *p)
{
  if (p)
    fftw_free (p);
}

/*------------------------------------------------------------------------*/
/* allocate/free just vectors */

REAL *
newvec_REAL (int size, char *tag)
{
  return (REAL *) safealloc (size, sizeof (REAL), tag);
}

void
delvec_REAL (REAL * vec)
{
  if (vec)
    safefree (vec);
}

IMAG *
newvec_IMAG (int size, char *tag)
{
  return (IMAG *) safealloc (size, sizeof (IMAG), tag);
}

void
delvec_IMAG (IMAG * vec)
{
  if (vec)
    safefree (vec);
}

COMPLEX *
newvec_COMPLEX (int size, char *tag)
{
  return (COMPLEX *) safealloc (size, sizeof (COMPLEX), tag);
}

void
delvec_COMPLEX (COMPLEX * vec)
{
  if (vec)
    safefree (vec);
}

/*------------------------------------------------------------------------*/
/* buffers (mainly i/o) */
/*------------------------------------------------------------------------*/
/* complex */

CXB
newCXB (int size, COMPLEX * base, char *tag)
{
  CXB p = (CXB) safealloc (1, sizeof (CXBuffer), tag);
  if (base)
    {
      CXBbase (p) = base;
      CXBmine (p) = false;
    }
  else
    {
      CXBbase (p) = newvec_COMPLEX (size, "newCXB");
      CXBmine (p) = true;
    }
  CXBsize (p) = CXBwant (p) = size;
  CXBovlp (p) = CXBhave (p) = CXBdone (p) = 0;
  return p;
}

void
delCXB (CXB p)
{
  if (p)
    {
      if (CXBmine (p))
	delvec_COMPLEX (CXBbase (p));
      safefree (p);
    }
}

/*------------------------------------------------------------------------*/
/* real */

RLB
newRLB (int size, REAL * base, char *tag)
{
  RLB p = (RLB) safealloc (1, sizeof (RLBuffer), tag);
  if (base)
    {
      RLBbase (p) = base;
      RLBmine (p) = false;
    }
  else
    {
      RLBbase (p) = newvec_REAL (size, "newRLB");
      RLBmine (p) = true;
    }
  RLBsize (p) = RLBwant (p) = size;
  RLBovlp (p) = RLBhave (p) = RLBdone (p) = 0;
  return p;
}

void
delRLB (RLB p)
{
  if (p)
    {
      if (p->mine)
	delvec_REAL (RLBbase (p));
      safefree (p);
    }
}

//========================================================================
// return normalization constant

REAL
normalize_vec_REAL (REAL * v, int n)
{
  if (v && (n > 0))
    {
      int i;
      REAL big = -(REAL) MONDO;
      for (i = 0; i < n; i++)
	{
	  REAL a = abs (v[i]);
	  big = max (big, a);
	}
      if (big > 0.0)
	{
	  REAL scl = (REAL) (1.0 / big);
	  for (i = 0; i < n; i++)
	    v[i] *= scl;
	  return scl;
	}
      else
	return 0.0;
    }
  else
    return 0.0;
}

REAL
normalize_vec_COMPLEX (COMPLEX * z, int n)
{
  if (z && (n > 0))
    {
      int i;
      REAL big = -(REAL) MONDO;
      for (i = 0; i < n; i++)
	{
	  REAL a = Cabs (z[i]);
	  big = max (big, a);
	}
      if (big > 0.0)
	{
	  REAL scl = (REAL) (1.0 / big);
	  for (i = 0; i < n; i++)
	    z[i] = Cscl (z[i], scl);
	  return scl;
	}
      else
	return 0.0;
    }
  else
    return 0.0;
}

void dumpREAL(REAL* buf, int size, char* text)
{
	wxLogDebug(text);

	for (int i = 0; i < size; i += 8)
		wxLogDebug("%04X: %g %g %g %g %g %g %g %g", i,
			buf[i + 0], buf[i + 1], buf[i + 2], buf[i + 3],
			buf[i + 4], buf[i + 5], buf[i + 6], buf[i + 7]);
}

void dumpCOMPLEX(COMPLEX* buf, int size, char* text)
{
	wxLogDebug(text);

	for (int i = 0; i < size; i += 8)
		wxLogDebug("%04X: %g:%g %g:%g %g:%g %g:%g %g:%g %g:%g %g:%g %g:%g", i,
			buf[i + 0].re, buf[i + 0].im, buf[i + 1].re, buf[i + 1].im,
			buf[i + 2].re, buf[i + 2].im, buf[i + 3].re, buf[i + 3].im,
			buf[i + 4].re, buf[i + 4].im, buf[i + 5].re, buf[i + 5].im,
			buf[i + 6].re, buf[i + 6].im, buf[i + 7].re, buf[i + 7].im);
}
