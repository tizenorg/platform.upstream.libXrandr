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
XRRGetCrtcInfo (Display *dpy, XRRScreenResources *resources, RRCrtc crtc)
{
    XExtDisplayInfo	    *info = XRRFindDisplay(dpy);
    xRRGetCrtcInfoReply	    rep;
    xRRGetCrtcInfoReq	    *req;
    int			    nbytes, nbytesRead, rbytes;
    int			    i;
    XRRCrtcInfo		    *xci;

    RRCheckExtension (dpy, info, 0);

    LockDisplay (dpy);
    GetReq (RRGetCrtcInfo, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRGetCrtcInfo;
    req->crtc = crtc;
    req->configTimestamp = resources->configTimestamp;

    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse))
    {
	UnlockDisplay (dpy);
	SyncHandle ();
	return NULL;
    }

    nbytes = (long) rep.length << 2;

    nbytesRead = (long) (rep.nOutput * 4 +
			 rep.nPossibleOutput * 4);

    /* 
     * first we must compute how much space to allocate for 
     * randr library's use; we'll allocate the structures in a single
     * allocation, on cleanlyness grounds.
     */

    rbytes = (sizeof (XRRCrtcInfo) +
	      rep.nOutput * sizeof (RROutput) +
	      rep.nPossibleOutput * sizeof (RROutput));

    xci = (XRRCrtcInfo *) Xmalloc(rbytes);
    if (xci == NULL) {
	_XEatData (dpy, (unsigned long) nbytes);
	UnlockDisplay (dpy);
	SyncHandle ();
	return NULL;
    }

    xci->timestamp = rep.timestamp;
    xci->x = rep.x;
    xci->y = rep.y;
    xci->width = rep.width;
    xci->height = rep.height;
    xci->mode = rep.mode;
    xci->rotation = rep.rotation;
    xci->noutput = rep.nOutput;
    xci->outputs = (RROutput *) (xci + 1);
    xci->npossible = rep.nPossibleOutput;
    xci->possible = (RROutput *) (xci->outputs + rep.nOutput);

    _XRead32 (dpy, xci->outputs, rep.nOutput << 2);
    _XRead32 (dpy, xci->possible, rep.nPossibleOutput << 2);
    
    /*
     * Skip any extra data
     */
    if (nbytes > nbytesRead)
	_XEatData (dpy, (unsigned long) (nbytes - nbytesRead));
    
    UnlockDisplay (dpy);
    SyncHandle ();
    return (XRRCrtcInfo *) xci;
}

void
XRRFreeCrtcInfo (XRRCrtcInfo *crtcInfo)
{
    Xfree (crtcInfo);
}

Status
XRRSetCrtcConfig (Display *dpy,
		  XRRScreenResources *resources,
		  RRCrtc crtc,
		  Time timestamp,
		  int x, int y,
		  RRMode mode,
		  Rotation rotation,
		  XRROutputConfig *outputs,
		  int noutputs)
{
    XExtDisplayInfo	    *info = XRRFindDisplay(dpy);
    xRRSetCrtcConfigReply   rep;
    xRRSetCrtcConfigReq	    *req;
    int			    i;

    RRCheckExtension (dpy, info, 0);

    LockDisplay(dpy);
    GetReq (RRSetCrtcConfig, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRSetCrtcConfig;
    req->length += noutputs << 1;
    req->crtc = crtc;
    req->timestamp = timestamp;
    req->configTimestamp = resources->configTimestamp;
    req->x = x;
    req->y = y;
    req->mode = mode;
    req->rotation = rotation;
    Data32 (dpy, outputs, noutputs << 3);

    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse))
	rep.status = RRSetConfigFailed;
    UnlockDisplay (dpy);
    SyncHandle ();
    return rep.status;
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
