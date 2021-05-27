/*
	Copyright (c) 1985, 1986 David Yost     All Rights Reserved
	Copyright (c) 1986, 1987 Grand Software, Inc. All Rights Reserved
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

#define stat statstruct
#include <sys/stat.h>
#undef stat
extern int stat (char* name, struct statstruct* buf);
extern int unlink (char* filename);
extern int creat (char* filename, int mode);
extern int close (int fd);
extern int getdtablesize ();

/* these are the actual global definitions */
#ifdef	SYSGETDTABLESIZE
int         la_maxchans = 0;	 /* maximum system opens allowed to la */
#else	SYSGETDTABLESIZE
int         la_maxchans = _NFILE;/* maximum system opens allowed to la */
#endif	SYSGETDTABLESIZE
La_linesize la_maxline = LA_MAXLINE;/* maximum line length allowed */
				/* affects la_open and la_lins, la_lrpl */
La_stream*  la_chglas;
Ff_stream*  la_chgffs;
La_flag     la_chgopen;         /* change file is open */
int         la_chans;           /* how many channels used by la package */
int         la_nbufs = 10;      /* how many cache buffers to use */
La_stream*  la_firststream;     /* first stream is always the changes file */
La_stream*  la_laststream;
int         la_errno;           /* last non-La_stream error that occurred */

La_stream* la_ffopen (Reg2 Ff_stream* pffs, La_stream* plas, long pos);
La_linepos la_parse (
    Ff_stream*  pffs,   /* NULL means parse memory buffer */
    long        seekpos,
    La_fsd**    ffsd,
    La_fsd**    lfsd,
    La_file*    plaf,
    La_bytepos  nchars,
    char*       buf
);

/* VARARGS3 */
La_stream*
la_open (
    Reg4 char* filename,
    char* modestr,
    Reg5 La_stream* plas,
    long offset,
    Ff_stream* pffs,
    int chan
) {
    Reg2 char mode;
    Reg3 char cflag;

    mode = 0;
    cflag = 0;
    {
	Reg1 char *cp;

	for (cp = modestr; *cp; cp++) {
	    switch (*cp) {
	    case 'n':
		mode |= LA_NEW;
		break;

	    case 't':
		mode |= LA_TMP | LA_NEW;
		break;

	    case 'c':
		cflag = YES;
		break;

	    default:
		la_errno = LA_INVMODE;
		return NULL;
	    }
	}
    }

    if (cflag && (mode & (LA_TMP | LA_NEW))) {
	la_errno = LA_INVMODE;
	return NULL;
    }

    if (mode & LA_NEW)
	return la_ffopen ((Ff_stream *) 0, plas, (long) 0);

    {
	Reg1 Ff_stream *ffl;

	if (!filename || *filename == '\0') {
	    if (pffs)
		return la_ffopen (pffs, plas, offset);
	    if (ffl = ff_fdopen (chan, 0, 0)) {
		La_stream *nlas;

     openit:    if ((nlas = la_ffopen (ffl, plas, offset)) == NULL) {
		    ff_close (ffl);
		    return NULL;
		}
		return nlas;
	    }
	} else {
	    struct statstruct st;

#if !defined (SYSFLOCK) && (defined (SYSLOCKF) || defined (SYSFCNTL_LOCK))
	    /*
	     *  in case caller wants to do an exclusive lock
	     *  on the file, we have to at least try to open it for writing.
	     */
	    if (ffl = ff_open (filename, 2, 0))
		goto openit;
#endif
	    if (ffl = ff_open (filename, 0, 0))
		goto openit;
	    if (cflag && stat (filename, &st) == -1)
		return la_ffopen ((Ff_stream *) 0, plas, (long) 0);
	}
    }
    la_errno = LA_NOOPN;
    return NULL;
}

