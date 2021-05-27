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

/*
/* La_stream::la_lget (buf, nchars)
/*
/* gets up to nchars characters from the La_stream and puts them
/* into the buffer pointed to by buf.  Input is terminated by a newline.
/*
/* returns the number of characters read.
/*
/**/
La_linesize
La_stream::la_lget (
    char *buf,
    Reg5 int nchars
) {
    Reg3 La_fsd    *cfsd;
    Reg1 int        j;
    Reg4 La_flag    nonewl;

    if (nchars <= 0) {
	la_errno = LA_NEGCOUNT;
	return -1;
    }
    if ((cfsd = la_cfsd) == la_file->la_lfsd)
	return 0;

    nonewl = NO;
    if ((j = la_lrsize ()) > nchars)
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
		 (long) (cfsd->fsdpos + la_ffpos), 0);
	if (ff_read (ffn, buf, j, 0) != j) {    /* yes, read directly */
	    la_errno = LA_READERR;
	    return -1;
	}
    }
    if (nonewl)
	j++;
    if (j)
	la_clseek ((La_linesize) j);
    return j;       /* return # bytes read */
}
