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

extern char *malloc ();

La_linepos
la_lcopy (slas, dlas, nlines)
La_stream *slas;
La_stream *dlas;
La_linepos nlines;
{
    Reg4 La_linepos tlines;
    Reg5 La_fsd *cfsd;
    Reg6 La_fsd *t1fsd;
    La_fsd *nffsd;
    La_fsd *nlfsd;
    La_linepos lntmp;
#ifdef LA_BP
    La_bytepos    nbytes;
#endif

    if (nlines <= 0)
	return 0;

    /* Don't grow dlas beyond highest int */
    lntmp = dlas->la_file->la_nlines;
    if (lntmp + nlines != la_ltrunc (lntmp + nlines)) {
	la_errno = LA_ERRMAXL;
	return -1;
    }

    if (slas->la_file->la_lfsd == (cfsd = slas->la_cfsd))
	return 0;

    if (!la_zbreak (dlas))
	return -1;

    /* break fsd and make note of current position */
    if (!la_break (slas, BRK_COPY, &nffsd, &nlfsd, &nlines
#ifdef LA_BP
	, &nbytes
#endif
	))
	return -1;

    /*printf ("Break results: lines=%D, bytes=%D, nffsd=0x%04x, nlfsd=0x%04x\n",
	    nlines, nbytes, nffsd, nlfsd);*/
    /*la_fsddump (slas->la_file->la_ffsd, nfsd, YES, "");*/
    /*la_sdump (slas, "");*/

    /* fill in the interior of the chain if necessary */
    tlines = nlines;
    if (nffsd) {
	tlines -= nffsd->fsdnlines;
	cfsd = cfsd->fsdforw;
    }
    t1fsd = nffsd;

    while (tlines > 0) {
	Reg2 int j;
	Reg3 La_fsd *t2fsd;

	if (tlines >= (j = cfsd->fsdnlines)) {
	    Reg1 char *cp;
	    unsigned int fsdsize;

	    tlines -= j;
	    /* copy the fsd */
	    if (*(cp = cfsd->fsdbytes)) {
		while (j--)
#ifndef NOSIGNEDCHAR
		    if (*cp++ < 0)
			cp++;
#else
		    if (*cp++ & LA_LLINE)
			cp++;
#endif
		fsdsize = LA_FSDSIZE + cp - cfsd->fsdbytes;
	    } else
		fsdsize = LA_FSDSIZE + 6;
	    if ((t2fsd = (La_fsd *) malloc (fsdsize)) == NULL) {
		la_errno = LA_NOMEM;
		la_freefsd (nffsd);
		la_freefsd (nlfsd);
		return 0;
	    }
	    move ((char *) cfsd, (char *) t2fsd, fsdsize);
	    t2fsd->fsdfile->la_fsrefs++;
	} else {
	    if (!nlfsd || nlfsd->fsdnlines != tlines)
		la_abort ("lacopy last fsd null");
	    tlines = 0;
	    t2fsd = nlfsd;
	}
	if (t1fsd)
	    t1fsd->fsdforw = t2fsd;
	else
	    nffsd = t2fsd;
	t2fsd->fsdback = t1fsd;
	t1fsd = t2fsd;
	cfsd = cfsd->fsdforw;
    }

    la_link (dlas, nffsd, t1fsd, nlines
#ifdef LA_BP
		 , nbytes
#endif
		);

    return nlines;
}
