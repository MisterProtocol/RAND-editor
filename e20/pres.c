#ifdef COMMENT

    pres - print E state file

    See State.fmt for a description of the latest state file format.

    David Yost 3/79 and later

    pres [file]...

    An argument consisting of '-' by itself means read the original
      standard input, and is treated as a file argument, not an option.

    If no files are specified, use stdin as input.
    If files are specified, use them as input.

    Default is to check access to all files and report those which can\t
      read, then to go ahead with those that can.  This behavior can
      be changed by setting the appropriate global flags below.
    Buffers output and input.

    Exits 0 if done OK.
    Exits -1 if error encountered and nothing touched.
    Exits -2 if error encountered after doing some output.

#endif

/******************* standard filter declarations ************************/
#include "e.h"
#include <sys/stat.h>
#include <time.h>
/*  #include <stdio.h>  is already included by e.h above */
/* _sobuf isn't used by stdio in modern times so this hack
 * doesn't work any more.
  #ifdef sun
  char _sobuf[BUFSIZ];
  #else
  extern unsigned char _sobuf[];
  #endif  / * sun * /
 */

char _sobuf[BUFSIZ];

FILE *tfopen ();

void getoptions();
void filterfiles();
void my_local_filter();
void my_getout();
void my_getprogname();
void getoptions();
void stop();
void setraw();
void badstart();
void doit();
int  intss();
void badstart();
void doscrsize();
void dotime();
void dofill();
void dosrch();
void doinmode();
void dooldmark();
char dowindow();
void sepwindows();
void domargins();
void dooldalt();
void dooldfile();
void dotermtype();
void dorexp();
void domodes();
void domark();
void doauto();
void dotrack();
void dosets();
void doalt();
void dofile();
void my_dotabs();
void doinplace();
void dokbfile();
void dotermname();
void docolors();
void domouse();
void dobrace();
void dohighlight();
void doinplace();

#define STATSIZE 100     /* max num of characters returned by stat call */
#define BELL 07
/* #define RAWMODE      / * define this if raw mode is needed               */
			/* NOT FIXED FOR UNIXV7 yet */
#ifdef UNIXV7
#include <sgtty.h>
#else
#include <sys/sgtty.h>
#endif
#ifdef RAWMODE
struct sgttyb instty;
#endif
int rawflg = 0;         /* input is set to raw mode                        */
int flushflg = 0;       /* means a user is sitting there waiting to see    */
			/*   the output, so give it to him as soon as it   */
			/* is ready                                        */

int numargs,            /* global copy of argc                             */
    curarg;             /* current arg to look at                          */
char **argarray,        /* global copy of argv                             */
     *curargchar,       /* current arg character to look at                */
     *progname;         /* global copy of argv[0] which is program name    */

int opterrflg = 0,      /* option error encountered                          */
    badfileflg = 0;     /* bad file encountered                            */

int opt_abort_flg = 1,  /* abort entirely if any option errors encountered */
    opt_stop_flg = 0;   /* stop processing at first option error           */
			/* do not turn this on if opt_abort_flg is on      */

int fil_abort_flg = 0,  /* abort entirely if any bad files encountered     */
    fil_stop_flg = 0;   /* stop processing at first bad file               */
			/* do not turn this on if fil_abort_flg is on      */

int show_errs_flg;      /* print error diagnostics to stderr               */
int output_done = 0;    /* set to one if any output done                   */

FILE *input;
/********************* end of standard filter declarations *****************/


/**/
int
main(argc, argv)
int argc;
char **argv;
{
    numargs = argc;
    curarg = 0;
    argarray = argv;
    curargchar = argarray[curarg];

    input = stdin;
    my_getprogname();

    opterrflg = badfileflg = 0;
    show_errs_flg = 0;

    do {
	getoptions(1);          /* check for option errors      */
	filterfiles(1);         /* check for bad files          */
    } while (curarg < numargs);
    if ( (opterrflg && !opt_stop_flg) || (badfileflg && !fil_stop_flg) ) {
	curarg = 1;
	opterrflg = badfileflg = 0;
	show_errs_flg = 1;
	do {
	    getoptions(1);          /* check for option errors      */
	    filterfiles(1);         /* check for bad files          */
	} while (curarg < numargs);
	if ( (opterrflg && opt_abort_flg) || (badfileflg && fil_abort_flg) ) {
	    fprintf(stderr,"%s: not performed.\n",progname);
	    my_getout (-2);
	}
    }

    curarg = 1;
    opterrflg = badfileflg = 0;

    if (!intss())
	setbuf(stdout, (char *)_sobuf);
    do {
	opterrflg = 0;
	show_errs_flg = opt_stop_flg;
	getoptions(0);
	if (opterrflg && opt_stop_flg)
	    stop ();
	badfileflg = 0;
	show_errs_flg = fil_stop_flg;
	filterfiles(0);
	if (badfileflg && fil_stop_flg)
	    stop ();
    } while (curarg < numargs);
    my_getout (0);
}


