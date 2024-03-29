#ifdef COMMENT
--------
file e.p.c
    Process printing characters
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.cm.h"
#ifdef LMCAUTO
#include "e.inf.h"
#include "e.m.h"
#endif /* LMCAUTO */

extern Scols putupdelta;
extern char *deletdwd;
extern void shortencline ();

extern void shortencline ();
extern void domark ();
Small inword (int);

#ifdef COMMENT
Small
printchar ()
.
    Process a printing char for mainloop ().
#endif
Small
printchar ()
{
    Reg1 Ncols curcol;
    Reg2 Nlines ln;
    Reg4 Flag bkspflg;
#ifdef LMCAUTO
    Ncols ccsave;
    Cmdret x;
#endif

    /* process a printing character */

    bkspflg = NO;
    numtyp++;             /* have modified text   */

    if (!okwrite ())
	return NOWRITERR;
#ifdef FILELOCKING
    if (!islocked (YES))
	return NOLOCKERR;
#endif /* FILELOCKING */

    GetLine (ln = curwksp->wlin + cursorline);

/**
dbgpr("printchar(), key=(%d)(%3o)('%c), curwksp->clin=%d, curwksp->ccol=%d, curwksp->wlin=%d, curwksp->wcol=%d\n",
  key, key, key,
  curwksp->clin, curwksp->ccol, curwksp->wlin, curwksp->wcol);
 **/

    curcol = cursorcol + curwksp->wcol;
    if (key == CCDELCH || key == CCBACKSPACE) {
	if (key == CCBACKSPACE) {
	    if (cursorcol == 0)
		return CONTIN;
	    movecursor (LT, 1);
	    curcol--;
	    bkspflg = YES;
	}
	if (curcol >= ncline - 1)    /* assumes \n at end of cline */
	    return CONTIN;
	if (   key == CCDELCH
	    || insmode
	   ) Block {
	    Reg3 Slines thislin;
	    thislin = cursorline;
	    if (ncline - 2 - curcol > 0)
		my_move (&cline[curcol + 1], &cline[curcol],
		      (Uint) (ncline - 2 - curcol));
	    ncline--;
	    curcol -= curwksp->wcol;
	    shortencline ();
	    putupdelta = -1;
	    putup (-1, cursorline, (Slines) curcol, MAXWIDTH);
	    poscursor ((Scols) curcol, thislin);
	    fcline = YES;
	    return CONTIN;
	}
	key = ' ';
    }
    else if (key == CCCTRLQUOTE) {
	key = ESCCHAR;
	ecline = YES;
    }

    /* margin-stick feature */
    if (cursorcol > curwin->rtext)
	return MARGERR;
    if (!optstick)
	if (cursorcol == curwin->rtext) {
#ifdef LMCAUTO
	    if (!autofill || (autofill && curcol <= linewidth)) {
		horzmvwin (defrwin);
		GetLine (ln);
	    }
#else /* LMCAUTO */
	    horzmvwin (defrwin);
	    GetLine (ln);
#endif /* LMCAUTO */
	}

    if (curcol >= lcline - 2 && key != ' ')
	excline (curcol + 2);
    if (curcol >= ncline - 1) {  /* equiv to (curcol + 2 - ncline > 0) */
	if (   xcline
	    && !extend (ln - la_lsize (curlas) + 1)
	   ) {
	    mesg (ERRALL + 1, "can't extend the file");
	    return CONTIN;
	}
/*      if (key != ' ') {   */
	    fill (&cline[ncline - 1], (Uint) (curcol + 2 - ncline), ' ');
	    cline[curcol] = (char)key;
	    ncline = curcol + 2;
	    cline[ncline - 1] = '\n';
/*      }   */
	if (xcline) Block {
	    Reg3 Ncols thiscol;
	    xcline = 0;
	    thiscol = cursorcol;
	    putup (-1, cursorline, (Scols) thiscol, MAXWIDTH);
	    poscursor ((Scols) (thiscol + 1), cursorline);
	}
	else
	    putch (key, YES);
    }
    else {
	if (insmode) Block {
		Ncols thiscol;
	    if (ncline >= lcline)
		excline ((Ncols) 0);
	    /* (ncline - curcol > 1) at this point */
	    my_move (&cline[curcol], &cline[curcol + 1],
		  (Uint) (ncline - curcol));
	    ncline++;
	    cline[curcol] = (char)key;
	    thiscol = curcol - curwksp->wcol;
	    putupdelta = 1;
	    putup (-1, cursorline, (Scols) thiscol, MAXWIDTH);
	    poscursor ((Scols) thiscol+1, cursorline);
	}
	else {
	    cline[curcol] = (char)key;
	    putch (key, YES);
	}
	shortencline ();
    }
#ifdef LMCAUTO
    x = CONTIN;
    if (key != ' ' && autofill && curcol > linewidth) Block {
	    Ncols  nc;
	    Nlines nl;
	    extern  Flag    fill_hyphenate;
	for (ccsave = curcol; cline[curcol - 1] != ' '; curcol-- )
	    /*
	     *  only split at hyphenated words if user has enabled
	     *  via "set hy"
	     */
	    if( fill_hyphenate && cline[curcol - 1] == '-' )
		break;
	if (curcol <= autolmarg)
	    curcol = ccsave;
	splitlines (curwksp->wlin + cursorline, curcol, (Nlines) 1, autolmarg, YES);
	curcol = autolmarg + ccsave - curcol + 1 - curwksp->wcol;
	for (nc = 0; curcol < 0; curcol += deflwin, nc += deflwin);
	nl = (cursorline >= curwin->bedit) ? defplline : 0;
	cursorline -= (Slines)nl;
	movewin (curwksp->wlin + nl, curwksp->wcol - nc,
	  cursorline, (Scols)curcol, YES);
	GetLine (curwksp->wlin + cursorline + 1);
	poscursor ((Scols)curcol, cursorline + 1);
	x = CROK;
    }
    if ((cursorcol == curwin->rtext - 10) && !autofill)
#else /* LMCAUTO */
    if (cursorcol == curwin->rtext - 10)
#endif /* LMCAUTO */
	d_put (007);

    fcline = YES;
    if (bkspflg)
	movecursor (LT, 1);
#ifdef LMCAUTO
    return x;
#else /* LMCAUTO */
    return CONTIN;
#endif
}

