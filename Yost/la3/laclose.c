/*
	Copyright (c) 1985, 1986 David Yost     All Rights Reserved
	Copyright (c) 1986 Grand Software, Inc. All Rights Reserved
	THIS FILE CONTAINS UNPUBLISHED PROPRIETARY SOURCE CODE
	which is property of Grand Software, Inc.
	Los Angeles, CA 90046 U.S.A 213-650-1089
	The copyright notice above does not evidence any
	actual or intended publication of such source code.
	This file is not to be copied by anyone except as
	covered by written agreement with Grand Software, Inc.,
	and this notice is not to be removed.
 */


#include "lalocal.h"

/*
    If this is the last stream into the last view into the file,
    we want to deallocate all the fsd's.  If there are still
    other files using some fsd's from this file, we can't
    deallocate them.  If we can, then return 0 (i.e. successful).
 */

La_linepos
la_close (plas)
Reg1 La_stream *plas;
{
    Reg2 La_view *nlav;
    Reg3 La_file *nlaf;
    Reg4 La_flag freelaf;
    Reg5 La_flag freelav;

    if (!la_verify (plas))
	return -1;
    nlav = plas->la_view;
    nlaf = nlav->la_file;
    if (   plas == la_chglas
	|| nlav->la_srefs < 1
	|| nlaf->la_vrefs < 1
       ) {
	la_errno = LA_BADSTREAM;
	return -1;
    }

    freelaf = NO;
    freelav = NO;
    if (nlav->la_srefs > 1)
	nlav->la_srefs--;
    else {
	nlav->la_fifsd = la_freeifsd (nlav->la_fifsd);
	if (nlaf->la_vrefs > 1)
	    nlaf->la_vrefs--;
	else {
	    if (nlaf->la_fsrefs > 0)
		la_freefsd (&nlaf->la_ffsd);
	    if (nlaf->la_fsrefs > 0)
		return nlaf->la_fsrefs;
	    if (nlaf->la_fsrefs < 0) {
		la_abort ("nlaf->la_fsrefs < 0 in la_close");
		/* NOTREACHED */
	    }
	    ff_close (nlaf->la_ffs);
	    freelaf = YES;
	    la_chans--;
	}
	free ((char *) nlav->la_fifsd);
	/* unlink the view from the chain views into this file */
	if (nlav->la_fback)
	    nlav->la_fback->la_fforw = nlav->la_fforw;
	else
	    nlaf->la_fview = nlav->la_fforw;
	if (nlav->la_fforw)
	    nlav->la_fforw->la_fback = nlav->la_fback;
	else
	    nlaf->la_lview = nlav->la_fback;
	freelav = YES;
    }

    /* unlink the stream from the chain of all streams */
    if (plas->la_sback)
	plas->la_sback->la_sforw = plas->la_sforw;
    if (plas->la_sforw)
	plas->la_sforw->la_sback = plas->la_sback;
    else
	la_laststream = plas->la_sback;

    /* unlink the stream from the chain streams into this view */
    if (plas->la_vback)
	plas->la_vback->la_vforw = plas->la_vforw;
    else
	nlav->la_fstream = plas->la_vforw;
    if (plas->la_vforw)
	plas->la_vforw->la_vback = plas->la_vback;
    else
	nlav->la_lstream = plas->la_vback;

    if (freelaf)
	free ((char *) nlaf);
    if (freelav)
	free ((char *) nlav);
    if (plas->la_sflags & LA_ALLOCED)
	free ((char *) plas);

    return 0;
}