void
my_getprogname()
{
    register char *cp;
    register char lastc = '\0';

    progname = cp = argarray[0];
    for (; *cp; cp++) {
	if (lastc == '/')
	    progname = cp;
	lastc = *cp;
    }
    curarg++;

    /******************************************************************/
    /**/
    /**/    /* determine what to do depending on prog name here */
    /**/
    /******************************************************************/
}


void
getoptions(check_only)
{
    register char *p;

    for (; curarg<numargs; curarg++) {
	curargchar = argarray[curarg];
	if (curargchar[0] != '-')           /* not an option arg */
	    return;
	if (curargchar[1] == '\0')          /* arg was '-' by itself */
	    return;

	p = ++curargchar;
	for (;; p++,curargchar++) {
	    switch (*p) {
	    case 'x':
		if (!check_only) {
		    /******************************************************/
		    /**/
		    /**/    /* process single char option, such as "-x" */
		    /**/
		    /******************************************************/
		}
		continue;

	    case 'l':
		/******************************************************/
		/**/
		/**/    /* process option with modifier, such as "-l65" */
		/**/    /*   checking for syntax errors               */
		/**/
		/******************************************************/
		if (!check_only)
		/******************************************************/
		/**/
		/**/    /* process option with modifier, such as "-l65" */
		/**/    /*   and actually set the appropriate         */
		/**/    /*   global variables                         */
		/**/
		/******************************************************/
		break;

	    case '\0':                    /* done */
		break;

	    default:                      /* unknown option or other errors */
		opterrflg = 1;
		if (show_errs_flg) {
		    fprintf(stderr,"%s: option error: -%s\n",
			progname,curargchar);
		    fflush(stderr);
		}
	    }
	    break;
	}
    }
}

void
filterfiles(check_only)
{
    register FILE *f = NULL;

    if (curarg >= numargs && !check_only) {
	input = stdin;
	my_local_filter();
	return;
    }

    for (; curarg<numargs; curarg++) {
	curargchar = argarray[curarg];
	if (curargchar[0] == '-') {
	    if (curargchar[1] == '\0')    /* "-" arg */
		f = stdin;
	    else
		return;
	}
	else {
	    f = fopen(curargchar,"r");
	    if (f == NULL) {
		struct stat scratch;

		if (stat(curargchar,&scratch) == -1) {
		    if (show_errs_flg) {
			fprintf(stderr,"%s: can't find %s\n",
			    progname,curargchar);
			fflush(stderr);
		    }
		}
		else {
		    if (show_errs_flg) {
			fprintf(stderr,"%s: not allowed to read %s\n",
			    progname,curargchar);
			fflush(stderr);
		    }
		}
		badfileflg = 1;
	    }
	}
	if (!check_only && f != NULL) {
	    input = f;
	    my_local_filter();
	} else if (check_only && f != NULL) {
	    fclose (f);
	}
    }
}

void
my_local_filter ()
{
    if ( input == stdin && intss() )
	fprintf(stderr,"%c%s: start typing.\n",BELL,progname);

    /* xxx can't find size of sgttyb
    if (xintss ())
	setraw ();
    */
    doit();
    fixtty ();

    if (input != stdin)
	fclose(input);
    else
	rewind(stdin);
    fflush(stdout);
}


