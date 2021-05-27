#include "lalocal.h"

La_linesize
la_lrsize (plas)
Reg2 La_stream *plas;
{
    Reg1 char *cp;
    Reg3 int fsdb;

    if (plas->la_cfsd == plas->la_file->la_lfsd)
	return 0;
    cp = &(plas->la_cfsd->fsdbytes[plas->la_fsbyte]);
    fsdb = *cp++;
    if (fsdb) {
#ifndef NOSIGNEDCHAR
	if (fsdb < 0)
	    return (-fsdb << LA_NLLINE) + *cp - plas->la_lbyte;
#else
	if (fsdb & LA_LLINE)
	    return (-(fsdb | LA_LLINE) << LA_NLLINE) + *cp - plas->la_lbyte;
#endif
	return fsdb - plas->la_lbyte;
    }

    {
	La_linesize speclength;

	my_move (&cp[1], (char *) &speclength, sizeof speclength);
	return speclength + *cp - plas->la_lbyte;
    }
}
