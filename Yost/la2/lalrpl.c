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