void
doit()
{
    int n, revision, nwinlist;
    Char chr, majdev, mindev;

    printf ("Revision %d\n", revision = - getshort (input));
    output_done = 1;
    if (revision <= 0)
	badstart();

    switch (revision) {
    case 9:
    case 10:
    case 11:
	if (revision >= 10)
	    printf ("Terminal type: %d\n", getshort (input));
	majdev = getc (input);
	mindev = getc (input);
	printf ("Working Directory device: %d, %d; inode: %d\n",
	    majdev, mindev, getshort (input));
	goto contcase;

    case 12:
    case 14:
	doscrsize();
contcase:
	dotime();
	my_dotabs();
	dofill();
	dosrch();
	doinmode();
	dooldmark();
	nwinlist = dowindow();
	for (n = 0; n < nwinlist; n++) {
	    sepwindows();
	    domargins();
	    dooldalt();
	    dooldfile();
	}
	break;

    case 13:
    case 15:
    case 17:
    case 18:
    case 19:
    case 20:
	dotermtype();
	doscrsize();
	dotime();
	my_dotabs();
	dofill();
	dosrch();
	doinmode();
	if (revision >= 18) {
		dorexp();
		domodes();
	}
	if (revision >= 20) {
	    doinplace();
	    dokbfile();
	    dotermname();
	    docolors();
	    domouse();
	    dobrace();
	    dohighlight();
	}
	domark();
	if (revision >= 19)
		doauto();
	nwinlist = dowindow ();
	for (n = 0; n < nwinlist; n++) {
		sepwindows();
		domargins(n);
		if (revision >= 19)
			dotrack();
		if (revision >= 18)
			dosets();
		doalt();
		dofile();
	}
	break;
    default:
	fputs ("Don't know how to interpret that version.\n", stdout);
	return;
    }
    chr = getc(input);
    if (!feof (input))
	fputs ("\nBad startup file.  Too long (wrong format?)\n", stdout);
    return;
}

void
badstart()
{
    int nerr;

    if ((nerr = ferror (input)))
	printf ("\nBad startup file.  Read error %d.\n", nerr);
    else if (feof (input))
	fputs ("\nBad startup file.  Premature EOF.\n", stdout);
    else
	fputs ("\nBad startup file.\n", stdout);
}


void
setraw ()
{
#ifdef RAWMODE
    if (gtty (INSTREAM, &instty) != -1) {
	int regi;
	regi = instty.sg_flags;
	instty.sg_flags = RAW | (instty.sg_flags & ~(ECHO | CRMOD));
	stty (INSTREAM, &instty);        /* set tty raw mode */
	instty.sg_flags = regi;             /* all set up for cleanup */
	rawflg = 1;
    }
#endif
    flushflg = 1;
}

void
stop ()
{
    fprintf(stderr,"%s: stopped.\n",progname);
    my_getout(-1 - output_done);
}

void
my_getout (status)
{
    fixtty ();
    exit (status);
}

void
fixtty ()
{
#ifdef RAWMODE
    if (rawflg)
	stty (0, &instty);
#endif
    flushflg = 0;
}

/*
xintss()
{
    struct sgttyb buf;

    if (gtty (fileno (input), &buf) != -1)
	return 1;
    return gtty (fileno (stdin), &buf) != -1;
}
*/

void
dotermtype ()
{
	short nletters;

	printf ("Terminal type: \"");
	if ((nletters = getshort (input))) {
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    if (getc (input))
		badstart();
	}
	printf ("\"\n");
}

void
doscrsize()
{
	char nlin, ncol;

	nlin = getc (input) & 0377;
	ncol = getc (input) & 0377;
	printf ("Screen size: %d x %d\n", nlin, ncol);
}

void
dotime()
{
	long tmpl;

	tmpl = getlong (input);
	printf ("Time of start of session: %s", ctime (&tmpl) );
}

void
my_dotabs()
{
	short n;

	if ((n = getshort (input)) > 0) {
	    printf ("%d tabstops: ", n);
	    do {
		printf ("%d, ", getshort (input));
		if (feoferr (input))
		    badstart();
	    } while (--n);
	}
	else
	    fputs ("No tabstops.", stdout);
	fputc ('\n',stdout);
}

void
dofill()
{
	printf ("Width for fill, etc. = %d\n", getshort (input));
}

void
dosrch()
{
	short nletters;

	if ((nletters = getshort (input))) {
	    printf ("Search string is \"");
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    if (getc (input))
		badstart();
	    printf ("\"\n");
	}
	else
	    printf ("No search string.\n");
}

