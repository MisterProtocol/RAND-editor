#ifdef COMMENT
--------
file e.wi.c
    Window mainpulation code.
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.cm.h"
#include "e.inf.h"
#include "e.tt.h"
#include "e.wi.h"

/* defined in e.t.c, set outside putup (), looked at by putup () */
extern Flag entfstline;     /* says write out entire first line */

extern void removewindow ();
extern void chgwindow (Small);
extern void drawborders (S_window *, Small);
extern void switchwindow (S_window *);
#ifdef LMCMARG
extern void drawmarg (S_window *, Ncols, Nlines, int);
#endif /* LMCMARG */
extern void drawsides (S_window *, Small);
extern void draw1side (S_window *, Scols, Small);

extern Flag winshift;
Cmdret makewindow (char *);

Flag borderChanged;


#ifdef COMMENT
Cmdret
makewindow (file)
char   *file;
.
    Do the "window" command.
    This routine commits itself to making the new window before it
    calls editfile ().  This is not the way it should be.  It should
    verify that a window can be made here, then it should call some
    routine that is simliar to the present editfile () and if editfile
    says OK, it should draw the window, otherwise return with no new
    window made.
#endif
Cmdret
makewindow (file)
char   *file;
{
    Reg2 S_window *oldwin;
    Reg3 S_window *newwin;
    Flag    horiz;                /* 1 if margin horiz, 0 if vert */

    if (nwinlist >= MAXWINLIST) {
	mesg (ERRALL + 1, "Can't make any more windows.");
	return CROK;
    }
    if (   cursorcol == 0
	&& cursorline > 0
	&& cursorline < curwin->btext
       )
	horiz = YES;
    else if (   cursorline == 0
	     && cursorcol > 0
	     && cursorcol < curwin->rtext - 1
	    )
	horiz = NO;
    else {
	mesg (ERRALL + 1, "Can't put a window there.");
	return CROK;
    }

    oldwin = curwin;
    if (!(newwin = setupwindow ((S_window *) 0,
				horiz ? oldwin->lmarg
				      : oldwin->lmarg + cursorcol + 1,
				horiz ? oldwin->tmarg + cursorline + 1
				      : oldwin->tmarg,
				oldwin->rmarg,
#ifdef LMCMARG
				oldwin->bmarg, 0, YES)
#else /* LMCMARG */
				oldwin->bmarg, YES)
#endif /* LMCMARG */
       ) )
	return NOMEMERR;
    winlist[nwinlist] = newwin;
    if (horiz) Block {
	/* newwin is below oldwin on the screen  */
	Reg1 Slines i;
	oldwin->bmarg = (ASlines) (oldwin->tmarg + cursorline + 1);
	oldwin->btext = oldwin->bedit = (ASlines)(cursorline - 1);
	if ((i = (newwin->btext + 1) * (int)sizeof oldwin->firstcol[0]) > 0) {
	    my_move ((char *)&oldwin->firstcol[cursorline + 1],
		   (char *)newwin->firstcol, (ulong) i);
	    my_move ((char *)&oldwin->lastcol[cursorline + 1],
		   (char *)newwin->lastcol, (ulong) i);
	}
    }
    else Block {
	/* newwin is to the right of oldwin on the screen  */
	Reg1 Slines i;
	oldwin->rmarg = (AScols) (oldwin->lmarg + cursorcol + 1);
	oldwin->rtext = oldwin->redit = (AScols) (cursorcol - 1);
	for (i = newwin->btext; i >= 0; i--) {
	    if (oldwin->lastcol[i] > oldwin->rtext + 1) {
		newwin->firstcol[i] = 0;
		newwin->lastcol[i] = (AScols) (oldwin->lastcol[i] - cursorcol - 1);
		oldwin->lastcol[i] = (AScols) (oldwin->rtext + 1);
		oldwin->rmchars[i] = MRMCH;
	    }
	}
    }

    Block {
	Small winnum;
	/* the number of curwin is new prevwin */
	for (winnum = 0; winlist[winnum] != curwin; winnum++)
	    continue;
	newwin->prevwin = (ASmall) winnum;
    }
    nwinlist++;

    savewksp (curwksp);
    drawborders (oldwin, WIN_INACTIVE | WIN_DRAWSIDES);
    drawborders (newwin, WIN_ACTIVE);

    infotrack (NO);
    inforange (NO);

    Block {
	Fn ocurfile, oaltfile;
	ocurfile = curwksp->wfile;
	oaltfile = curwin->altwksp->wfile;

	switchwindow (newwin);
	poscursor (0,0);

	if (file) {
	    if (strcmp (file, names[ocurfile]))
		editfile (names[ocurfile], (Ncols) -1, (Nlines) -1, 0, NO);
	    if (editfile (file, (Ncols) -1, (Nlines) -1, 1, YES) <= 0)
		eddeffile (1);
	}
	else {
	    /* 9/21:  Why do we want new window to start at btext + 2 ?? */
	    /* Because that way, the lower window doesn't have to
	     * shift one line down. Instead, one line is hidden by
	     * the new window's horizontal top border.
	     * The new default behavior, is to do the shift (winshift = 1)
	     * so all of the text in the file is visible.
	     * The -nowinshift option sets winshift to 0 and starts
	     * the new window at btext + 2, the previous behavior.
	     */
	    int topln = winshift ? 1 : 2;

	    if (oaltfile)
		editfile (names[oaltfile], (Ncols) -1, (Nlines) -1, 0, NO);

	    editfile (names[ocurfile],
		  (Ncols) (horiz ? -1
			   : oldwin->wksp->wcol + oldwin->rtext + 2),
		  (Nlines) (horiz
		       /*  ? oldwin->wksp->wlin + oldwin->btext + 2 : -1), */
			   ? oldwin->wksp->wlin + oldwin->btext + topln : -1),
		  0, YES);  /* puflg is YES to initalize border chars */
	    poscursor (0, 0);
	}
    }

    return CROK;
}

