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
