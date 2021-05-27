#include "lalocal.h"

#define min(a,b) ((a)<(b)?(a):(b))

La_linepos
la_blank (plas, nlines)
Reg4 La_stream *plas;
Reg5 La_linepos nlines;
{
    static char blnks[LA_FSDLMAX];
    static int blnks_done;
    static La_stream blas;
    static La_flag opened = NO;
    static La_flag inserted = NO;

    if (nlines <= 0)
	return 0;

    /* Don't grow plas beyond highest int */
    {
	Reg1 La_linepos cnt;

	cnt = plas->la_file->la_nlines;
	if (cnt + nlines != la_ltrunc (cnt + nlines)) {
	    la_errno = LA_ERRMAXL;
	    return 0;
	}
    }

    if (!inserted) {
	if (!opened) {
	    if (la_clone (la_chglas, &blas) == NULL)
		return 0;
	    la_stayset (&blas);
	    opened = YES;
	}
	if (!blnks_done) {
	    fill (blnks, (unsigned int) LA_FSDLMAX, LA_NEWLINE);
	    blnks_done = 1;
	}
	if (la_linsert (&blas, blnks, LA_FSDLMAX) != LA_FSDLMAX)
	    return 0;
	inserted = YES;
    }

    {
	Reg3 La_linepos cnt;

	cnt = 0;
	while (nlines > 0) {
	    Reg1 La_linepos nins;
	    Reg2 La_linepos incr;

	    incr = min (nlines, LA_FSDLMAX);
	    if ((nins = la_lcopy (&blas, plas, incr)) != incr) {
		if (nins > 0)
		    cnt += nins;
		(void) la_lseek (plas, -cnt, 1);
		(void) la_ldelete (plas, cnt, (La_stream *) 0);
		return 0;
	    }
	    cnt += nins;
	    nlines -= nins;
	}
	return cnt;
    }
}