void
doinmode()
{
	printf ("INSERT mode ");
	if (getc (input))
	    printf ("on\n");
	else
	    printf ("off\n");
}

void
dorexp ()
{
	printf ("RE mode ");
	if (getc (input))
	    printf ("on\n");
	else
	    printf ("off\n");
}

void
doinplace()
{
	
	printf ("INPLACE %s.\n",  getc(input) ? "on" : "off" );
}

void
dokbfile()
{
	short nletters;

	if ((nletters = getshort (input))) {
	    printf ("Keyboard file name is \"");
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    if (getc (input))
		badstart();
	    printf ("\"\n");
	}
	else
	    printf ("No keyboard file.\n");
}

void
dotermname()
{
	short nletters;

	if ((nletters = getshort (input))) {
	    printf ("Terminal name is \"");
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    if (getc (input))
		badstart();
	    printf ("\"\n");
	}
	else
	    printf ("No terminal name.\n");
}

void
docolors()
{
	char fg_rgb_options;
	char bg_rgb_options;
	short fg_r, fg_g, fg_b;
	short bg_r, bg_g, bg_b;

	char setaf_set, setab_set;
	short setaf, setab;

	fg_rgb_options = getc (input);
	fg_r = getshort (input);
	fg_g = getshort (input);
	fg_b = getshort (input);
	bg_rgb_options = getc (input);
	bg_r = getshort (input);
	bg_g = getshort (input);
	bg_b = getshort (input);

	setaf_set = getc (input);
	setaf = getshort (input);
	setab_set = getc (input);
	setab = getshort (input);

	if (fg_rgb_options)
	    printf ("Foreground colors: %d, %d, %d\n", fg_r, fg_g, fg_b);
	else
	    printf ("No foreground colors set.\n");

	if (bg_rgb_options)
	    printf ("Background colors: %d, %d, %d\n", bg_r, bg_g, bg_b);
	else
	    printf ("No background colors set.\n");

	if (setaf_set)
	    printf ("ANSI foreground color set: %d\n", setaf);
	else
	    printf ("No ANSI foreground color set.\n");
	if (setab_set)
	    printf ("ANSI background color set: %d\n", setab);
	else
	    printf ("No ANSI background color set.\n");
}

void
domouse()
{
	printf ("SHOWBUTTONS %s.\n",   getc(input) ? "on" : "off" );
	printf ("SKIPMOUSE %s.\n",   getc(input) ? "on" : "off" );
	printf ("USEEXTNAMES %s.\n",   getc(input) ? "on" : "off" );
}

void
dobrace()
{
	printf ("BRACEMATCH %s.\n",   getc(input) ? "on" : "off" );
}

void
dohighlight()
{
	int nletters;

	if ((nletters = getshort (input))) {
	/***/ printf ("highlight nletters = %d\n", nletters);
	    if (feoferr (input))
		badstart();
	    fputs ("Highlight info: ", stdout);
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    fputc ('\n', stdout);
	    if (getc (input))
		badstart();
	}
}

void
domark()
{
	long winlin;
	short col, wincol;
	char lin;

	if (getc (input)) {
	    printf ("MARK in effect:\n");
	    winlin = getlong  (input);
	    wincol = getshort (input);
	    lin    = getc     (input);
	    col    = getshort (input);
	    printf ("  window at (%ld, %d); cursor at (%d, %d)\n",
		     winlin, wincol, lin, col);
	}
	else
	    printf ("MARK not in effect\n");
}

void
dooldmark()
{
	short col, wincol, winlin;
	char lin;

	if (getc (input)) {
	    printf ("MARK in effect:\n");
	    winlin = getshort (input);
	    wincol = getshort (input);
	    lin    = getc     (input);
	    col    = getshort (input);
	    printf ("  window at (%d, %d); cursor at (%d, %d)\n",
		     winlin, wincol, lin, col);
	}
	else
	    printf ("MARK not in effect\n");
}

void
doauto()
{
	if (getc (input))
		printf ("AUTOfill is on.\n");
	else
		printf ("AUTOfill is off.\n");
	printf ("Left margin set on column %d.\n", getshort (input));
}

