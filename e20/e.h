/*
 * file e.h - "include" file used by all
 **/

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#ifdef GCC
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
/* #include <sys/resource.h> */
#define USE_MKSTEMP     /* instead of mktemp which is unsafe */
#endif /* GCC */

/* This is an arbitrary limit, E will handle more.
 * primarily for the error case in movewin(), e.t.c
 */
#define E_MAXNLINES 20000000


/* various options.  All should be turned on. */

/* for a recovery, show places in keystroke file to halt replay */
#define SHOWKEYS_INPLACE

#define RECORDING       /* simple record/playback facility */
#define STARTUPFILE     /* .e_profile startup mechanism */
#define TRACK           /* track command */
#define RANGE           /* range command */
#define NAME            /* name command */
/** ine DELETE          / * delete command */
#define KBFILE          /* -kbfile option */
#define SVINPLACE       /* in-place saves */
#define DUPCMD          /* duplicated commands on command line */
#define TABFILE         /* tabfile command */
#define ELOGIN          /* try to run as a login program */
/** ine DOCALL          / * call command -- should be defunct */
/** ine DOSHELL         / * shell command -- should be changed */
#define DOCOMMAND       /* command command */
#define BULLETS         /* border bullets */
#define NOTRACKS        /* -notracks option */
#define GETPATH         /* getpath routines, used by e.ru.c and tabfile */
#define LMCHELP         /* help function */
/** ine LMCVBELL        / * visible bell support */
#define LMCDWORD        /* delete word command */
#define LMCAUTO         /* power typing mode */
/**#define LMCGO           / * special case go command <CMD>###<RET> */
#define LMCCASE         /* CCASE and CCAPS commands */
/** #define LMCTRAK         / * make TRACK and RANGE commands toggle */
#define LMCMARG         /* tab and margin tics (cute) */
/** ine LMCLDC          / * Tcap.c uses line drawing defs for windows */
/** ine LMCSRMFIX       / * Replace "stuck at rt margin" with reasonable sub */
/** #define VBSTDIO		/ * Can't cheat by using _sobuf -- it doesn't exist */


/*
 * Our use of ncurses is primarily for low-level support of the mouse.
 * Curses is not (yet) used to handle screen management of text and windows.
 */

#include <ncurses.h>
#include <term.h>

#include <c_env.h>
#include <localenv.h>

#define NCURSES
#ifdef NCURSES_MOUSE_VERSION
#define NCURSES_MOUSE
#define MOUSE_BUTTONS

#define USER_FKEYS  /* ekbfile:  KEY_F[1-10]:<efunc>  (eg: <+page>) */

#ifdef NCURSES_MOUSE
#ifdef MOUSE_BUTTONS  /* experimental */
/* MOUSE_BUTTONS displays a row of E function keys (eg, "+Pg/-Pg") at the bottom
 * of the screen that can be useful for "hands-off" browsing through a file.
 * The option -showbuttons enables this feature.
 */

#define BUTTON_FONT  /* highlight the buttons in eg, bold */

typedef struct button_table {
    short bnum;
    short begx;
    short endx;
    int   ecmd;
    char *label;
} mouse_button_table;

/* force cursor position, used in highlighting buttons  */
#define MVCUR(l,c)      (*term.tt_addr)  (l,c)

/*
 * USE_MOUSE_POSITION enables mouse selecting and highlighting text.
 * In the 1980s, we called this "marking".
 */
#define USE_MOUSE_POSITION


#endif /* MOUSE_BUTTONS */
#endif /* NCURSES_MOUSE */
#endif /* NCURSEs */

#ifdef UNIXV7
#include <sys/types.h>
#endif

#include <la.h>     /* ff.h is included by la.h */

/* IMPORTANT: NOFILE is defined in ff.h -- BE SURE it is the true number of
 * file descriptors you are allowed to have open! */

#ifndef SYSV
#ifndef SOLARIS
#ifndef LINUX
extern char *sprintf ();
#endif
#endif
#endif

#define Block

/* phototypesetter v7 c compiler has a bug with items declared as chars
 * If the variable is used as an index i.e. inside [], then it is likely
 * to get movb'ed to a register, which is then used as the index,
 * including the undefined upper byte.  Also true of things declared
 * "register char"
 * By assigning "Z" to the register, a sign extension will occur
 **/

/* Char and Short are used to point out the minimum storage required for
 *   the item.  It may be that for a given compiler, int will take up no
 *   more storage and actually execute faster for one or both of these,
 *   so they should be defined to be ints for that case.
 *   Especially, note that defining these to be ints gets around bugs in
 *   both the Ritchie PDP-11 compiler and the Johnson VAX compiler regarding
 *   declaring types smaller than int in registers.
 **/
#define Char  int
#define UChar unsigned int
#define Short int

#define Uint unsigned int
#define Uchar unsigned char

#define Z 0  /* define to "zero" for the VAX compiler bug if Char and Short */
	     /* above aren't both defined to int */

/* begin section moved from e.t.h */

