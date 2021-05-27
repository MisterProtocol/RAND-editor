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

La_stream*
La_stream::la_other ()
{
    Reg1 La_stream *tlas;
    Reg3 La_file *nlaf;

    if (la_file->la_refs > 1) {
	nlaf = la_file;
	for (tlas = nlaf->la_fstream; tlas; tlas = tlas->la_fforw)
	    if (tlas != this)
		return tlas;
#ifdef LA_DEBUG
	{
	    static char errbuf[]
		= "la_other can't find stream. Refs = %000000\n";
	    extern char *rindex ();

	    sprintf (rindex (errbuf, '%'), "%d\n", la_file->la_refs);
	    la_abort (errbuf);
	}
#endif
    }
    return (La_stream *) 0;
}
