/* New version of fill/justify/center for calling from within e */
/* comment block moved to eof */

#include <ctype.h>
#include <sys/types.h>
#include <localenv.h>
#include <la.h>

#ifdef BUILTINFILL
/********************* end of standard filter declarations *****************/
#define MAXINBUF 1024           /* must be at least 512                    */
#define MAXOUTLINE 162
#define MAXINT 32767
#define MAXTIE 10
#define TABCOL 8
#define LINELENGDEFLT 65
#define sp_or_tab(c) (c == ' ' || c == '\t')
#define printing(c) (c > ' ' && c < 0177)
#define min(a,b) (a<b? a: b)
#define max(a,b) (a>b? a: b)
#define curindent() (outparline == 1? indent[0]: indent[1])
#define putch(c)   putc(c, output)

#define NWORDS MAXINBUF/2
#define ENDPAR 1

#define PVT static

PVT char inbuf[MAXINBUF];

PVT int justflg;                /* 1 if just, 0 if fill                    */
PVT int hyphenate;              /* 0 = don't break hyphenated words        */

PVT int indent[2];              /* first and second line indents           */
PVT int lineleng;               /* length of output line                   */
PVT int npar;                   /* number of paragraphs to do              */
PVT int prespace;               /* num of spaces at last beg of line       */

PVT char dblstr[] = ".?!:";     /* put two spaces after these chars when   */
				/* next word starts with a cap. letter     */

  /* info relating to line to be output */
PVT int o_nchars;               /* num of chars accumulated in the line    */
PVT int o_nwords;               /* number of words in line                 */
PVT int o_fullflg;              /* we have a full line already             */
PVT int o_moreflg;              /* there is more after this line is output */
PVT struct lwrd {
    char candidate;             /* candidate for spreading on this pass    */
    char nspaces;               /* num of spaces after word                */
} oword[MAXOUTLINE/2];

PVT struct wrd {
    int nchars;                 /* num of chars in word                    */
    int strleng;                /* num of chars collected for word         */
    char *chars;                /* where word is in inbuf                  */
    char firstchar;             /* first char of word                      */
    char lastchar;              /* last char of word                       */
    char brkchar;               /* char that caused end-of-word            */
    char nextchar;              /* first char of next word                 */
};                              /*   or \n or \f or EOF                    */

PVT int firstword;              /* index of first word in wordtbl              */
PVT int lastword;               /* index of last word in wordtbl               */
PVT struct wrd word[NWORDS];    /* the words                                   */

PVT int bksperr;                /* illegal use of backspace encountered    */
PVT int inline;                 /* line number of input                    */
PVT int outparline;             /* line num within current para output     */
PVT int nextcflg;               /* there was a char pushed back onto input */
PVT int nextc;                  /* this is the char pushed back            */

PVT int linecount;              /* number of lines to read from input */
PVT La_stream *input;           /* input is an la stream */
PVT FILE      *output;          /* output is a normal stream ....*/
PVT char getbuf[MAXINBUF];      /* buffer for getc fakery */
PVT int  gbufct;                /* chars left in getbuf */
PVT char *progname;             /* fill or justify */

/* Fill or Justify a specified number of lines */

filljust(jflag, hflag, in_plas, out, number, width)
int        jflag;               /* 1 if just, 0 if fill  */
int        hflag;               /* 1 if break on hyphens */
La_stream *in_plas;             /* where to get the data */
FILE      *out;                 /* where to put the output */
La_linepos number;              /* number of lines to read */
unsigned int width;             /* max width of output */
{
	justflg = jflag;        /* set globals (ugh) */
	hyphenate = hflag;
	progname = justflg ? "justify" : "fill";
	lineleng = (width <= 0) ? LINELENGDEFLT : width;
	input = in_plas;
	output = out;
	linecount = number;

	initdata();             /* init misc. globals */

	my_filter();

	return 0;
}


PVT
initdata()
{
    npar = MAXINT;              /* number of paragraphs to do              */
    o_nchars = 0;               /* num of chars accumulated in the line    */
    o_nwords = 0;               /* number of words in line                 */
    o_fullflg = 0;              /* we have a full line already             */
    o_moreflg = 0;              /* there is more after this line is output */

    bksperr = 0;                /* illegal use of backspace encountered    */
    nextcflg = 0;               /* there was a char pushed back onto input */

    gbufct = 0;                 /* nothing in the getbuf */
}