/* MAXWIDTH is the maximum of all defined widths in e.tt.h */
/*#define MAXWIDTH 260            / * length of longest possible screen line */
#define MAXWIDTH 223              /* length of longest possible screen line */
/* if (MAXWIDTH > 127)  then you must define WIDE
 **/
/* #undef  WIDE   */
#define WIDE
#ifdef WIDE
typedef Short Scols;            /* number of columns on the screen */
typedef short AScols;           /* number of columns on the screen */
#else
/* unfortunately, these cannot be unsigned chars */
typedef Char Scols;
typedef char AScols;
#endif
typedef Char Slines;            /* number of lines on the screen */
typedef short ASlines;          /* number of lines on the screen */

/* end section moved from e.t.h */

#include "e.t.h"

/* For each type there is Type and Atype.
 * use AType for array and structure member declarations,
 * and use Type everywhere else
 **/
typedef Char  Flag;             /* YES or NO */
typedef char  AFlag;            /* YES or NO */
typedef Char  Small;            /* small integer that will fit into char */
typedef char  ASmall;           /* small integer that will fit into char */
#ifdef LA_LONGFILES
typedef La_linepos Nlines;      /* number of lines in a file */
typedef La_linepos ANlines;     /* number of lines in a file */
#else
typedef La_linepos Nlines;      /* number of lines in a file */
typedef La_linepos ANlines;     /* number of lines in a file */
#endif
#ifdef LA_LONGLINES
typedef La_linesize Ncols;      /* number of columns in a line */
typedef La_linesize ANcols;     /* number of columns in a line */
#else
typedef int         Ncols;      /* number of columns in a line */
typedef La_linesize ANcols;     /* number of columns in a line */
#endif
typedef Char  Fd;               /* unix file descriptor */
typedef char  AFd;              /* unix file descriptor */
typedef Char  Fn;               /* index into files we are editing */
typedef Uchar AFn;              /* index into files we are editing */
typedef Small Cmdret;           /* comand completion status */
typedef unsigned long ulong;	/* God knows why this is needed */

#ifdef NOSIGNEDCHAR
# define Uchar char
# define UNSCHAR
#else  /*NOSIGNEDCHAR*/
# ifdef UNSCHAR
#  define Uchar unsigned char
# else  /*UNSCHAR*/
#  define Uchar char
# endif /*UNSCHAR*/
#endif /* NOSIGNEDCHAR */

#ifdef UNIXV6
#define wait        waita       /* wait(a) waita () */
#define sgttyb      sgtty
#define sg_ispeed   sg_ispd
#define sg_ospeed   sg_ospd
#define sg_flags    sg_flag
#define ALLDELAY    (NLDELAY | TBDELAY | CRDELAY | VTDELAY)
#define B4800       12
#define st_mode     st_flags
#define S_IREAD     IREAD
#define S_IWRITE    IWRITE
#define S_IEXEC     IEXEC
#define S_IFMT      IFMT
#define S_IFDIR     IFDIR
#define S_IFCHR     IFCHR
#define S_IFBLK     IFBLK
#define S_IFREG     0
#define SIGKILL     SIGKIL
#define SIGALRM     SIGCLK
#define SIGQUIT     SIGQIT
#endif

#ifdef  NDIR
#define FNSTRLEN 255            /* max allowable filename string length */
#else   /*NDIR*/
#define FNSTRLEN 14             /* max allowable filename string length */
#endif /* NDIR */

#define abs(a) ((a)<0?(-(a)):(a))
#define min(a,b) (((a) < (b))? (a): (b))
#define max(a,b) (((a) > (b))? (a): (b))
#define Goret(a) {retval= a;goto ret;}
#ifndef YES
#define YES 1
#define NO  0
#endif
/* #define feoferr(p)         (((p)->_flag&(_IOERR|_IOEOF))!=0) */
#define feoferr(p)    (feof(p) || ferror(p))


# ifdef UNIXV6
# define ENAME "/usr/local/bin/e" /* this will get exec'ed after forked shell */
# endif

/* #define RUNSAFE              / * see e.ru.c */
/* #define MESG_NO                 / * if messages to be disallowed during edit */
/* #define SIGARG               / * if signal# passed as arg to signal(SIG) */
#define LOGINNAME "E"
/* #define CLEARONEXIT          / * if defined, clears screen on exit    */
/* #define DEBUGDEF             / * misc debugging stuff                 */
# define sfree(c) free (c)
/* # define sfree(c) */

#define CATCHSIGS               /* ignore signals */
#define DUMPONFATAL             /* dump always on call to fatal (..)    */
#define FATALEXDUMP -1          /* "exit dump" command given */
#define FATALMEM 0              /* out of memory */
#define FATALIO  1              /* fatal I/O err */
#define FATALSIG 2              /* signal caught */
#define FATALBUG 3              /* bug */
#define LAFATALBUG 4            /* bug in LA Package */
#define DEBUGFILE   "e.dbg"
#define CTRLCHAR   (key < 040 || 0177 <= key)
#define PGMOTION (key==CCPLPAGE||key==CCPLLINE||key==CCMIPAGE||key==CCMILINE)

