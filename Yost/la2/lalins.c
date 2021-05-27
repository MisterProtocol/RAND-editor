/*
	Copyright 1988, 1987, 1986 Grand Software, Inc. All Rights Reserved
	Copyright 1986, 1985 David Yost                 All Rights Reserved

	This file is proprietary to and a trade secret of Grand Software,
	Inc. (Los Angeles, CA, USA, Telephone +1 213-650-1089) and may be
	used only in accordance with the terms of the license agreement
	that accompanies the product.  An electronic copy of that license
	is provided in the file "AGREEMENT" in the main directory of the
	distribution.
 */


#include "lalocal.h"

La_linepos
la_linsert (plas, buf, nchars)
Reg2 La_stream *plas;
Reg5 char *buf;
Reg1 int nchars;
{
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

    if (!la_zbreak (plas))
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

	/* Don't grow plas beyond highest int */
	{
	    La_linepos lntmp;

	    lntmp = plas->la_file->la_nlines;
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

    la_link (plas, ffsd, lfsd, nl
#ifdef LA_BP
	     , (La_bytepos) nchars
#endif
	    );
    return nl;
}
