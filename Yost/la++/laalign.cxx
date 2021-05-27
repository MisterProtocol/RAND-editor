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

La_linepos
La_stream::la_align (
    Reg1 La_stream* olas
)
{
    if (!olas->la_verify ())
	return -1;
    if (olas->la_file != la_file) {
	la_errno = LA_NOTSAME;
	return -1;
    }

    move ((char *) olas, (char *) this,
	  (unsigned int) sizeof (struct la_spos));
    return la_lpos;
}
