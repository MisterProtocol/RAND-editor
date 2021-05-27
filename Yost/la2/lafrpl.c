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

la_freplace (filename, plas)
char *filename;
Reg1 La_stream *plas;
{
    Reg2 Ff_stream *tffs;

    if (!(tffs = ff_open (filename, 0, 0))) {
	la_errno = LA_NOOPN;
	return NO;
    }

    if (ff_close (plas->la_file->la_ffs) < 0) {
	la_errno = LA_FFERR;
	return NO;
    }

    plas->la_file->la_ffs = tffs;
    return YES;
}
