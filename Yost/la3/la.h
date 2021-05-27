/*
	Copyright (c) 1985, 1986 David Yost	All Rights Reserved
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

/*
    file la.h - include stuff for the la package
 */

#include <ff.h>

#ifndef YES
#define YES 1
#define NO  0
#endif

/* #define LA_LONGLINES		/* very esoteric */

/* You may still want this, even if your machine is not INT4 */
/* but you will probably run out of address space on large files. */
#ifdef	INT4
# define LA_LONGFILES		/* not completely debugged ? */
#endif/*INT4 */

#ifdef	LA_LONGLINES
# define LA_MAXLINE MAXLONG
  typedef long	La_linesize;	/* must not be unsigned */
#else/* LA_LONGLINES */
# define LA_MAXLINE LA_MAX_NON_SPECIAL_FSD
  typedef short La_linesize;	/* must not be unsigned */
#endif/*LA_LONGLINES */

#ifdef	LA_LONGFILES
# define LA_MAXNLINES MAXLONG
typedef long La_linepos;	/* must not be unsigned */
#else/* LA_LONGLINES */
# define LA_MAXNLINES MAXSHORT
typedef short La_linepos;	/* must not be unsigned */
#endif/*LA_LONGLINES */

#define LA_NLLINE  (CHARNBITS - 1)
#define LA_LLINE   (~(CHARMASK >> 1))
#define LA_MAX_NON_SPECIAL_FSD	((1 << (LA_NLLINE + LA_NLLINE)) + ~LA_LLINE)

typedef long  La_bytepos;	/* must not be unsigned */
typedef int La_flag;

/*
    fsd - file segment descriptor.  Describes some number of contiguous lines
	in an actual disk file.
 */
typedef struct la_fsd {
    struct la_fsd  *fsdforw;	 /* next fsd in chain; 0 if last fsd	    */
    struct la_fsd  *fsdback;	 /* previous fsd in chain		    */
    struct la_file *fsdfile;	 /* lafile pointer of file containing lines.*/
				/*  0 if this is the last fsd in chain.	   */
    long fsdpos;		/* location of first character		   */
    long fsdrefs;		/* number of ifsd's pointing here */
    unsigned short fsdnbytes;	/* num of bytes represented.		   */
    unsigned short fsdnlines;	/* number of lines represented.		   */
				/*  0 if this is the last fsd in chain.	   */
    char fsdbytes[1];		/* bytes describing the linelengths.	   */
} La_fsd;

/*
    ifsd - indirect file segment descriptor.
    Describes some number of contiguous lines in an virtual disk file.
 */
typedef struct la_ifsd {
    struct la_ifsd *ifsdforw;	 /* 0 if last				    */
    struct la_ifsd *ifsdback;
    struct la_fsd *ifsdfsd;	 /* where to find the lines.		    */
    char *ifsdbytes;		/* bytes describing the linelengths.	   */
    unsigned short ifsdnlines;	/* number of lines represented.		   */
				/*  0 if this is the last fsd in chain.	   */
    char ifsdflags;
} La_ifsd;
/* ifsdflags */
#define IFSD_DISK   0
#define IFSD_BLANK  1
#define IFSD_GHOST  2
#define IFSD_DIFFER 4

/* one per view into the file */
typedef struct la_view {
    struct la_view   *la_fforw;	 /* chain of views			    */
    struct la_view   *la_fback;	 /* chain of compared views		    */
    struct la_stream *la_fstream;/* first in chain			    */
    struct la_stream *la_lstream;/* last  in chain			    */
    La_ifsd	    *la_fifsd;	/* first in chain			   */
    La_ifsd	    *la_lifsd;	/* last  in chain			   */
    struct la_view   *la_fcview; /* chain of compared views                 */
    struct la_file   *la_file;
    La_linepos	     la_nlines; /* number of lines in file		   */
#ifdef	LA_BP
    La_bytepos	     la_nbytes; /* number of bytes in file */
#endif/*LA_BP */
    short	     la_srefs;	/* number of refs by La_streams		   */
} La_view;

/* one per file */
typedef struct la_file {
    La_view	    *la_fview;	/* first view in the chain		   */
    La_view	    *la_lview;	/* last  view in the chain		   */
    La_fsd	    *la_ffsd;	/* first fsd in the chain		   */
    La_fsd	    *la_lfsd;	/* last  fsd in the chain		   */
    Ff_stream	    *la_ffs;	/* chain of ff streams for this file	   */
    La_linepos	     la_fsrefs; /* number of refs by fsds		   */
    La_linesize	     la_maxline;/* max line size in the file		   */
    short	     la_vrefs;	/* number of refs by La_views		   */
    char	     la_mode;	/* mode bits (see below) */
} La_file;
/* la_mode element of La_file: */
#define LA_NEW	      01	/* no associated disk file */
#define LA_TMP	      02	/* tmp file */
#define LA_MODIFIED   04	/* file has been modified */

