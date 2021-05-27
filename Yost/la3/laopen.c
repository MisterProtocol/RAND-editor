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


#include "lalocal.h"
#include <malloc.h>
#include <sys/stat.h>

/* these are the actual global definitions */
#ifdef	SYSGETDTABLESIZE
int         la_maxchans = 0;	 /* maximum system opens allowed to la */
#else/* SYSGETDTABLESIZE */
int         la_maxchans = _NFILE;/* maximum system opens allowed to la */
#endif/*SYSGETDTABLESIZE */
La_linesize la_maxline = LA_MAXLINE;/* maximum line length allowed */
				/* affects la_open and la_lins, la_lrpl */
La_stream  *la_chglas;
Ff_stream  *la_chgffs;
La_flag     la_chgopen;         /* change file is open */
int         la_chans;           /* how many channels used by la package */
int         la_nbufs = 10;      /* how many cache buffers to use */
La_stream  *la_firststream;     /* first stream is always the changes file */
La_stream  *la_laststream;
int         la_errno;           /* last non-La_stream error that occurred */

extern La_stream  *la_newstream ();
extern La_view    *la_newview ();
extern La_stream  *la_ffopen ();

/* VARARGS3 */
La_stream *
la_open (filename, modestr, plas, offset, pffs, chan)
Reg4 char *filename;
char *modestr;
long offset;
Reg5 La_stream *plas;
Ff_stream *pffs;
int chan;
{
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
	    struct stat st;

	    if (ffl = ff_open (filename, 0, 0))
		goto openit;
	    if (cflag && stat (filename, &st) == -1)
		return la_ffopen ((Ff_stream *) 0, plas, (long) 0);
	}
    }
    la_errno = LA_NOOPN;
    return NULL;
}

La_stream *
la_ffopen (pffs, plas, pos)
La_stream *plas;
Reg2 Ff_stream *pffs;
long pos;
{
    Reg4 La_stream *nlas;   /* the new La_stream struct we are making */
    La_view *nlav;
    Reg3 La_file *nlaf;


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
		    return la_clone (tlas, nlas);
	}
    }

    if (pffs && la_chans >=
#ifdef	SYSGETDTABLESIZE
		(la_maxchans ? la_maxchans : (la_maxchans = getdtablesize ()))
#else/* SYSGETDTABLESIZE */
		 la_maxchans
#endif/*SYSGETDTABLESIZE */
       ) {
	la_errno = LA_NOCHANS;
	goto bad;
    }

    if (!(nlav = la_newview ())) {
	la_errno = LA_NOMEM;
	goto bad;
    }
    nlav->la_srefs = 1;

    /* We have to make a new la_file for this stream */
    if ((nlaf = (La_file *) calloc (1, (unsigned int) sizeof *nlaf)) == NULL) {
	la_errno = LA_NOMEM;
	goto bad2;
    }
    nlaf->la_vrefs = 1;

    if (pffs) {
	long totlft;
	char nonewl;    /* must be char */
	La_fsd *ffsd;
	La_fsd *lfsd;

	nlaf->la_ffs = pffs;
	totlft = pffs->f_file->fn_size - pos;
	/* parse the file */
	if ((nlav->la_nlines =
	    la_parse (pffs, pos, &ffsd, &lfsd, nlaf, totlft, &nonewl))
	    < 0) {
 bad1 :     free ((char *) nlaf);
 bad2 :     free ((char *) nlav);
 bad :      if (nlas->la_sflags & LA_ALLOCED)
		free ((char *) nlas);
	    return NULL;
	}
	nlaf->la_ffsd = ffsd;
#ifdef LA_BP
	nlaf->la_nbytes = totlft + nonewl;
#endif
	/*
	    If there were any lines in the file, make an fsd
	    with no lines in it and link it to the end of the chain.
	 */
	if (nlav->la_nlines > 0) {
	    if (la_parse ((Ff_stream *) 0, (La_bytepos) 0, &ffsd, &ffsd,
			  la_chglas->la_file, (La_bytepos) 0, "") < 0)
		goto bad1;
	    lfsd->fsdforw = ffsd;
	    ffsd->fsdback = lfsd;
	}
	la_chans++;
	nlaf->la_lfsd = ffsd;
    } else {
	La_fsd *ffsd;

	nlaf->la_mode = LA_NEW;
	if (!(nlaf->la_ffs = ff_fdopen (la_chan (la_chglas), 0, 0)))
	    goto bad1;
	if (la_parse ((Ff_stream *) 0, (La_bytepos) 0, &ffsd, &ffsd,
		      la_chglas->la_file, (long) 0, "") < 0)
	    goto bad1;
#ifdef LA_BP
	nlaf->la_nbytes = 0;
#endif
	nlaf->la_ffsd = ffsd;
	nlaf->la_lfsd = ffsd;
    }

    /* zero out all the variable fields in the stream */
    {
	Reg1 char sflags;

	sflags = nlas->la_sflags;
	fill ((char *) nlas, (unsigned int) sizeof *nlas, 0);
	nlas->la_sflags = sflags;
    }

    nlas->la_file = nlaf;
    if (!la_makeview (nlav, nlaf)) {
	la_freefsd (&nlaf->la_ffsd);
	goto bad1;
    }
    nlas->la_view = nlav;
    nlas->la_pos.la_cifsd = nlav->la_fifsd;
    la_makestream (nlas, nlav);
    return nlas;
}