#ifdef COMMENT
S_window *
setupwindow (win, cl, lt, cr, lb, wf, editflg)
    S_window *win;
    Scols   cl;
    Scols   cr;
    Slines  lt;
    Slines  lb;
    Aflag   wf;
    Flag    editflg;
.
    Initialize the window using 'cl', 'cr', 'lt', 'lb' as the left, right,
    top, and bottom.
    If 'editflg' == YES then 'win' is an editing window -- i.e. borders, etc.
    If 'win' == 0, then alloc a new window.
    Return 'win' if no alloc failure, else return 0.
#endif
S_window *
setupwindow (win, cl, lt, cr, lb, wf, editflg)
Reg3 S_window *win;
Scols   cl;
Slines  lt;
Scols   cr;
Slines  lb;
Flag   wf;
Flag    editflg;
{
    if (   !win
	&& !(win = (S_window *) okalloc (SWINDOW))
       )
	return 0;

    win->winflgs = (AFlag) wf;
    win->lmarg = (AScols) cl;
    win->tmarg = (ASlines) lt;
    win->rmarg = (AScols) cr;
    win->bmarg = (ASlines) lb;
    if (editflg) {
	win->ltext = (AScols) (cl + 1);
	win->ttext = (ASlines) (lt + 1);
	win->rtext = (AScols) (cr - cl - 2);
	win->btext = (ASlines) (lb - lt - 2);
    }
    else {
	win->ltext = (AScols) cl;
	win->ttext = (ASlines) lt;
	win->rtext = (AScols) (cr - cl);
	win->btext = (ASlines) (lb - lt);
    }
    win->ledit = 0;
    win->tedit = 0;
    win->redit = win->rtext;
    win->bedit = win->btext;
    win->miline = defmiline;
    win->plline = defplline;
    win->mipage = defmipage;
    win->plpage = defplpage;
    win->rwin = defrwin;
    win->lwin = deflwin;

    if ((win->wksp = (S_wksp *) okalloc (sizeof (S_wksp)))) {
	if (!editflg)
	    return win;
	if ((win->altwksp = (S_wksp *) okalloc (sizeof (S_wksp)))) Block {
	    Reg2 Slines size;
	    size = term.tt_height - NINFOLINES - NENTERLINES - NHORIZBORDERS;

	    /*  10/2022:  With the addition of window resizing, the window arrays
	     *  arrays (lmchars, rmchars, firstcol, and lastcol) could be too small
	     *  when a window is enlarged.  For now, just initialize the array
	     *  sizes to accommmodate a large window height.  This only applies
	     *  to edit windows.
	     */
	    size = max(MAXWINLINES, size); /* default 80 */
    /** /   dbgpr("setupwindow: size=%d, tt_height=%d\n", size, term.tt_height); / **/

	    if ((win->firstcol = (AScols *) okalloc (2 * size * ((int)sizeof *win->firstcol)))) {
		win->lastcol = &win->firstcol[size];
		if ((win->lmchars
		    = (unsigned char *)okalloc (2 * size * ((int)sizeof *win->lmchars)))) Block {
		    Reg1 int i;
		    win->rmchars = &win->lmchars[size];
		    /* can't use bfill here because firstcol may not be a char */
		    for (i = Z; i < size; i++) {
			win->firstcol[i] = (AScols) (win->rtext + 1);
			win->lastcol[i] = 0;
		    }
		    return win;
		}
		sfree ((char *) win->firstcol);
	    }
	    sfree ((char *) win->altwksp);
	}
	sfree ((char *) win->wksp);
    }
    return 0;
}

