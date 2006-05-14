/* filter.cpp

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

#include "filter.h"
#include "fromsys.h"
#include "bufvec.h"
#include "Window.h"

#include <wx/wx.h>		// FIXME


RealFIR* newFIR_REAL(unsigned int size)
{
	RealFIR* p = new RealFIR;
	FIRcoef(p) = new REAL[size];
	::memset(FIRcoef(p), 0, size * sizeof(REAL));

	FIRsize(p) = size;
	FIRtype(p) = FIR_Undef;
	FIRiscomplex(p) = false;
	FIRfqlo(p) = FIRfqhi(p) = -1.0;

	return p;
}

ComplexFIR* newFIR_COMPLEX(unsigned int size)
{
	ComplexFIR* p = new ComplexFIR;
	FIRcoef(p) = new COMPLEX[size];
	::memset(FIRcoef(p), 0, size * sizeof(COMPLEX));

	FIRsize(p) = size;
	FIRtype(p) = FIR_Undef;
	FIRiscomplex(p) = true;
	FIRfqlo(p) = FIRfqhi(p) = -1.0;

	return p;
}

void delFIR_REAL(RealFIR* p)
{
	if (p != NULL) {
		delvec_REAL(FIRcoef(p));
		delete p;
	}
}

void delFIR_COMPLEX(ComplexFIR* p)
{
	if (p != NULL) {
		delvec_COMPLEX(FIRcoef(p));
		delete p;
	}
}

RealFIR* newFIR_Lowpass_REAL(REAL cutoff, REAL sr, unsigned int size)
{
  if ((cutoff < 0.0) || (cutoff > (sr / 2.0)))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      REAL fc = cutoff / sr;
      REAL corrector = 0.0;

      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      RealFIR* p = newFIR_REAL(size);
      REAL* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      for (unsigned int i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j] =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    h[midpoint - 1] = 2.0f * fc;
	}

      delete[] w;
      FIRtype (p) = FIR_Lowpass;
      return p;
    }
}

ComplexFIR* newFIR_Lowpass_COMPLEX(REAL cutoff, REAL sr, unsigned int size)
{
  if ((cutoff < 0.0) || (cutoff > (sr / 2.0)))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      REAL fc = cutoff / sr;

      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      ComplexFIR* p = newFIR_COMPLEX(size);
      COMPLEX* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      for (unsigned i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j].re =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    h[midpoint - 1].re = 2.0f * fc;
	}

      delete[] w;
      FIRtype (p) = FIR_Lowpass;
      return p;
    }
}

RealFIR* newFIR_Bandpass_REAL(REAL lo, REAL hi, REAL sr, unsigned int size)
{
  if ((lo < 0.0) || (hi > (sr / 2.0)) || (hi <= lo))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      RealFIR* p = newFIR_REAL(size);
      REAL* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      lo /= sr, hi /= sr;
      REAL fc = (hi - lo) / 2.0f;
      REAL ff = (REAL) ((lo + hi) * M_PI);

      for (unsigned int i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j] =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    h[midpoint - 1] = 2.0f * fc;
	  h[j] *= (REAL) (2.0 * cos (ff * (i - midpoint)));
	}

      delete[] w;
      FIRtype (p) = FIR_Bandpass;
      return p;
    }
}

ComplexFIR* newFIR_Bandpass_COMPLEX(REAL lo, REAL hi, REAL sr, unsigned int size)
{
  if ((lo < -(sr / 2.0)) || (hi > (sr / 2.0)) || (hi <= lo))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      ComplexFIR* p = newFIR_COMPLEX(size);
      COMPLEX* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      lo /= sr, hi /= sr;
      REAL fc = (REAL) ((hi - lo) / 2.0);
      REAL ff = (REAL) ((lo + hi) * M_PI);

      for (unsigned int i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  REAL k = REAL(int(i) - int(midpoint));
	  REAL tmp, phs = ff * k;
	  if (i != midpoint)
	    tmp = (REAL) ((sin (TWOPI * k * fc) / (M_PI * k)) * w[j]);
	  else
	    tmp = (REAL) (2.0 * fc);
	  tmp *= 2.0;
	  h[j].re = (REAL) (tmp * cos (phs));
	  h[j].im = (IMAG) (tmp * sin (phs));
	}

      delete[] w;
      FIRtype (p) = FIR_Bandpass;
      return p;
    }
}

RealFIR* newFIR_Highpass_REAL(REAL cutoff, REAL sr, unsigned int size)
{
  if ((cutoff < 0.0) || (cutoff > (sr / 2.0)))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      REAL fc = cutoff / sr;

      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      RealFIR* p = newFIR_REAL(size);
      REAL* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

	  unsigned int i;
      for (i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j] =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    h[midpoint - 1] = (REAL) (2.0 * fc);
	}

      for (i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j] = -h[j];
	  else
	    h[midpoint - 1] = 1.0f - h[midpoint - 1];
	}

      delete[] w;
      FIRtype (p) = FIR_Highpass;
      return p;
    }
}

ComplexFIR* newFIR_Highpass_COMPLEX(REAL cutoff, REAL sr, unsigned int size)
{
  if ((cutoff < 0.0) || (cutoff > (sr / 2.0)))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      REAL fc = cutoff / sr;

      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      ComplexFIR* p = newFIR_COMPLEX(size);
      COMPLEX* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

	  unsigned int i;
      for (i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j].re =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    h[midpoint - 1].re = (REAL) (2.0 * fc);
	}

      for (i = 1; i <= size; i++)
	{
	  int j = i - 1;
	  if (i != midpoint)
	    h[j].re = -h[j].re;
	  else
	    h[midpoint - 1].re = (REAL) (1.0 - h[midpoint - 1].re);
	}

      delete[] w;
      FIRtype (p) = FIR_Highpass;
      return p;
    }
}

RealFIR* newFIR_Hilbert_REAL(REAL lo, REAL hi, REAL sr, unsigned int size)
{
  if ((lo < 0.0) || (hi > (sr / 2.0)) || (hi <= lo))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      RealFIR* p = newFIR_REAL(size);
      REAL* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      lo /= sr, hi /= sr;
      REAL fc = (REAL) ((hi - lo) / 2.0);
      REAL ff = (REAL) ((lo + hi) * M_PI);

      for (unsigned int i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j] =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    h[midpoint - 1] = (REAL) (2.0 * fc);
	  h[j] *= (REAL) (2.0 * sin (ff * (i - midpoint)));
	}

      delete[] w;
      FIRtype (p) = FIR_Hilbert;
      return p;
    }
}

ComplexFIR* newFIR_Hilbert_COMPLEX(REAL lo, REAL hi, REAL sr, unsigned int size)
{
  if ((lo < 0.0) || (hi > (sr / 2.0)) || (hi <= lo))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      ComplexFIR* p = newFIR_COMPLEX(size);
      COMPLEX* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      lo /= sr, hi /= sr;
      REAL fc = (REAL) ((hi - lo) / 2.0);
      REAL ff = (REAL) ((lo + hi) * M_PI);

      for (unsigned int i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  REAL tmp, phs = ff * (i - midpoint);
	  if (i != midpoint)
	    tmp =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    tmp = (REAL) (2.0 * fc);
	  tmp *= 2.0f;
	  /* h[j].re *= tmp * cos(phs); */
	  h[j].im *= (REAL) (tmp * sin (phs));
	}

      delete[] w;
      FIRtype (p) = FIR_Hilbert;
      return p;
    }
}

