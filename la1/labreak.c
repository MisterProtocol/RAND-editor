#include "lalocal.h"
#include <stdlib.h>

#include "la_prototypes.h"

#ifdef GCC
extern void	*malloc ();
#else
extern char	*malloc ();
#endif /*GCC*/
/* simply break the chain at the current position. */
/* shorthand for la_break of 0 lines */
La_flag
la_zbreak (plas)
La_stream *plas;
{
    La_fsd    *d1fsd, *d2fsd;
    static La_linepos lzero;
#ifdef LA_BP
    static La_bytepos bzero;
#endif

    return la_break (plas, BRK_REAL, &d1fsd, &d2fsd, &lzero
#ifdef LA_BP
	    , &bzero
#endif
	    );
}

#define REALLOC         /* use realloc() -- more efficient & more code */
#ifdef	REALLOC
/* extern char *realloc (); */
#endif/*REALLOC */

#ifdef COMMENT
    La_break puts one or two breaks in an fsd chain so that chain insertions
    or deletions can be made at those points.  La_linsert needs to make a
    single break in the chain so that it can insert a new fsd chain into the
    file.  La_ldelete needs to make two breaks, one at the beginning of the
    lines to be deleted and one at the end.  La_lcopy also calls la_break,
    but it doesnot really want to break the fsds in the chain.  It needs to
    make a replica of part of the chain, but the start and/or end points of
    this part of the chain may be in the middle of fsds.  So it gets its new
    first and last fsds from la_break if they will be required, because
    la_break is expert at making such new fsds.

    In the two-break case, it is possible that both breaks will be
    within the same fsd.  We do both of them in the same call because
    it is  more efficient.

    What la_break is to do is specified by the mode argument:
	BRK_REAL - break the chain in one or two places if *pnlines is 0 or
		   > 0 respectively.  Return pointer to the fsd after
		   the second break in *lfsd.  (If *pnlines == 0, there is
                   no second break, so the pointer after *the* break.)
	BRK_COPY - donot break the chain, but make new beginning and
		   ending fsds (if those points are in mid-fsd) for what
		   will eventually be a copy of the selected range in the
		   file.  *pnlines will always be > 0.
		   Return pointers to the fsds in *ffsd and *lfsd.

    If both breaks are not within the same fsd, la_break calls itself
    once for the first break and again for the second break.  When this
    happens in BRK_COPY mode, the mode argument is set to BRK_AGAIN when
    la_break calls itself to do the second break of a BRK_COPY.

    Return YES on success, NO on failure for lack of memory.
#endif
La_flag
la_break (plas, mode, ffsd, lfsd, pnlines
#ifdef LA_BP
    , nbytes
#endif
    )
