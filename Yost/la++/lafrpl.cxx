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

int
La_stream::la_freplace (
    char* filename
)
{
    Reg2 Ff_stream *tffs;

    if (!(tffs = ff_open (filename, 0, 0))) {
	la_errno = LA_NOOPN;
	return NO;
    }

    if (ff_close (la_file->la_ffs) < 0) {
	la_errno = LA_FFERR;
	return NO;
    }

    la_file->la_ffs = tffs;
    return YES;
}
