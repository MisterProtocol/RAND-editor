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

    move ((char *) &olas->la_pos, (char *) &nlas->la_pos,
	  (unsigned int) sizeof (struct la_pos));
    return nlas->la_pos.la_lpos;
}