#ifdef COMMENT
void
removewindow ()
.
    Eliminates the last made window by expanding its ancestor
#endif
void
removewindow ()
{
    Scols   stcol;              /* start col for putup  */
    Slines  stlin;              /* start lin for putup  */
    Small   ppnum;              /* prev window number   */
    register S_window *thewin;  /* window to be removed */
    register S_window *pwin;    /* previous window      */

    if (nwinlist == 1) {
	mesg (ERRALL + 1, "Can't remove remaining window.");
	return;
    }
    thewin = winlist[--nwinlist];
    ppnum = thewin->prevwin;
    pwin = winlist[ppnum];
    savewksp (thewin->wksp);

    if (pwin->bmarg != thewin->bmarg) Block {
	/* thewin is below pwin on the screen  */
	Slines j;
	register Slines tmp;
	pwin->firstcol[j = pwin->btext + 1] = 0;
	pwin->lastcol[j++] = (AScols) (pwin->rtext + 1);
	if ((tmp = (thewin->btext + 1) * (int)sizeof *thewin->firstcol) > 0) {
	    my_move ((char *)&thewin->firstcol[0],
		  (char *)&pwin->firstcol[j], (ulong) tmp);
	    my_move ((char *)&thewin->lastcol[0],
		  (char *)&pwin->lastcol[j], (ulong) tmp);
	}
	stcol = 0;
	stlin = pwin->btext + 1;
	pwin->bmarg = thewin->bmarg;
	pwin->btext = (ASlines) (pwin->bmarg - pwin->tmarg - 2);
	pwin->bedit = pwin->btext;
    }
    else Block {
	/* thewin is to the right of pwin on the screen  */
	register Slines tmp;
	for (tmp = Z; tmp <= pwin->btext; tmp++) {
	    pwin->lastcol[tmp] = (AScols) (thewin->lastcol[tmp] +
		thewin->lmarg - pwin->lmarg);
	    if (pwin->firstcol[tmp] > pwin->rtext)
		pwin->firstcol[tmp] = pwin->rtext;
	}
	stcol = pwin->rtext + 1;
	stlin = 0;
	pwin->rmarg = thewin->rmarg;
	pwin->rtext = (AScols) (pwin->rmarg - pwin->lmarg - 2);
	pwin->redit = pwin->rtext;
    }
    chgwindow (ppnum);
    putup (stlin, curwin->btext, stcol, MAXWIDTH);
    poscursor (pwin->wksp->ccol, pwin->wksp->clin);

    sfree ((char *) thewin->firstcol);
    sfree (thewin->lmchars);
    if (thewin->altwksp->wfile != NULLFILE)
	(void) la_close (&thewin->altwksp->las);
    sfree ((char *) thewin->altwksp);
    (void) la_close (&thewin->wksp->las);
    sfree ((char *) thewin->wksp);
    sfree ((char *) thewin);
    return;
}

#ifdef COMMENT
void
chgwindow (winnum)
    Small winnum;
.
    Moves current window to another window.
    if 'winnum' < 0 means go to next higher window in winlist[].
