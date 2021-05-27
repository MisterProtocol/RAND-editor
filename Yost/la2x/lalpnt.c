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

/*
/* la_lpnt (plas, &buf)
/*
/* gets some or all of the rest of the current line into memory and returns
/* how many characters are pointed at.  Also returns the address of the
/* buffer in &buf if return is > 0.
/*
/**/
La_linesize
la_lpnt (plas, buf)
Reg2 La_stream *plas;
char **buf;
{
    Reg1 La_linesize nchars;
    Reg3 La_fsd *cfsd;
    static char newl[] = { LA_NEWLINE };

    if ((nchars = la_lrsize (plas)) <= 0)
	return 0;
    if (   (cfsd = plas->la_cfsd)->fsdbytes[0] == 0
	&& cfsd->fsdbytes[1] == 1
       ) {
	/* special fsd - no newline */
	if (nchars == 1) {
	    *buf = newl;
	    goto gotit;
	}
	nchars--;
    }

    if ((nchars = ff_point (cfsd->fsdfile->la_ffs,
			    cfsd->fsdpos + plas->la_ffpos, buf, (long)nchars))
	< 0) {
	la_errno = LA_READERR;
	return -1;
    }

 gotit:
    la_clseek (plas, nchars);
    return nchars;
}
