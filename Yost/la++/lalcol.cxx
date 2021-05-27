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

#define DORMANT    0
#define COLLECTING 1
static La_flag prevstate = DORMANT;
static La_bytepos chgpos;
static La_bytepos nxchgpos;

La_linepos
la_lcollect (
    int        state,     /* 1=start, 0=continue */
    Reg3 char* buf,
    Reg1 int   nchars
)
{
    Reg2 La_linepos errval;

    if (state)  
	nxchgpos = chgpos = ff_grow (la_chgffs);
    else if (prevstate == DORMANT) {
	errval = LA_BADCOLL;
	goto err;
    }
    prevstate = COLLECTING;
    if (nchars < 0) {
	errval = LA_NONEWL;
	goto err;
    }
    if (nxchgpos != ff_size (la_chgffs)) {
	errval = LA_BRKCOLL;
	goto err;
    }
    if (nchars > 0) {
	ff_seek (la_chgffs, nxchgpos, 0);
	if (ff_write (la_chgffs, buf, nchars) != nchars) {
	    errval = LA_WRTERR;
	    goto err;
	}
	nxchgpos += nchars;
    }
    return 0;


 err:
    prevstate = DORMANT;
    la_errno = errval;
    return -1;
}

int
la_tcollect (
    long pos
)
{
    if ((chgpos = pos) > (nxchgpos = ff_grow (la_chgffs))) {
	prevstate = DORMANT;
	return 0;
    }
    prevstate = COLLECTING;
    return 1;
}

/* VARARGS2 */
La_linepos
La_stream::la_lrcollect (
    Reg6 La_linepos* nlines,
    La_stream*       dlas
)
{
    Reg3 La_linepos nl;
    Reg4 La_linepos errval;
    Reg5 La_bytepos totchars;
    La_fsd *ffsd;               /* first fsd to be made for buf */
    La_fsd *lfsd;               /* last  fsd to be made for buf */

    if (prevstate == DORMANT) {
	errval = LA_BADCOLL;
	goto err;
    }
    if (!la_zbreak ())
	goto err1;
    /*printf ("about to parse: chgpos=%d, nchars=%d\n", chgpos, nchars);*/
    if ((totchars = nxchgpos - chgpos) < 0) {
	errval = LA_NEGCOUNT;
	prevstate = DORMANT;
	goto err;
    }
    if ((nl = la_parse (la_chgffs, chgpos, &ffsd, &lfsd,
			la_chglas->la_file, totchars, ""))
	< 0)
	goto err1;

    {
	Reg1 La_linepos nlsize;

	/* Don't grow beyond highest int */
	nlsize = la_file->la_nlines;
	if (nlsize + nl != la_ltrunc (nlsize + nl)) {
	    la_errno = LA_ERRMAXL;
	    la_freefsd (ffsd);
	    prevstate = DORMANT;
	    return -1;
	}

	if (*nlines > 0) {
	    La_linepos ndel;

	    if ((ndel = la_ldelete (*nlines, dlas)) == -1) {
		la_freefsd (ffsd);
		goto err1;
	    }
	    *nlines = ndel;
	}

	if (nl > 0)
	    la_link (ffsd, lfsd, nl
#ifdef LA_BP
		     , (La_bytepos) totchars
#endif
		    );
    }
    prevstate = DORMANT;
    return nl;

 err:
    la_errno = errval;
 err1:
    return -1;
}
