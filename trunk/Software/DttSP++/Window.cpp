/* Window.cpp

This file is part of a program that implements a Software-Defined Radio.

Copyright (C) 2004, 2005, 2006 by Frank Brickle, AB2KT and Bob McGwier, N4HY
Implemented from code by Bill Schottstaedt of Snd Editor at CCRMA

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

#include "Window.h"
#include "banal.h"
#include "FromSys.h"


/* shamelessly stolen from Bill Schottstaedt's clm.c */
/* made worse in the process, but enough for our purposes here */

/* mostly taken from
 *    Fredric J. Harris, "On the Use of Windows for Harmonic Analysis with the
 *    Discrete Fourier Transform," Proceedings of the IEEE, Vol. 66, No. 1,
 *    January 1978.
 *    Albert H. Nuttall, "Some Windows with Very Good Sidelobe Behaviour", 
 *    IEEE Transactions of Acoustics, Speech, and Signal Processing, Vol. ASSP-29,
 *    No. 1, February 1981, pp 84-91
 *
 * JOS had slightly different numbers for the Blackman-Harris windows.
 */

REAL* CWindow::create(Windowtype type, unsigned int size, REAL* window)
{
	unsigned int i, j;
	REAL sr1;

	unsigned int midn = size >> 1;
	unsigned int midp1 = (size + 1) / 2;
	unsigned int midm1 = (size - 1) / 2;

	REAL freq = REAL(TWOPI / REAL(size));
	REAL rate = 1.0F / REAL(midn);
	REAL angle = 0.0F;
	REAL expn = REAL(::log(2.0) / REAL(midn + 1));
	REAL expsum = 1.0F;

	if (window == NULL)
		window = new REAL[size];
	::memset(window, 0, size * sizeof(REAL));

	switch (type) {
		case RECTANGULAR_WINDOW:
			for (i = 0; i < size; i++)
				window[i] = 1.0F;
			break;

		case HANN_WINDOW:
			for (i = 0, j = size - 1, angle = 0.0; i <= midn; i++, j--, angle += freq)
				window[j] = window[i] = REAL(0.5 - 0.5 * ::cos(angle));
			break;

		case WELCH_WINDOW:
			for (i = 0, j = size - 1; i <= midn; i++, j--)
				window[j] = window[i] = REAL(1.0 - ::sqr(REAL(i - midm1) / REAL(midp1)));
			break;

		case PARZEN_WINDOW:
			for (i = 0, j = size - 1; i <= midn; i++, j--)
				window[j] = window[i] = REAL(1.0 - ::fabs(REAL(i - midm1) / REAL(midp1)));
			break;

		case BARTLETT_WINDOW:
			for (i = 0, j = size - 1, angle = 0.0; i <= midn; i++, j--, angle += rate)
				window[j] = window[i] = angle;
			break;

		case HAMMING_WINDOW:
			for (i = 0, j = size - 1, angle = 0.0; i <= midn; i++, j--, angle += freq)
				window[j] = window[i] = REAL(0.54 - 0.46 * ::cos(angle));
			break;

		case BLACKMAN2_WINDOW:	/* using Chebyshev polynomial equivalents here */
			for (i = 0, j = size - 1, angle = 0.0; i <= midn; i++, j--, angle += freq) {
				REAL cx = (REAL)::cos(angle);
				window[j] = window[i] = REAL(0.34401 + (cx * (-0.49755 + (cx * 0.15844))));
			}
			break;

		case BLACKMAN3_WINDOW:
			for (i = 0, j = size - 1, angle = 0.0; i <= midn; i++, j--, angle += freq) {
				REAL cx = (REAL)::cos(angle);
				window[j] = window[i] = REAL(0.21747 + (cx * (-0.45325 + (cx * (0.28256 - (cx * 0.04672))))));
			}
			break;

	    case BLACKMAN4_WINDOW:
			for (i = 0, j = size - 1, angle = 0.0; i <= midn; i++, j--, angle += freq) {
				REAL cx = (REAL)::cos(angle);
				window[j] = window[i] = REAL(0.084037 + (cx * (-0.29145 + (cx * (0.375696 + (cx * (-0.20762 + (cx * 0.041194))))))));
			}
			break;

		case EXPONENTIAL_WINDOW:
			for (i = 0, j = size - 1; i <= midn; i++, j--) {
				window[j] = window[i] = REAL(expsum - 1.0);
				expsum *= expn;
			}
			break;

		case RIEMANN_WINDOW:
			sr1 = REAL(TWOPI / REAL(size));
			for (i = 0, j = size - 1; i <= midn; i++, j--) {
				if (i == midn) {
					window[j] = window[i] = 1.0F;
				} else {
					REAL cx = sr1 * REAL(midn - i);
					window[j] = window[i] = REAL(::sin(cx) / cx);
				}
			}
			break;

		case BLACKMANHARRIS_WINDOW: {
				const REAL a0 = 0.35875F;
				const REAL a1 = 0.48829F;
				const REAL a2 = 0.14128F;
				const REAL a3 = 0.01168F;

				for (i = 0; i < size; i++)
					window[i] = REAL(a0 - a1 * (REAL)::cos(      TWOPI * REAL(i + 0.5F) / REAL(size - 1)) +
										  a2 * (REAL)::cos(2.0 * TWOPI * REAL(i + 0.5F) / REAL(size - 1)) -
										  a3 * (REAL)::cos(3.0 * TWOPI * REAL(i + 0.5F) / REAL(size - 1)));
			}
			break;

		case NUTTALL_WINDOW: {
				const REAL a0 = 0.3635819F;
				const REAL a1 = 0.4891775F;
				const REAL a2 = 0.1365995F;
				const REAL a3 = 0.0106411F;

				for (i = 0; i < size; i++)
					window[i] = REAL(a0 - a1 * (REAL)::cos(      TWOPI * REAL(i + 0.5) / REAL(size - 1)) +
										  a2 * (REAL)::cos(2.0 * TWOPI * REAL(i + 0.5) / REAL(size - 1)) -
										  a3 * (REAL)::cos(3.0 * TWOPI * REAL(i + 0.5) / REAL(size - 1)));
			}
			break;

		default:
			delete[] window;
			return NULL;
	}

	return window;
}
