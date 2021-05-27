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

#ifndef LA_BP
La_bytepos
la_bsize (plas)
La_stream *plas;
{
    Reg3 La_fsd *tfsd;
    Reg4 La_bytepos nbytes;
    Reg5 La_linepos nlines;

    nbytes = 0;
    tfsd = plas->la_file->la_ffsd;
    for (nlines = plas->la_file->la_nlines; nlines > 0; ) {
	Reg1 char *cp;
	Reg2 int j;

	nlines -= j = tfsd->fsdnlines;
	if (*(cp = tfsd->fsdbytes)) {
	    while (j--) {
#ifndef NOSIGNEDCHAR
		if (*cp < 0)
		    nbytes += -(*cp++) << LA_NLLINE;
#else
		if (*cp & LA_LLINE)
		    nbytes += - (*cp++ | LA_LLINE) << LA_NLLINE;
#endif
		nbytes += *cp++;
	    }
	} else {
	    La_linesize speclength;

	    if (*++cp)
		nbytes++;
	    move (&cp[1], (char *) &speclength,
		  (unsigned int) sizeof speclength);
	    nbytes += speclength;
	}
	if (!(tfsd = tfsd->fsdforw))
	    la_abort ("fsdforw problem in la_bsize");
    }
    return nbytes;
}
#endif