La_stream *
la_clone (oldlas, newlas)
Reg3 La_stream *oldlas;
La_stream *newlas;
{
    Reg1 La_stream *nlas;
    Reg2 La_view   *tlav;

    if (!la_verify (oldlas))
	return NULL;

    if (!(nlas = la_newstream (newlas)))
	return NULL;

    tlav = oldlas->la_view;
    tlav->la_srefs++;

    {
	Reg4 char sflags;

	sflags = nlas->la_sflags;
	/* the following move is really: *nlas = *oldlas; */
	move ((char *) oldlas, (char *) nlas, (unsigned int) sizeof *nlas);
	nlas->la_sflags = sflags;
    }
    la_makestream (nlas, tlav);
    return nlas;
}

la_verify (plas)
Reg2 La_stream *plas;
{
    Reg1 La_stream *tlas;

    for (tlas = la_firststream; tlas; tlas = tlas->la_sforw)
	if (tlas == plas)
	    return YES;
    la_errno = LA_BADSTREAM;
    return NO;
}

static La_stream *
la_newstream (plas)
Reg1 La_stream *plas;
{
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

static La_view *
la_newview ()
{
    Reg1 La_view *plav;

    if ((plav = (La_view *) calloc (1, (unsigned int) sizeof *plav))
	== NULL) {
	la_errno = LA_NOMEM;
	return NULL;
    }
    return plav;
}

static int
la_makestream (plas, plav)
Reg1 La_stream *plas;
Reg2 La_view *plav;
{
    plas->la_sforw = 0;
    plas->la_sback = la_laststream;
    if (!la_firststream)
	la_firststream = plas;
    if (la_laststream)
	la_laststream->la_sforw = plas;
    la_laststream = plas;

    plas->la_vforw = 0;
    plas->la_vback = plav->la_lstream;
    if (!plav->la_fstream)
	plav->la_fstream = plas;
    if (plav->la_lstream)
	plav->la_lstream->la_vforw = plas;
    plav->la_lstream = plas;
    return;
}

static La_flag
la_makeview (plav, plaf)
Reg5 La_view *plav;
Reg6 La_file *plaf;
{
    Reg1 La_ifsd *cifsd;
    Reg2 La_fsd *cfsd;
    Reg3 La_ifsd *fifsd = NULL;
    Reg4 La_ifsd *lifsd = 0;

    plav->la_file = plaf;

    plav->la_fforw = 0;
    plav->la_fback = plaf->la_lview;
    if (!plaf->la_fview)
	plaf->la_fview = plav;
    if (plaf->la_lview)
	plaf->la_lview->la_fforw = plav;
    plaf->la_lview = plav;

    /* Make an ifsd chain pointing at the fsd chain. */
    for (cfsd = plaf->la_ffsd; cfsd; cfsd = cfsd->fsdforw) {
	if ((cifsd = (La_ifsd *) malloc ((unsigned int) sizeof *cifsd))
	    == NULL) {
	    la_freeifsd (fifsd);
	    return NO;
	}
	cifsd->ifsdforw = 0;
	cifsd->ifsdback = lifsd;
	if (fifsd)
	    lifsd->ifsdforw = cifsd;
	else
	    fifsd = cifsd;
	lifsd = cifsd;
	if (cifsd->ifsdnlines = cfsd->fsdnlines) {
	    cifsd->ifsdfsd = cfsd;
	    cifsd->ifsdbytes = cfsd->fsdbytes;
	    cfsd->fsdrefs++;
	} else
	    break;
    }
    plav->la_fifsd = fifsd;
    plav->la_lifsd = lifsd;
    return YES;
}


La_linepos
la_parse (pffs, seekpos, ffsd, lfsd, plaf, nchars, buf)
Ff_stream     *pffs;     /* NULL means parse memory buffer */
long           seekpos;
La_fsd       **ffsd;
La_fsd       **lfsd;
La_file       *plaf;
La_bytepos     nchars;
char          *buf;
{
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
    union fsb       fsb;        /* a tmp array for fsdbytes */
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
    short           totlcnt;    /* total # lines in this new chain */
    long            curpos;     /* current pos for ff_point */
    La_linesize     maxline;    /* maximum chars in any line we have parsed */
#ifdef LA_LONGLINES
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

    /* do this loop once per line */
    for (lcnt = 0; ;lcnt++) {
	Reg4 long lnchcnt;     /* character count in current line */
	La_linesize savelength;

#ifdef lint
	savelength = 0;
#endif
	if (   lcnt >= LA_FSDLMAX
	    || totchcnt >= nchars
	    || nfsb >= LA_FSDBMAX - 2
	   ) {
	    /* It's time to make an fsd */
 makefsd:   if (totlcnt + lcnt < totlcnt) {
		/* can only happen if La_linepos is typedefed to short */
		la_errno = LA_ERRMAXL;
		goto err;
	    }
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
	    move (fsb.bytes.b, cfsd->fsdbytes, (unsigned int) nfsb);
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
		fsb.bytes.b[0] = 0;
		fsb.bytes.b[1] = nonewline;
		fsb.spcl.len = savelength;
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
		    fsb.bytes.b[nfsb++] = -(shorttmp >> LA_NLLINE);
		    shorttmp &= ~LA_LLINE;
		}
		fsb.bytes.b[nfsb++] = shorttmp;
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
    la_freefsd (ffsd);
    return -1;
}

/*
   Free all fsds in a chain with 0 reference counts.
   Keep those with non-0 counts.
 */
void
la_freefsd (tfsd)
Reg2 La_fsd **tfsd;
{
    Reg1 La_fsd *cfsd;

    cfsd = *tfsd;
    while (cfsd) {
	if (cfsd->fsdrefs == 0) {
	    if (cfsd->fsdnlines)
		cfsd->fsdfile->la_fsrefs--;
	    cfsd = cfsd->fsdforw;
	    free ((char *) cfsd);
	} else {
	    *tfsd = cfsd;
	    tfsd = &cfsd->fsdforw;
	    cfsd = cfsd->fsdforw;
	}
    }
    return;
}

/* free all but the last one if null */
La_ifsd *
la_freeifsd (tifsd)
Reg1 La_ifsd *tifsd;
{
    Reg2 La_ifsd *nifsd;

    while (tifsd) {
	nifsd = tifsd->ifsdforw;
	if (tifsd->ifsdfsd)
	    tifsd->ifsdfsd->fsdrefs--;
	if (!tifsd->ifsdnlines)
	    break;
	free ((char *) tifsd);
	tifsd = nifsd;
    }
    return tifsd;
}
