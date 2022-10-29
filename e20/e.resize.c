/*
 * handle resize windows events
 */

#include "e.h"
#include "e.tt.h"
#include "e.wi.h"
#include "e.m.h"

/*  Note: some coding in this file may seem to include
 *  unnecessary casts, but an oldish compiler was
 *  generating warnings for statements like "a += b"
 *  when both are of type short.
 */

void ResizeWindows (int h, int w);
void debugWindow (S_window *, char *);
void debugAllWindows (void);
void ClearUtilityWindows(void);
void debugLMchars(S_window *wp);
void debugMrk(void);
int  CheckWindowValues(void);
void adj_h_borders(ASlines bm_orig, ASlines h_chg);
void adj_w_borders(AScols rm_orig, AScols w_chg);
void alt_h_winresize(ASlines bm_orig, ASlines h_chg, int adjwin);
void alt_w_winresize(AScols rm_orig, AScols w_chg, int adjwin);

extern S_term term;
extern Flag optshowbuttons;
extern int nButtonLines;
extern void initwindows(Flag);
extern void buttoninit();
extern Uchar *image;
extern void infoinit(void);
extern Flag freshputup;
extern Nlines marklines;
Flag noresizeall = NO;

void
ResizeWindows (int h, int w)
{
    int h_chg = (h - term.tt_height);
    int w_chg = (w - term.tt_width);
    int i;
    S_window *oldwin = curwin;

    S_wksp *wksp_old = curwksp;

    int ccol = cursorcol;
    int clin = cursorline;

    static int cnt = 0;

//  dbgpr("%03d -----------\n", cnt++);
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

    /* Determine if all windows have the same orientation, eg:
     * all touch the bottom margin or all touch the right margin.
     * If so, adjust the space alternatively between them.
     * Otherwise, adjust only window touching the border.
     */
    int alternate_h = 0;    /* alternate vertical windows */
    int alternate_w = 0;    /* alternate horizontal windows */

    int adj_winnum = cnt % nwinlist;  /* which win num to adjust */

    /* -noresize:  option specifes only adjust border windows */
    if (nwinlist > 1 && !noresizeall) {
	if (h_chg) {
	    int rm = winlist[0]->rmarg;
	    /* are all windows horizontal? */
	    alternate_h = 1;  /* assume they are */
	    for (i=1; i<nwinlist; i++) {
		if (winlist[i]->rmarg != rm) {
		    alternate_h = 0;    /* no, not all horizontal */
		    break;
		}
	    }
	}
	if (w_chg) {
	    int bm = winlist[0]->bmarg;
	    /* are all windows vertical? */
	    alternate_w = 1;  /* assume they are */
	    for (i=1; i<nwinlist; i++) {
		if (winlist[i]->bmarg != bm) {
		    alternate_w = 0;    /* no, not all vertical */
		    break;
		}
	    }
	}
    }

/** /
dbgpr("adj_winnum=%d, alt_w=%d alt_h=%d\n",
adj_winnum, alternate_w, alternate_h);
/ **/

    /* Adjust the sizes */

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
	    buttonwin.bmarg = (ASlines) (buttonwin.bmarg + h_chg);
	    buttonwin.btext = buttonwin.bmarg; /* not used */
	}

	if (alternate_h) {
	    /* todo: alternate which window to adjust */
	    alt_h_winresize((ASlines) bm_orig, (ASlines) h_chg, adj_winnum);
	    // adj_h_borders (bm_orig, h_chg);
	}
	else {
	    /* adj windows touching bottom margin */
	    adj_h_borders ((ASlines) bm_orig, (ASlines) h_chg);
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
	    buttonwin.tedit = 0;
	}

	if (alternate_w) {
	    /* todo: alternate which window to adjust */
	    alt_w_winresize((AScols) rm_orig, (AScols) w_chg, adj_winnum);
	    // adj_w_borders (rm_orig, w_chg);
	}
	else {
	    /* adj windows touching right margin */
	    adj_w_borders ((AScols) rm_orig, (AScols) w_chg);
	}
    }

/** /
dbgpr("after\n");
    debugAllWindows();
/ **/

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

    fresh();
    d_put (0);

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

    if( curwksp != wksp_old)
      dbgpr("curwksp != wksp_old\n");

    /* as per limitcursor() in e.t.c */
    for (i=0; i<nwinlist; i++) {
	winlist[i]->wksp->ccol = min (winlist[i]->wksp->ccol, winlist[i]->rtext);
	winlist[i]->wksp->clin = min (winlist[i]->wksp->clin, winlist[i]->btext);
	winlist[i]->altwksp->ccol = min (winlist[i]->altwksp->ccol, winlist[i]->rtext);
	winlist[i]->altwksp->clin = min (winlist[i]->altwksp->clin, winlist[i]->btext);
    }

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
    /* valid settings */



}


