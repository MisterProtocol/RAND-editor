#include "lalocal.h"

#include "la_prototypes.h"

La_linesize
la_lwsize (plas)
Reg2 La_stream *plas;
{
    Reg1 char *cp;
    Reg3 int fsdb;

    if (plas->la_cfsd == plas->la_file->la_lfsd)
	return 0;
    cp = &(plas->la_cfsd->fsdbytes[plas->la_fsbyte]);
    fsdb = *cp++;

/*dbug*/
/*
int lnum = (int) plas->la_fsbyte;
dbgpr("la_lwsize, TOP:  fsdb=(%d) lnum=(%d) plas->(la_fsbyte=(%ld) refs=%d la_nlines=%ld)\n",
  fsdb, lnum, plas->la_fsbyte, plas->la_file->la_refs,
  plas->la_file->la_nlines);
*/

    if (fsdb) {
#ifndef NOSIGNEDCHAR
	if (fsdb < 0) {
	    La_linesize n;
	    n = (-fsdb << LA_NLLINE) + *cp;
	    /*return (-fsdb << LA_NLLINE) + *cp;*/
	    // dbgpr("la_lwsize1:  n=%ld vs fsdb=%d cp=(%d) la_lbyte=(%ld)\n",
	    //   n, fsdb, *cp, plas->la_lbyte);
	    return n;
	}
#else
	if (fsdb & LA_LLINE)
	    return (-(fsdb | LA_LLINE) << LA_NLLINE) + *cp;
#endif
	return fsdb;
    }

    {
     /* La_linesize speclength = 0; */
	La_linelength speclength = 0;
	La_linesize n = 0;

	my_move (&cp[1], (char *) &speclength, sizeof speclength);
	/* return speclength + *cp; */

	n = speclength + (unsigned char)*cp;
/*
	dbgpr("la_lwsize4b:  n=%ld, speclength=%ld fsdb=%d, cp=(%d) la_lbyte=%ld la_fsbyte=%ld\n",
	  n, speclength, fsdb, *cp, plas->la_lbyte, plas->la_fsbyte);
*/
	return n;
    }
}