#define NHORIZBORDERS 2         /* number of horiz borders per window   */
#ifndef VBSTDIO
#define SMOOTHOUT               /* if defined, try to smooth output     */
				/* to terminal by doing flushing at     */
				/* line boundaries near end of buffer   */
#endif
#define TABCOL 8                /* num of cols per tab character        */

#define NENTERLINES 1           /* number of ENTER lines on screen      */
#define NINFOLINES 1            /* number of INFO lines on screen      */
#define NPARAMLINES (NENTERLINES + NINFOLINES)

#define FILEMODE 0644           /* mode of editor-created files         */

#ifdef OUT
#ifdef SYSSELECT
#define MAXTYP 128              /* # of modifying chars typed before    */
#else
#define MAXTYP  28              /* # of modifying chars typed before    */
#endif /* SYSSELECT */          /* keyfile buffer is flushed            */
#endif /* OUT */

#define MAXTYP 8                /* # of modifying chars typed before    */
				/* keyfile buffer is flushed            */

#define DPLYPOL 4               /* how often to poll dintrup            */
#define SRCHPOL 50              /* how often to poll sintrup            */
#define EXECTIM 5               /* alarm time limit for Exec function   */

#define NTABS 80

/* switches for mesg() note: rightmost 3 bits are reserved for arg count */
#define TELSTRT 0010    /* Start a new message                      */
#define TELSTOP 0020    /* End of this message                      */
#define TELCLR  0040    /* Clear rest of Command Line after message */
#define TELDONE 0060    /* (TELSTOP | TELDONE)                      */
#define TELALL  0070    /* (TELSTRT | TELDONE)                      */
#define TELERR  0100    /* This is an error message.                */
#define ERRSTRT 0110
#define ERRSTOP 0120    /* no more to write     */
#define ERRCLR  0140    /* clear rest of line   */
#define ERRDONE 0160
#define ERRALL  0170

extern Flag optstick;


/* workspace - two per window: wksp and altwksp
 **/
typedef struct workspace {
    La_stream las;              /* lastream opened for this workspace   */
    AFn      wfile;             /* File number of file - 0 if none      */
    ASlines clin;               /* cursorline when inactive             */
    AScols   ccol;              /* curorcol when inactive               */
    ANlines wlin;               /* line no of ulhc of screen            */
    ANcols  wcol;               /* col no of column 0 of screen         */
    ANlines rngline;            /* start line of search range */
    La_stream *brnglas;         /* beginning of range, if set */
    La_stream *ernglas;         /* end of range, if set */
    ASmall wkflags;
} S_wksp;
/* S_window flags: */
# define RANGESET 1                /* RANGE is set */

extern
S_wksp  *curwksp;


extern
La_stream *curlas;


/* The routine "read2" in e.t.c that gets characters for the editor
 * reads NREAD characters from the keyboard into a char array and then
 * takes them out one at a time.  If a macro is being executed, then
 * read2 is pointed at a char array that contains the keystrokes of the
 * macro.  When the macro buffer is exhausted, then read2 goes back to
 * reading the previous buffer; thus macros can be nested.
 **/
#define NREAD 32

typedef struct inp
{   int         icnt;   /* no. of chars left in the buffer */
    char       *iptr;   /* pointer into the current position in the buffer */
    char       *ibase;  /* where the base of the buffer is */
    struct inp *iprevinp;   /* pointer to previous structure for nesting */
} S_inp;

extern char  keybuf[NREAD];

#define STDIN   0
#define STDOUT  1
#define STDERR  2
#define MAXSTREAMS NOFILE
/* We need some channels for overhead stuff:
 *  0 - keyboard input
 *  1 - screen output
 *      change file
 *      (not yet: fsd file - future feature of la package)
 *      keystroke file
 *      replay input file
 *      pipe[0] --                     -- origfd
 *      pipe[1]  | for run    for save |  tempfd
 *      pipe[2] --                     --
 *      (pipe[2] not needed if RUNSAFE)
 **/
#ifdef RUNSAFE
#define WORKFILES 7
#else
#define WORKFILES 8
#endif
#define MAXOPENS (MAXSTREAMS - WORKFILES)
/*#define MAXFILES (MAXOPENS + 10)*/
#define MAXFILES (MAXOPENS + 20)

#ifdef VBSTDIO
char *outbuf;           /* stdout buf */
#endif

extern Fd nopens;

/* There is an entry in each of these arrays for every file we have open
 * for editing.  The correct type for an array index is an Fn.
 **/
#define NULLFILE  0     /* This is handy since workspaces are calloc-ed */
#define CHGFILE   1
#define PICKFILE  2     /* file where picks go. Gets special */
			/* consideration: can't be renamed, etc.        */

#define OLDLFILE  3     /* file where closes go. Gets special */
			/* consideration: can't be renamed, etc.        */
#define NTMPFILES 2

#define FIRSTFILE PICKFILE