#ifdef LMCAUTO
#ifdef COMMENT
void
infoauto (Flag cmdmod __attribute__((unused)));
.
	This function sets/resets the autofill capability.
	Changed to not toggle:  wp sets (cmdmod=NO), -wp resets (cmdmod=YES).
#endif
void
infoauto (Flag cmdmod __attribute__((unused)))
{
    Flag moved;

/* Nah, lets' make it a toggle again */
#ifdef OUT
    if ((!cmdmod && autofill) || (cmdmod && !autofill))  /* already set/reset */
	return;
#endif

    autofill = autofill ? NO : YES;
    if (autofill) {
	info (inf_auto, 2, "WP");
	if (curmark) {
	    moved = gtumark(YES);
#ifdef LMCMARG
	    setmarg (&autolmarg, leftmark());
	    if (markcols)
		setmarg ((Ncols *) &linewidth, autolmarg + markcols);
#else /* LMCMARG */
	    autolmarg = leftmark();
	    linewidth = autolmarg + markcols;
#endif /* LMCMARG */
	    domark (moved);
	}
    } else
	info (inf_auto, 2, "");
}
#endif /* LMCAUTO */

#ifdef COMMENT
Small
mword(dir, nwords)
.
    Move to the end or beginning of nth word (as dir = -1 or 1, respectively)
