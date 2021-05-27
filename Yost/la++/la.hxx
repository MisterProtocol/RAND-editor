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

/*
    file la.h - include stuff for the la package
 */

#include <ff.hxx>

#ifndef YES
#define YES 1
#define NO  0
#endif

#ifdef  BIGDADDR
#define LA_LONGFILES            /* handle files with more than 32,767 lines*/
#endif/*BIGDADDR */

/* #define LA_LONGLINES         /* very esoteric */

#ifdef  LA_LONGLINES
# define LA_MAXLINE MAXLONG
  typedef long  La_linesize;    /* must not be unsigned */
#else /*LA_LONGLINES */
# define LA_MAXLINE LA_MAX_NON_SPECIAL_FSD
  typedef short La_linesize;    /* must not be unsigned */
#endif/*LA_LONGLINES */

#ifdef  LA_LONGFILES
# define LA_MAXNLINES MAXLONG
typedef long La_linepos;        /* must not be unsigned */
#else /*LA_LONGLINES */
# define LA_MAXNLINES MAXSHORT
typedef short La_linepos;       /* must not be unsigned */
#endif/*LA_LONGLINES */

#define LA_NLLINE  (CHARNBITS - 1)
#define LA_LLINE   (~(CHARMASK >> 1))
#define LA_MAX_NON_SPECIAL_FSD  ((1 << (LA_NLLINE + LA_NLLINE)) + ~LA_LLINE)

typedef long  La_bytepos;       /* must not be unsigned */
typedef int La_flag;

struct La_file;

/*  fsd - file segment descriptor.  Describes 1 to 127 contiguous lines
/*      in file.
/**/
struct La_fsd {
    La_fsd*        fsdforw;     /* next fsd in chain; 0 if last fsd        */
    La_fsd*        fsdback;     /* previous fsd in chain                   */
    La_file*       fsdfile;     /* lafile pointer of file containing lines.*/
				/*  0 if this is the last fsd in chain.    */
    long fsdpos;                /* location of first character             */
    short fsdnbytes;            /* num of bytes in this fsd if regular fsd */
    char fsdnlines;             /* number of lines in this fsd.            */
				/*  0 if this is the last fsd in chain.    */
    char fsdbytes[1];           /* bytes describing the linelengths.       */
};

class La_stream;

struct La_file {
    La_fsd*          la_ffsd;   /* first fsd in the chain                  */
    La_fsd*          la_lfsd;   /* last fsd in the chain                   */
    Ff_stream*       la_ffs;    /* chain of ff streams for this file       */
    La_stream*       la_fstream;/* first in chain for this lafile          */
    La_stream*       la_lstream;/* last  in chain for this lafile          */
    La_file*         la_fwfile; /* chain of diffed files                   */
    La_linepos       la_fsrefs; /* number of refs by fsds                  */
    short            la_refs;   /* number of refs by La_streams            */
    La_linesize      la_maxline;/* max line size in the file               */
    La_linepos       la_nlines; /* number of lines in file                 */
#ifdef  LA_BP
    La_bytepos       la_nbytes; /* number of bytes in file */
#endif/*LA_BP */
    char             la_mode;   /* mode bits (see below) */
};
/* la_mode element of La_file: */
#define LA_NEW        01        /* no associated disk file */
#define LA_TMP        02        /* tmp file */
#define LA_MODIFIED   04        /* file has been modified */

class La_stream {
 public: /* for now */
    short            la_fsline; /* which line of current fsd starting at 0 */
    short            la_fsbyte; /* index into fsdbytes in current fsd */
    short            la_lbyte;  /* offset from beginning of current line */
    short            la_ffpos;  /* fsd fsdpos + this = offset in la_ffs */
    La_bytepos       la_bpos;   /* current byte position in file */
    La_linepos       la_lpos;   /* current line number */
    La_fsd*          la_cfsd;   /* current fsd */
    La_linepos       la_rlines; /* reserved lines from cur position */
    La_file*         la_file;   /* pointer to stream-independent stuff */
    La_stream*       la_sback;  /* chain of all streams */
    La_stream*       la_sforw;  /* chain of all streams */
    La_stream*       la_fback;  /* chain of streams into la_file */
    La_stream*       la_fforw;  /* chain of streams into la_file */
    char             la_sflags; /* stream flags */

