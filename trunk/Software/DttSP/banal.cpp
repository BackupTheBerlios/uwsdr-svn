/* banal.c

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

#include <fromsys.h>
#include <banal.h>

REAL
sqr (REAL x)
{
  return x * x;
}

int
popcnt (int k)
{
  int c, i;
  c = k & 01;
  for (i = 1; i < 32; i++)
    c += (k >> i) & 01;
  return c;
}

int
npoof2 (int n)
{
  int i = 0;
  --n;
  while (n > 0)
    n >>= 1, i++;
  return i;
}

int
nblock2 (int n)
{
  return 1 << npoof2 (n);
}

int
in_blocks (int count, int block_size)
{
  if (block_size < 1)
    {
      fprintf (stderr, "block_size zero in in_blocks\n");
      exit (1);
    }
  return (1 + ((count - 1) / block_size));
}

#define MILLION (1000000)

// linear integer interpolation:
// real vector v, n long, -> real vector u, m long
// *** n must divide m
// returns actual number of valid points in u
// (== n - m/n since v[n] is undefined)

int
hinterp_vec (REAL * u, int m, REAL * v, int n)
{
  if (!u || !v || (n < 2) || (m < n) || (m % n))
    return 0;
  else
    {
      int div = m / n, i, j = 0;
      for (i = 1; i < n; i++)
	{
	  int k;
	  REAL vl = v[i - 1], del = (v[i] - vl) / div;
	  u[j++] = vl;
	  for (k = 1; k < div; k++)
	    u[j++] = vl + k * del;
	}
      u[j++] = v[n - 1];
      return j;
    }
}

void
status_message (char *msg)
{
  write (2, msg, strlen (msg));
}

//------------------------------------------------------------------------

unsigned long
hash (unsigned char *str)
{
  unsigned long hash = 5381;
  int c;
  while (c = *str++)
    hash = ((hash << 5) + hash) + c;	// (hash * 33 + c) better
  return hash;
}

int
gcd (int m, int n)
{
  return (m % n == 0 ? n : gcd (n, m % n));
}

int
least_common_mul (int i, int j)
{
  return (abs ((i * j) / gcd (i, j)));
}