#endif
#include <ctype.h>
Small
mword (dir, nwords)
int dir;
int nwords;
{
    Reg1 Ncols curcol;
    Reg2 Nlines ln;
    Reg3 Ncols cwcol;
    Reg4 Nlines cwln;
    Reg5 int i;

    curcol = cursorcol + curwksp->wcol;
    GetLine (ln = curwksp->wlin + cursorline);
    if (curcol > ncline)
		curcol = ncline;
    if (dir == 1){
	for(i = 0; i < nwords; i++){
	    while(inword(cline[curcol]) && ncline >= curcol)
		    curcol++;
	    curcol--;
	    do{
		curcol++;
		if (ncline <= curcol){
		    if (++ln >= la_lsize (curlas))
			return CROK;
		    else{
			GetLine(ln);
			curcol = 0;
		    }
		}
	    }while(!inword(cline[curcol]));
	}
    }
    else if (dir == -1){
	for(i = 0; i < nwords; i++){
	    do{
		curcol--;
		if (0 > curcol){
		    if (--ln <  0)
			return CROK;
		    else{
			GetLine(ln);
			curcol = ncline - 1;
		    }
		}
	    }while(!inword(cline[curcol]));
	    while(inword(cline[curcol]) && curcol >= 0)
		    curcol--;
	    curcol++;
	}
    }

    cwcol = cursorcol + curwksp->wcol;
    if (curcol < cwcol)
	movecursor(LT, cwcol - curcol);
    else if (curcol > cwcol)
	movecursor(RT, curcol - cwcol);
    cwln = cursorline + curwksp->wlin;
    if (ln < cwln)
	movecursor(UP, cwln - ln);
    else if (cwln < ln)
	movecursor(DN, ln - cwln);

    return CROK;
}


#define WORD_WHITESPACE 1
#define WORD_ALPHNUM    2
static Small WordMode;

#ifdef COMMENT
Small
inword(c)
.
    Determine whether 'c' is part of a word.   The supported modes are
	1.  whitespace delimits words
	2.  Any non-alphanumeric character delimites words
    Returns 1 if in a word, 0 if not.
#endif

Small
inword(c)
int c;
{

/*  Reg1 char *s; */
    char *index();

    switch (WordMode) {

    default:
    case WORD_WHITESPACE:
	if (isspace(c))
	    return(0);
	return(1);

    case WORD_ALPHNUM:
	if (isspace(c))
	    return(0);
	return (isalnum(c));
    }
}

#ifdef COMMENT
Cmdret
setwordmode(opt)

	Word mode may be either
		whitesp - blanks, newlines delimit words
		alphanum - all nonalphanum chars delimit words
.
#endif /* COMMENT */

Cmdret
setwordmode(opt)
char *opt;
{
	Reg1 int ind;
    /*  Reg2 Cmdret retval; */
	static S_looktbl wordopttable[] = {
	    {"alphanumeric", WORD_ALPHNUM},
	    {"whitespace",   WORD_WHITESPACE},
	    {0,              0}
	};

	ind = lookup (opt, wordopttable);
	if (ind == -1 || ind == -2) {
	    mesg (ERRSTRT + 1, opt);
	    return ind;
	}

	switch( wordopttable[ind].val ) {

	case WORD_ALPHNUM:
	    WordMode = WORD_ALPHNUM;
	    return CROK;

	case WORD_WHITESPACE:
	    WordMode = WORD_WHITESPACE;
	    return CROK;

	default:
	    return CRBADARG;
	}
}

#ifdef LMCDWORD
#ifdef COMMENT
Cmdret
dodword (ind)

	dword - delete the current word. Uses the same word criteria as
		+word and -word commands. ind indicates whether to
		remove the word (ind=YES) or restore the last removal
.               (ind=NO), a la <CLOSE>.
#endif /* COMMENT */