    inline int        active();

    inline int        modified();
    inline int        domodify();
    inline int        unmodify();
    inline La_linepos reserved();
    inline La_linepos setrlines(La_linepos l);

    inline int        la_stay();
    inline int        la_stayset();
    inline int        la_stayclr();

    inline Ff_stream* la_ffchan();
    inline int        la_chan();
    inline short      nstreams();
    inline La_linepos linepos();
    inline int        la_eof();
    inline La_linepos la_lsize();
    inline La_bytepos bytepos();
#ifdef  LA_BP
    inline La_bytepos la_bsize();
#else /*LA_BP */
    La_bytepos        la_bsize();
#endif/*LA_BP */
    inline La_linepos lmax();

    La_linesize       la_lrsize ();


    La_stream*        la_clone (La_stream* newlas);
    int               la_verify ();
    void              la_makestream (La_file* plaf);

    void              la_link (
	La_fsd* ffsd,           /* first fsd in the chain */
	La_fsd* lfsd,           /* last  fsd in the chain */
	La_linepos nl           /* number of lines in the chain */
#ifdef LA_BP
	,
	La_bytepos nchars       /* number of chars in the chain */
#endif
    );

    La_linesize       la_lget (char* buf, int nchars);
    La_linesize       la_lwsize ();

    La_linesize       la_lpnt (
	char** buf
    );

    La_linepos        la_lcount (
	La_linepos start,
	La_linepos number,
	int mode
    );

    La_linepos        la_lrcollect (
	La_linepos* nlines,
	La_stream*  dlas
    );

    La_linepos        la_align (La_stream* oldlas);
    La_linepos        la_blank (La_linepos nlines);
    La_linepos        la_linsert (char* buf, int nchars);
    La_linepos        la_lcopy (La_stream* dlas, La_linepos nlines);
    La_linepos        la_lseek (La_linepos nlines, int type);
    La_linesize       la_clseek (La_linesize nchars);
    La_flag           la_zbreak ();
    La_flag           la_break (int mode, La_fsd** ffsd, La_fsd** lfsd,
	La_linepos* pnlines
#ifdef LA_BP
	,
	La_bytepos* nbytes
#endif
    );

    La_linepos        la_lflush (
	La_linepos start,
	La_linepos nlines,
	int chan,
	int oktoint,
	unsigned int timeout = 0
    );

    La_linepos        la_ldelete (La_linepos nlines, La_stream* dlas);
    La_linepos        la_lreplace (
	char* buf,
	int nchars,
	La_linepos* nlines,
	La_stream* dlas
    );
    La_linepos        la_finsert (
	La_bytepos stchar,
	La_bytepos nchars,
	La_linepos stline,
	La_linepos nlines,
	char* filename,
	Ff_stream* pffs,
	int chan
    );
    La_stream*        la_other ();
    int               la_freplace (char* filename);
    La_linepos        la_close ();
    void              la_fschaindump (char *txt);
    void              la_schaindump (char *txt);
    void              la_sdump (char* txt);
};

/* la_sflags element of La_stream: */
#define LA_ALLOCED   001        /* open call alloced la_stream struc */
#define LA_STAY      002        /* stay at same line after insert there */

#ifdef  LA_BP
    inline La_bytepos la_bsize() { return la_file->la_nbytes; }
#endif/*LA_BP */
    inline int        La_stream::active() { return la_file ? YES : NO; }

    inline int        La_stream::modified() { return la_file->la_mode & LA_MODIFIED ? YES : NO;}
    inline int        La_stream::domodify() { return la_file->la_mode |= LA_MODIFIED; }
    inline int        La_stream::unmodify() { return la_file->la_mode &= ~LA_MODIFIED; }
    inline La_linepos La_stream::reserved() { return la_rlines; }
    inline La_linepos La_stream::setrlines(La_linepos l) { return la_rlines = l; }

    inline int        La_stream::la_stay() { return la_sflags & LA_STAY ? YES : NO; }
    inline int        La_stream::la_stayset() { return la_sflags |= LA_STAY; }
    inline int        La_stream::la_stayclr() { return la_sflags &= ~LA_STAY; }

    inline Ff_stream* La_stream::la_ffchan() { return la_file->la_ffs; }
    inline int        La_stream::la_chan() { return ff_fd (la_file->la_ffs); }
    inline short      La_stream::nstreams() { return la_file->la_refs; }
    inline La_linepos La_stream::linepos() { return la_lpos; }
    inline La_bytepos La_stream::bytepos() { return la_bpos; }
    inline int La_stream::la_eof() { return la_cfsd == la_file->la_lfsd; }
    inline La_linepos La_stream::la_lsize() { return la_file->la_nlines; }
    inline La_linepos La_stream::lmax() { return la_file->la_maxline; }