typedef struct la_stream {
    struct la_pos {
	La_ifsd	    *la_cifsd;	/* current fsd */
	short	     la_fsline;	/* which line of current ifsd starting at 0 */
	short	     la_fsbyte;	/* index into fsdbytes in current ifsd */
	short	     la_lbyte;	/* offset from beginning of current line */
	short	     la_ffpos;	/* fsd fsdpos + this = offset in la_ffs */
	La_linepos   la_lpos;	/* current line number */
#ifdef  LA_BP
	La_bytepos   la_bpos;	/* current byte position in file */
#endif/*LA_BP */
    } la_pos;
    struct la_file  *la_file;
    La_view	    *la_view;	/* pointer to stream-independent stuff */
    struct la_stream *la_sback;	 /* chain of all streams */
    struct la_stream *la_sforw;	 /* chain of all streams */
    struct la_stream *la_vback;  /* chain of streams into la_view */
    struct la_stream *la_vforw;  /* chain of streams into la_view */
    La_linepos	     la_rlines; /* reserved lines from cur position */
    char	     la_sflags; /* stream flags */
} La_stream;
/* la_sflags element of La_stream: */
#define LA_ALLOCED   001	/* open call alloced la_stream struc */
#define LA_STAY	     002	/* stay at same line after insert there */

#define la_active(plas)	    ((plas) && (plas)->la_file)
#define la_modified(plas)   (((plas)->la_file->la_mode & LA_MODIFIED)? YES : NO)
#define la_domodify(plas)   ((plas)->la_file->la_mode |= LA_MODIFIED)
#define la_unmodify(plas)   ((plas)->la_file->la_mode &= ~LA_MODIFIED)

#define la_reserved(plas)   ((plas)->la_rlines)
#define la_setrlines(plas,l) ((plas)->la_rlines = l)

#define la_stay(plas)	    (((plas)->la_sflags & LA_STAY) ? YES : NO)
#define la_stayset(plas)    ((plas)->la_sflags |= LA_STAY)
#define la_stayclr(plas)    ((plas)->la_sflags &= ~LA_STAY)

#define la_chan(plas)	    (ff_fd ((plas)->la_file->la_ffs))
#define la_nstreams(plas)   ((plas)->la_file->la_refs)
#define la_linepos(plas)    ((plas)->la_lpos)
#define la_bytepos(plas)    ((plas)->la_bpos)
#define la_eof(plas)	    ((plas)->la_pos.la_cfsd == (plas)->la_file->la_lfsd)
#define la_lsize(plas)	    ((plas)->la_file->la_nlines)
#define la_lmax(plas)	    ((plas)->la_file->la_maxline)
#define la_sync(flg)	    ff_sync (flg)
#define la_ltrunc(i) ((i) > LA_MAXNLINES ? LA_MAXNLINES : (i) < 0 ? 0 : (i))
#ifdef	LA_BP
#define la_bsize(plas)	    ((plas)->la_file->la_nbytes)
#else/* LA_BP */
extern La_bytepos la_bsize ();
#endif/*LA_BP */

extern La_linepos   la_align (), la_blank (), la_close (), la_finsert ();
extern La_linepos   la_lcollect (), la_lcopy (), la_lcount (), la_ldelete ();
extern La_linepos   la_lflush (), la_linsert (), la_lrcollect ();
extern La_linepos   la_lreplace (), la_lseek ();
extern La_linesize  la_clseek (), la_lget (), la_lpnt ();
extern La_linesize  la_lrsize (), la_lwsize ();
extern La_stream   *la_clone (), *la_open (), *la_other ();
extern int	    la_tcollect ();

#define LA_NEWLINE ('\n')

/*  If any of these is not supplied by the user,
/*  a default one will be loaded from the library.
/**/
extern void   la_abort();	/* called in some bug situations */
extern int    la_int();		/* is called periodically during some long  */
				/* operations, and if it returns non-0, the */
				/* operation is aborted */

/*  Either of these may be stored into by the user. */
extern int	   la_maxchans; /* maximum system opens allowed to la */
extern char	  *la_cfile;	/* the name to use for the changes file */
extern La_linesize la_maxline;	/* maximum line length allowed */
				/* affects la_open and la_lins, la_lrpl */

/* These are only to be modified by the library */
extern La_stream *la_chglas;	/* change file */
extern Ff_stream *la_chgffs;	/* change file */
extern La_flag	  la_chgopen;	/* YES means change file has been la_opened */
extern int	  la_chans;	/* number of system opens in use by la */
extern int	  la_nbufs;	/* how many cache buffers to use */
extern La_stream *la_firststream;/* first in chain of open La_streams */
extern La_stream *la_laststream;/* last in chain of open La_streams */
extern int	  la_errno;	/* last non-La_stream error that occurred */
extern La_flag	  la_colstate;
/* la_errno values: */
#define LA_BADBAD     2 /* bad stream ptr to la_error call */
#define LA_INT	      3 /* operation was interrupted */
#define LA_EOF	      4 /* attempt to read past EOF */
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
