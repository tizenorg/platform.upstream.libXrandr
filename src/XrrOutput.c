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

#define OutputInfoExtra	(SIZEOF(xRRGetOutputInfoReply) - 32)
				
XRROutputInfo *
XRRGetOutputInfo (Display *dpy, XRRScreenResources *resources, RROutput output)
{
    XExtDisplayInfo		*info = XRRFindDisplay(dpy);
    xRRGetOutputInfoReply	rep;
    xRRGetOutputInfoReq		*req;
    int				nbytes, nbytesRead, rbytes;
    int				i;
    xRRQueryVersionReq		*vreq;
    XRROutputInfo		*xoi;

    RRCheckExtension (dpy, info, 0);

    LockDisplay (dpy);
    GetReq (RRGetOutputInfo, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRGetOutputInfo;
    req->output = output;
    req->configTimestamp = resources->configTimestamp;

    if (!_XReply (dpy, (xReply *) &rep, OutputInfoExtra >> 2, xFalse))
    {
	UnlockDisplay (dpy);
	SyncHandle ();
	return NULL;
    }

    nbytes = ((long) (rep.length) << 2) - OutputInfoExtra;

    nbytesRead = (long) (rep.nCrtcs * 4 +
			 rep.nModes * 4 +
			 rep.nClones * 4 +
			 ((rep.nameLength + 3) & ~3));

    /* 
     * first we must compute how much space to allocate for 
     * randr library's use; we'll allocate the structures in a single
     * allocation, on cleanlyness grounds.
     */

    rbytes = (sizeof (XRROutputInfo) +
	      rep.nCrtcs * sizeof (RRCrtc) +
	      rep.nModes * sizeof (RRMode) +
	      rep.nClones * sizeof (RROutput) +
	      rep.nameLength + 1);	    /* '\0' terminate name */

    xoi = (XRROutputInfo *) Xmalloc(rbytes);
    if (xoi == NULL) {
	_XEatData (dpy, (unsigned long) nbytes);
	UnlockDisplay (dpy);
	SyncHandle ();
	return NULL;
    }

    xoi->timestamp = rep.timestamp;
    xoi->crtc = rep.crtc;
    xoi->current_options = rep.currentOptions;
    xoi->mm_width = rep.mmWidth;
    xoi->mm_height = rep.mmHeight;
    xoi->connection = rep.connection;
    xoi->subpixel_order = rep.subpixelOrder;
    xoi->possible_options = rep.possibleOptions;
    xoi->ncrtc = rep.nCrtcs;
    xoi->crtcs = (RRCrtc *) (xoi + 1);
    xoi->nmode = rep.nModes;
    xoi->npreferred = rep.nPreferred;
    xoi->modes = (RRMode *) (xoi->crtcs + rep.nCrtcs);
    xoi->nclone = rep.nClones;
    xoi->clones = (RROutput *) (xoi->modes + rep.nModes);
    xoi->name = (char *) (xoi->clones + rep.nClones);

    _XRead32 (dpy, xoi->crtcs, rep.nCrtcs << 2);
    _XRead32 (dpy, xoi->modes, rep.nModes << 2);
    _XRead32 (dpy, xoi->clones, rep.nClones << 2);
    
    /*
     * Read name and '\0' terminate
     */
    _XReadPad (dpy, xoi->name, rep.nameLength);
    xoi->name[rep.nameLength] = '\0';
    
    /*
     * Skip any extra data
     */
    if (nbytes > nbytesRead)
	_XEatData (dpy, (unsigned long) (nbytes - nbytesRead));
    
    UnlockDisplay (dpy);
    SyncHandle ();
    return (XRROutputInfo *) xoi;
}

void
XRRFreeOutputInfo (XRROutputInfo *outputInfo)
{
    Xfree (outputInfo);
}

Atom *
XRRListOutputProperties (Display *dpy, RROutput output, int *nprop)
{
    XExtDisplayInfo		*info = XRRFindDisplay(dpy);
    xRRListOutputPropertiesReply rep;
    xRRListOutputPropertiesReq	*req;
    int				nbytes, nbytesRead, netbytes;
    int				i;
    xRRQueryVersionReq		*vreq;
    Atom			*props;

    RRCheckExtension (dpy, info, 0);

    LockDisplay (dpy);
    GetReq (RRListOutputProperties, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRListOutputProperties;
    req->output = output;

    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse)) {
	UnlockDisplay (dpy);
	SyncHandle ();
	*nprop = 0;
	return NULL;
    }

    if (rep.nProperties) {
	nbytes = rep.nProperties * sizeof (Atom);
	netbytes = rep.nProperties << 2;

	props = (Atom *) Xmalloc (nbytes);
	if (props == NULL) {
	    _XEatData (dpy, nbytes);
	    UnlockDisplay (dpy);
	    SyncHandle ();
	    *nprop = 0;
	    return NULL;
	}

	_XRead32 (dpy, props, nbytes);
    }

    *nprop = rep.nProperties;
    UnlockDisplay (dpy);
    SyncHandle ();
    return props;
}