#include <time.h>
void
debugAllWindows()
{
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
    int w, h;
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

	w = winlist[i]->rmarg - winlist[i]->lmarg;
	h = winlist[i]->bmarg - winlist[i]->tmarg;
	dbgpr("%12s file: %s w=%d h=%d\n", " ", names[winlist[i]->wksp->wfile], w,h);

	/*debugLMchars(winlist[i]);*/
	/*debugMrk();*/
    }
    dbgpr("%12s ", "w");
    for (i=0; i<nwinlist; i++) {
	dbgpr(" %d", (int) (winlist[i]->rmarg - winlist[i]->lmarg));
    }
    dbgpr("\n");

    dbgpr("%12s ", "h");
    for (i=0; i<nwinlist; i++) {
	dbgpr(" %d", (int) (winlist[i]->bmarg - winlist[i]->tmarg));
    }
    dbgpr("\n");

    dbgpr("%12s %s\n", "win values", CheckWindowValues() ? "errors" : "ok");

    dbgpr("------\n");
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

    // They don't appear to need it, but
    // todo, also clear the enter and info wins

    return;
}


/*
 * adjust windows touching bottom margin
 */
void
adj_h_borders(ASlines bm_orig, ASlines h_chg)
{
    int i;
    S_window *wp;

    /* adjust vertical size of edit windows touching bmarg */

    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	if (bm_orig == wp->bmarg) {
	    if (wp->btext <= 1)  /* maintain at least 1 line in window */
		return;
	    wp->bmarg = (ASlines) (wp->bmarg + h_chg);
	    wp->btext = (ASlines) (wp->bmarg - wp->tmarg - 2);
	    wp->bedit = wp->btext;
	// dbgpr("win[%d], bm goes from %d to %d\n", i, bm_orig, wp->bmarg);
	}
    }

    /* BTW, don't ever change the wp->tedit field.  Doing so,
     * leaves the cursor "stuck" on the last line of the window!
     */

    return;
}

/*
 * adjust width of windows touching right margin
 */
void
adj_w_borders(AScols rm_orig, AScols w_chg)
{
    int i;
    S_window *wp;

    /* adjust width */
    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];

	/* adjust windows touching rmarg */
	if (rm_orig == wp->rmarg) {
	    wp->rmarg = (AScols) (wp->rmarg + w_chg);
	    wp->rtext = (AScols) (wp->rmarg - wp->lmarg - 2);
	    wp->redit = wp->rtext;
	// dbgpr("win[%d], rm goes from %d to %d\n", i, rm_orig, wp->rmarg);
	}
    }
    return;
}

/*
 *   alternate which window to adjust
 */
void
alt_h_winresize(ASlines bm_orig, ASlines h_chg, int adjwin)
{
    int i;
    int bottommost_win;
    S_window *wp;

    /* the bottommost window changes in all cases */
    for (i=0; i<nwinlist; i++) {
	if (winlist[i]->bmarg == bm_orig) {
	    adj_h_borders(bm_orig, h_chg);
	    if (i == adjwin) {  /* we're done */
	    //  dbgpr("alt_h no=%d:  case: bottommost win\n", adjwin);
		return;
	    }
	    bottommost_win = i;
	    break;
	}
    }




    /*  Case N windows:
     *     change bm of adjwin
     *     change tm and bm of each win below
     *       except for bm of bottommost_win
     */

//  ASlines tm_old, bm_old;
    ASlines bm_alt;

    /* adjust bm of adj window */
    wp = winlist[adjwin];
    bm_alt = wp->bmarg;

    /* Don't allow a window to be less than 1 line in height */
    if (wp->bmarg < 3 && h_chg < 0)
	return;

    wp->bmarg = (ASlines) (wp->bmarg + h_chg);
    wp->btext = (ASlines) (wp->bmarg - wp->tmarg - 2);
    wp->bedit = wp->btext;

// dbgpr("\nalt_h, adj win[%d], bm goes from %d to %d\n", i, bm_alt, wp->bmarg );

    /* change each win below adjwin */
    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	if (bm_alt <= wp->tmarg) {
	//  tm_old = wp->tmarg;
	    wp->tmarg = (ASlines) (wp->tmarg + h_chg);
	    wp->ttext = (ASlines) (wp->tmarg + 1);
	    wp->tedit = 0;

	    wp->wksp->wlin += h_chg;

	//  dbgpr("alt_h, win[%d], tm goes from %d to %d\n\n", i, tm_old, wp->tmarg);
	    if (i != bottommost_win) {
	    //  bm_old = wp->bmarg;
		wp->bmarg = (ASlines) (wp->bmarg + h_chg);
		wp->btext = (ASlines) (wp->bmarg - wp->tmarg - 2);
		wp->bedit = wp->btext;
	    //  dbgpr("alt_h, win[%d], bm goes from %d to %d\n\n", i, bm_old, wp->bmarg);
	    }
	    else {
		wp->btext = (ASlines) (wp->bmarg - wp->tmarg - 2);
		wp->bedit = wp->btext;
	    //  dbgpr("no bmarg change to bottommost win=%d\n", i);
	    }
	}
    }

    return;
}

