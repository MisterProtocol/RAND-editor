/*
 * handle resize windows events
 */

#include "e.h"
#include "e.tt.h"
#include "e.wi.h"
#include "e.m.h"

extern S_term term;

void ResizeWindows (int h, int w);
void debugWindow (S_window *, char *);
void debugAllWindows (void);
void ClearUtilityWindows(void);

extern Flag optshowbuttons;
extern int nButtonLines;
extern void initwindows(Flag);
extern void buttoninit();
extern Uchar *image;
extern void infoinit(void);
extern Flag freshputup;
void debugLMchars(S_window *wp);
void debugMrk(void);
extern Nlines marklines;


void
ResizeWindows (int h, int w)
{
    int h_chg = (h - term.tt_height);
    int w_chg = (w - term.tt_width);
    int i, n;
    S_window *oldwin = curwin;

    int ccol = cursorcol;
    int clin = cursorline;

//  dbgpr("ResizeWindow: h=%d w=%d LINES=%d COLS=%d, h_chg=%d w_chg=%d term(h=%d,w=%d)\n",
//      h, w, LINES, COLS, h_chg, w_chg, term.tt_height, term.tt_width);

//  debugAllWindows();

    blanks = realloc (blanks, (size_t)w);
    fill (blanks, (Uint) w, ' ');

    /* clear utility windows */
    ClearUtilityWindows();

    term.tt_width = (short) w;
    term.tt_height = (char) h;  /* todo, chg to short in e.tt.h/tcap.c */

    /* curses needs to adjust new sizes, reset LINES and COLS */
    resizeterm(h, w);
    refresh();

//dbgpr("After resizeterm: COLS=%d, LINES=%d\n", COLS, LINES);


    /* these 2 are for debugging in e.mk.c */
    extern Short screensize;
    int __attribute__((unused)) oldscreensize = screensize;

// dbgpr("screensize=%d\n", screensize);

    extern Uchar *image;
    if (screensize) sfree (image);
    image = (Uchar *)NULL;

    d_init(YES,NO);     /* YES for clearmem, NO clearscr */

/** /
    dbgpr("after d_init, screensize=%d vs old=%d\n", screensize, oldscreensize);
/ **/

    if (h_chg) {
	ASlines bmarg;
	bmarg = (ASlines) (wholescreen.bmarg + h_chg);

	int bm_orig = wholescreen.bmarg - NPARAMLINES - nButtonLines;

  /* dbgpr("bm_orig=%d\n", bm_orig); */

	wholescreen.bmarg = bmarg;
	wholescreen.btext = bmarg;
	wholescreen.bedit = bmarg;

	enterwin.tmarg = (ASlines) (enterwin.tmarg + h_chg);
	enterwin.bmarg = enterwin.tmarg;
	enterwin.ttext = enterwin.tmarg;

	infowin.tmarg = (ASlines) (infowin.tmarg + h_chg);
	infowin.bmarg = infowin.tmarg;
	infowin.ttext = infowin.tmarg;

	if (nButtonLines) {
	    buttonwin.tmarg = (ASlines) (buttonwin.tmarg + h_chg);
	    buttonwin.ttext = buttonwin.tmarg;
	    buttonwin.tedit = buttonwin.tmarg;
	    buttonwin.bmarg = (ASlines) (buttonwin.bmarg + h_chg);
	    buttonwin.btext = buttonwin.bmarg; /* not used */
	}

	/* now adjust vertical size of edit widnows */
	for (i=0; i<nwinlist; i++) {
		/* for now, only adjust windows touching bmarg */
	    if (bm_orig == winlist[i]->bmarg) {
		bmarg = (ASlines) (winlist[i]->bmarg + h_chg);
		winlist[i]->bmarg = bmarg;
		n = bmarg - winlist[i]->ttext - 1;
		winlist[i]->btext = (ASlines) n;
		winlist[i]->bedit = (ASlines) n;
	   /** /dbgpr("win[%d], bm goes from %d to %d\n",
		    i, bm_orig, bmarg); / **/
	    }
	    /*
	    else {
		dbgpr("win[%d], not at bottom, bmarg=%d\n",
		    i, winlist[i]->bmarg);
	    }
	    */
	}
    }
    if (w_chg) {
	AScols rmarg;

	rmarg = (AScols) (wholescreen.rmarg + w_chg);

	int rm_orig = wholescreen.rmarg;

	wholescreen.rmarg = rmarg;
	wholescreen.rtext = rmarg;
	wholescreen.redit = rmarg;

	enterwin.rmarg = rmarg;
	enterwin.rtext = rmarg;
	enterwin.redit = rmarg;

	infowin.rmarg = rmarg;
	infowin.rtext = rmarg;
	infowin.redit = rmarg;

	if (nButtonLines) {
	    buttonwin.rmarg = rmarg;
	    buttonwin.rtext = rmarg;
	    buttonwin.tedit = rmarg;
	}

	/* now adjust horizontal size of edit widnows */
	for (i=0; i<nwinlist; i++) {
		/* for now, only adjust windows touching rmarg */
	    if (rm_orig == winlist[i]->rmarg) {
		rmarg = (AScols) (winlist[i]->rmarg + w_chg);
		winlist[i]->rmarg = rmarg;
		n = rmarg - winlist[i]->ltext - 1;
		winlist[i]->rtext = (AScols) n;
		winlist[i]->redit = (AScols) n;
	  /** / dbgpr("win[%d], rm goes from %d to %d\n",
		    i, rm_orig, rmarg); / **/
	    }
	    /*
	    else {
		dbgpr("win[%d], not at right margin, rmarg=%d\n",
		    i, winlist[i]->rmarg);
	    }
	    */
	}
    }

/** /
dbgpr("after\n");
    debugAllWindows();
/ **/
    /* ??? todo, make sure curwin is winlist[0] */

    infoinit(); /* redraw the info line */

    if (nButtonLines) {
	buttoninit();
    }

    Small chgborders_save = chgborders;
    chgborders = 0;  /* ? */
    for (i=0; i<nwinlist; i++) {
	switchwindow(winlist[i]);
	limitcursor();  /* see e.t.c */
	poscursor(0, 0);
	putupwin();
	if (curwin == winlist[i]) {
	    //chgborders = 1;
	    drawborders (winlist[i], WIN_ACTIVE | WIN_DRAWSIDES);
	    //chgborders = 0;
	}
	else
	    drawborders (winlist[i], WIN_INACTIVE | WIN_DRAWSIDES);
	fresh();
	d_put(0);
    }

    curwin = oldwin;
    switchwindow(curwin);
    chgborders = chgborders_save;
    drawborders (curwin, WIN_ACTIVE | WIN_DRAWSIDES);

#ifdef xxMOUSE_BUTTONS
    buttoninit();
#endif

    fresh();
    d_put (0);

ungetch(0631);
    ungetch('\015');    /* force redraw */
    ungetch('d');
    ungetch('e');
    ungetch('r');
    ungetch(CCCMD);

    /* seems we need to redraw borders again */
    for (i=0; i<nwinlist; i++) {
	if (curwin != winlist[i]) {
	    drawborders (winlist[i], WIN_INACTIVE | WIN_DRAWSIDES);
	}
	fresh();
	d_put(0);
    }
    drawborders (curwin, WIN_ACTIVE|WIN_DRAWSIDES);

    switchwindow(curwin);

    /* Move the cursor to its original position; or if its
     * no longer on the screen, move it to just within
     * the current window's borders.
     */
    if (ccol > curwin->rtext) ccol = curwin->rtext;
    if (clin > curwin->btext) clin = curwin->btext;
    poscursor(ccol, clin);
    d_put(0);

    return;
}

