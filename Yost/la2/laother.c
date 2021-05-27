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

La_stream *
la_other (plas)
Reg2 La_stream *plas;
{
    Reg1 La_stream *tlas;
    Reg3 La_file *nlaf;

    if (plas->la_file->la_refs > 1) {
	nlaf = plas->la_file;
	for (tlas = nlaf->la_fstream; tlas; tlas = tlas->la_fforw)
	    if (tlas != plas)
		return tlas;
#ifdef LA_DEBUG
	{
	    static char errbuf[]
		= "la_other can't find stream. Refs = %000000\n";
	    extern char *rindex ();

	    sprintf (rindex (errbuf, '%'), "%d\n", plas->la_file->la_refs);
	    la_abort (errbuf);
	}
#endif
    }
    return (La_stream *) 0;
}
