/* FromSys.h

   stuff we need to import everywhere 
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

#ifndef _fromsys_h
#define _fromsys_h

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

// For semaphores and logging
#if defined(__WXMSW__) || defined(__WXGTK__)
#include <wx/wx.h>

#define	ASSERT(x)	wxASSERT(x)
#define	SEM_POST(x)	(x)->Post()
#define	SEM_WAIT(x)	(x)->Wait()

#define	NEED_MINMAX	1

#elif defined(WIN32)
#include <windows.h>
#include <cassert>

#define	ASSERT(x)	assert(x)
#define	SEM_POST(x)	::ReleaseSemaphore(x, 1, NULL)
#define	SEM_WAIT(x)	::WaitForSingleObject(x, INFINITE)

#else
#include <pthread.h>
#include <semaphore.h>
#include <syslog.h>
#include <cassert>

#define	ASSERT(x)	assert(x)
#define	SEM_POST(x)	::sem_post(&(x))
#define	SEM_WAIT(x)	::sem_wait(&(x))

#define	NEED_MINMAX	1
#endif


#ifndef M_PI
const double M_PI = 3.14159265358979323846;
#endif

#ifndef TWOPI
const double TWOPI = 2.0 * M_PI;
#endif

#endif
