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


#include "lalocal.hxx"

La_linepos
La_stream::la_close ()
{
    Reg2 La_file *nlaf;
    Reg3 La_flag freelaf;

    if (!la_verify ())
	return -1;
    if (   this == la_chglas
	|| (nlaf = la_file)->la_refs < 1
       ) {
	la_errno = LA_BADSTREAM;
	return -1;
    }

    freelaf = NO;
    if (nlaf->la_refs > 1)
	nlaf->la_refs--;
    else {
	if (nlaf->la_fsrefs > 0)
	    la_freefsd (nlaf->la_ffsd);
	if (nlaf->la_fsrefs > 0)
	    return nlaf->la_fsrefs;
	else if (nlaf->la_fsrefs < 0)
	    la_abort ("nlaf->la_fsrefs < 0 in la_close");
	else {
	    ff_close (nlaf->la_ffs);
	    freelaf = YES;
	    la_chans--;
	}
    }

    /* unlink the stream from the chain of all streams */
    if (la_sback)
	la_sback->la_sforw = la_sforw;
    if (la_sforw)
	la_sforw->la_sback = la_sback;
    else
	la_laststream = la_sback;

    /* unlink the stream from the chain streams into this file */
    if (la_fback)
	la_fback->la_fforw = la_fforw;
    else
	nlaf->la_fstream = la_fforw;
    if (la_fforw)
	la_fforw->la_fback = la_fback;
    else
	nlaf->la_lstream = la_fback;

    if (freelaf)
	free ((char *) nlaf);
    if (la_sflags & LA_ALLOCED)
	free ((char *) this);
    return 0;
}