void debugWindow(S_window *w, char *winname)
{
    dbgpr("%12s lmarg=%3d tmarg=%3d rmarg=%3d bmarg=%3d\n",
	winname, w->lmarg, w->tmarg, w->rmarg, w->bmarg);
    dbgpr("%12s ltext=%3d ttext=%3d rtext=%3d btext=%3d\n",
	" ", w->ltext, w->ttext, w->rtext, w->btext);
    dbgpr("%12s ledit=%3d tedit=%3d redit=%3d bedit=%3d\n",
	" ", w->ledit, w->tedit, w->redit, w->bedit);
}


#include <time.h>
void
debugAllWindows()
{
    static int n=1;

    /** /
    time_t tm;
    time(&tm);
    dbgpr("%s", ctime(&tm));  // debug
    / **/

    dbgpr("utility windows: COLS=%d LINES=%d\n", COLS, LINES);
    debugWindow (&wholescreen, "wholescreen");
    debugWindow (&enterwin, "enterwin");
    debugWindow (&infowin, "infowin");
#ifdef MOUSE_BUTTONS
    if (optshowbuttons == YES) {
      debugWindow (&buttonwin, "buttonwin");
    }
#endif

    /* edit windows */
    int i;
    char tmp[16];
    for (i=0; i<nwinlist; i++) {
	sprintf(tmp, "win %d", i);
	debugWindow(winlist[i], tmp);
	dbgpr("%12s wksp: clin=%d  ccol=%d  wlin=%d, wcol=%d\n",
	    " ",
	    winlist[i]->wksp->clin,
	    winlist[i]->wksp->ccol,
	    winlist[i]->wksp->wlin,
	    winlist[i]->wksp->wcol);

	if (curwin == winlist[i])
	    dbgpr("%12s curwin = winlist[%d]\n", " ", i);

	dbgpr("%12s file: %s\n", " ", names[winlist[i]->wksp->wfile]);

	/*debugLMchars(winlist[i]);*/
	/*debugMrk();*/
    }
/*    dbgpr("curwin=(%o) winlist[%d]=(%o)\n", curwin, --i, winlist[0]); */
    dbgpr("%d------\n", n++);
}