extern La_stream     fnlas[];       /* first La_stream open for the file    */
extern char         *tmpnames[];    /* names for tmp files */
extern char         *names[];       /* current name of the file            */
extern char         *oldnames[];    /* if != 0, orig name before renamed   */
extern S_wksp        lastlook[];
extern short         fileflags[];
#define INUSE        1              /* this array element is in use        */
#define DWRITEABLE   2              /* directory is writeable              */
#define FWRITEABLE   4              /* file is writeable                   */
#define CANMODIFY  010              /* ok to modify the file               */
#define INPLACE    020              /* to be saved in place                */
#define SAVED     0100              /* was saved during the session        */
			/* A file can have no more than one of the
			 * following three bits set.
			 * The same name can appear in more than one fn,
			 *  but only in the following combinations:
			 *  names[i] (DELETED)    == names[j] (NEW)
			 *  names[i] (DELETED)    == names[j] (RENAMED)
			 *  oldnames[j] (RENAMED) == names[j] (NEW)
			 * If (NEW | DELETED | RENAMED) == 0
			 *   file exists and we are using it               */
#define NEW       0200  /* doesn't exist yet, we want to create it         */
#define DELETED   0400  /* exists, and we'll delete it on exit             */
#define RENAMED  01000  /* exists and we will rename it on exit            */
#define UPDATE   02000  /* update this file on exit */
#ifdef SYMLINKS
#define SYMLINK  04000  /* file pointed to by a symbolic link */
#endif /* SYMLINKS */
#ifdef FILELOCKING
#define FILELOCKED  010000  /* got exclusive lock */
#define CANTLOCK    020000  /* lock failed */
#endif /* FILELOCKING */

extern
Fn      curfile;

/* This array is indexed by unix file descriptor */

/*
 window - describes a viewing window with file
   all marg values, and ltext and ttext, are limits relative
	to (0,0) = ulhc of screen.  the other six limits are
	relative to ltext and ttext.
 **/
typedef struct window
{
    S_wksp *wksp;               /* workspace window is showing          */
    S_wksp *altwksp;            /* alternate workspace                  */
    ASmall  prevwin;            /* number of the ancester win           */
				/* boundaries of text within window     */
				/*  may be same as or one inside margins*/
    ASlines ttext;              /* rel to top of full screen            */
    AScols   ltext;             /* rel to left of full screen           */
    AScols   rtext;             /*  = width - 1                         */
    ASlines btext;              /*  = height - 1                        */
    ASlines tmarg;              /*  rel to upper left of full screen    */
    AScols   lmarg;             /* margins                              */
    AScols   rmarg;
    ASlines bmarg;
    ASlines tedit;
    AScols   ledit;             /* edit window limits on screen         */
    AScols   redit;
    ASlines bedit;
    AScols  *firstcol;          /* first col containing nonblank        */
    AScols  *lastcol;           /* last col containing nonblank         */
    unsigned char   *lmchars;            /* left margin characters               */
    unsigned char   *rmchars;            /* right margin characters              */
    AFlag   winflgs;            /* flags */
    Nlines  plline;             /* default plus a line         */
    Nlines  miline;             /* default minus a line        */
    Nlines  plpage;             /* default plus a page         */
    Nlines  mipage;             /* default minus a page        */
    Ncols   lwin;               /* default window left         */
    Ncols   rwin;               /* default window right        */
} S_window;
/* S_window flags: */
#define TRACKSET 1              /* track wksp and altwksp */

#define SWINDOW (sizeof (S_window)) /* size in bytes of window */

#define MAXWINLIST 40   /* should be a linked list - not an array */
extern
S_window       *winlist[MAXWINLIST],
	       *curwin,         /* current editing win                  */
		wholescreen,    /* whole screen                         */
		infowin,        /* window for info                      */
		enterwin,       /* window for CMD and ARG               */
		buttonwin;      /* window for function buttons          */
extern
Small   nwinlist;

#define COLMOVED    8
#define LINMOVED    4
#define WCOLMOVED   2
#define WLINMOVED   1
#define WINMOVED    (WCOLMOVED | WLINMOVED)
#define CURSMOVED   (COLMOVED | LINMOVED)

/*      savebuf - structure that describes a pick or delete buffer      */

typedef struct savebuf {
    La_stream buflas;
    ANcols  ncols;
} S_svbuf;


/* input control character assignments */

