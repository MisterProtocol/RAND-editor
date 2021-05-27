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
la_lreplace (plas, buf, nchars, nlines, dlas)
Reg2 La_stream *plas;
char *buf;
int nchars;
Reg3 La_linepos *nlines;
La_stream *dlas;
{
    Reg1 La_linepos nins;

    if (nchars != 0) {
	if ((nins = la_linsert (plas, buf, nchars)) < 0)
	    return -1;
    } else
	nins = 0;

    if (*nlines == 0)
	return 0;

    {
	Reg4 La_linepos ndel;

	if ((ndel = la_ldelete (plas, *nlines, dlas)) == -1) {
	    if (nins > 0) {
		if (!la_stay (plas))
		    (void) la_lseek (plas, -nins, 1);
		(void) la_ldelete (plas, nins, (La_stream *) 0);
	    }
	    return -1;
	}
	*nlines = ndel;
	return ndel;
    }
}
