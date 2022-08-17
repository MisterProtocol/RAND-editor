#include "lalocal.h"

#include "la_prototypes.h"

La_linesize
la_lrsize (plas)
Reg2 La_stream *plas;
{
    Reg1 char *cp;
    Reg3 int fsdb;

/* dbg
int lnum = (int) plas->la_fsbyte;
La_linesize  linenumber = la_linepos(plas);
*/

/*
int fsdnlines = plas->la_cfsd->fsdnlines;
long fsdnbytes = plas->la_cfsd->fsdnbytes;
*/

    if (plas->la_cfsd == plas->la_file->la_lfsd)
	return 0;
    cp = &(plas->la_cfsd->fsdbytes[plas->la_fsbyte]);

    fsdb = *cp++;

/*
char *cp0 = &(plas->la_cfsd->fsdbytes[0]);
dbgpr("la_lrsize, TOP:  fsdb=(%d) lnum=(%d) plas->(la_fsbyte=(%ld) refs=%d la_nlines=%ld)\n",
  fsdb, lnum, plas->la_fsbyte, plas->la_file->la_refs,
  plas->la_file->la_nlines);
dbgpr("   cfsb=(%p) cp-cp0 = %d, linenumber=%ld, plas->la_cfsd->fsdbytes=(%ld)\n",
plas->la_cfsd, (int) (cp-cp0), linenumber, plas->la_cfsd->fsdbytes+1);
*/

/*
dbgpr("la_lrsize, fsdnlines=%d fsdnbytes=%ld\n",
  fsdnlines, fsdnbytes);
*/
    if (fsdb) {
#ifndef NOSIGNEDCHAR
	La_linesize n;

	if (fsdb < 0) {
	    /* return (-fsdb << LA_NLLINE) + *cp - plas->la_lbyte;*/
	    n = (-fsdb << LA_NLLINE) + *cp - plas->la_lbyte;
	/*  dbgpr("la_lrsize1:  n=%ld vs fsdb=%d *cp=(%d) la_lbyte=(%ld)\n",
	       n, fsdb, *cp, plas->la_lbyte); */
	    return n;
	}
#else
	if (fsdb & LA_LLINE) {
	/*  return (-(fsdb | LA_LLINE) << LA_NLLINE) + *cp - plas->la_lbyte; */
	    n = (-(fsdb | LA_LLINE) << LA_NLLINE) + *cp - plas->la_lbyte;
	 /* dbgpr("la_lrsize2:  n=%ld\n", n); */
	    return n;
	}
#endif
    /*  return fsdb - plas->la_lbyte; */
	n = fsdb - plas->la_lbyte;
/*      dbgpr("la_lrsize3:  n=%ld vs fsdb=(%d) la_lbyte=(%ld)\n", n, fsdb, plas->la_lbyte); */

	return n;
    }

    {
	La_linelength speclength = 0;
	La_linesize n = 0;

	my_move (&cp[1], (char *) &speclength, sizeof speclength);

	/*return speclength + *cp - plas->la_lbyte;*/

    /*  dbgpr("la_lrsize4a, after my_move:  speclength=%ld, cp[0]=(%d) cp[1]=(%d) cp[2]=(%d) cp[3]=(%d)\n",
	   speclength, cp[0], cp[1], cp[2], cp[3]); */

    /*  n = speclength + *cp - plas->la_lbyte; */
    /*  n = speclength + (unsigned int)*cp - plas->la_lbyte; */
	long lcp = *cp;
	n = speclength + (unsigned int)(lcp - plas->la_lbyte);
/*
dbgpr("la_lrsize4b:  n=%ld, speclength=%ld fsdb=%d, cp=(%d) la_lbyte=%ld la_fsbyte=%ld\n",
  n, speclength, fsdb, *cp, plas->la_lbyte, plas->la_fsbyte);
*/
	return n;
    }
}

/* Notes/examples on the en/decoding of fsd->fsdbytes[0]

examples:
Case 1, cp[0] < 1:

cp[0] is -1, cp[1] is 36, then the line length is 164
    164 = 1 << 7+ 36

cp[0] is -3, cp[1]=18 the line length is 402
   402 = 3 << 7 + 18

Case 2, cp[0] > 0:

fsdb > 0 for line length values < 127:
    n=90 fsdb=(90) la_lbyte=(0)

Case 3, cp[0] == 0:

cp[0] == 0 for lengths > 32767
my_move (&cp[1], (char *) &speclength, sizeof speclength);
	La_linelength speclength = 0;

*/