Cmdret
dodword (ind)
    int ind;
{
    Reg1 Ncols curcol;
    Reg2 Nlines ln;
    Reg3 Ncols cwcol;
    Reg4 Nlines cwln;
    Ncols nchfol;
    Nlines sav_clineno;

    if (!okwrite ())
	return NOWRITERR;
#ifdef FILELOCKING
    if (!islocked (YES))
	return NOLOCKERR;
#endif /* FILELOCKING */

    curcol = cursorcol + curwksp->wcol;
    GetLine (ln = curwksp->wlin + cursorline);
    if (ind) {
	/* get a line that has *something* on it. */
	while (curcol >= ncline - 1) {
	    if (++ln >= la_lsize (curlas))
		return CROK;
	    else{
		GetLine(ln);
		curcol = 0;
	    }
	}
	if (inword (cline [curcol])) {
	    /* if in the middle of a word, back up to its start. */
	    while (inword (cline [curcol]) && curcol >= 0)
		curcol--;
	    curcol++;
	} else {
	    /* if not in a word, advance till you find one. */
	    do{
		curcol++;
		while (curcol >= ncline - 1){
		    if (++ln >= la_lsize (curlas))
			return CROK;
		    else{
			GetLine(ln); /* this shouldn't ever happen, but...*/
			curcol = 0;
		    }
		}
	    }while (!inword (cline [curcol]));
	}

	/* remember where the word starts */
	cwcol = curcol;
	/* advance to end of word */
	while (inword (cline [++cwcol]) && cwcol < ncline-1)
	    ;
	/* advance over trailing white space */
	while (! inword (cline [cwcol]) && cwcol < ncline-1)
	    cwcol++;
	nchfol = ncline - cwcol;
	if (cwcol >= ncline -1) nchfol++;

	/* ok - delete the word. */

	ncline += (putupdelta = (Scols)(curcol - cwcol));
	if (deletdwd != (char *) 0) sfree (deletdwd);
	deletdwd = salloc ((int) 1 - putupdelta, YES);
/*      my_move (&cline[curcol], deletdwd, (int) - putupdelta); */
	my_move (&cline[curcol], deletdwd, (ulong) - putupdelta);
/*      deletdwd [-putupdelta] = '\0';  */

	/*
	 * previous statement gave the following compiler error on
	 * the Sun: "expression causes compiler loop: try simplifying"
	 */

	*(deletdwd - putupdelta) = '\0';

	my_move (&cline[cwcol], &cline[curcol], (Uint) nchfol);
	shortencline ();
	cwcol = cursorcol + curwksp->wcol;
	sav_clineno = clineno;

	/*
	 *  At this point, the word has been deleted from
	 *  'cline'.  But, since the following 'movecursor' calls
	 *  may result in a 'putup' (and calls to GetLine),
	 *  the changed line never gets written out before it
	 *  is reread from disk.  To fix, set fcline here.
	 */
	fcline = YES;

	/* get the cursor caught up to the beginning of the deletion */
	if (curcol < cwcol)
	    movecursor(LT, cwcol - curcol);
	else if (curcol > cwcol)
	    movecursor(RT, curcol - cwcol);
	cwln = cursorline + curwksp->wlin;
	if (ln < cwln)
	    movecursor(UP, cwln - ln);
	else if (cwln < ln)
	    movecursor(DN, ln - cwln);
	/*
	 * Since one of the above movecursor calls may result
	 * in a putup() call, repeat the GetLine() here.
	 */
	if (clineno != sav_clineno)
	    GetLine(ln);
    } else {           /* ind=NO ==> restore the last deleted word */
	fcline = YES;
	putupdelta = (Scols)strlen (deletdwd);
	if (xcline)
	    extend (ln - la_lsize(curlas));
	putbks (curcol, putupdelta);
	my_move (deletdwd, &cline [curcol], (ulong) putupdelta);
    }
    savecurs();
    curcol -= curwksp->wcol;
    putup (-1, cursorline, (Slines) curcol, MAXWIDTH);
    restcurs();

    return CROK;
}
#endif /* LMCDWORD */