void
XRRChangeOutputProperty (Display *dpy, RROutput output,
			 Atom property, Atom type,
			 int format, int mode,
			 _Xconst unsigned char *data, int nelements)
{
    XExtDisplayInfo		*info = XRRFindDisplay(dpy);
    xRRChangeOutputPropertyReq	*req;
    xRRQueryVersionReq		*vreq;
    long len;

    RRSimpleCheckExtension (dpy, info);

    LockDisplay(dpy);
    GetReq (RRChangeOutputProperty, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRChangeOutputProperty;
    req->output = output;
    req->property = property;
    req->type = type;
    req->mode = mode;
    if (nelements < 0) {
	req->nUnits = 0;
	req->format = 0; /* ask for garbage, get garbage */
    } else {
	req->nUnits = nelements;
	req->format = format;
    }

    switch (req->format) {
    case 8:
	len = ((long)nelements + 3) >> 2;
	if (dpy->bigreq_size || req->length + len <= (unsigned) 65535) {
	    SetReqLen(req, len, len);
	    Data (dpy, (char *)data, nelements);
	} /* else force BadLength */
	break;

    case 16:
	len = ((long)nelements + 1) >> 1;
	if (dpy->bigreq_size || req->length + len <= (unsigned) 65535) {
	    SetReqLen(req, len, len);
	    len = (long)nelements << 1;
	    Data16 (dpy, (short *) data, len);
	} /* else force BadLength */
	break;

    case 32:
	len = nelements;
	if (dpy->bigreq_size || req->length + len <= (unsigned) 65535) {
	    SetReqLen(req, len, len);
	    len = (long)nelements << 2;
	    Data32 (dpy, (long *) data, len);
	} /* else force BadLength */
	break;

    default:
	/* BadValue will be generated */ ;
    }

    UnlockDisplay(dpy);
    SyncHandle();
}

void
XRRDeleteOutputProperty (Display *dpy, RROutput output, Atom property)
{
    XExtDisplayInfo		*info = XRRFindDisplay(dpy);
    xRRDeleteOutputPropertyReq *req;

    RRSimpleCheckExtension (dpy, info);

    LockDisplay(dpy);
    GetReq(RRDeleteOutputProperty, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRDeleteOutputProperty;
    req->output = output;
    req->property = property;
    UnlockDisplay(dpy);
    SyncHandle();
}

int
XRRGetOutputProperty (Display *dpy, RROutput output,
		      Atom property, long offset, long length,
		      Bool delete, Atom req_type, 
		      Atom *actual_type, int *actual_format,
		      unsigned long *nitems, unsigned long *bytes_after,
		      unsigned char **prop)
{
    XExtDisplayInfo		*info = XRRFindDisplay(dpy);
    xRRGetOutputPropertyReply	rep;
    xRRGetOutputPropertyReq	*req;
    int				nbytes, nbytesRead;
    int				i;
    xRRQueryVersionReq		*vreq;

    RRCheckExtension (dpy, info, 1);

    LockDisplay (dpy);
    GetReq (RRGetOutputProperty, req);
    req->reqType = info->codes->major_opcode;
    req->randrReqType = X_RRGetOutputProperty;
    req->output = output;
    req->property = property;
    req->type = req_type;
    req->longOffset = offset;
    req->longLength = length;
    req->delete = delete;

    if (!_XReply (dpy, (xReply *) &rep, 0, xFalse))
    {
	UnlockDisplay (dpy);
	SyncHandle ();
	return 1;
    }

    *prop = (unsigned char *) NULL;
    if (rep.propertyType != None) {
	long nbytes, netbytes;
	/*
	 * One extra byte is malloced than is needed to contain the property
	 * data, but this last byte is null terminated and convenient for
	 * returning string properties, so the client doesn't then have to
	 * recopy the string to make it null terminated.
	 */
	switch (rep.format) {
	case 8:
	    nbytes = netbytes = rep.nItems;
	    if (nbytes + 1 > 0 &&
		(*prop = (unsigned char *) Xmalloc ((unsigned)nbytes + 1)))
		_XReadPad (dpy, (char *) *prop, netbytes);
	    break;

	case 16:
	    nbytes = rep.nItems * sizeof (short);
	    netbytes = rep.nItems << 1;
	    if (nbytes + 1 > 0 &&
		(*prop = (unsigned char *) Xmalloc ((unsigned)nbytes + 1)))
		_XRead16Pad (dpy, (short *) *prop, netbytes);
	    break;

	case 32:
	    nbytes = rep.nItems * sizeof (long);
	    netbytes = rep.nItems << 2;
	    if (nbytes + 1 > 0 &&
		(*prop = (unsigned char *) Xmalloc ((unsigned)nbytes + 1)))
		_XRead32 (dpy, (long *) *prop, netbytes);
	    break;

	default:
	    /*
	     * This part of the code should never be reached.  If it is,
	     * the server sent back a property with an invalid format.
	     */
	    _XEatData(dpy, (unsigned long) netbytes);
	    UnlockDisplay(dpy);
	    SyncHandle();
	    return(BadImplementation);
	}
	if (! *prop) {
	    _XEatData(dpy, (unsigned long) netbytes);
	    UnlockDisplay(dpy);
	    SyncHandle();
	    return(BadAlloc);
	}
	(*prop)[nbytes] = '\0';
    }

    *actual_type = rep.propertyType;
    *actual_format = rep.format;
    *nitems = rep.nItems;
    *bytes_after = rep.bytesAfter;
    UnlockDisplay (dpy);
    SyncHandle ();

    return Success;
}