La_stream *plas;
Reg6 int mode;
La_fsd **ffsd;  /* returned value of pointer to first fsd in new chain */
La_fsd **lfsd;  /* returned value of pointer to last  fsd in new chain */
La_linepos *pnlines;
#ifdef LA_BP
La_bytepos *nbytes;
#endif
{
    Reg3 La_fsd *cfsd;  /* fast pointer to current fsd */
    Reg5 La_linepos nlines;     /* fast copy of *pnlines */
    La_fsd *n1fsd;      /* new fsd 1 of 3 */
    La_fsd *n2fsd;      /* new fsd 2 of 3 */
    La_fsd *n3fsd;      /* new fsd 3 of 3 */
    La_file *claf;      /* fast pointer to current la_file */
    long /*short*/ fsline;       /* fast copy of current la_fsline */
    char *cp2;          /* pointer to middle portion fsdbyte */
    char *cp3;          /* pointer to third  portion fsdbyte */
    unsigned int /*short*/ fsbyte1;      /* num of fsdbytes in new fsd 1 of 3 */
    unsigned int /*short*/ fsbyte2;      /* num of fsdbytes in new fsd 2 of 3 */
    unsigned int /*short*/ fsbyte3;      /* num of fsdbytes in new fsd 3 of 3 */
    long /*short*/ ffpos;        /* fast copy of current la_ffpos */
    static La_linepos lzero;    /* always 0 */
#ifdef LA_BP
    static La_bytepos bzero;    /* always 0 */
#endif
    long /*short*/ nbytes2;      /* number of bytes described by  middle fsd portion */

    /* see if we are at the end of the chain */
    if ((cfsd = plas->la_cfsd) == plas->la_file->la_lfsd) {
	*pnlines = 0;
 noneed:
	/*printf ("Break - none at line %d\n", plas->la_lpos); **/
    /*  dbgpr("la_break: Break - none at line %ld, at end of chain??\n", plas->la_lpos); */
	/*la_sdump (plas, ""); **/
#ifdef DBG_DEBUG
	/*  dbgpr_la_sdump(plas, ""); */
#endif
#ifdef LA_BP
	*nbytes = 0;
#endif
	if (mode != BRK_REAL) {
	    *ffsd = (La_fsd *) 0;
	    *lfsd = (La_fsd *) 0;
	} else
	    *lfsd = cfsd;
	return (YES);   /* no need to break */
    }

    /* make sure we're at the beginning of the line */
#ifdef LA_BP
    plas->la_bpos -= plas->la_lbyte;
#endif
    ffpos = plas->la_ffpos -= plas->la_lbyte;
    plas->la_lbyte = 0;

    /*  If there are 2 breaks, not in the same fsd, then we will
     *  have to call ourselves once each for beginning and end.
     */
    fsline = plas->la_fsline;
    if ((nlines = *pnlines) == 0) {
	if (fsline == 0)
	    /* at beginning of fsd */
	    goto noneed;
	else {
	    /* break cfsd into 2 new fsds */
	    /*printf ("Break in 2 at line %d\n", plas->la_lpos); **/
	/*  dbgpr("Break cfsd into 2 new fsds at line %d\n", plas->la_lpos); */
	}
    } else if (   fsline > 0
	       && fsline + nlines < cfsd->fsdnlines
	      ) {
	/* break cfsd into 3 new fsds */
	/*printf ("Break in 3 at line %d\n", plas->la_lpos); **/
	/* dbgpr("Break cfsd into 3 new fsds at line %d\n", plas->la_lpos); */
    } else {
	/* can't do the whole thing within current fsd */
	/*printf ("Break twice at line %d\n", plas->la_lpos); **/
    /*  dbgpr("Break twice at line %d\n", plas->la_lpos); */
	/*la_sdump (plas, ""); **/
#ifdef DBG_DEBUG
	dbgpr_la_sdump (plas, "");
#endif
	Reg1 La_fsd *tfsd;
	La_stream nlas;

	if (!la_break (plas, mode, ffsd, lfsd, &lzero
#ifdef LA_BP
		      , &bzero
#endif
	   ))
	    goto bad1;

	if (la_clone (plas, &nlas) == (La_stream *) 0)
	    goto bad1;
	/*la_sdump (&nlas, ""); **/
#ifdef DBG_DEBUG
	dbgpr_la_sdump (&nlas, "");
#endif
	/*  seek to end of area to be deleted, break fsd */
	(void) la_lseek (&nlas, nlines, 1);
	if (mode == BRK_COPY)
	    tfsd = *ffsd;
	if (!la_break (&nlas, mode == BRK_COPY ? BRK_AGAIN : BRK_REAL, ffsd,
	    lfsd, &lzero
#ifdef LA_BP
		  , &bzero
#endif
	   )) {
	    if (mode == BRK_COPY)
		la_freefsd (tfsd);
	    (void) la_close (&nlas);
	    goto bad1;
	}

	*pnlines = nlas.la_lpos - plas->la_lpos;
#ifdef LA_BP
	*nbytes = nlas.la_bpos - plas->la_bpos;
#endif
	(void) la_close (&nlas);
	if (mode == BRK_COPY)
	    *ffsd = tfsd;
	/* *lfsd set as a result of the second labreak call above */
	return (YES);
    }
    /*la_sdump (plas, ""); **/
#ifdef DBG_DEBUG
    dbgpr_la_sdump (plas, "");
#endif

    /* figure how many fsdbytes in each of the 2 (possibly 3) new fsds */
    fsbyte1 = (unsigned int) plas->la_fsbyte;
    {
	Reg1 int /*short*/ nb2;
	Reg2 char *cp1;
	Reg4 int /*short*/ j;

	cp1 = &cfsd->fsdbytes[fsbyte1];
	nb2 = 0;
	if (nlines) {
	    /* can't possibly be a special fsd */
	    cp2 = cp1;
	    for (j = (int)nlines; j--;) {
#ifndef NOSIGNEDCHAR
		if (*cp1 < 0)
		    nb2 += -(*cp1++) << LA_NLLINE;
#else
		if (*cp1 & LA_LLINE)
		    nb2 += - (*cp1++ | LA_LLINE) << LA_NLLINE;
#endif
		nb2 += *cp1++;
	    }
	    fsbyte2 = (unsigned int) (cp1 - cp2);
	} else
	    fsbyte2 = 0;
	cp3 = cp1;
	for (j = (int) (cfsd->fsdnlines - (fsline + nlines)); j--;)
#ifndef NOSIGNEDCHAR
	    if (*cp1++ < 0)
		cp1++;
#else
	    if (*cp1++ & LA_LLINE)
		cp1++;
#endif
	fsbyte3 = (unsigned int) (cp1 - cp3);
	nbytes2 = nb2;
    }

    /* alloc the 2 (possibly 3) new fsds and fill them in */
    /*printf ("size1=%d size3=%d\n", LA_FSDSIZE+fsbyte1, LA_FSDSIZE+fsbyte3); **/
/*
dbgpr("  fsbyte1=%d fsbyte3=%d size1=%ld size3=%ld\n",
    fsbyte1, fsbyte2, (long)LA_FSDSIZE+fsbyte1, (long)LA_FSDSIZE+fsbyte3);
*/

    claf = cfsd->fsdfile;
#ifdef  REALLOC
    if (mode == BRK_AGAIN) {
#else/* REALLOC */
    if (mode != BRK_COPY) {
#endif/*REALLOC */
	Reg1 La_fsd *tfsd;

	if ((n1fsd = tfsd =
	    (La_fsd *) malloc ((unsigned int) (LA_FSDSIZE + fsbyte1)))
	    == NULL) {
 bad1:      la_errno = LA_NOMEM;
	    return (NO);
	}
	tfsd->fsdnbytes = ffpos;
	tfsd->fsdnlines = (char) fsline;
	tfsd->fsdfile   = claf;
	tfsd->fsdpos    = cfsd->fsdpos;
	my_move (cfsd->fsdbytes, tfsd->fsdbytes, fsbyte1);
    }
    if (mode == BRK_REAL || (mode == BRK_COPY && nlines == 0)) {
	Reg1 La_fsd *tfsd;

	if ((n3fsd = tfsd =
	    (La_fsd *) malloc ((unsigned int) (LA_FSDSIZE + fsbyte3)))
	    == NULL) {
 bad2:
#ifndef REALLOC
	    free ((char *) n1fsd);
#endif/*REALLOC */
	    goto bad1;
	}
	tfsd->fsdnbytes = cfsd->fsdnbytes - ffpos - nbytes2;
	tfsd->fsdnlines = (char) (cfsd->fsdnlines - fsline - nlines);
	tfsd->fsdfile = claf;
	tfsd->fsdpos = cfsd->fsdpos + ffpos + nbytes2;
	my_move (cp3, tfsd->fsdbytes, fsbyte3);
    }
    if (nlines) {
	Reg1 La_fsd *tfsd;

	if ((n2fsd = tfsd =
	    (La_fsd *) malloc ((unsigned int) (LA_FSDSIZE + fsbyte2)))
	    == NULL) {
	    free ((char *) n3fsd);
	    goto bad2;
	}
	tfsd->fsdnbytes = nbytes2;
	tfsd->fsdnlines = (char) nlines;
	tfsd->fsdfile = claf;
	tfsd->fsdpos = cfsd->fsdpos + ffpos;
	my_move (cp2, tfsd->fsdbytes, fsbyte2);
    }
#ifdef  REALLOC
    else
	n2fsd = (La_fsd *) 0;
#endif/*REALLOC */

    if (mode == BRK_REAL) {
#ifdef  REALLOC


	if ((n1fsd = (La_fsd *) realloc ((char *) cfsd,
		    (unsigned int) (LA_FSDSIZE + fsbyte1))) == NULL) {
	    if (n2fsd)
		free ((char *) n2fsd);
	    goto bad1;
	}
	n1fsd->fsdnbytes = ffpos;
	n1fsd->fsdnlines = (char) fsline;
#else/* REALLOC */
	n1fsd->fsdback = cfsd->fsdback;
#endif/*REALLOC */
	/* link in the new fsds */
	if (n1fsd->fsdback)
	    n1fsd->fsdback->fsdforw = n1fsd;
	else
	    plas->la_file->la_ffsd = n1fsd;
#ifdef  REALLOC
	n1fsd->fsdforw->fsdback = n3fsd;
	n3fsd->fsdforw = n1fsd->fsdforw;
#else/* REALLOC */
	cfsd->fsdforw->fsdback = n3fsd;
	n3fsd->fsdforw = cfsd->fsdforw;
#endif/*REALLOC */
	if (nlines) {
	    n2fsd->fsdback = n1fsd;
	    n1fsd->fsdforw = n2fsd;
	    n3fsd->fsdback = n2fsd;
	    n2fsd->fsdforw = n3fsd;
	} else {
	    n3fsd->fsdback = n1fsd;
	    n1fsd->fsdforw = n3fsd;
	}

	{
	    /* fix up any streams whose current fsd was the one we are breaking */
	    Reg2 La_stream *tlas;
	    Reg4 long /*short*/ fsln;

	    fsln = fsline;
	    for (tlas = la_firststream; tlas; tlas = tlas->la_sforw) {
		if (tlas->la_cfsd == cfsd) {
		    if (tlas->la_fsline < fsln)
			tlas->la_cfsd = n1fsd;
		    else {
			Reg1 int /*short*/ j;

			if (tlas->la_fsline < (j = (int) (fsln + nlines))) {
			    tlas->la_cfsd = n2fsd;
			    tlas->la_fsline -= fsln;
			    tlas->la_fsbyte -= fsbyte1;
			    tlas->la_ffpos  -= ffpos;
			} else {
			    tlas->la_cfsd = n3fsd;
			    tlas->la_fsline -= j;
			    tlas->la_fsbyte -= fsbyte1 + fsbyte2;
			    tlas->la_ffpos  -= ffpos + nbytes2;
			}
		    }
		}
	    }
	}
	claf->la_fsrefs += nlines ? 2 : 1;
#ifndef REALLOC
	free ((char *) cfsd);      /* free up original fsd */
#endif/*REALLOC */
	*lfsd = n3fsd;
    } else {
	if (nlines) {
	    *ffsd = n2fsd;
	    *lfsd = (La_fsd *) 0;
	} else {
	    *ffsd = n3fsd;  /* for mode == BRK_COPY */
	    *lfsd = n1fsd;  /* for mode == BRK_AGAIN */
	}
	claf->la_fsrefs++;
    }
#ifdef LA_BP
    *nbytes = nlines ? nbytes2 : 0;
#endif
    /*la_sdump (plas, ""); **/

#ifdef DBG_DEBUG
    dbgpr_la_sdump (plas, "");
#endif

    return (YES);
}
