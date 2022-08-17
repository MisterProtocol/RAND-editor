#include "lalocal.h"

#include "la_prototypes.h"

La_linesize
la_clseek (plas, nchars)
Reg1 La_stream *plas;
Reg2 La_linesize nchars;
{
    Reg4 La_linesize j;

    if (nchars < 0) {
	if (plas->la_lbyte == 0) {
	    Reg4 La_linesize lsize;

	    /*  set position to the end of the previous line. */
	    (void) la_lseek (plas, (La_linepos) -1, 1);
	    /* then back up from there */
	    lsize = la_lwsize (plas);
	    if (nchars < -lsize)
		nchars = -lsize;
	    plas->la_ffpos += plas->la_lbyte = lsize + nchars;
#ifdef  LA_BP
	    plas->la_bpos += lsize;
#endif
	} else {
	    /* back no farther than beginning of line */
	    plas->la_lbyte += nchars;
	    plas->la_ffpos += nchars;
	}
    } else if (nchars >= (j = la_lrsize (plas)) || nchars == 0) {
	/* must advance pointer to next line */
	Reg3 La_fsd *cfsd;

	nchars = j;
	cfsd = plas->la_cfsd;
	plas->la_lpos++;
	plas->la_lbyte = 0;
	if (++plas->la_fsline >= cfsd->fsdnlines) {  /* end of this fsd */
	    plas->la_fsline = 0;
	    plas->la_cfsd = cfsd->fsdforw;
	    plas->la_fsbyte = 0;
	    plas->la_ffpos = 0;
	} else {
	    /* can't be a special fsd here */
#ifndef NOSIGNEDCHAR
#ifndef XENIX_86_BUG3
	    if (cfsd->fsdbytes[plas->la_fsbyte++] < 0)
#else/* XENIX_86_BUG3 */
	    char tmp = cfsd->fsdbytes[plas->la_fsbyte];

	    plas->la_fsbyte++;
	    if (tmp < 0)
#endif/*XENIX_86_BUG3 */
#else/* NOSIGNEDCHAR */
	    if (cfsd->fsdbytes[plas->la_fsbyte++] & LA_LLINE)
#endif/*NOSIGNEDCHAR */
		plas->la_fsbyte++;
	    plas->la_ffpos += nchars;
	}
    } else {
	/* not going past this line */
	plas->la_lbyte += nchars;
	plas->la_ffpos += nchars;
    }
#ifdef  LA_BP
    plas->la_bpos += nchars;
#endif
    return nchars;
}
