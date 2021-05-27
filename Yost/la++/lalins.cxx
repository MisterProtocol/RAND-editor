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
La_stream::la_linsert (
    Reg5 char *buf,
    Reg1 int nchars
) {
    Reg3 La_linepos nl;
    La_fsd *ffsd, *lfsd;        /* first and last fsds to be made for buf */

    if (   nchars < 0
	|| (   nchars > 0
	    && buf[nchars-1] != LA_NEWLINE
	   )
       ) {
	la_errno = nchars < 0 ? LA_NEGCOUNT : LA_NONEWL;
	return -1;
    }

    if (!la_zbreak ())
	return -1;

    if (nchars == 0)
	return 0;

    {
	Reg4 Ff_stream *ffs;
	Reg6 long fsdpos;

	ffs = la_chglas->la_file->la_ffs;
	fsdpos = ff_size (ffs);
	/*printf ("about to parse: fsdpos=%d, nchars=%d\n", fsdpos, nchars);*/
	if ((nl = la_parse ((Ff_stream *) 0, fsdpos, &ffsd, &lfsd,
			    la_chglas->la_file, (La_bytepos) nchars, buf))
	    < 0)
	    return -1;

	/* Don't grow beyond highest int */
	{
	    La_linepos lntmp;

	    lntmp = la_file->la_nlines;
	    if (lntmp + nl != la_ltrunc (lntmp + nl)) {
		la_errno = LA_ERRMAXL;
 err:           la_freefsd (ffsd);
		return -1;
	    }
	}

	ff_seek (ffs, fsdpos, 0);
	if (ff_write (ffs, buf, nchars) != nchars) {
	    la_errno = LA_WRTERR;
	    goto err;
	}
    }

    la_link (ffsd, lfsd, nl
#ifdef LA_BP
	     , (La_bytepos) nchars
#endif
	    );
    return nl;
}
