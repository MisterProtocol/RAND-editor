/*
	Copyright (c) 1985, 1986 David Yost     All Rights Reserved
	Copyright (c) 1986, 1987 Grand Software, Inc. All Rights Reserved
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

La_linesize
La_stream::la_clseek (
    Reg2 La_linesize nchars
)
{
    Reg4 La_linesize j;

    if (nchars < 0) {
	if (la_lbyte == 0) {
	    Reg4 La_linesize lsize;

	    /*  set position to the end of the previous line. */
	    (void) la_lseek ((La_linepos) -1, 1);
	    /* then back up from there */
	    lsize = la_lwsize ();
	    if (nchars < -lsize)
		nchars = -lsize;
	    la_ffpos += la_lbyte = lsize + nchars;
#ifdef  LA_BP
	    la_bpos += lsize;
#endif
	} else {
	    /* back no farther than beginning of line */
	    la_lbyte += nchars;
	    la_ffpos += nchars;
	}
    } else if (nchars >= (j = la_lrsize ()) || nchars == 0) {
	/* must advance pointer to next line */
	Reg3 La_fsd *cfsd;

	nchars = j;
	cfsd = la_cfsd;
	la_lpos++;
	la_lbyte = 0;
	if (++la_fsline >= cfsd->fsdnlines) {  /* end of this fsd */
	    la_fsline = 0;
	    la_cfsd = cfsd->fsdforw;
	    la_fsbyte = 0;
	    la_ffpos = 0;
	} else {
	    /* can't be a special fsd here */
#ifndef NOSIGNEDCHAR
#ifndef XENIX_86_BUG3
	    if (cfsd->fsdbytes[la_fsbyte++] < 0)
#else/* XENIX_86_BUG3 */
	    char tmp = cfsd->fsdbytes[la_fsbyte];

	    la_fsbyte++;
	    if (tmp < 0)
#endif/*XENIX_86_BUG3 */
#else/* NOSIGNEDCHAR */
	    if (cfsd->fsdbytes[la_fsbyte++] & LA_LLINE)
#endif/*NOSIGNEDCHAR */
		la_fsbyte++;
	    la_ffpos += nchars;
	}
    } else {
	/* not going past this line */
	la_lbyte += nchars;
	la_ffpos += nchars;
    }
#ifdef  LA_BP
    la_bpos += nchars;
#endif
    return nchars;
}
