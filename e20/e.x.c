/*
 * file e.x.c: external definitions
 **/

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.e.h"
#include "e.inf.h"
#include "e.m.h"
#include "e.sg.h"
#include SIG_INCL

#ifdef VBSTDIO
char *outbuf;           /* stdout buf */
#endif

Fd nopens;

La_stream     fnlas[MAXFILES];  /* first Lastream open for the file   */
char      *tmpnames[NTMPFILES] = {       /* similarly as for tmplas */
    "#p", "#o"
};

char      *names[MAXFILES];
char      *oldnames[MAXFILES];
S_wksp     lastlook[MAXFILES];
short      fileflags[MAXFILES];


S_wksp  *curwksp;

La_stream *curlas;


Fn        curfile;

S_window       *winlist[MAXWINLIST],
	       *curwin,         /* current editing window               */
		wholescreen,	/* whole screen 			*/
		infowin,        /* window for info                      */
		buttonwin,      /* window for func buttons              */
		enterwin;       /* window for CMD                       */
Small   nwinlist;

S_svbuf qbuf[NQBUFS];

Flag optstick = NO;

/* see e.e.h */
AFn qtmpfn[NQBUFS] = {
    OLDLFILE,   /* QADJUST  */
    PICKFILE,   /* QPICK    */
    OLDLFILE,   /* QCLOSE   */
    OLDLFILE,   /* QERASE   */
    OLDLFILE,   /* QRUN     */
};


Scols   cursorcol;              /* physical screen position of cursor   */
Slines  cursorline;             /*  from (0,0)=ulhc of text in window   */

Small   chgborders = 1;

Small   numtyp;                 /* number of text chars since last      */
				/* keyfile flush			*/

/* table of motions for control chars (see defs in e.h)
	UP  1	up
	DN  2	down		 : 4 or less change cursorline
	RN  3	carriage return
	HO  4	home
	RT  5   right
	LT  6   left
	TB  7   tab
	BT  8   backtab
 **/
ASmall  cntlmotions[MAXMOTIONS] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    LT, TB, DN, HO, 0, RN, UP, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, BT, 0, RT,
};

char   *myname,
       *mypath,
       *progname;
Flag	inplace;		/* do in-place file updates?		*/
Flag    uptabs;                 /* replace all blanks by tabs           */
Flag    upblanks;               /* replace all tabs by blanks           */
Flag    upnostrip;              /* don't strip trailing blanks, tabs    */

Flag    smoothscroll = NO;      /* do scroll putups one line at a time */
Flag    singlescroll = YES;     /* do scrolling one line at a time */

ANcols *tabs;                   /* array of tabstops */
Short   stabs = NTABS;          /* number of tabs we have alloced room for */
Short   ntabs = NTABS / 2;      /* number of set tabs */

char    *blanks;

Char    key;                /* last char read from tty */
Flag    keyused = YES;      /* last char read from tty has been used. */

/* parameters for line motion buttons */
Nlines  defplline = 10,         /* default plus a line         */
	defmiline = 10,         /* default minus a line        */
	defplpage = 1,          /* default plus a page         */
	defmipage = 1;          /* default minus a page        */
Ncols   deflwin  = 16,          /* default window left         */
	defrwin  = 16;          /* default window right        */
char    deffile[] = "errmsg";   /* default filename            */
Fn      deffn     = -1;         /* file descriptor of default file */

#ifdef LMCAUTO
Short   linewidth    = 74;      /* used by just, fill, center */

#ifdef WPMODESET
Flag    autofill = YES;         /* YES indicates autofill mode */
#else /* WPMODESET */
Flag    autofill = NO;          /* YES indicates autofill mode */
#endif /* WPMODESET */

Ncols   autolmarg = 0;          /* left margin on autofill */
#else /* LMCAUTO */
Short   linewidth    = 75;      /* used by just, fill, center */
#endif /* LMCAUTO */
Short   tmplinewidth;           /* used on return from parsing "width=xx" */

char   *paramv;                 /* * globals for param read routine     */
Small   paramtype;





/* initialize cline to be empty */
char   *cline = NULL;			/* array holding current line		*/

