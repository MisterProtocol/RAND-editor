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

La_linesize
La_stream::la_lwsize ()
{
    Reg1 char *cp;
    Reg3 int fsdb;

    if (la_cfsd == la_file->la_lfsd)
	return 0;
    cp = &(la_cfsd->fsdbytes[la_fsbyte]);
    fsdb = *cp++;
    if (fsdb) {
#ifndef NOSIGNEDCHAR
	if (fsdb < 0)
	    return (-fsdb << LA_NLLINE) + *cp;
#else
	if (fsdb & LA_LLINE)
	    return (-(fsdb | LA_LLINE) << LA_NLLINE) + *cp;
#endif
	return fsdb;
    }

    {
	La_linesize speclength;

	move (&cp[1], (char *) &speclength, sizeof speclength);
	return speclength + *cp;
    }
}