#define CCCMD           000 /* ^@ enter parameter       */
#define CCLWINDOW       001 /* ^A window left           */
#define CCSETFILE       002 /* ^B set file              */
#define CCINT           003 /* ^C interrupt         *** was chgwin */
#define CCOPEN          004 /* ^D insert                */
#define CCMISRCH        005 /* ^E minus search          */
#define CCCLOSE         006 /* ^F delete                */
#define CCMARK          007 /* ^G mark a spot       *** was PUT */
#define CCMOVELEFT      010 /* ^H backspace             */
#define CCTAB           011 /* ^I tab                   */
#define CCMOVEDOWN      012 /* ^J move down 1 line      */
#define CCHOME          013 /* ^K home cursor           */
#define CCPICK          014 /* ^L pick                  */
#define CCRETURN        015 /* ^M return                */
#define CCMOVEUP        016 /* ^N move up 1 lin         */
#define CCINSMODE       017 /* ^O insert mode           */
#define CCREPLACE       020 /* ^P replace           *** was GOTO */
#define CCMIPAGE        021 /* ^Q minus a page          */
#define CCPLSRCH        022 /* ^R plus search           */
#define CCRWINDOW       023 /* ^S window right          */
#define CCPLLINE        024 /* ^T minus a line          */
#define CCDELCH         025 /* ^U character delete      */
#define CCRWORD         026 /* ^V move right one word   */
#define CCMILINE        027 /* ^W plus a line           */
#define CCLWORD         030 /* ^X move left one word    */
#define CCPLPAGE        031 /* ^Y plus a page           */
#define CCCHWINDOW      032 /* ^Z change window     *** was WINDOW */
#define CCTABS          033 /* ^[ set tabs              */
#define CCCTRLQUOTE     034 /* ^\ knockdown next char   */
#define CCBACKTAB       035 /* ^] tab left              */
#define CCBACKSPACE     036 /* ^^ backspace and erase   */
#define CCMOVERIGHT     037 /* ^_ forward move          */
#define CCDEL          0177 /* <del>    -- not assigned --  *** was EXIT */
#define CCSTOP         0200 /* *@ stop replay           */
#define CCERASE        0201 /* *A erase                 */
#define CCUNAS1        0202 /* *B -- not assigned --    */
#define CCSPLIT        0203 /* *C split                 */
#define CCJOIN         0204 /* *D join                  */

#define CCEXIT         0205
#define CCABORT        0206
#define CCREDRAW       0207
#define CCCLRTABS      0210
#define CCCENTER       0211
#define CCFILL         0212
#define CCJUSTIFY      0213
#define CCCLEAR        0214
#define CCTRACK        0215
#define CCBOX          0216
#define CCSTOPX        0217
#define CCQUIT         0220
#define CCCOVER        0221
#define CCOVERLAY      0222
#define CCBLOT         0223
#define CCRANGE        0230
#define CCNULL         0231
#ifdef LMCHELP
#define CCHELP         0224
#else  /*CCHELP*/
#define CCHELP         CCUNAS1
#endif /* LMCHELP */
#ifdef LMCCASE
#define CCCCASE        0225
#define CCCAPS         0226
#else  /*LMCCASE*/
#define CCCCASE        CCUNAS1
#define CCCAPS         CCUNAS1
#endif /* LMCCASE */
#ifdef LMCAUTO
#define CCAUTOFILL     0227
#else  /*LMCAUTO*/
#define CCAUTOFILL     CCUNAS1
#endif /* LMCAUTO */

#ifdef LMCDWORD
#define CCDWORD        0232
#else
#define CCDWORD        CCUNAS1
#endif /* LMCDWORD */

#ifdef RECORDING
#define CCRECORD       0234
#define CCPLAY         0235
#endif

#ifdef NCURSES
#define CCMOUSE        0236
#define CCMOUSEONOFF   0237
#define CCBRACE        0240     /* toggle brace match mode */
#endif

#define CCPUT          0241     /* insert PICK buffer (button func) */
#define CCUNMARK       0242     /* unmark (for button funct) */
#define CCREGEX        0243     /* RE toggle (for button func) */

#define CCPLWIN        0244     /* +w (for button function) */
#define CCMIWIN        0245     /* -w (for button function) */
#define CCMICLOSE      0246     /* -close (for button function) */
#define CCMIERASE      0247     /* -er    (for button function) */
#define CCMIRECORD     0250     /* -record (for button function) */

#define CCHIGHEST      0250 /* nxt is 0251 */

extern
Scols   cursorcol;              /* physical screen position of cursor   */
extern
Slines  cursorline;             /*  from (0,0)=ulhc of text in window   */

/* chgborders is set outside putup (), then looked at by putup ()
 *                              then putup resets it to 1 */
extern Small chgborders;        /* 0: don't change the borders          */
				/* 1: update them */
				/* 2: set them to inactive (dots) */

extern
Small   numtyp;                 /* number of text chars since last      */
				/* keyfile flush                        */


#define MAXMOTIONS 32
extern
ASmall cntlmotions[MAXMOTIONS];
#define UP 1    /* Up           */
#define DN 2    /* Down         */
#define RN 3    /* Return       */
#define HO 4    /* Home         */
#define RT 5    /* Right        */
#define LT 6    /* Left         */
#define TB 7    /* Tab          */
#define BT 8    /* Backtab      */

extern
char   *myname,
       *mypath,
       *progname;
extern
Flag    inplace;                /* do in-place file updates?            */

extern
Flag    smoothscroll;           /* do scroll putups one line at a time */
extern
Flag    singlescroll;           /* do scrolling one line at a time */

extern ANcols *tabs;            /* array of tabstops */
extern Short   stabs;           /* number of tabs we have alloced room for */
extern Short   ntabs;           /* number of tabs set */
extern
char    *blanks;