#endif
void
chgwindow (winnum)
Small winnum;
{
    register S_window *newwin;
    register S_window *oldwin;

    oldwin = curwin;
    if (winnum < 0) {
	winnum = 0;
	while (winnum < nwinlist && oldwin != winlist[winnum++])
	    continue;
    }
    curwksp->ccol = (AScols) cursorcol;
    curwksp->clin = (ASlines) cursorline;
    newwin = winlist[winnum % nwinlist];    /* wrap back to window 0 */
    if (newwin == oldwin)     /* ALWAYS rewrite first line */
	entfstline = YES;       /* don't skip over blanks */
    drawborders (oldwin, WIN_INACTIVE | WIN_DRAWSIDES);
    drawborders (newwin, WIN_ACTIVE | WIN_DRAWSIDES);
    infotrack (newwin->winflgs & TRACKSET);
    inforange (newwin->wksp->wkflags & RANGESET);
    switchwindow (newwin);
    limitcursor ();
    poscursor (curwksp->ccol, curwksp->clin);
    return;
}

#ifdef COMMENT
void
drawborders (window, how)
    S_window *window;
    Small how;
.
    Draw borders for active or inactive window with or without drawing sides.
#endif
void
drawborders (window, how)
S_window *window;
Small how;
{
    S_window *oldwin;
    register Short i;
    register Short j;
#ifdef LMCMARG
/*  Ncols k, k1; */
    Ncols k;
#endif /* LMCMARG */

    borderChanged = 1;  /* for e.m.c */

    oldwin = curwin;
    switchwindow (&wholescreen);

    j = window->rmarg;

    poscursor (i = window->lmarg, window->tmarg);

/** /dbgpr("drawborders:  lm=%d, rm=%d\n", i, j);  / **/

#ifdef LMCMARG
    for (k=Z; k < ntabs && tabs[k] < window->wksp->wcol; k++)
	{}
 /*  k1 = k; */
#endif /* LMCMARG */
    if (!(how & WIN_ACTIVE))
	for (; i <= j; i++)
	    putch (INMCH, NO);
    else {
	putch (TLCMCH, NO);
	for (i++; i < j; i++)
#ifdef LMCMARG
	    /*  The tabs array can be null if "CMD: -tabs" is followed by
	     *  an exit, and e is restarted w/o any cmdline args
	     */
	/*  if (tabs[k] == i - window->lmarg - 1 + window->wksp->wcol) { */
	    if (ntabs && tabs[k] == i - window->lmarg - 1 + window->wksp->wcol) {
		/* no tab mark in col 0 */
		if( tabs[k] != 0 )
		    putch (TTMCH, NO);
		else
		    putch (TMCH, NO);
		if (k < ntabs) k++;
	    } else
#endif /* LMCMARG */
		putch (TMCH, NO);
	putch (TRCMCH, NO);
/*****   no left/rt  marg indicators on top border *******************/
#ifdef OUT
#ifdef LMCMARG
#ifdef LMCAUTO
	drawmarg (window, autolmarg, window->tmarg, BTMCH);
#endif /* LMCAUTO */
	drawmarg (window, linewidth, window->tmarg, BTMCH);
#endif /* LMCMARG */
#endif /* OUT */
/******************************************************************/

    }

    poscursor (i = window->lmarg, window->bmarg);
    if (!(how & WIN_ACTIVE))
	for (; i <= j; i++)
	    putch (INMCH, NO);
    else {
	putch (BLCMCH, NO);
	for (i++; i < j; i++)
/***************  no tab marks on bottom margin ************************/
#ifdef OUT
#ifdef LMCMARG
	    /* if (tabs[k1] == i - window->lmarg - 1 + window->wksp->wcol) { */
	    if (ntabs && tabs[k1] == i - window->lmarg - 1 + window->wksp->wcol) {
		putch (BTMCH, NO);
		if (k1 < ntabs) k1++;
	    } else
#endif /* LMCMARG */
#endif /* OUT */
/**********************************************************************/
		putch (BMCH, NO);
	putch (BRCMCH, NO);
#ifdef LMCMARG
#ifdef LMCAUTO
	/* display left mar char, if one is set */
	if( autolmarg )
	    drawmarg (window, autolmarg, window->bmarg, BTMCH);
#endif /* LMCAUTO */
	drawmarg (window, linewidth, window->bmarg, BTMCH);
#endif /* LMCMARG */
    }

    drawsides (window, how);
    switchwindow (oldwin);
    return;
}

#ifdef LMCMARG
#ifdef COMMENT
void
drawmarg (window, col, line, chr)
    S_window *window;
    Nlines line;
    Ncols col;
    Uchar chr;
.
    This routine will draw the margin indicator character chr in the window
    at line, col if it is within the current window frame.
