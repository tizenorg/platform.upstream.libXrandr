/*
 * Copyright © 2006 Keith Packard
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
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <X11/Xlib.h>
/* we need to be able to manipulate the Display structure on events */
#include <X11/Xlibint.h>
#include <X11/extensions/render.h>
#include <X11/extensions/Xrender.h>
#include "Xrandrint.h"

XRRCrtcInfo *
XRRGetCrtcInfo (Display *dpy, XRRScreenResources *resources, RRCrtc crtc);

void
XRRFreeCrtcInfo (XRRCrtcInfo *crtcInfo);

Status
XRRSetCrtc (Display *dpy,
	    XRRScreenResources *resources,
	    RRCrtc crtc,
	    RRMode mode,
	    Rotation rotation,
	    RROutput *outputs,
	    int noutputs)
{
}

int
XRRGetCrtcGammaSize (Display *dpy, RRCrtc crtc)
{
}

XRRCrtcGamma *
XRRGetCrtcGamma (Display *dpy, RRCrtc crtc)
{
}

XRRCrtcGamma *
XRRAllocGamma (int size)
{
}

void
XRRSetCrtcGamma (Display *dpy, RRCrtc crtc, XRRCrtcGamma *gamma)
{
}

void
XRRFreeGamma (XRRCrtcGamma *gamma)
{
}