/*
 *   alternate which window to adjust
 */
void
alt_w_winresize(AScols rm_orig, AScols w_chg, int adjwin)
{
    int i;
    int rightmost_win;
    S_window *wp;

    /* the rightmost window changes in all cases */
    for (i=0; i<nwinlist; i++) {
	if (winlist[i]->rmarg == rm_orig) {
	    adj_w_borders(rm_orig, w_chg);
	    if (i == adjwin) {  /* we're done */
	    //  dbgpr("alt_w no=%d:  case: rightmost win\n", adjwin);
		return;
	    }
	    rightmost_win = i;
	    break;
	}
    }

    /*  Case N windows:
     *     change rm of adjwin
     *     change lm and rm of each win to the right
     *       except for rm of rightmost_win (done above)
     */

//  AScols lm_old, rt_old;
    AScols rm_old;

    /* adjust rm of adj window */
    wp = winlist[adjwin];

    rm_old = wp->rmarg;
//  rt_old = wp->rtext;
    wp->rmarg = (AScols) (wp->rmarg + w_chg);
    wp->rtext = (AScols) (wp->rmarg - wp->lmarg - 2);
    wp->redit = wp->rtext;

/** /
dbgpr("\nalt_w, adj win[%d], rm goes from %d to %d\n", i, rm_old, wp->rmarg );
dbgpr("\nalt_w, adj win[%d], rt goes from %d to %d\n", i, rt_old, wp->rtext );
/ **/

    /* change each win to right of adjwin */
    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	if (rm_old <= wp->lmarg) {
	//  lm_old = wp->lmarg;
	    wp->lmarg = (AScols) (wp->lmarg + w_chg);
	    wp->ltext = (AScols) (wp->lmarg + 1);
	//  dbgpr("alt_w, win[%d], lm goes from %d to %d\n\n", i, lm_old, wp->lmarg);

	    if (i != rightmost_win) {
	    //  rm_old = wp->rmarg;
	    //  rt_old = wp->rtext;
		wp->rmarg = (AScols) (wp->rmarg + w_chg);
		wp->rtext = (AScols) (wp->rmarg - wp->lmarg - 2);
	    //  dbgpr("alt_w, win[%d], rm goes from %d to %d\n\n", i, rm_old, wp->rmarg);
	    //  dbgpr("alt_w, win[%d], rt goes from %d to %d\n\n", i, rt_old, wp->rtext);
	    }
	    else {
		wp->rtext = (AScols) (wp->rmarg - wp->lmarg - 2);
		wp->redit = wp->rtext;
	    //  dbgpr("no rmarg change to rightmost win=%d, rtext=%d\n", i, wp->rtext );
	    }
	}
    }

    return;
}


int
CheckWindowValues()
{
    S_window *wp;
    int rc = 0;
    int i;

    /* check for valid entries */
    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	if (wp->ltext != wp->lmarg + 1) {
	    dbgpr("win %d bad ltext=%d should be %d\n",
		i, wp->ltext, wp->lmarg + 1);
	    rc = 1;
	}

	if (wp->ttext != wp->tmarg + 1) {
	    dbgpr("win %d bad ttext=%d should be %d\n",
		i, wp->ttext, wp->tmarg + 1);
	    rc = 1;
	}

	if (wp->rtext != (wp->rmarg - wp->lmarg - 2)) {
	    dbgpr("win %d bad rtext=%d should be %d\n",
		i, wp->rtext,  (wp->rmarg - wp->lmarg - 2));
	    rc = 1;
	}

	if (wp->btext != (wp->bmarg - wp->tmarg - 2)) {
	    dbgpr("win %d bad btext=%d should be %d\n",
		i, wp->btext,  (wp->bmarg - wp->tmarg - 2));
	    rc = 1;
	}
    }

    return rc;
}