/* Argument to getkey is one of these: */
#define WAIT_KEY      0 /* wait for a char, ignore interrupted read calls. */
#define PEEK_KEY      1 /* peek for a char */
#define WAIT_PEEK_KEY 2 /* wait for a char, then peek at it;      */
			/* if read is interrupted, return NOCHAR. */

#define NOCHAR 0400
extern Char key;             /* last char read from tty */
extern Flag keyused;         /* last char read from tty has been used */

/* default parameters */
extern Nlines defplline,        /* default plus a line          */
	      defplpage,        /* default minus a line         */
	      defmiline,        /* default plus a page          */
	      defmipage;        /* default minus a page         */
extern Ncols  deflwin,          /* default window left          */
	      defrwin;          /* default window right         */
extern char  deffile[];         /* default filename             */
extern Fn    deffn;             /* file descriptor of default file      */

extern
Short   linewidth,              /* used by just, fill, center           */
	tmplinewidth;           /* used on return from parsing "width=xx" */
#ifdef LMCAUTO
extern Flag autofill;           /* YES if in autofill mode */
extern Ncols autolmarg;         /* left margin on autofill */
#endif

extern
char   *paramv;                 /* * globals for param read routine     */
extern
Small   paramtype;

/* array to hold current line being edited */
/* tabs are expanded on input. */
extern
char   *cline;                  /* array holding current line           */
extern
/* trw: nxt 3 were Short*/
Ncols   lcline,                 /* length of cline buffer               */
	ncline,                 /* number of chars in current line      */
	icline;                 /* increment for next expansion         */
extern
Flag    fcline,                 /* flag - has line been changed ?       */
	cline8,                 /* line may contain chars with 8th bit set */
	ecline,                 /* line may contain ESCCHAR(s)          */
	xcline;                 /* cline was beyond end of file         */
extern
Fn      clinefn;                /* Fn of cline                          */
extern
Nlines  clineno;                /* line number in workspace of cline    */
extern
La_stream *clinelas;            /* La_stream of current line */

extern char
	prebak[],               /* text to precede/follow               */
	postbak[];              /* original name for backup file        */

extern
char *searchkey;

/* trw, add nxt */
#define _UID_T

#ifdef _UID_T
extern uid_t userid, groupid;
#else
#ifdef UID_IS_CHAR
extern char userid, groupid;
#else
extern short userid, groupid;
#endif /* UID_IS_CHAR */
#endif /* _UID_T */

extern
FILE   *keyfile;                /* channel number of keystroke file     */

extern
Fd      inputfile;              /* channel number of command input file */
extern
Flag    intok;                  /* enable la_int ().  Normally NO, */
				/* set to YES for duration desired */
extern
Small   intrupnum;              /* how often to call intrup             */
extern
Flag    alarmed;

#define NUMFFBUFS       10

/*extern char putscbuf[];*/
extern Uchar putscbuf[];

extern Flag windowsup;   /* screen is in use for windows */

#define d_put(c) (putscbuf[0]=(c),d_write(putscbuf,1))

extern
FILE   *dbgfile;
extern
FILE   *replay_fp;

extern short revision;  /* revision number of this e */
extern short subrev;    /* sub-revision number of this e */

extern
Char evrsn;   /* the character used in the chng, strt, & keys filename   */
		/* '0', '1', ...        */

extern Flag notracks;   /* don't use or leave any strt file */
extern Flag norecover,
	    replaying,
	    recovering,
	    silent;     /* replaying silently */
extern Flag keysmoved;  /* keys file has been moved to backup */
#ifdef LMCHELP
extern Flag nokeyfile;  /* don't record strokes in key file */
#endif
extern char *keycaps[CCHIGHEST+1-127+32];
extern char *cmdkeycaps[CCHIGHEST+1-127+32];

/*********/
/* e.mouse.c */

extern Flag initMouseDone;
extern Flag initCursesDone;
extern Flag bs_flag;

/* e.d.c */

#ifdef LMCVBELL
extern char *vbell;
extern Flag VBell;
#endif /* LMCVBELL */
extern Flag NoBell;
extern int DebugVal;

int d_vmove (Slines, Scols, Slines, Scols, int, Flag);

/* these used to be in e.c only */


extern
Flag cmdmode;
extern
Flag litmode;           /* is YES when in literalmode Added 2/8/83 MAB */
extern
Flag patmode;           /* is YES when in patternmode (RE) Added 10/18/82 MAB */
extern
Flag insmode;           /* is YES when in insertmode                      */
extern
Nlines parmlines;       /* lines in numeric arg                         */
extern
Ncols parmcols;         /* columns in numeric arg e.g. 8 in "45x8"      */

extern
char *shpath;

typedef struct lookuptbl
{   char *str;
    short val;
} S_looktbl;

extern
long strttime;  /* time of start of session */

extern
Flag loginflg;  /* = 1 if this is a login process */

extern Flag ischild;    /* YES if this is a child process */

extern int zero;
extern Flag uptabs, upblanks, upnostrip;
extern void tgltabmode (void), tglinsmode (void), tglpatmode (void),
    tglblamode (void), tgllitmode(void), tglstrmode (void);
