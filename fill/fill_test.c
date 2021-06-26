#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#ifdef COMMENT

    fill & just

    4/89: fixed double spacing at abbreviations bug, and allowed double sp..
	after tmac.eR footnote construct:  <punctuation>\**

    6/85: changed to not split hyphenated words, by default, and added
    -h flag to enable splitting.  Also, no longer filters out CTRL
    chars.

    +----------------------------------------------+
    | Upgraded to VAX, Sept 18, 1979, by W. Giarla |
    +----------------------------------------------+

    David Yost 2/79

    fill {[-l[65]] -h {filename}}

    An argument consisting of "-" by itself means read the original
      standard input, and is treated as if it were a filename.

    lineleng is the total linelength intended for the output line
    -l sets lineleng to 65
    -l20 sets lineleng of output to 20
    if no -l option specified, use 65
    -h enable breaking lines at hyphens.

    Each input file argument is treated according to the options up to
      that point.

    Default is for fill/just to verify all options before proceeding and
      to abort if any errors.  This behavior can be changed by setting
      the appropriate global flags below.
    Default is to check access to all files and report those which cannot
      read, then to go ahead with those that can.  This behavior can
      be changed by setting the appropriate global flags below.

    Handles backspaces and tabs.

	 ##### not yet smart about returns ######

    Decides whether it is fill or just by looking at the last part of the
      pathname in argv[0].

    If any arguments beginning with "." but not starting with ".ll" are
      encountered, everything is shipped off to nroff like the old days.

    Buffers output and input.

    Uses rw stdio library (-lSRW) +
      new faccess(name,mode) fpipe(files) and intss()

    Exits 0 if done OK.
    Exits -1 if error encountered and nothing touched.
    Exits -2 if error encountered after doing some output.

#endif

/**/
#include <stdio.h>
#include <ctype.h>  /* uptovax WG */
/* #include <sys/types.h> */
/* #include <sgtty.h> */
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BELL 07

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
#define MAXINBUF (1024 * 8)     /* must be at least 512                    */
#define MAXOUTLINE 162
#define MAXINT 32767
#define MAXTIE 10
#define TABCOL 8
#define LINELENGDEFLT 65
#define spcortab(c) (c == ' ' || c == '\t')
#define printing(c) (c > ' ' && c < 0177)
#define min(a,b) (a<b? a: b)
#define max(a,b) (a>b? a: b)
#define curindent() (outparline == 1? indent[0]: indent[1])
/*#define putchar(c) fputc(c, stdout)*/

#define NWORDS 512
#define ENDPAR 1


char inbuf[MAXINBUF];

int centerflg = 0;              /* 0 if fill/just, 1 if center             */
int justflg = 0;                /* 1 if just, 0 if fill                    */
int hyphenate = 0;              /* set to not break hyphenated words       */

int indent[2];                  /* first and second line indents           */
int lineleng = LINELENGDEFLT;   /* length of output line                   */
int npar = MAXINT;              /* number of paragraphs to do              */
int prespace;                   /* num of spaces at last beg of line       */

char dblstr[] = ".?!:";         /* put two spaces after these chars when   */
				/* next word starts with a cap. letter     */

  /* info relating to line to be output */
int o_nchars = 0;               /* num of chars accumulated in the line    */
int o_nwords = 0;               /* number of words in line                 */
int o_fullflg = 0;              /* we have a full line already             */
int o_moreflg = 0;              /* there is more after this line is output */
struct lwrd {
    char candidate;             /* candidate for spreading on this pass    */
    char nspaces;               /* num of spaces after word                */
} oword[MAXOUTLINE/2];

struct wrd {
    int nchars;                 /* num of chars in word                    */
    int strleng;                /* num of chars collected for word         */
    char *chars;                /* where word is in inbuf                  */
    char firstchar;             /* first char of word                      */
    char lastchar;              /* last char of word                       */
    char brkchar;               /* char that caused end-of-word            */
    char nextchar;              /* first char of next word                 */
};                              /*   or \n or \f or EOF                    */

int firstword;              /* index of first word in wordtbl              */
int lastword;               /* index of last word in wordtbl               */
struct wrd word[NWORDS];    /* the words                                   */

int bksperr = 0;                /* illegal use of backspace encountered    */
int Inline;                     /* line number of input                    */
int outparline;                 /* line num within current para output     */
int nextcflg = 0;               /* there was a char pushed back onto input */
int nextc;                      /* this is the char pushed back            */