#endif
void
drawmarg (window, col, line, chr)
    S_window *window;
    Ncols col;
    Nlines line;
    Uchar chr;
{
    col -= (window->wksp->wcol - 1);
    if (col >= 0 && col <= window->rtext) {
	poscursor ((Scols) (col + window->lmarg), (Slines) line);
	putch (chr, NO);
    }
}
#endif /* LMCMARG */

#ifdef COMMENT
void
drawsides (window, how)
    S_window *window;
    Small how;
.
    Draw the side borders of window.
#endif
void
drawsides (window, how)
S_window *window;
Small how;
{
    if (!(how & WIN_DRAWSIDES))
	return;

    if (window->rmarg - window->lmarg < term.tt_width - 1) {
	draw1side (window, window->lmarg, how);
	draw1side (window, window->rmarg, how);
    }
    else {
	/* this way is faster when window is full-width */
	Reg3 Slines line;
	Reg4 Slines bottom;
	bottom = window->bmarg - 1;
	line = window->tmarg + 1;
	if (how & WIN_ACTIVE) Block {
	    Reg1 unsigned char *lmcp;
	    Reg2 unsigned char *rmcp;
	    lmcp = window->lmchars;
	    rmcp = window->rmchars;
	    for (; line <= bottom; line++) {
		poscursor (window->lmarg, line);
		putch (*lmcp++, NO);
		poscursor (window->rmarg, line);
		putch (*rmcp++, NO);
	    }
	}
	else {
	    for (; line <= bottom; line++) {
		poscursor (window->lmarg, line);
		putch (INMCH, NO);
		poscursor (window->rmarg, line);
		putch (INMCH, NO);
	    }
	}
    }
    return;
}

#ifdef COMMENT
void
draw1side (window, border, how)
    S_window *window;
    register Scols border;
    Small how;
.
    Draw each side border of window.
#endif
void
draw1side (window, border, how)
S_window *window;
register Scols border;
Small how;
{
    Reg3 Slines line;
    Reg4 Slines bottom;

    bottom = window->bmarg - 1;
    line = window->tmarg + 1;
    if (how & WIN_ACTIVE) Block {
	register unsigned char *bchrp;
	bchrp = border == window->lmarg ? window->lmchars : window->rmchars;
	for (; line <= bottom; line++) {
	    poscursor (border, line);
	    putch (*bchrp++, NO);
	}
    }
    else {
	for (; line <= bottom; line++) {
	    poscursor (border, line);
	    putch (INMCH, NO);
	}
    }
    return;
}

#ifdef COMMENT
void
switchwindow (win)
    S_window *win;
.
    Adjust all the 'cur' globals like curfile, etc.
    Changes cursorline, cursorcol to be relative to new upper lefthand
    corner.
    You must do a poscursor after switchwindow.
#endif
void
switchwindow (win)
register S_window *win;
{
    register S_window *cwin;

    cwin = curwin;
    cursorcol  += cwin->ltext - win->ltext;
    cursorline += cwin->ttext - win->ttext;
    if ((curwksp = (curwin = win)->wksp)) {
	curfile = curwksp->wfile;
	curlas = &curwksp->las;
    }
    if (!curwin || !curwksp)
	fatal (FATALBUG, "switchwindow %o %o", curwin, curwksp);
/*  defplline = defmiline = win->btext / 4 + 1;                         */


    /*
     *  If a "set +line" or "set -line" cmd has been issued for this
     *  window, honor the values.  Otherwise, setup the default values
     *  for +line and -line for the window.
     */

    if (win->winflgs & PLINESET)
	defplline = win->plline;
    else
	defplline = win->btext / 4 + 1;

    if (win->winflgs & MLINESET)
	defmiline = win->miline;
    else
	defmiline = win->btext / 4 + 1;

    defplpage = win->plpage;
    defmipage = win->mipage;
    defrwin = win->rwin;
    deflwin = win->lwin;

    return;
}

#ifdef COMMENT
void
infotrack (onoff)
    Flag onoff;
.
    Update the TRACK display on the info line
#endif
void
infotrack (onoff)
Reg1 Flag onoff;
{
    static Flag wason;

    if ((onoff = onoff ? YES : NO) ^ wason)
	info (inf_track, 3, (wason = onoff) ? "TRK" : "");
    return;
}
