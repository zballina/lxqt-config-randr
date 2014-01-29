/////////////////////////////////////////////////////////////////////////////
// Some functions from xrandr.c (Xorg) 

// Licenced under The Open Group licence:

/* 
 * Copyright © 2001 Keith Packard, member of The XFree86 Project, Inc.
 * Copyright © 2002 Hewlett Packard Company, Inc.
 * Copyright © 2006 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * Thanks to Jim Gettys who wrote most of the client side code,
 * and part of the server code for randr.
 */

#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "randrgammainfo.h"

/* Returns the index of the last value in an array < 0xffff */
static int find_last_non_clamped(unsigned short array[], int size) {
    int i;
    for (i = size - 1; i > 0; i--) {
        if (array[i] < 0xffff)
	    return i;
    }
    return 0;
}

void get_gamma_info(Display *dpy, XRRScreenResources *res, RRCrtc crtc, float *brightness, float *red, float *blue, float *green)
{
    XRRCrtcGamma *crtc_gamma;
    double i1, v1, i2, v2;
    int size, middle, last_best, last_red, last_green, last_blue;
    unsigned short *best_array;

    size = XRRGetCrtcGammaSize(dpy, crtc);
    if (!size) {
      printf("Failed to get size of gamma for output\n");
      return;
    }

    crtc_gamma = XRRGetCrtcGamma(dpy, crtc);
    if (!crtc_gamma) {
      printf("Failed to get gamma for output\n");
      return;
    }

    /*
     * Here is a bit tricky because gamma is a whole curve for each
     * color.  So, typically, we need to represent 3 * 256 values as 3 + 1
     * values.  Therefore, we approximate the gamma curve (v) by supposing
     * it always follows the way we set it: a power function (i^g)
     * multiplied by a brightness (b).
     * v = i^g * b
     * so g = (ln(v) - ln(b))/ln(i)
     * and b can be found using two points (v1,i1) and (v2, i2):
     * b = e^((ln(v2)*ln(i1) - ln(v1)*ln(i2))/ln(i1/i2))
     * For the best resolution, we select i2 at the highest place not
     * clamped and i1 at i2/2. Note that if i2 = 1 (as in most normal
     * cases), then b = v2.
     */
    last_red = find_last_non_clamped(crtc_gamma->red, size);
    last_green = find_last_non_clamped(crtc_gamma->green, size);
    last_blue = find_last_non_clamped(crtc_gamma->blue, size);
    best_array = crtc_gamma->red;
    last_best = last_red;
    if (last_green > last_best) {
      last_best = last_green;
      best_array = crtc_gamma->green;
    }
    if (last_blue > last_best) {
      last_best = last_blue;
      best_array = crtc_gamma->blue;
    }
    if (last_best == 0)
      last_best = 1;

    middle = last_best / 2;
    i1 = (double)(middle + 1) / size;
    v1 = (double)(best_array[middle]) / 65535;
    i2 = (double)(last_best + 1) / size;
    v2 = (double)(best_array[last_best]) / 65535;
    if (v2 < 0.0001) { /* The screen is black */
      *brightness = 0;
      *red = 1;
      *green = 1;
      *blue = 1;
    } else {
    if ((last_best + 1) == size)
        *brightness = v2;
    else
        *brightness = exp((log(v2)*log(i1) - log(v1)*log(i2))/log(i1/i2));
        *red = log((double)(crtc_gamma->red[last_red / 2]) / *brightness
              / 65535) / log((double)((last_red / 2) + 1) / size);
        *green = log((double)(crtc_gamma->green[last_green / 2]) / *brightness
                / 65535) / log((double)((last_green / 2) + 1) / size);
        *blue = log((double)(crtc_gamma->blue[last_blue / 2]) / *brightness
               / 65535) / log((double)((last_blue / 2) + 1) / size);
    }

    XRRFreeGamma(crtc_gamma);
}

static double dmin (double x, double y)
{
    return x < y ? x : y;
}

void
set_gamma(Display *dpy, XRRScreenResources *res, RRCrtc crtc_id, float brightness, float red, float blue, float green)
{
	int i, size, shift;
	XRRCrtcGamma *crtc_gamma;
	float gammaRed;
	float gammaGreen;
	float gammaBlue;

	qDebug() << "[set_gamma] Appling brightness " << brightness;

	//XRRCrtcInfo *info = XRRGetCrtcInfo(dpy, res, crtc_id);

	size = XRRGetCrtcGammaSize(dpy, crtc_id);

	if (!size) {
	    qDebug() << "Gamma size is 0.\n";
	    return;
	}

	/*
	 * The gamma-correction lookup table managed through XRR[GS]etCrtcGamma
	 * is 2^n in size, where 'n' is the number of significant bits in
	 * the X Color.  Because an X Color is 16 bits, size cannot be larger
	 * than 2^16.
	 */
	if (size > 65536) {
	    qDebug() << "Gamma correction table is impossibly large.\n";
	    return;
	}

	/*
	 * The hardware color lookup table has a number of significant
	 * bits equal to ffs(size) - 1; compute all values so that
	 * they are in the range [0,size) then shift the values so
	 * that they occupy the MSBs of the 16-bit X Color.
	 */
	shift = 16 - (ffs(size) - 1);

	crtc_gamma = XRRAllocGamma(size);
	if (!crtc_gamma) {
	    qDebug() << "Gamma allocation failed.\n";
	    return;
	}

	if (red == 0.0)
	    red = 1.0;
	if (green == 0.0)
	    green = 1.0;
	if (blue == 0.0)
	    blue = 1.0;

	gammaRed = 1.0 / red;
	gammaGreen = 1.0 / green;
	gammaBlue = 1.0 / blue;

	for (i = 0; i < size; i++) {
	    if (gammaRed == 1.0 && brightness == 1.0)
		crtc_gamma->red[i] = i;
	    else
		crtc_gamma->red[i] = dmin(pow((double)i/(double)(size - 1),
					      gammaRed) * brightness,
					  1.0) * (double)(size - 1);
	    crtc_gamma->red[i] <<= shift;

	    if (gammaGreen == 1.0 && brightness == 1.0)
		crtc_gamma->green[i] = i;
	    else
		crtc_gamma->green[i] = dmin(pow((double)i/(double)(size - 1),
						gammaGreen) * brightness,
					    1.0) * (double)(size - 1);
	    crtc_gamma->green[i] <<= shift;

	    if (gammaBlue == 1.0 && brightness == 1.0)
		crtc_gamma->blue[i] = i;
	    else
		crtc_gamma->blue[i] = dmin(pow((double)i/(double)(size - 1),
					       gammaBlue) * brightness,
					   1.0) * (double)(size - 1);
	    crtc_gamma->blue[i] <<= shift;
	}

	XRRSetCrtcGamma(dpy, crtc_id, crtc_gamma);

	free(crtc_gamma);

}

