/*
 * handle resize windows events
 */

#include "e.h"
#include "e.tt.h"
#include "e.wi.h"
#include "e.m.h"

extern Ncols ncline;
extern Nlines clineno;
extern S_term term;

void ResizeWindows (int h, int w);
void debugWindow (S_window *, char *);
void debugAllWindows (void);
Cmdret DoMacro(char *, char *);

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

int COLS, LINES;  /* curses uses these names */

void
ResizeWindows (int h, int w)
{
    LINES = term.tt_height;
    COLS  = term.tt_width;

    int h_chg = (h - LINES);
    int w_chg = (w - COLS);
    int i, n;
    S_window *oldwin = curwin;

    int ccol = cursorcol;
    int clin = cursorline;

//  dbgpr("ResizeWindow: h=%d w=%d LINES=%d COLS=%d, h_chg=%d w_chg=%d\n",
//      h, w, LINES, COLS, h_chg, w_chg);

/*  debugAllWindows(); */

    term.tt_width = (short) w;
    term.tt_height = (char) h;  /* todo, chg to short in e.tt.h/tcap.c */

    COLS = w;
    LINES = h;

    /* these 2 are for debugging in e.mk.c */
    extern Short screensize;
    int __attribute__((unused)) oldscreensize = screensize;

/*dbgpr("screensize=%d\n", screensize); */

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

	int bm_orig = wholescreen.bmarg - NPARAMLINES;

  /* dbgpr("bm_orig=%d\n", bm_orig); */

	wholescreen.bmarg = bmarg;
	wholescreen.btext = bmarg;
	wholescreen.bedit = bmarg;

	enterwin.tmarg = (ASlines) (bmarg - 1);
	enterwin.bmarg = enterwin.tmarg;
	enterwin.ttext  = enterwin.tmarg;

	infowin.tmarg = bmarg;
	infowin.bmarg = infowin.tmarg;
	infowin.ttext = infowin.tmarg;

	/* now adjust vertical size of edit widnows */
	for (i=0; i<nwinlist; i++) {
		/* for now, only adjust windows touching bmarg */
	    if (bm_orig == winlist[i]->bmarg) {
		bmarg = (ASlines) (winlist[i]->bmarg + h_chg);
		winlist[i]->bmarg = bmarg;
		n = bmarg - winlist[i]->ttext - 1;
		winlist[i]->btext = (ASlines) n;
		winlist[i]->bedit = (ASlines) n;

	    //  dbgpr("win[%d], bm goes from %d to %d\n",
	    //      i, bm_orig, bmarg);
	    }
	  //else {
	  //    dbgpr("win[%d], not at bottom, bmarg=%d\n",
	  //        i, winlist[i]->bmarg);
	  //}
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
	infowin.rmarg = rmarg;
	infowin.rtext = rmarg;

	/* now adjust horizontal size of edit widnows */
	for (i=0; i<nwinlist; i++) {
		/* for now, only adjust windows touching rmarg */
	    if (rm_orig == winlist[i]->rmarg) {
		rmarg = (AScols) (winlist[i]->rmarg + w_chg);
		winlist[i]->rmarg = rmarg;
		n = rmarg - winlist[i]->ltext - 1;
		winlist[i]->rtext = (AScols) n;
		winlist[i]->redit = (AScols) n;
	    //  dbgpr("win[%d], rm goes from %d to %d\n",
	    //      i, rm_orig, rmarg);
	    }
	//  else {
	//      dbgpr("win[%d], not at right margin, rmarg=%d\n",
	//          i, winlist[i]->rmarg);
	//  }
	}
    }

/** /
dbgpr("after\n");
    debugAllWindows();
/ **/
    /* ??? todo, make sure curwin is winlist[0] */

    infoinit(); /* redraw the info line */

    Small chgborders_save = chgborders;
    chgborders = 0;  /* ? */
    for (i=0; i<nwinlist; i++) {
	switchwindow(winlist[i]);
	limitcursor();  /* see e.t.c */
	poscursor(0, 0);
	putupwin();
	if (curwin == winlist[i]) {
	    chgborders = 1;
	    drawborders (winlist[i], WIN_ACTIVE | WIN_DRAWSIDES);
	    chgborders = 0;
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

    fresh();
    d_put (0);

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

    fresh();
    fflush(stdout);

    /* will be read when getkey() wakes up */
    DoMacro("$REDRAW", "1");

    /* move to enter line, display msg */
    (*term.tt_addr) (term.tt_height-2, 1);
    fprintf(stdout, "  *** Hit <RETURN> to continue.  ");
    fflush(stdout);

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
