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

La_linesize
la_clseek (plas, nchars)
Reg1 La_stream *plas;
Reg2 La_linesize nchars;
{
    Reg4 La_linesize j;

    if (nchars < 0) {
	if (plas->la_lbyte == 0) {
	    Reg4 La_linesize lsize;

	    /*  set position to the end of the previous line. */
	    (void) la_lseek (plas, (La_linepos) -1, 1);
	    /* then back up from there */
	    lsize = la_lwsize (plas);
	    if (nchars < -lsize)
		nchars = -lsize;
	    plas->la_ffpos += plas->la_lbyte = lsize + nchars;
#ifdef  LA_BP
	    plas->la_bpos += lsize;
#endif
	} else {
	    /* back no farther than beginning of line */
	    plas->la_lbyte += nchars;
	    plas->la_ffpos += nchars;
	}
    } else if (nchars >= (j = la_lrsize (plas)) || nchars == 0) {
	/* must advance pointer to next line */
	Reg3 La_fsd *cfsd;

	nchars = j;
	cfsd = plas->la_cfsd;
	plas->la_lpos++;
	plas->la_lbyte = 0;
	if (++plas->la_fsline >= cfsd->fsdnlines) {  /* end of this fsd */
	    plas->la_fsline = 0;
	    plas->la_cfsd = cfsd->fsdforw;
	    plas->la_fsbyte = 0;
	    plas->la_ffpos = 0;
	} else {
	    /* can't be a special fsd here */
#ifndef NOSIGNEDCHAR
	    if (cfsd->fsdbytes[plas->la_fsbyte++] < 0)
#else
	    if (cfsd->fsdbytes[plas->la_fsbyte++] & LA_LLINE)
#endif
		plas->la_fsbyte++;
	    plas->la_ffpos += nchars;
	}
    } else {
	/* not going past this line */
	plas->la_lbyte += nchars;
	plas->la_ffpos += nchars;
    }
#ifdef  LA_BP
    plas->la_bpos += nchars;
#endif
    return nchars;
}