void getoptions ();
void filterfiles ();
void dohere ();
int cmpstr ();
void filter ();
int intss ();
void doit ();
int getword ();
void finish ();
void putwords ();
int next ();
int doublesp ();
void addword ();
int hyonly ();
void justify ();
void my_getprogname ();
int my_getch ();
int abbreviation ();

void docenter();


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

    dohere();

    exit(0);
}


void
dohere()
{
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
	    exit(-2);
	}
    }

    curarg = 1;
    opterrflg = badfileflg = 0;

    do {
	opterrflg = 0;
	show_errs_flg = opt_stop_flg;
	getoptions(0);
	if (opterrflg && opt_stop_flg) {
	    fprintf(stderr,"%s: stopped.\n",progname);
	    exit(-1 - output_done);
	}
	badfileflg = 0;
	show_errs_flg = fil_stop_flg;
	filterfiles(0);
	if (badfileflg && fil_stop_flg) {
	    fprintf(stderr,"%s: stopped.\n",progname);
	    exit(-1 - output_done);
	}
    } while (curarg < numargs);
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

    if (cmpstr(progname,"just"))
	justflg = 1;
    else if (cmpstr(progname,"center"))
	centerflg = 1;
    curarg++;
}


int
cmpstr(s1,s2)
register char *s1, *s2;
{
    for (; *s1 == *s2; s1++, s2++)
	if (*s1 == '\0')
	    return(1);
    return(0);
}


void
getoptions (check_only)
{
    register char *p;


    for (; curarg<numargs; curarg++) {
	curargchar = argarray[curarg];
	p = curargchar;
	if (*p == '-') {
	    if (*++p == '\0')          /* arg was '-' by itself */
		return;
	}
	else
	    return;


	switch( *p ) {
	    case 'l':
		lineleng = atoi(++p);
		if (lineleng <= 0)
		    lineleng = LINELENGDEFLT;
		break;
	    case 'h':
		hyphenate = 1;
		break;
	    default:
		fprintf(stderr, "fill: bad option: %s\n", p);
		fflush(stderr);
		break;
	}
    }

    return;
}