/* nxt 3 were Short */
Ncols   ncline = 0;                 /* number of chars in current line      */
Ncols   lcline = 0;                 /* length of cline buffer               */
Ncols   icline = 128;           /* initial increment for expansion */

Flag    fcline = 0,                 /* flag - has line been changed ?       */
	ecline = 0,                 /* line contains DEL escapes            */
	cline8 = 0,                 /* line may contain chars with 8th bit set */
	xcline = 0;                 /* cline was beyond end of file         */

Fn      clinefn;                /* Fn of cline                          */
Nlines  clineno = 0;                /* line number in workspace of cline    */
La_stream *clinelas = (La_stream *)NULL;            /* La_stream of current line */

/* One of the following strings must be non-null.
 * That is to say you can NOT disable the backups feature
 * If you try to do so, things won't work because fsd's wil point to oblivion.
 * It's too involved to go into here.  Has to do with the way the LA package
 * works. */
char    prebak[]  = ",",        /* text to precede/follow         */
	postbak[] = "";          /* original name for backup file  */

char *searchkey;

#ifdef _UID_T
uid_t  userid, groupid;
#else
#ifdef UID_IS_CHAR
char
#else
short
#endif /* UID_IS_CHAR */
    userid,
    groupid;
#endif /* _UID_T */

char *la_cfile;

FILE   *keyfile;		/* channel number of keystroke file	*/


Fd	inputfile;		/* channel number of command input file */

Flag    intok;                  /* enable la_int () */
Small   intrupnum;              /* how often to call intrup             */



Flag	alarmed;


/*char    putscbuf[10]; trw, 8.28.10 */
Uchar    putscbuf[10];

Flag windowsup = NO;

FILE   *dbgfile = NULL;
FILE   *replay_fp = NULL;

Char evrsn;   /* the character used in the chng, strt, & keys filename   */
		/* '0', '1', ...	*/


Flag notracks = NO;
Flag norecover = NO;
Flag replaying = NO;
Flag recovering = NO;
Flag silent = NO;       /* replaying silently */
Flag keysmoved = NO;    /* keys file has been moved to backup */

/*********/
/* e.d.c */

#ifdef LMCVBELL
char *vbell = (char *)0;
Flag VBell = NO;
#endif /* LMCVBELL */
Flag NoBell = NO;
int DebugVal = 0;

/************/
/* e.fn.h */
/* pathnames for standard files */
char   *tmppath   = "/tmp/etmp/"; /* the x will be replaced with */
#ifdef UNIXV7
char   *ttynstr;
#endif
#ifdef UNIXV6
char   *ttynstr   = "/dev/tty ";
#endif
char    scratch[] = "scratch";

char    tmpnstr[] = "c1";    /* The 1 may be replaced with a higher digit */
char    keystr[]  = "k1";
char    bkeystr[] = "k1b";
char    rstr[]    = "s1";

char   *keytmp,
       *bkeytmp,
       *rfile,          /* strt file name and backup name */
       *inpfname;

/************/
/* e.sg.h */

#ifdef  SYSIII
struct termio in_termio, out_termio;     /* System III ioctl */
int fcntlsave;
Flag cbreakflg = YES;

#else /* SYSIII */
struct sgttyb instty, outstty;


#ifdef  CBREAK
#ifdef  TIOCGETC
struct tchars spchars;
Flag cbreakflg = YES;
#else /* TIOCGETC */
Flag cbreakflg = NO;    /* can't turn off intr and quit without TIOCGETC */
#endif /* TIOCGETC */
#ifdef  TIOCGLTC
struct ltchars lspchars;
#endif /* TIOCGLTC */
#endif /* CBREAK */

#endif /* SYSIII */

Flag istyflg,
     ostyflg;

unsigned Short oldttmode;

/************/

Flag cmdmode;

Flag litmode;           /* is 1 when in literalmode Added 2/8/83 MAB */

Flag patmode;           /* is 1 when in patternmode Added 10/18/82 MAB */

Flag insmode;           /* is 1 when in insertmode */

Nlines parmlines;       /* lines in numeric arg */

Ncols parmcols;         /* columns in numeric arg e.g. 8 in "45x8"      */

char *shpath;

long strttime;	/* time of start of session */

Flag loginflg;  /* = 1 if this is a login process */

Flag ischild;