extern void d_init (Flag, Flag);

#ifdef LMCLDC

/* file e.tt.c */

extern
Flag line_draw;       /* we are in line drawing mode */

#endif /* LMCLDC */
/* Functions */

/* ../lib/getshort.c */
extern short getshort (FILE *);
extern void putshort (short, FILE *);

/* ../lib/getlong.c */
extern long getlong (FILE *);
extern long putlong (long, FILE *);

/* e.c */
extern _Noreturn void getout (Flag, char *, ...);
extern void checkargs (void);
extern void startup (void);
extern void showhelp (void);
extern void gettermtype (void);
extern void setitty (void);
extern void setotty (void);
extern void makescrfile (void);
extern void infoinit (void);
extern Small getcap (char *);
#ifdef LMCAUTO
extern void infoint0 (void);
#endif /* LMCAUTO */
#ifdef  KBFILE
extern void getkbfile (char *);
#endif /* KBFILE */

/* e.cm.c */
extern Cmdret command (int, char *);
extern Cmdret gotocmd ();
extern Cmdret my_doupdate (Flag);
extern char *highlight_info_str;
extern int doSetBraceMode (char *);
/* e.dif.c */
extern Cmdret diff (Small);
/* e.e.c */
extern Cmdret areacmd (Small);
extern Cmdret splitmark (void);
extern Cmdret splitlines (Nlines, Ncols, Nlines, Ncols, Flag);
extern Cmdret joinmark (void);
extern Cmdret split (void);
extern Cmdret join (void);
Cmdret ed (Small, Small, Nlines, Ncols, Nlines, Ncols, Flag);
Cmdret edmark (Small, Small);
/* e.f.c */
extern Flag multlinks (char *);
extern Flag fmultlinks (Fd);
extern Fn hvname (char *);
extern Fn hvoldname (char *);
extern Fn hvdelname (char *);
extern int dircheck (char *, char **, char **, Flag, Flag);
extern int dirncheck (char *, Flag, Flag);
extern int filetype (char *);
extern int fgetpriv (Fd);
/* e.iit.c */

/* e.la.c */
extern void GetLine (Nlines);

extern Flag putline (void);
extern void chkcline (void);
extern void shortencline (void);

extern Ncols dechars (char *);
extern Flag extend (Nlines);
extern Nlines lincnt (Nlines, Nlines, Flag);
extern void excline (Ncols);
/* e.mk.c */
extern Nlines topmark (void);
extern Ncols leftmark (void);
extern Flag gtumark (Flag);
extern Small exchmark (Flag);
extern int Pch(int);
extern void markprev (void);
extern int FindBraceBack(int, int, int, long *, int *);
extern int FindBraceForw(int, int, int, long *, int *);
extern int win_has_eof(void);
/* e.nm.c */
extern Cmdret name (void);
extern Cmdret delete ();
extern Flag dotdot ();
/* e.p.c */
extern Small printchar (void);
extern void infoauto (Flag);
extern Cmdret dodword (int);
Cmdret setwordmode (char *);
extern Small mword (int, int);
/* e.pa.c */
extern Small getpartype (char **, Flag, Flag, Nlines);
extern char *getword (char **);
extern Cmdret scanopts (char **, Flag, S_looktbl *, int (*)());
extern Small getopteq ();
extern Small doeq (char **, int *);
extern int lookup (char *, S_looktbl *);
/* e.put.c */
extern Cmdret insert ();
extern Cmdret insbuf ();
/* e.q.c */
extern Cmdret eexit (void);
extern Flag saveall (void);
extern Flag savestate (void);
/* e.ra.c */
extern Cmdret rangecmd (Small);
extern Nlines rangelimit (Nlines, Small, Nlines);
extern void showrange ();
/* e.re.c */
extern char *re_comp (char *);
extern char *re_exec (char *);
extern char *re_replace (char *);
extern int re_len (void);
extern Small patsearch (char *, Nlines, Ncols, Nlines, Small, Flag);
/* e.ru.c */
extern Cmdret print ();
extern Cmdret my_filter (Small, Flag);
extern Cmdret filtmark (Small, Flag);
extern Cmdret filtlines (Small, Nlines, Nlines, Flag, Flag);
extern Cmdret parsauto (Flag);
extern Cmdret run (char *, Short);
extern Cmdret runlines(char *,char*,char **,Nlines,Nlines,Small,Flag,Flag,Flag);
#ifdef RUNSAFE
extern Flag dowait (int);
#else
extern Flag dowait (int,int,int);
#endif
extern Flag receive (long, Nlines, Nlines, Small, Flag);
/* e.sb.c */
extern void getpath (char *, char **, Flag);
extern char *getmypath (void);
extern char *gsalloc (char *, int, int, Flag);
extern char *salloc (Ncols, Flag);
extern char *okalloc (int);
extern char *append (char *, char *);
extern char *copy (char *, char*);
extern char *s2i (char *, long *);
extern char *itoa();
extern Flag mv (char *, char *);
extern Flag okwrite (void);
extern Small filecopy (char *, Fd, char *, Fd, Flag, int);
extern void sig (int);
extern void srprintf ();
extern Flag islocked (Flag);
/* e.se.c */
extern Cmdret replace (Small);
extern Small dsplsearch (char *, Nlines, Ncols, Nlines, Small, Flag, Flag);
extern Small strsearch (char *, Nlines, Ncols, Nlines, Small, Flag);
extern Ncols skeylen (char *, Flag, Flag, Flag);
extern int zaprpls (void);
/* e.sv.c */
extern Cmdret save (void);
extern Flag savefile (char *, Fn, Flag, Flag);
extern Flag svrename (Fn);
/* e.t.c */
extern Small vertmvwin (Nlines);
extern Small horzmvwin (Ncols);
extern Small movewin (Nlines, Ncols, Slines, Scols, Flag);
extern unsigned Short getkey (Flag, struct timeval *);
extern unsigned Short mGetkey (Flag, struct timeval *);
extern Flag dintrup ();
extern Flag la_int();
extern Flag sintrup (void);
extern void setmarg (Ncols *, Ncols);
extern void offbullets (void);
extern void dobullets (Flag, Flag);

