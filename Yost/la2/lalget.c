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
/* la_lget (plas, buf, nchars)
/*
/* gets up to nchars characters from the La_stream plas and puts them
/* into the buffer pointed to by buf.  Input is terminated by a newline.
/*
/* returns the number of characters read.
/*
/**/
La_linesize
la_lget (plas, buf, nchars)
Reg2 La_stream *plas;
char *buf;
Reg5 int nchars;
{
    Reg3 La_fsd    *cfsd;
    Reg1 int        j;
    Reg4 La_flag    nonewl;

    if (nchars <= 0) {
	la_errno = LA_NEGCOUNT;
	return -1;
    }
    if ((cfsd = plas->la_cfsd) == plas->la_file->la_lfsd)
	return 0;

    nonewl = NO;
    if ((j = la_lrsize (plas)) > nchars)
	j = nchars;
    else if (   cfsd->fsdbytes[0] == 0
	     && cfsd->fsdbytes[1] == 1
	    ) {
	/* special fsd - no newline */
	buf[--j] = LA_NEWLINE;
	nonewl = YES;
    }
    if (j > 0) {
	Reg6 Ff_stream *ffn;

	ff_seek (ffn = cfsd->fsdfile->la_ffs,
		 (long) (cfsd->fsdpos + plas->la_ffpos), 0);
	if (ff_read (ffn, buf, j, 0) != j) {    /* yes, read directly */
	    la_errno = LA_READERR;
	    return -1;
	}
    }
    if (nonewl)
	j++;
    if (j)
	la_clseek (plas, (La_linesize) j);
    return j;       /* return # bytes read */
}