PVT
my_filter()
{
    La_linepos la_lseek();
    La_linepos inpos;

    inpos = la_linepos(input);          /* remember current position */

    doit();

    la_lseek(input, inpos, 0);          /* restore input stream */
    fflush(output);
}


PVT
doit()
{
    register int c;
    int np;

    inline = 1;
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
		putch(c);
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
		    putch('\n');
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

PVT
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
	inline++;
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
	c = getch();
    if (col == 0) {
	for (i=0; c != EOF; c=getch()) {
	    if ( c == ' ')
		i++;
	    else if (c == '\t')
		i += i%TABCOL + TABCOL;
	    else if (c == '\b') {
		if (i > 0)
		    i--;
		else {
		    fprintf(output,
		      "%s: backsp past newline ignored on input line %d.\n",
		      progname,inline);
		    bksperr = 1;
		    c = getch();
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
    for (; c != EOF; c=getch() ) {
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
		if ( !backspaced || !doublesp(wp->lastchar))
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
		fprintf(output,
		  "%s: backsp past beg of word ignored on input line %d.\n",
		  progname,inline);
		bksperr = 1;
		c = getch();
		break;
	    }
	}
	else if (sp_or_tab(c) && col < nextcol) {
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
	    fprintf(output,"%s: line overflow on input line %d!\n",
		progname,inline);
	    break;
	}
    }
    wp->brkchar = c;
    wp->nchars = nextcol - strtcol;
    wp->strleng = cp - wp->chars;

    for (; c != EOF; c=getch() ) {
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


PVT
next(wordindex)
{
    return( wordindex < NWORDS - 1? wordindex + 1: 0);
}


PVT
prev(wordindex)
{
    return( wordindex > 0 ? wordindex - 1: NWORDS - 1);
}


PVT
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
		&& doublesp(lwp->lastchar)
		&& wp->chars[1] != '.'      /* ie., not U. S. */
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


PVT
hyonly(cp,n)
register char *cp;
register n;
{
    register c;

    for (; n > 0; n--) {
	c = *cp++;
	if (printing(c) && c != '-')
	    return(0);
    }
    return(1);
}

PVT
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
	    putch(' ');
	for (n=0,i=firstword; n<o_nwords; n++,i=next(i)) {
	    wp = &word[i];
	    for (cp=wp->chars,j=0; j<wp->strleng; j++)
		putch(*cp++);
	    if (n < o_nwords - 1) {
		for (j=0; j<oword[n].nspaces; j++)
		    putch(' ');
	    }
	}
	putch('\n');
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


PVT
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
	if (oword[n].candidate = oword[n].nspaces > 0)
	    ncandidates++;
    }
    if (ncandidates == 0)
	return;         /* no spaces to pad */
    /* if there are more ems than gaps to put them in,                     */
    /*   add equal numbers of spaces to all the gaps                       */
    maxgap = 2;
    if (i = ems/ncandidates) {
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
	if (oword[n].candidate =
		oword[n].candidate && oword[n].nspaces < maxgap)
	    ncandidates++;
    }
    /* if none of the gaps are of the smaller size then they are all the   */
    /* same - mark all non-zero sized gaps as candidates and inc maxgap    */
    if (ncandidates == 0) {
	for (n=0; n<o_nwords-1; n++) {
	    if (oword[n].candidate = oword[n].nspaces > 0)
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


PVT
finish()
{
    putwords(ENDPAR);
    if (bksperr) {
	fprintf(output,"%s: stopped.\n",progname);
	return -2;
    }
}


PVT
getch()
{
	static char *nextch;
	if (gbufct == 0  &&             /* if buffer empty and am */
	    linecount--) {              /* supposed to read more */
	      gbufct = la_lget(input, getbuf, MAXINBUF);
	      nextch = &getbuf[0];
	}
	if (gbufct <= 0) {
/* dbgpr("fi: getch about to return EOF\n"); */
	      return EOF;
	}

	gbufct--;
/* dbgpr("fi: getch about to return '%c'\n", *nextch); */
	return *nextch++;
}

#ifdef notnow
PVT
putch(c)
int c;
{
	dbgpr("fi: putch about to put '%c'\n", c);
	putc(c, output);
}
#endif


PVT
doublesp(c)
register int c;
{
    register char *cp;

    for (cp=dblstr; *cp; cp++)
	if (c == *cp)
	    return(1);
    return(0);
}




#ifdef COMMENT

    fill & just

    6/85: changed to not split hyphenated words, by default, and added
    -h flag to enable splitting.  Also, no longer filters out CTRL
    chars.

    +----------------------------------------------+
    | Upgraded to VAX, Sept 18, 1979, by W. Giarla |
    +----------------------------------------------+

    Original:  David Yost, 2/79
    Vax work:  W. Giarla, 9/79
    Rewrote:   J. Guyton 12/86

    lineleng is the total linelength intended for the output line.
    -l sets lineleng to 65
    -l20 sets lineleng of output to 20
    if no -l option specified, use 65
    -h enable breaking lines at hyphens.

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

    Exits 0 if done OK.
    Exits -1 if error encountered and nothing touched.
    Exits -2 if error encountered after doing some output.

#endif

/*
 * Could/Should be rewritten to avoid e.ru.c, but for now ...
 */

PVT int frstcol;                /* first col of input line                 */
PVT int inlinel;                /* line length of printing text on inp line*/


centerit(in_plas, out, number, width)
La_stream *in_plas;             /* where to get the data */
FILE      *out;                 /* where to put the output */
La_linepos number;              /* number of lines to read */
unsigned int width;             /* max width of output */
{
    La_linepos la_lseek(), inpos;       /* reset input stream when done */

    lineleng = (width <= 0) ? LINELENGDEFLT : width;
    input = in_plas;
    output = out;
    linecount = number;
    frstcol = 0;
    inlinel = 0;

    inpos = la_linepos(input);          /* remember current position */

    if (number > 0)
	while (number--) {
	    GetLine();
	    centline();
	}

    la_lseek(input, inpos, 0);          /* restore input stream */
    fflush(output);

    return 0;
}

PVT
GetLine()
{
    register int i, c;
    register char *cp;
    int thislinel;
    int j;
    static eof_flg = 0;

    if (eof_flg) {
	eof_flg = 0;
	return(EOF);
    }
    frstcol = MAXINT;
    inlinel = 0;
    cp = inbuf;
    for (;;) {
	thislinel = 0;
	for (i=0; (c = getch()) != EOF; ) {
	    if ( c == ' ') {
		*cp++ = c;
		i++;
	    }
	    else if (c == '\t') {
		j = (j=i%TABCOL)? TABCOL-j: TABCOL;
		do {
		    *cp++ = ' ';
		    i++;
		} while (--j);
	    }
	    else
		break;
	}
	frstcol = min(frstcol,i);
	for (; c != EOF; c=getch() ) {
	    if (cp >= &inbuf[MAXINBUF-1]) {
		fprintf(stderr,"%s: line overflow!\n",progname);
		break;
	    }
	    if (c == '\t') {
		j = (j=i%TABCOL)? TABCOL-j: TABCOL;
		do {
		    *cp++ = ' ';
		    i++;
		} while (--j);
	    }
	    else {
		if (c == '\n' || c == '\f'|| c == '\r') {
		    i = max(thislinel,i);
		    for (cp--; cp >= inbuf; cp--) {
			if (*cp == ' ')
			    i--;
			else if (*cp == '\b')
			    {}
			else
			    break;
		    }
		    *++cp = c;
		    *++cp = '\0';
		    thislinel = i;
		    break;
		}
		*cp++ = c;
		if (c == '\b') {
		    if (i > thislinel)
			thislinel = i;
		    i--;
		}
		else
		    i++;
	    }
	}
	inlinel = max(inlinel,thislinel);
	if (c != '\r')
	    break;
    }
    if (c == EOF) {
	if (inlinel == 0)
	    return(EOF);
	else {
	    eof_flg = 1;
	    return(inlinel);
	}
    }
    return(inlinel);
}

PVT
centline()
{
    register int i, j;
    register char *cp;
    int cenwidth;
    int spaceover;

    cenwidth = inlinel - frstcol;
    if (cenwidth > lineleng)
	spaceover = 0;
    else
	spaceover = (lineleng - cenwidth) / 2;
    if (cenwidth > 0) {
	cp = inbuf;
	for(;;) {
	    for (j=0; j<spaceover; j++)
		putch(' ');
	    for (cp += frstcol; *cp; cp++) {    /* ### buggy */
		putch(*cp);
		if (*cp == '\r')
		    break;
	    }
	    if (*cp++ != '\r')
		break;
	}
    }
    else
	fputs(inbuf,output);
}

#endif /* BUILTINFILL */