/************/
/* e.m.h */
char *cmdname;           /* the full name of the command given */
char *cmdopstr;
char *opstr;
char *nxtop;

/**********/
/* e.p.c  */
char *deletdwd = (char *) 0;

struct loopflags loopflags;

struct markenv *curmark,
	       *prevmark;

char    mklinstr [6],   /* string of display of num of lines marked */
	mkcolstr [6];   /* string of display of num of lines marked */

Small   infoarealen;    /* len of string of marked area display */

/************/
/* e.inf.h */

Scols inf_insert;               /* "INSERT" */
#ifdef RECORDING
Scols inf_record;               /* "REC" */
#endif
Scols inf_track;                /* "TRK" */
Scols inf_range;                /* "=RANGE" */
#ifdef LMCAUTO
Scols inf_auto;                 /* "WP" */
#endif
Scols inf_mark;                 /* "MARK" */
Scols inf_area;                 /* "30x16" etc. */
Scols inf_pat;                  /* "RE" */
Scols inf_tab;                  /* "TAB"/"LIT"/"BLA" */
Scols inf_str;                  /* "NOS" */
Scols inf_at;                   /* "At"         */
Scols inf_line;                 /* line         */
Scols inf_in;                   /* "in"         */
Scols inf_file;                 /* filename     */

Nlines  infoline;       /* line number displayed */
Fn      infofile;       /* file number of filename displayed */

Nlines  marklines;      /* display of num of lines marked */
Ncols   markcols;       /* display of num of columns marked */

/************/
/* file e.ru.c */

char *filters[] = {
    "fill",
    "just",
    "center",
    "print"
};

#ifdef LMCLDC
/* file e.tt.c */

Flag line_draw = NO;  /* we are in line drawing mode */
#endif /* LMCLDC */

#ifdef BSD
/* #ifdef sun	/ * more than just suns don't have _sobuf */
unsigned char _sobuf[BUFSIZ];
/* #endif / * sun */
#endif /* BSD */

#ifdef LMCHELP
Flag nokeyfile    = NO;
#endif /* LMCHELP */


#ifdef RECORDING
char *keycaps[] = {
		"cmd",          /* 00 */
		"wleft",        /* 01 */
		"edit",         /* 02 */
		"int",          /* 03 */
		"open",         /* 04 */
		"-sch",         /* 05 */
		"close",        /* 06 */
		"mark",         /* 07 */
		"left",         /* 10 */
		"tab",          /* 11 */
		"down",         /* 12 */
		"home",         /* 13 */
		"pick",         /* 14 */
		"ret",          /* 15 */
		"up",           /* 16 */
		"insmd",        /* 17 */
		"replace",      /* 20 */
		"-page",        /* 21 */
		"+sch",         /* 22 */
		"wright",       /* 23 */
		"+lines",       /* 24 */
		"dchar",        /* 25 */
		"+word",        /* 26 */
		"-lines",       /* 27 */
		"-word",        /* 30 */
		"+page",        /* 31 */
		"chwin",        /* 32 */
		"srtab",        /* 33 */
		"cchar",        /* 34 */
		"-tab",         /* 35 */
		"bksp",         /* 36 */
		"right",        /* 37 */
		"del",          /* 177 */
		"X",            /* 200 */
		"erase",        /* 201 */
		"X",            /* 202 */
		"split",        /* 203 */
		"join",         /* 204 */
		"exit",         /* 205 */
		"abort",        /* 206 */
		"redraw",       /* 207 */
		"cltabs",       /* 210 */
		"center",       /* 211 */
		"fill",         /* 212 */
		"justify",      /* 213 */
		"Uclear",       /* 214 */
		"track",        /* 215 */
		"box",          /* 216 */
		"stopx",        /* 217 */
		"quit",         /* 220 */
		"cover",        /* 221 */
		"overlay",      /* 222 */
		"blot",         /* 223 */
		"help",         /* 224 */
		"ccase",        /* 225 */
		"caps",         /* 226 */
		"wp",           /* 227 */
		"range",        /* 230 */
		"X",            /* 231 */
		"dword",        /* 232 */
		"X",            /* 233 */
		"record",       /* 234 */
		"play"          /* 235 */
};
#endif /* RECORDING */

char *etcdir;

