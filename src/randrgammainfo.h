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
 
#ifndef RANDRGAMMAINFO_H
#define RANDRGAMMAINFO_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

void get_gamma_info(Display *dpy, XRRScreenResources *res, RRCrtc crtc, float *brightness, float *red, float *blue, float *green);

void set_gamma(Display *dpy, XRRScreenResources *res, RRCrtc crtc_id, float brightness, float red, float blue, float green);

#endif