char
dowindow()
{
	char nwinlist, winnum;

	nwinlist = getc (input);
	if (ferror(input) || nwinlist > MAXWINLIST)
	    badstart();
	printf ("Number of windows: %d\n", nwinlist);
	winnum = getc (input);
	printf ("Current window: %d\n", winnum);
	return nwinlist;
}

void
domargins(n)
	int n;
{
	char tmarg, bmarg;
	short lmarg, rmarg;

	printf ("Window %d:\n", n);
	printf ("  Previous window: %d\n", getc (input));
	tmarg = getc     (input);
	lmarg = getshort (input);
	bmarg = getc     (input);
	rmarg = getshort (input);
	printf ("  (%d, %d, %d, %d) = (t, l, b, r) window margins\n",
	    tmarg, lmarg, bmarg, rmarg);
}

void
dotrack ()
{
	if (getc (input))
	    printf ("  TRACK set.\n");
	else
	    printf ("  TRACK not set.\n");
}

void
dosets ()
{
	short plline, miline, plpage;
	short mipage, lwin, rwin;

	plline = getshort (input);
	miline = getshort (input);
	plpage = getshort (input);
	mipage = getshort (input);
	lwin = getshort (input);
	rwin = getshort (input);
	printf ("  (+l, -l, +p, -p, wl, wr) = (%d, %d, %d, %d, %d, %d)\n",
		     plline, miline, plpage, mipage, lwin, rwin);
}

void
doalt()
{
	short nletters, wincol;
	long winlin;

	if ((nletters = getshort (input))) {
	    if (feoferr (input))
		badstart();
	    fputs ("  Alternate file: ", stdout);
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    if (getc (input))
		badstart();
	    fputc ('\n', stdout);
	    winlin = getlong (input);
	    wincol = getshort (input);
	    printf ("    (%ld, %d) = (lin, col) window upper left\n",
		winlin, wincol);
	    printf ("    (%d, ", getc (input));
	    printf ("%d) = (lin, col) cursor position\n", getshort (input));
	} else
		fputs ("  No alt wksp\n", stdout);
	if (feoferr (input))
		badstart();
}

void
dooldalt()
{
	short nletters, wincol, winlin;

	if ((nletters = getshort (input))) {
	    if (feoferr (input))
		badstart();
	    fputs ("  Alternate file: ", stdout);
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    if (getc (input))
		badstart();
	    fputc ('\n', stdout);
	    winlin = getshort (input);
	    wincol = getshort (input);
	    printf ("    (%d, %d) = (lin, col) window upper left\n",
		winlin, wincol);
	    printf ("    (%d, ", getc (input));
	    printf ("%d) = (lin, col) cursor position\n", getshort (input));
	}
	else
	    fputs ("  No alt wksp\n", stdout);
	if (feoferr (input))
	    badstart();
}

void
dofile()
{
	short nletters, wincol;
	long winlin;

	fputs ("  File: ", stdout);
	nletters = getshort (input);
	while (--nletters > 0)
	    fputc (getc (input), stdout);
	if (getc (input))
	    badstart();
	fputc ('\n', stdout);
	winlin = getlong (input);
	wincol = getshort (input);
	printf ("    (%ld, %d) = (lin, col) window upper left\n",
	    winlin, wincol);
	printf ("    (%d, ", getc     (input));
	printf ("%d) = (lin, col) cursor position\n", getshort (input));
}

void
dooldfile()
{
	short nletters, wincol, winlin;

	fputs ("  File: ", stdout);
	    nletters = getshort (input);
	    while (--nletters > 0)
		fputc (getc (input), stdout);
	    if (getc (input))
		badstart();
	    fputc ('\n', stdout);
	    winlin = getshort (input);
	    wincol = getshort (input);
	    printf ("    (%d, %d) = (lin, col) window upper left\n",
		winlin, wincol);
	    printf ("    (%d, ", getc     (input));
	    printf ("%d) = (lin, col) cursor position\n", getshort (input));
}

void
sepwindows ()
{
	fputs ("============================================\n", stdout);
}


void
domodes ()
{
	printf ("LITMODE %s.\n",  getc(input) ? "on" : "off" );
	printf ("UPTABS %s.\n",   getc(input) ? "on" : "off" );
	printf ("BLANKS %s.\n",   getc(input) ? "on" : "off" );
	printf ("NOSTRIPE %s.\n", getc(input) ? "on" : "off" );
}