La_stream*
la_ffopen (
Reg2 Ff_stream *pffs,
La_stream *plas,
long pos
) {
    Reg4 La_stream*  nlas;     /* the new La_stream struct we are making */
    Reg3 La_file*    tlaf;
    La_fsd*          ffsd;
    La_fsd*          lfsd;

    if (!la_chgopen) {
	Reg1 int regi;
	static La_stream chglas;

	la_chgopen = YES;   /* must be here because of recursion */
	if ((regi = la_nbufs - ff_flist.fr_count) > 0)
	    ff_alloc (regi, 0);
	unlink (la_cfile);
	if (   (regi = creat (la_cfile, 0600)) == -1
	    || close (regi) == -1
	    || (la_chgffs = ff_open (la_cfile, 2, 0)) == (Ff_stream *) 0
	    || (la_chglas = la_open ((char *) 0, "", &chglas, (La_bytepos) 0,
				     la_chgffs)
	       ) == NULL
	   ) {
	    la_chgopen = NO;
	    la_errno = LA_NOCHG;
	    return NULL;
	}
    }

    if (!(nlas = la_newstream (plas)))
	return NULL;

    /* see if this is another stream into one we have already */
    if (pffs) {
	if (pffs->f_file->fn_refs > 1) {
	    Reg1 La_stream *tlas;

	    /* let's see if we have this file already */
	    for (tlas = la_firststream; tlas; tlas = tlas->la_sforw)
		if (ff_fd (tlas->la_file->la_ffs) == ff_fd (pffs))
		    /* yes, it is */
		    return tlas->la_clone (nlas);
	}
    }

    if (pffs && la_chans >=
#ifdef	SYSGETDTABLESIZE
		(la_maxchans ? la_maxchans : (la_maxchans = getdtablesize ()))
#else	SYSGETDTABLESIZE
		 la_maxchans
#endif	SYSGETDTABLESIZE
       ) {
	la_errno = LA_NOCHANS;
	goto bad;
    }

    /* We have to make a new la_file for this stream */
    if ((tlaf = (La_file *) malloc ((unsigned int) sizeof (La_file))) == NULL) {
	la_errno = LA_NOMEM;
	    goto bad;
    }
    fill ((char *) tlaf, (unsigned int) sizeof *tlaf, 0);
    tlaf->la_refs = 1;
    if (pffs) {
	long totlft;
	char nonewl;    /* must be char */

	tlaf->la_ffs = pffs;
	totlft = pffs->f_file->fn_size - pos;
	/* parse the file */
	if ((tlaf->la_nlines =
	    la_parse (pffs, pos, &ffsd, &lfsd, tlaf, totlft, &nonewl))
	    < 0) {
 bad1:      free ((char *) tlaf);
 bad:       if (nlas->la_sflags & LA_ALLOCED)
		free ((char *) nlas);
	    return NULL;
	}
	tlaf->la_ffsd = ffsd;
#ifdef LA_BP
	tlaf->la_nbytes = totlft + nonewl;
#endif
	/* if there were any lines in the file, make an fsd */
	/* with no lines in it and link it to the end of the chain */
	if (tlaf->la_nlines > 0) {
	    if (la_parse ((Ff_stream *) 0, (La_bytepos) 0, &ffsd, &ffsd,
			  la_chglas->la_file, (La_bytepos) 0, "") < 0)
		goto bad1;
	    lfsd->fsdforw = ffsd;
	    ffsd->fsdback = lfsd;
	}
	la_chans++;
    } else {
	tlaf->la_mode = LA_NEW;
	if (!(tlaf->la_ffs = ff_fdopen (la_chglas->la_chan (), 0, 0)))
	    goto bad1;
	if (la_parse ((Ff_stream *) 0, (La_bytepos) 0, &ffsd, &ffsd,
		      la_chglas->la_file, (long) 0, "") < 0)
	    goto bad1;
	tlaf->la_ffsd = ffsd;
#ifdef LA_BP
	tlaf->la_nbytes = 0;
#endif
    }

    tlaf->la_lfsd = ffsd;

    {
	Reg1 char sflags;

	sflags = nlas->la_sflags;
	fill ((char *) nlas, (unsigned int) sizeof *nlas, 0);
	nlas->la_sflags = sflags;
    }

    nlas->la_cfsd = tlaf->la_ffsd;
    nlas->la_file = tlaf;
    nlas->la_makestream (tlaf);
    return nlas;
}

La_stream *
La_stream::la_clone (
    La_stream *newlas
) {
    Reg1 La_stream* nlas;
    Reg2 La_file*   tlaf;

    if (!la_verify ())
	return NULL;

    if (!(nlas = la_newstream (newlas)))
	return NULL;

    tlaf = la_file;
    tlaf->la_refs++;

    {
	Reg4 char sflags;

	sflags = nlas->la_sflags;
	*nlas = *this;
	nlas->la_sflags = sflags;
    }
    nlas->la_makestream (tlaf);
    return nlas;
}

/* YES or NO */
int
La_stream::la_verify ()
{
    Reg1 La_stream *tlas;

    for (tlas = la_firststream; tlas; tlas = tlas->la_sforw)
	if (tlas == this)
	    return YES;
    la_errno = LA_BADSTREAM;
    return NO;
}