void
filterfiles(check_only)
{
    register FILE *f = (FILE *)-1;

    if (curarg >= numargs && !check_only) {
	input = stdin;
	filter();
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
	    if (check_only) {
		int i;
/*                f = faccess(curargchar,"r");  / * uptovax WG */
/*		f = (FILE *)(!access(curargchar,4)); */
		i = access (curargchar, 4);
		if (i == 0)	/* access OK */
		    f = (FILE *) NULL;
	    }
	    else
		f = fopen(curargchar,"r");
	    if (f == NULL) {
		struct stat scratch;

		if (stat(curargchar, &scratch) == -1) {
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
	    filter();
	}
    }
}


void
filter()
{
    if ( input == stdin && intss() )
	fprintf(stderr,"%c%s: start typing.\n",BELL,progname);

    if (centerflg)
	docenter();
    else
	doit();

    if (input != stdin)
	fclose(input);
    else
	rewind(stdin);
    fflush(stdout);
}



/**/
void
doit()
{
    register int c;
    int np;

    Inline = 1;
    for (np=0; np<npar; np++) {
	indent[0] = indent[1] = 0;
	outparline = 1;

	for (;;) {                      /* treat blank lines at beginning */
	    firstword = 0;
	    lastword = NWORDS - 1;
	    word[firstword].chars = 0;  /* to detect overflow on first */
					/* two lines in paragraph */
	    word[lastword].chars = inbuf + MAXINBUF;
	    word[lastword].nextchar = 0;
	    word[lastword].brkchar = 0;
	    word[lastword].strleng = 0;
	    o_nchars = o_nwords = o_fullflg = o_moreflg = 0;

	    if ((c = getword()) == EOF) {
		finish();
		return;
	    }
	    if (!printing(c) && word[lastword].nchars == 0)
		putchar(c);
	    else
		break;
	}
	indent[0] = prespace;           /* set first line indent */
	for (; c!= EOF; c=getword()) {  /* get remainder of first line */
	    if (!printing(c))
		break;
	}
	if (c == EOF) {
	    finish();
	    return;
	}
	if ( (c=getword()) == EOF) {    /* get line 2 indent           */
	    finish();
	    return;
	}
	indent[1] = prespace;
	for (; c!= EOF; c=getword()) {
	    if (!printing(c)) {
		putwords(!ENDPAR);
		if (word[lastword].nchars == 0) { /* line was blank */
		    putwords(ENDPAR);
		    putchar('\n');
		    break;
		}
	    }
	}
	if (c == EOF) {
	    finish();
	    return;
	}
    }
}


int
getword()
{
    register int i;
    register struct wrd *wp;
    register char *cp;
    static int col = 0;
    int strtcol, nextcol;
    int c;
    int leadhyphens;
    int backspaced;

    /* wrap around char pointer and word index as necessary */
    wp = &word[lastword];
    if (wp->chars + wp->strleng - inbuf + lineleng < MAXINBUF)
	cp = wp->chars + wp->strleng;
    else {
	if (word[firstword].chars == inbuf)
	    return '\n';    /* inbuf is full! */
	else
	    cp = inbuf;
    }
    if (wp->nextchar == '\n') {
	Inline++;
	col = 0;
    }
    lastword = next(lastword);
    wp = &word[lastword];
    wp->chars = cp;
    wp->nchars = wp->strleng = 0;
    wp->firstchar = 0;
    wp->lastchar = 0;

    if (nextcflg) {
	nextcflg = 0;
	c = nextc;
    }
    else
	c = my_getch();
    if (col == 0) {
	for (i=0; c != EOF; c=my_getch()) {
	    if ( c == ' ')
		i++;
	    else if (c == '\t')
		i += i%TABCOL + TABCOL;
	    else if (c == '\b') {
		if (i > 0)
		    i--;
		else {
		    fprintf(stderr,
		      "%s: backsp past newline ignored on input line %d.\n",
		      progname,Inline);
		    bksperr = 1;
		    c = my_getch();
		    break;
		}
	    }
	    else
		break;
	}
	prespace = col = i;
    }
    strtcol = nextcol = col;
    leadhyphens = 1;
    backspaced = 0;
    for (; c != EOF; c=my_getch() ) {
	if ( printing(c) ) {
	    if ( col == strtcol ) {
		if ( !(isupper(wp->firstchar) || wp->firstchar == '-') )
		    wp->firstchar = c;
	    }
	/*  else if (   (!leadhyphens)  */
	    else if (  hyphenate && (!leadhyphens)
		     && cp[-1] == '-'
		     && c != '-'
		     && col == nextcol
		    )
		break;
	    *cp++ = c;
	    if (c != '-')
		leadhyphens = 0;
	    if (++col >= nextcol) {
		nextcol = col;
	/*      if ( !backspaced || !doublesp(wp->lastchar))    */
		if ( !backspaced || !doublesp(wp))
		    wp->lastchar = c;
		backspaced = 0;
	    }
	}
	else if ( c == '\b' ) {
	    backspaced = 1;
	    if (col > strtcol) {
		col--;
		*cp++ = c;
	    }
	    else {
		fprintf(stderr,
		  "%s: backsp past beg of word ignored on input line %d.\n",
		  progname,Inline);
		bksperr = 1;
		c = my_getch();
		break;
	    }
	}
	else if (spcortab(c) && col < nextcol) {
	    if ( c == ' ') {
		*cp++ = c;
		++col;
	    }
	    else if (c == '\t'){
		i = (i=col%TABCOL)? TABCOL-i: TABCOL;
		do {
		    *cp++ = ' ';
		    col++;
		} while (--i);
	    }
	    if (col > nextcol)
		nextcol = col;
	}
	else if( c == ' ' || c == '\t' || c == '\n' || c == '\f' ) {
	    for ( ; col<nextcol; ) {
		*cp++ = ' ';
		col++;
	    }
	    break;
	}
	else                            /* a CTRL char */
	    *cp++ = c;

	if (cp >= &inbuf[MAXINBUF-1]) {
	    fprintf(stderr,"%s: line overflow on input line %d!\n",
		progname,Inline);
	    break;
	}
    }
    wp->brkchar = c;
    wp->nchars = nextcol - strtcol;
    wp->strleng = cp - wp->chars;

    for (; c != EOF; c=my_getch() ) {
	if ( c == ' ')
	    ++col;
	else if (c == '\t') {
	    i = (i=col%TABCOL)? TABCOL-i: TABCOL;
	    do
		col++;
		while (--i);
	}
	else
	    break;
    }
    wp->nextchar = c;
    if (c != '\n' && c != '\f' && c != EOF) {
	nextc = c;
	nextcflg = 1;
    }
    addword(lastword);
    return(bksperr? EOF: c);
}


int
next(wordindex)
{
    return( wordindex < NWORDS - 1? wordindex + 1: 0);
}


int
prev(wordindex)
{
    return( wordindex > 0 ? wordindex - 1: NWORDS - 1);
}


void
addword(wordindex)
{
    register int i;
    register struct wrd *wp;
    register struct wrd *lwp;
    int space;

    wp = &word[wordindex];
    if (wp->nchars == 0)
	return;
    if (o_fullflg == 0) {
	if (o_nwords == 0)
	    space = 0;
	else {
	    lwp = &word[prev(wordindex)];
	    if (   isupper(wp->firstchar)
	    /*  && doublesp(lwp->lastchar)   */
		&& doublesp(lwp)
	    /*  && wp->chars[1] != '.'      / * ie., not U. S. */
		&& !abbreviation(lwp, wp)   /* ie., not U. S. */
	       )
		space = 2;
	    else if (   lwp->lastchar == '-'
		     && (!hyonly(lwp->chars,lwp->strleng))
		     && (   printing(lwp->brkchar)
			 || lwp->nextchar == '\n'
			 || lwp->nextchar == '\f'
			)
		     && wp->firstchar != '-'
		    )
		space = 0;
	    else
		space = 1;
	    oword[o_nwords-1].nspaces = space;
	}
	if (  (i = o_nchars + space + wp->nchars) <= lineleng - curindent()
	   || o_nwords == 0
	   ) {
	    o_nchars = i;
	    o_nwords++;
	}
	else
	    o_moreflg = 1;
	if (i > lineleng - curindent())
	    o_fullflg = 1;
    }
    else
	o_moreflg = 1;
}


int
hyonly(cp,n)
register char *cp;
register int n;
{
    register int c;

    for (; n > 0; n--) {
	c = *cp++;
	if (printing(c) && c != '-')
	    return(0);
    }
    return(1);
}


void
putwords(endparflg)
{
    struct wrd *wp;
    register char *cp;
    register int j;
    int n;
    int i;

    for (;;) {
	/* try to put out one line's worth     */
	if (!endparflg && !o_fullflg)
	    return;
	else if (justflg && o_moreflg)
	    justify();
	n = curindent();
	for (i=0; i<n; i++)
	    putchar(' ');
	for (n=0,i=firstword; n<o_nwords; n++,i=next(i)) {
	    wp = &word[i];
	    for (cp=wp->chars,j=0; j<wp->strleng; j++)
		putchar(*cp++);
	    if (n < o_nwords - 1) {
		for (j=0; j<oword[n].nspaces; j++)
		    putchar(' ');
	    }
	}
	putchar('\n');
	outparline++;
	firstword = i;
	o_nchars = 0;
	o_nwords = 0;
	o_fullflg = 0;
	if (o_moreflg) {
	    o_moreflg = 0;
	    while (o_moreflg == 0) {
		addword(i);
		if (i == lastword)
		    break;
		else
		    i = next(i);
	    }
	}
	else
	    break;
    }
}


void
justify()
{
    register int i, n;
    int ems;                /* number of spaces to distribute in the line  */
    int maxgap, ncandidates;

    if ((ems = lineleng - o_nchars - curindent()) == 0)
	return;         /* already justified */
    /* mark certain gaps as candidates for expansion                       */
    ncandidates = 0;
    for (n=0; n<o_nwords-1; n++) {
	if ((oword[n].candidate = oword[n].nspaces > 0))
	    ncandidates++;
    }
    if (ncandidates == 0)
	return;         /* no spaces to pad */
    /* if there are more ems than gaps to put them in,                     */
    /*   add equal numbers of spaces to all the gaps                       */
    maxgap = 2;
    if ((i = ems/ncandidates)) {
	for (n=0; n<o_nwords-1; n++) {
	    if (oword[n].candidate)
		oword[n].nspaces += i;
	}
	if ((ems %= ncandidates) == 0)
	    return;
	maxgap += i;
    }
    /* we now have fewer ems to distribute than gaps                       */
    /* mark the smaller gaps as candidates for expansion                   */
    ncandidates = 0;
    for (n=0; n<o_nwords-1; n++) {
	if ((oword[n].candidate =
		oword[n].candidate && oword[n].nspaces < maxgap))
	    ncandidates++;
    }
    /* if none of the gaps are of the smaller size then they are all the   */
    /* same - mark all non-zero sized gaps as candidates and inc maxgap    */
    if (ncandidates == 0) {
	for (n=0; n<o_nwords-1; n++) {
	    if ((oword[n].candidate = oword[n].nspaces > 0))
		ncandidates++;
	}
	maxgap++;
    }
    /* else if there are more ems than small gaps, fill up all the small   */
    /*     first                                                           */
    else if (ems >= ncandidates) {
	ems -= ncandidates;
	ncandidates = 0;
	for (n=0; n<o_nwords-1; n++) {
	    if (oword[n].candidate) {
		oword[n].nspaces = maxgap;
		oword[n].candidate = 0;
	    }
	    else {
		oword[n].candidate = oword[n].nspaces > 0;
		ncandidates++;
	    }
	}
    }
    if (ems == 0)
	return;         /* all spaces are equal & line is justified */

    /* here is where you have to decide where to fill out a line by        */
    /* distributing leftover spaces among the candidates.                  */

    /* the algorythm presented here starts widening from the right on odd  */
    /* lines in paragraphs and from the left on even lines.                */
    /* N.B. other tricks to sprinkle extra spaces more randomly throughout */
    /* the line are annoying to the reader;  I tried it. - D. Yost         */

    if (outparline & 1) {
	for (n=o_nwords-2; ems; n--) {
	    if (  oword[n].candidate ) {
		oword[n].nspaces++;
		ems--;
	    }
	}
    }
    else {
	for (n=0; ems; n++) {
	    if (  oword[n].candidate ) {
		oword[n].nspaces++;
		ems--;
	    }
	}
    }
    return;
}


void
finish()
{
    putwords(ENDPAR);
    if (bksperr) {
	fprintf(stderr,"%s: stopped.\n",progname);
	exit(-2);
    }
}


int
my_getch()
{

/*  return(getc(input)); */
    return(fgetc(input));

#ifdef OUT
    register int c;

    for (c=fgetc(input); ; c=fgetc(input) ) {
	if (  spcortab(c)
	   || printing(c)
	   || c == '\b'
	   || c == '\n'
	   || c == '\f'
	   || c == EOF
	   )
	    return(c);
	else
	    fprintf(stderr,"%s: \\%o char ignored.\n",progname,c);
    }
#endif /* OUT */
}


#ifdef OLD
int
doublesp(c)
register int c;
{
    register char *cp;

    for (cp=dblstr; *cp; cp++)
	if (c == *cp)
	    return(1);
    return(0);
}
#endif


int
doublesp(lw)
register struct wrd *lw;
{
    register char *cp;
    register int c;

    if (lw->lastchar == '*') {      /* include <punct>\**   */
	if (lw->nchars < 6)
	    return(0);
	c = lw->chars[lw->nchars - 4];
    }
    else
	c = lw->lastchar;

    for (cp=dblstr; *cp; cp++)
	if (c == *cp)
	    return(1);
    return(0);
}


int
intss()
{
    return isatty (fileno (stdin));
}


int
abbreviation(lw, cw)
register struct wrd *lw, *cw;       /* ptrs to last and current word */
{
    if (cw->chars[1] != '.' || lw->nchars != 2 || lw->lastchar != '.')
	return 0;

    return 1;
}

void
docenter()
{
    char buf[512], *cp;
    int llen, n_sp, i;
    int maxlen = lineleng;

    while (fgets(buf, 512, stdin) != NULL) {
	/* strip leading blanks */
	for (cp = buf; *cp == ' ' || *cp == '\t'; cp++)
	    if (*cp == '\n' || *cp == '\0')
		break;

	llen = (int) strlen(cp) - 1;   /* omit \n */

	if (llen > 0 && llen < maxlen) {
	    n_sp = (maxlen-llen)/2;
	/*  fprintf(stderr, "center: llen=%d, n_sp=%d maxlen=%d\n", llen, n_sp, maxlen); */
	    for (i=0; i < n_sp; i++) {
		putchar(' ');
	    }
	}
	fputs(cp, stdout);
    }
    fflush(stdout);
    return;
}
