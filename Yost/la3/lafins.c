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

La_linepos
la_finsert (plas, stchar, nchars, stline, nlines, filename, pffs, chan)
La_stream *plas;
La_bytepos stchar;
La_bytepos nchars;
La_linepos stline;
La_linepos nlines;
char *filename;
Ff_stream *pffs;
int chan;
{
    La_linepos nl;
    La_linepos lntmp;
    La_fsd *ffsd, *lfsd;        /* first and last fsds to be made for buf */
    Ff_stream *ffs;
    long fsdpos;

    if (   stline < 0
	&& stchar < 0
       ) {
	stchar = 0;
	nchars = -1;
    }

    if (   (   stline >= 0
	    && nlines == 0
	   )
	|| (   stchar >= 0
	    && nchars == 0
	   )
       ) {
	return 0;

    if (!la_zbreak (plas))
	return -1;

    if (!la_open (filename, "",

    ffs = la_chglas->la_file->la_ffs;
    fsdpos = ff_size (ffs);
    /*printf ("about to parse: fsdpos=%d, nchars=%d\n", fsdpos, nchars);*/
    if ((nl = la_parse ((Ff_stream *) 0, fsdpos, &ffsd, &lfsd,
			la_chglas->la_file, (La_bytepos) nchars, buf))
	< 0)
	return -1;

    /* Don't grow plas beyond highest int */
    lntmp = plas->la_file->la_nlines;
    if (lntmp + nl != la_ltrunc (lntmp + nl)) {
	la_errno = LA_ERRMAXL;
 err:   la_freefsd (ffsd);
	return -1;
    }

    ff_seek (ffs, fsdpos);
    if (ff_write (ffs, buf, nchars) != nchars) {
	la_errno = LA_WRTERR;
	goto err;
    }

    la_link (plas, ffsd, lfsd, nl
#ifdef LA_BP
	     , (La_bytepos) nchars
#endif
	    );
    return nl;
}
