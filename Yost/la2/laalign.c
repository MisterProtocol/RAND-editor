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
la_align (olas, nlas)
Reg1 La_stream *olas;
Reg2 La_stream *nlas;
{
    if (!la_verify (olas))
	return -1;
    if (olas->la_file != nlas->la_file) {
	la_errno = LA_NOTSAME;
	return -1;
    }

    STRUCT_COPY (&olas->la_spos, &nlas->la_spos);

    return nlas->la_lpos;
}