La_stream*
la_newstream (
Reg1 La_stream *plas
) {
    if (!plas) {
	if ((plas = (La_stream *) malloc ((unsigned int) sizeof *plas))
	    == NULL) {
	    la_errno = LA_NOMEM;
	    return NULL;
	}
	plas->la_sflags = LA_ALLOCED;
    }
    fill ((char *) plas, (unsigned int) sizeof *plas, 0);
    return plas;
}

void
La_stream::la_makestream (
    Reg2 La_file *plaf
) {
    la_sforw = 0;
    la_sback = la_laststream;
    if (!la_firststream)
	la_firststream = this;
    if (la_laststream)
	la_laststream->la_sforw = this;
    la_laststream = this;

    la_fforw = 0;
    la_fback = plaf->la_lstream;
    if (!plaf->la_fstream)
	plaf->la_fstream = this;
    if (plaf->la_lstream)
	plaf->la_lstream->la_fforw = this;
    plaf->la_lstream = this;
    return;
}

La_linepos
la_parse (
    Ff_stream*  pffs,   /* NULL means parse memory buffer */
    long        seekpos,
    La_fsd**    ffsd,
    La_fsd**    lfsd,
    La_file*    plaf,
    La_bytepos  nchars,
    char*       buf
) {
    /*  The following union is a trick to get a character array 'b'
    /*  aligned so that b[2] falls on a long boundary.
    /**/
    union fsb {
	struct {
	    char        _bchr[6];
	    char        b[LA_FSDBMAX];
	} bytes;
	struct {
	    char        _schr[8];
	    La_linesize len; /* len must be on a long boundary AND be */
	} spcl;
    };
    union fsb fsbytes;          /* a tmp array for fsdbytes */
    Reg1 char      *cp;
    Reg2 short      nlft;       /* number of chars left in current block */
    Reg3 La_fsd    *cfsd;       /* current fsd */
    Reg5 La_bytepos totlft;     /* total characters left to parse */
    La_bytepos      ototlft;    /* old totlft */
    Reg6 La_flag    nonewline;
    short           nfsb;       /* number of fsdbytes */
    La_bytepos      totchcnt;   /* total character count */
    long            fsdchcnt;   /* fsd character count (long is correct) */
    short           lcnt;       /* fsd line count */
#ifdef  LA_LONGFILES
    long            totlcnt;    /* total # lines in this new chain */
#else   LA_LONGFILES
    short           totlcnt;    /* total # lines in this new chain */
#endif  LA_LONGFILES
    long            curpos;     /* current pos for ff_point */
    La_linesize     maxline;    /* maximum chars in any line we have parsed */
#ifdef  LA_LONGLINES
    La_flag         longline;
#endif
    La_flag         toobigfsd;

    *ffsd = 0;
    *lfsd = 0;
    cfsd = 0;
    totlcnt = 0;
    nfsb = 0;
    totchcnt = 0;
    fsdchcnt = 0;
#ifdef LA_LONGLINES
    longline = NO;
#endif
    toobigfsd = NO;
    nonewline = 0;
    maxline = plaf->la_maxline;

    if (nchars < 0) {
	la_errno = LA_NEGCOUNT;
	goto err;
    }
    if (pffs) {
	*buf = 0;
	nlft = 0;
	totlft = nchars;
	curpos = seekpos;
    } else {
	cp = buf;
	if (   nchars > 0
	    && cp[nchars - 1] != LA_NEWLINE
	   ) {
	    la_errno = LA_NONEWL;
	    goto err;
	}
    }

    fsbytes.bytes.b[0] = 0; /* no, wise compuyler, it isn't used before set */

    /* do this loop once per line */
    for (lcnt = 0; ;lcnt++) {
	Reg4 long lnchcnt;     /* character count in current line */
	La_linesize savelength;

	savelength = 0;     /* no, wise compuyler, it isn't used before set */
	/* if it's time to make an fsd, do so */
	if (   lcnt >= LA_FSDLMAX
	    || totchcnt >= nchars
	    || nfsb >= LA_FSDBMAX - 2
	   ) {
 makefsd:
#ifndef LA_LONGFILES
	    if (totlcnt + lcnt < totlcnt) {
		la_errno = LA_ERRMAXL;
		goto err;
	    }
#endif  LA_LONGFILES
	    if (la_int ()) {
		la_errno = LA_INT;
		goto err;
	    }
	    if ((cfsd = (La_fsd *) malloc ((unsigned int) (LA_FSDSIZE + nfsb)))
		== NULL) {
		la_errno = LA_NOMEM;
		goto err;
	    }
	    cfsd->fsdforw = 0;
	    cfsd->fsdback = *lfsd;
	    cfsd->fsdpos = seekpos;
	    seekpos += fsdchcnt;
	    cfsd->fsdfile = plaf;
	    cfsd->fsdnbytes = fsdchcnt;
	    if (cfsd->fsdnlines = lcnt)
		plaf->la_fsrefs++;
	    move (fsbytes.bytes.b, cfsd->fsdbytes, (unsigned int) nfsb);
	    if (*ffsd)
		(*lfsd)->fsdforw = cfsd;
	    else
		*ffsd = cfsd;
	    *lfsd = cfsd;
	    totlcnt += lcnt;
	    if (totchcnt >= nchars)
		break;
 makefirst:
#ifdef LA_LONGLINES
	    if (nonewline || longline) {
#else
	    if (nonewline) {
#endif
		lcnt = 1;
		fsbytes.bytes.b[0] = 0;
		fsbytes.bytes.b[1] = nonewline;
		fsbytes.spcl.len = savelength;
		fsdchcnt = savelength;
		totchcnt += savelength + nonewline;
		nfsb = 6;
		nonewline = 0;
#ifdef LA_LONGLINES
		longline = NO;
#endif
		goto makefsd;
	    } else {
		lcnt = 0;
		nfsb = 0;
		fsdchcnt = 0;
		if (toobigfsd) {
		    toobigfsd = NO;
		    lnchcnt = savelength;
		    goto makebytes;
		}
	    }
	}

	/* count the characters in the current line */
	if (pffs) {     /* parsing from a file */
	    ototlft = totlft + nlft;
	    /* parse a line.  */
	    do {
		if (--nlft < 0) {
		    /* need a block to work on */
		    char *newcp;

		    if (la_int ()) {
			la_errno = LA_INT;
			goto err;
		    }
		    if ((nlft = ff_point (pffs, curpos, &newcp, totlft))
			< 0) {
			la_errno = LA_FFERR;
			goto err;
		    }
		    curpos += nlft;
		    if (nlft == 0) {
			/* end of file */
			nchars -= totlft; /* truncate nchars to file length */
			ototlft -= totlft;
			totlft = 0;
			if (ototlft == 0) {
			    /* end of file before any chars in line */
			    goto makefsd;
			}
			/*
			 *  We get to this point when we have been asked
			 *  to parse more than there is in the file
			 */
		    }
		    if (totlft <= 0) {
			/* don't want to parse any more */
			nlft = 0;
			nonewline = 1;  /* must be 1 */
			*buf = 1;
			break;
		    }
		    cp = newcp;
		    totlft -= nlft--;
		}
	    } while (*cp++ != LA_NEWLINE);
	    lnchcnt = ototlft - (totlft + nlft);
	    if (maxline < lnchcnt + nonewline)
		maxline = lnchcnt + nonewline;
	} else {    /* parsing an array */
	    char *ocp;

	    ocp = cp;
	    while (*cp++ != LA_NEWLINE)
		continue;
	    lnchcnt = cp - ocp;
	    if (maxline < lnchcnt)
		maxline = lnchcnt;
	}
	if (   maxline > la_maxline
	    || lnchcnt < 0
	   ) {
	    la_errno = LA_TOOLONG;
	    goto err;
	}

	/* make the fsdbytes for the line */
	if (lnchcnt <= LA_MAX_NON_SPECIAL_FSD) {
	    if (nonewline)
		goto save;
	    if (fsdchcnt + lnchcnt > (long) LA_FSDNBMAX) {
		toobigfsd = YES;
 save:          savelength = lnchcnt;
		if (lcnt)
		    goto makefsd;
		else
		    goto makefirst;
	    }
 makebytes:
	    {
		short shorttmp;

		fsdchcnt += shorttmp = lnchcnt;
		totchcnt += shorttmp;
		if (shorttmp > ~LA_LLINE) {
		    fsbytes.bytes.b[nfsb++] = -(shorttmp >> LA_NLLINE);
		    shorttmp &= ~LA_LLINE;
		}
		fsbytes.bytes.b[nfsb++] = shorttmp;
	    }
	} else {
#ifdef LA_LONGLINES
	    longline = YES;
	    goto save;
#else
	    la_errno = LA_TOOLONG;
	    goto err;
#endif
	}
    }

    plaf->la_maxline = maxline;
    return totlcnt;

 err:
    la_freefsd (*ffsd);
    return -1;
}

void
la_freefsd (
    Reg1 La_fsd *tfsd
) {
    Reg2 La_fsd *nfsd;

    while (tfsd) {
	nfsd = tfsd->fsdforw;
	if (tfsd->fsdnlines)
	    tfsd->fsdfile->la_fsrefs--;
	free ((char *) tfsd);
	tfsd = nfsd;
    }
    return;
}