RealFIR* newFIR_Bandstop_REAL(REAL lo, REAL hi, REAL sr, unsigned int size)
{
  if ((lo < 0.0) || (hi > (sr / 2.0)) || (hi <= lo))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      RealFIR* p = newFIR_REAL(size);
      REAL* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      lo /= sr, hi /= sr;
      REAL fc = (REAL) ((hi - lo) / 2.0);
      REAL ff = (REAL) ((lo + hi) * M_PI);

	  unsigned int i;
      for (i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j] =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    h[midpoint - 1] = (REAL) (2.0 * fc);
	  h[j] *= (REAL) (2.0 * cos (ff * (i - midpoint)));
	}

      for (i = 1; i <= size; i++)
	{
	  int j = i - 1;
	  if (i != midpoint)
	    h[j] = -h[j];
	  else
	    h[midpoint - 1] = (REAL) (1.0 - h[midpoint - 1]);
	}

      delete[] w;
      FIRtype (p) = FIR_Bandstop;
      return p;
    }
}

ComplexFIR* newFIR_Bandstop_COMPLEX(REAL lo, REAL hi, REAL sr, unsigned int size)
{
  if ((lo < 0.0) || (hi > (sr / 2.0)) || (hi <= lo))
    return 0;
  else if (size < 1)
    return 0;
  else
    {
      if (!(size & 01))
	size++;
      unsigned int midpoint = (size >> 01) | 01;
      ComplexFIR* p = newFIR_COMPLEX(size);
      COMPLEX* h = FIRcoef (p);
      REAL* w = CWindow::create(BLACKMANHARRIS_WINDOW, size);

      lo /= sr, hi /= sr;
      REAL fc = (REAL) ((hi - lo) / 2.0);
      REAL ff = (REAL) ((lo + hi) * M_PI);

	  unsigned int i;
      for (i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  REAL tmp, phs = ff * (i - midpoint);
	  if (i != midpoint)
	    tmp =
	      (REAL) ((sin (TWOPI * (i - midpoint) * fc) /
		       (M_PI * (i - midpoint))) * w[j]);
	  else
	    tmp = (REAL) (2.0 * fc);
	  tmp *= 2.0;
	  h[j].re *= (REAL) (tmp * cos (phs));
	  h[j].im *= (REAL) (tmp * sin (phs));
	}

      for (i = 1; i <= size; i++)
	{
	  unsigned int j = i - 1;
	  if (i != midpoint)
	    h[j] = Cmul (h[j], cxminusone);
	  else
	    h[midpoint - 1] = Csub (cxone, h[midpoint - 1]);
	}

      delete[] w;
      FIRtype (p) = FIR_Bandstop;
      return p;
    }
}
