#include "lalocal.h"

#include "la_prototypes.h"

/*
 * la_lget (plas, buf, nchars)
 *
 * gets up to nchars characters from the La_stream plas and puts them
 * into the buffer pointed to by buf.  Input is terminated by a newline.
 *
 * returns the number of characters read.
 *
 **/
La_linesize
la_lget (plas, buf, nchars)
Reg2 La_stream *plas;
char *buf;
Reg5 int nchars;
{
    Reg3 La_fsd    *cfsd;
    Reg1 int        j;
    Reg4 La_flag    nonewl;

    if (nchars <= 0) {
	la_errno = LA_NEGCOUNT;
	return (-1);
    }
    if ((cfsd = plas->la_cfsd) == plas->la_file->la_lfsd)
	return (0);

    nonewl = NO;
    if ((j = (int) la_lrsize (plas)) > nchars) {
    /*  dbgpr("la_lget:  la_rsize=%d, setting j to %d\n", j, nchars); */
	j = nchars;
    }
    else if (   cfsd->fsdbytes[0] == 0
	     && cfsd->fsdbytes[1] == 1
	    ) {
	/* special fsd - no newline */
	buf[--j] = LA_NEWLINE;
	nonewl = YES;
    }

/*
dbgpr("la_lget:j=%d, nchars=%d fsdbytes[0]=(%d), fsdbytes[1]=(%d)\n",
  j, nchars, cfsd->fsdbytes[0], cfsd->fsdbytes[1]);
*/

    if (j > 0) {
	Reg6 Ff_stream *ffn;

	ff_seek (ffn = cfsd->fsdfile->la_ffs,
		 (long) (cfsd->fsdpos + plas->la_ffpos), 0);
/*      if (ff_read (ffn, buf, j, 0) != j) {  */  /* yes, read directly */
	int rc;
	if ((rc = ff_read (ffn, buf, j, 0, NULL)) != j) {    /* yes, read directly */
	    la_errno = LA_READERR;
/*
dbgpr("la_lget returning -1: rc=%d from ff_read, seekpos=%ld, returning -1., j=%d nchars=%d\n",
    rc, cfsd->fsdpos + plas->la_ffpos, j, nchars);
*/
	    return (-1);
	}
    }
    if (nonewl)
	j++;
    if (j)
	la_clseek (plas, (La_linesize) j);
    return (j);       /* return # bytes read */
}