#define la_active(plas)     ((plas) && (plas)->active())

struct la_spos {                /* used by la_align */
    short            la_fsline;
    short            la_fsbyte;
    short            la_lbyte;
    short            la_ffpos;
    La_bytepos       la_bpos;
    La_linepos       la_lpos;
    La_fsd*          la_cfsd;
 };
struct la_fpos {                /* used by la_ldelete */
    short            la_fsline;
    short            la_fsbyte;
    short            la_lbyte;
    short            la_ffpos;
 };

#define la_sync(flg)        ff_sync (flg)
#define la_ltrunc(i) ((i) > LA_MAXNLINES ? LA_MAXNLINES : (i) < 0 ? 0 : (i))

extern La_linepos la_lcollect (
    int        state,     /* 1=start, 0=continue */
    char* buf,
    int   nchars
);

extern La_stream* la_open (
    char*          filename,
    char*          modestr,
    La_stream*     plas,
    long           offset,
    Ff_stream*     pffs = 0,
    int chan = 0
);

extern void la_fsddump (
    La_fsd*     ffsd,
    La_fsd*     lfsd,
    La_flag     fsdbyteflg,
    char*       txt
);

extern void la_fdump (
    Reg1 La_file* plaf,
    char* txt
);

extern La_stream* la_newstream (La_stream* plas);

extern int la_tcollect (long pos);

#define LA_NEWLINE ('\n')

/*  If any of these is not supplied by the user,
/*  a default one will be loaded from the library.
/**/

/* called in some bug situations */
extern void la_abort(char* message);

/*
   la_int () is called periodically during some long operations,
   and if it returns non-0, the operation is aborted
 */
extern int la_int();

/*  Either of these may be stored into by the user. */
extern int         la_maxchans; /* maximum system opens allowed to la */
extern char* la_cfile;          /* the name to use for the changes file */
extern La_linesize la_maxline;  /* maximum line length allowed */
				/* affects la_open and la_lins, la_lrpl */

/* These are only to be modified by the library */
extern La_stream* la_chglas;    /* change file */
extern Ff_stream* la_chgffs;    /* change file */
extern La_flag    la_chgopen;   /* YES means change file has been la_opened */
extern int        la_chans;     /* number of system opens in use by la */
extern int        la_nbufs;     /* how many cache buffers to use */
extern La_stream* la_firststream;/* first in chain of open La_streams */
extern La_stream* la_laststream;/* last in chain of open La_streams */
extern int la_error ();
/* la_error() values: */
#define LA_BADBAD     2 /* bad stream ptr to la_error call */
#define LA_INT        3 /* operation was interrupted */
#define LA_EOF        4 /* attempt to read past EOF */
#define LA_ERRMAXL    5 /* a file had too many lines to process */
#define LA_READERR    6 /* a read error was received from ffio */
#define LA_NONEWL     7 /* last character not a newline */
#define LA_WRTERR     8 /* a write error was received from ffio */
#define LA_NOCHG      9 /* can't open change file */
#define LA_NOMEM     10 /* no more alloc space */
#define LA_NOCHANS   11 /* no more system open channels left */
#define LA_INVMODE   12 /* invalid mode to open, lcount, lseek, or bseek */
#define LA_NOOPN     13 /* can't open file */
#define LA_BADSTREAM 14 /* bad lastream */
#define LA_FFERR     15 /* error in ff package */
#define LA_GETERR    16 /* la_getlin returned fewer than asked */
#define LA_NEGCOUNT  17 /* negative count arg to function */
#define LA_TOOLONG   18 /* line to long */
#define LA_NOTSAME   19 /* la_align of 2 streams into different files */
#define LA_BADCOLL   20 /* la_lcollects interleaved */
#define LA_BRKCOLL   21 /* la_lcollect broken by other insert */