void
debugLMchars(S_window *wp)
{
    int i;
    Uchar *lmcp, *rmcp;
    short *fc, *lc;
    lmcp = wp->lmchars;
    rmcp = wp->rmchars;
    fc = &wp->firstcol[0];
    lc = &wp->lastcol[0];

    for(i=wp->tmarg; i<wp->bmarg-1; i++) {
	dbgpr("line=%d lmcp=(%d) rmcp=(%d) fc=(%d) lc=(%d)\n",
	    i+1, *lmcp++, *rmcp++, *fc++, *lc++);
    }
}


void
debugMrk()
{
    if (curmark) {
	dbgpr("marklines=%d mrkwinlin=%d mrkwincol=%d mrklin=%d, mrkcol=%d\n",
	marklines,
	curmark->mrkwinlin,
	curmark->mrkwincol,
	curmark->mrklin,
	curmark->mrkcol);
    }
    else {
	dbgpr("curmark is NULL\n");
    }
}


void
ClearUtilityWindows ()
{

    if (nButtonLines) {
	int i;
	int h = buttonwin.tmarg;
	S_window *savwin;

	/* clear the overlay highlighting, which is not part of E's data */

	savwin = curwin;
	switchwindow(&buttonwin);

	/* move to top of button window */
//      dbgpr("ClearUtil:  move to %d,%d\n", h,0);

	/* button lines */
	for (h = buttonwin.tmarg; h <= buttonwin.bmarg; h++) {
	//  (*term.tt_addr) (h, 0);
	    MVCUR(h,0);
	    fwrite(blanks, (size_t) buttonwin.rmarg-1, sizeof(char), stdout);
	}
	fflush(stdout);
	//sleep(3);

	/* clear E's data (the button labels) */

	poscursor(0, 1); /* 2nd line, 1st is blank */
	for (i=0; i<buttonwin.rmarg; i++) {
	    putch('x', NO);
	}
	poscursor(0, 3); /* 4th line, 3rd is blank */
	for (i=0; i<buttonwin.rmarg; i++) {
	    putch('y', NO);;
	}
	switchwindow(savwin);
	fflush(stdout);

    }
    return;
}