/* e.tb.c */
extern Cmdret dotab (Flag);
extern Cmdret dotabs (Flag);
/*extern Small getptabs ();*/
extern Cmdret tabfile (Flag);
extern Flag gettabs (char *, Flag);
/* e.u.c */
extern Cmdret use ();
extern Cmdret edit (void);
extern Small editfile (char *, Ncols, Nlines, Small, Flag);
/*extern Fn getnxfn ();*/
/* e.wi.c */
extern S_window *setupwindow (S_window *,Scols,Slines,Scols,Slines,Flag,Flag);
extern Cmdret makewindow (char *);
/* e.wk.c */
extern Flag swfile (void);
extern void switchfile (void);

extern void chgwindow (Small);
extern void clean ();
extern void cleanup (Flag, Flag);
extern void clrbul (void);
extern void credisplay (Flag);
extern void d_write ();
extern void dbgpr (char *, ...);
extern void dobullets ();
extern void dosearch ();
extern void dostop (void);
extern void drawborders ();
extern void eddeffile ();
extern void edscrfile ();
extern void exchgwksp (Flag);
extern void excline ();
_Noreturn extern void fatal (Flag, char*, ...);
extern void fatalpr (char *, ...);
extern void fixtty (void);
extern void flushkeys (void);
extern void fsynckeys (void);
extern void fresh (void);
extern void getarg (void);
extern ssize_t getline ();	/* type changed from "void" 2/11/21 MOB */
extern void getpath ();
extern void gotomvwin (Nlines);
extern void igsig ();
extern void info (Scols, Scols, char *);
extern void inforange (Flag);
extern void infoprange (Nlines);
extern void infotrack (Flag);
extern void limitcursor (void);
extern void mainloop (void);
extern void mark (void);
/*extern void mesg ();*/
extern void mesg (int, ...);
extern void movecursor (Small, Nlines);
extern void multchar (Char, Scols);
extern void param (void);
extern void poscursor (Scols, Slines);
extern void putbks ();
extern void putch (int, Flag);
extern void putup (Slines, Slines, Scols, Scols);
extern void putupwin (void);
extern void redisplay (Fn, Nlines, Nlines, Nlines, Flag);
extern void releasewk (S_wksp *);
extern void removewindow (void);
extern void replkey (void);
extern void burncurs (void);
extern void restcurs (void);
extern void savecurs (void);
extern void savewksp (S_wksp *);
extern void screenexit (Flag);
extern void sctab (Ncols, Flag);
extern void setbul (Flag);
extern void setitty ();
extern void setotty ();
extern void switchfile ();
extern void switchwindow ();
extern void tabevery ();
extern void tglpatmode (void);      /* added MAB */
extern void tglinsmode (void);
extern void unmark (void);
extern void writekeys (int, char *, int);

#ifdef  STARTUPFILE
/* e.profile.c */
extern Flag dot_profile;
extern FILE *fp_profile;
#define NORUNCMD_IN_PROFILE /* don't allow <cmd>run xxx<ret> in .e_profile */
#endif /* STARTUPFILE */

#ifdef RECORDING
/* e.record.c */
extern Flag recording, playing;
extern Uchar *rec_p, *rec_text;
extern int rec_size, rec_count, rec_len, play_count;
extern Flag play_silent;
extern Cmdret SetRecording (Short);
extern void RecordChar (unsigned Short);
extern Cmdret UnSetRecording (void);
extern Cmdret PlayRecording (int);
extern Uchar *PlayChar(Flag);
extern Cmdret DoMacro (char *, char *);
extern Cmdret UndefMacro (char *);
extern Cmdret StoreMacro (char *);
extern void SaveMacros (void);
extern void ReadMacroFile (void);
extern Cmdret ShowMacros (char *);

#endif

extern char *etcdir;
extern char *keytmp;

extern char *my_move (char *, char *, unsigned long);

extern Flag noresizeall;

