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


#ifdef COMMENT

 la_lseek - seek by line number

 type = 0  absolute line number
	1  relative line number
	2  relative from end of file

 It is currently not allowed to seek beyond end-of-file.  If you try to
 do so, you will simply be left at the end-of-file.

 Returns the current line number after the seek.

#endif

#include "lalocal.hxx"

La_linepos
La_stream::la_lseek (
    Reg6 La_linepos nlines,
    int type
) {
    Reg3 La_fsd *cfsd;
    unsigned short fsoff;
#ifdef LA_BP
    La_linesize speclength;
    La_linesize lbyte;
    Reg5 La_bytepos bp;
#endif

    switch (type) {
    case 0:     /* relative to beginning of file */
	break;

    case 1:     /* relative to current position */
	if (nlines == 0)
	    goto cur;
	nlines += la_lpos;
	break;

    case 2:     /* relative to end of file */
	nlines += la_file->la_nlines;
	break;

    default:
	la_errno = LA_INVMODE;
	return -1;
    }

    /* bound nlines to be within the file */
    {
	La_linepos ldelta;

	if (nlines <= 0) {
	    nlines = 0;
	    goto begin;
	} else if (nlines >= la_file->la_nlines) {
	    nlines = 0;
	    goto end;
	} else
	/* decide which is easier: relative to beginning, current, or end */
	     if
	    (   (ldelta = nlines - la_lpos) < 0
	     && -ldelta > la_fsline  /* won't be within this fsd */
	     && nlines * 2 <= la_lpos
	    ) {
	    /* go with relative to beginning */
 begin:     /*printf ("end %D\n", nlines);*/
	    la_lpos = 0;
#ifdef LA_BP
	    la_bpos = 0;
	    bp = 0;
#endif
	    la_cfsd = la_file->la_ffsd;
	    goto abs1;
	} else if
	   (   ldelta > 0
	    && ldelta >= la_cfsd->fsdnlines - la_fsline
		     /* won't be within this fsd */
	    && ldelta * 2 > la_file->la_nlines - la_lpos
	   ) {
	    /* convert to relative to end */
	    nlines -= la_file->la_nlines;
 end:       /*printf ("end %D\n", nlines);*/
	    la_lpos = la_file->la_nlines;
#ifdef LA_BP
	    la_bpos = bp = la_file->la_nbytes;
#endif
	    la_cfsd = la_file->la_lfsd;
 abs1:
	    la_fsline = 0;
	    la_fsbyte = 0;
	    la_ffpos  = 0;
	} else {
	    /* convert to relative to current */
	    nlines -= la_lpos;
 cur:       /*printf ("cur %D\n", nlines);*/
#ifdef LA_BP
	    lbyte = la_lbyte;
	    la_bpos = bp = la_bpos - lbyte;
	    la_ffpos -= lbyte;
#else
	    la_ffpos -= la_lbyte;
#endif
	}
    }

    la_lbyte = 0;
    if (nlines == 0)
	return la_lpos;

    la_lpos += nlines;
    cfsd = la_cfsd;
    if (nlines > 0) {
	Reg1 short j;
	Reg2 char *cp;

	j = cfsd->fsdnlines - la_fsline;
	cp = &cfsd->fsdbytes[la_fsbyte];
	if (nlines >= j) {
	    while (nlines >= j) {
		/* we're going to have to go past this fsd */
		nlines -= j;
#ifdef LA_BP
		if (*cp) {
		    if (j == cfsd->fsdnlines)
			bp += cfsd->fsdnbytes;
		    else
			for (;j--;) {
#ifndef NOSIGNEDCHAR
			    if (*cp < 0)
				bp += -(*cp++) << LA_NLLINE;
#else
			    if (*cp & LA_LLINE)
				bp += - (*cp++ | LA_LLINE) << LA_NLLINE;
#endif
			    bp += *cp++;
			}
		} else {
		    cp++;
		    if (*cp++)
			bp++;
		    move (cp, (char *) &speclength, sizeof speclength);
		    bp += speclength;
		}
#endif
		if (!(cfsd = cfsd->fsdforw))
		    la_abort ("fsdforw problem in la_lseek");
		j = cfsd->fsdnlines;
		cp = cfsd->fsdbytes;
	    }
	    la_fsline = 0;
	    la_cfsd = cfsd;
	    fsoff = 0;
	} else
	    fsoff = la_ffpos;
	/* If the fsd at this point is a special fsd, then nlines == 0 */
	la_fsline += j = nlines;
	{
	    Reg4 short ffpos;

	    ffpos = 0;
	    for (;j--;) {
#ifndef NOSIGNEDCHAR
		if (*cp < 0)
		    ffpos += -(*cp++) << LA_NLLINE;
#else
		if (*cp & LA_LLINE)
		    ffpos += - (*cp++ | LA_LLINE) << LA_NLLINE;
#endif
		ffpos += *cp++;
	    }
	    fsoff += ffpos;
#ifdef LA_BP
	    bp += ffpos;
#endif
	}
	la_fsbyte = cp - cfsd->fsdbytes;
    } else { /* (nlines < 0) */
	Reg1 short j;

	nlines = -nlines;
	j = la_fsline;
	if (nlines > j) {
	    Reg2 char *cp;

	    cp = cfsd->fsdbytes;
	    /* got to go back past this fsd */
	    for (;nlines > j;) {
		/* we're going to have to go back past this fsd */
		nlines -= j;
#ifdef LA_BP
		if (j > 0 && *cp) {
		    if (j == cfsd->fsdnlines)
			bp -= cfsd->fsdnbytes;
		    else
			for (;j--;) {
#ifndef NOSIGNEDCHAR
			    if (*cp < 0)
				bp -= -(*cp++) << LA_NLLINE;
#else
			    if (*cp & LA_LLINE)
				bp -= - (*cp++ | LA_LLINE) << LA_NLLINE;
#endif
			    bp -= *cp++;
			}
		} else if (j > 0) {
		    cp++;
		    if (*cp++)
			bp--;
		    move (cp, (char *) &speclength, sizeof speclength);
		    bp -= speclength;
		}
#endif
		if (!(cfsd = cfsd->fsdback))
		    la_abort ("fsdback problem in la_lseek");
		j = cfsd->fsdnlines;
		cp = cfsd->fsdbytes;
	    }
	    la_cfsd = cfsd;
	    la_fsline = j -= nlines;
	    {
		Reg4 short ffpos;

		ffpos = 0;
		for (;j--;) {
#ifndef NOSIGNEDCHAR
		    if (*cp < 0)
			ffpos += -(*cp++) << LA_NLLINE;
#else
		    if (*cp & LA_LLINE)
			ffpos += - (*cp++ | LA_LLINE) << LA_NLLINE;
#endif
		    ffpos += *cp++;
		}
		fsoff = ffpos;
	    }
	    la_fsbyte = cp - cfsd->fsdbytes;
#ifdef LA_BP
	    if (*cp)
		for (j = nlines; j--; ) {
#ifndef NOSIGNEDCHAR
		    if (*cp < 0)
			bp -= -(*cp++) << LA_NLLINE;
#else
		    if (*cp & LA_LLINE)
			bp -= - (*cp++ | LA_LLINE) << LA_NLLINE;
#endif
		    bp -= *cp++;
		}
	    else {
		cp++;
		if (*cp++)
		    bp--;
		move (cp, (char *) &speclength, sizeof speclength);
		bp -= speclength;
	    }
#endif
	} else {
	    /*printf ("within %D\n", nlines);*/
	    /* walk back within this fsd */
	    /* can't be a spceial fsd */
	    Reg4 short ffpos;
	    Reg2 char *cp;

	    cp = &cfsd->fsdbytes[la_fsbyte - 1];
	    la_fsline -= j = nlines;
	    ffpos = 0;
	    for (;j--;) {
		ffpos += *cp;
#ifndef NOSIGNEDCHAR
		if (*--cp < 0)
		    ffpos += -(*cp--) << LA_NLLINE;
#else
		if (*--cp & LA_LLINE)
		    ffpos += - (*cp-- | LA_LLINE) << LA_NLLINE;
#endif
	    }
	    fsoff = la_ffpos - ffpos;
#ifdef LA_BP
	    bp -= ffpos;
#endif
	    la_fsbyte = (cp - cfsd->fsdbytes) + 1;
	}
    }
    la_ffpos = fsoff;
#ifdef LA_BP
    la_bpos = bp;
#endif

    return la_lpos;
}
