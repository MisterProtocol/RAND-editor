/*
 * handle resize windows events, 3 cases:
 *
 * 1.  a SIGWINCH by resizing the terminal window results in
 *     resizing e's internal windows to fit.
 *
 * 2.  a border move between two adjacent e windows, terminal
 *     window remains unchanged
 *
 * 3.  handle resize's while replaying a keyboard filel
 *
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

void hiliteactive(void);

//  case 1:
void ResizeWindows (int h, int w);
void debugWindow (S_window *, char *);
void debugAllWindows (void);
void ClearUtilityWindows(void);
void debugLMchars(S_window *wp);
void debugMrk(void);
void debug_fileflags(void);
Flag isFdinWindow(int fd);
void updateKeyfile(int, int);
extern int resized_max_h, resized_max_w;

int  CheckWindowValues(void);

// case 2

void adj_h_borders(ASlines bm_orig, ASlines h_chg);
void adj_w_borders(AScols rm_orig, AScols w_chg);
void alt_h_winresize(ASlines bm_orig, ASlines h_chg, int adjwin);
void alt_w_winresize(AScols rm_orig, AScols w_chg, int adjwin);
Flag atMoveableBorder(int, int);
int isTMmoveable(S_window *);  /* top margin moveable */
int isLMmoveable(S_window *);
int isRMmoveable(S_window *);
int isBMmoveable(S_window *);
void hiliteTopBorder(S_window *);
void hiliteLeftBorder(S_window *);

extern S_term term;
extern Flag optshowbuttons;
extern int nButtonLines;
extern void initwindows(Flag);
extern void buttoninit();
extern Uchar *image;
extern void infoinit(void);
//extern Flag freshputup;
extern Nlines marklines;
Flag noresizeall = NO;

extern char *hilite_str;
extern char *bold_str;
extern char *gray255;
extern char *sgr0;       /* reset all modes */

void MoveHorBorder (S_window *, S_window *, ASlines);
void MoveVerBorder (S_window *, S_window *, AScols);
int CheckWindowValues(void);
void clearImageArray(int, int, int);
int xlateBorderChar(int);
int adjCursor(S_window *, Nlines, int, int);
extern int moveBorder(MEVENT *);
extern int doHborder(MEVENT *ev, S_window *, S_window *);
extern int doVborder(MEVENT *ev, S_window *, S_window *);

//extern char *mouse_decode(MEVENT *ep);
extern int WinNumber(S_window *);

// case 3
void addResizeKeyfile(void);
void replayResize(FILE *);
void setResizeWindows (int, int);

Flag didReplayResize = NO;
extern int startup_h, startup_w;

/* Case 1*/

/*
 * called by SIGWINCH signal
 */
void
ResizeWindows (int h, int w)
{
    int h_chg = (h - term.tt_height);
    int w_chg = (w - term.tt_width);
    int i;

    /* Skip change if it ends up below overall minimum window size:
     *   top + bot borders, plus
     *   size of enter, info and button windows, plus
     *   2 lines for each window that have margin of 0
     */
    if( h_chg < 0) {
	int h_min = NPARAMLINES + nButtonLines + 2;  /* 2 for top/bottom borders*/
	for (i=0; i<nwinlist; i++) {
	    if (winlist[i]->lmarg == 0)
		h_min += 2; /* 1 line of text plus bottom border */
	}
	// dbgpr("h_min=%d, nwinlist=%d\n", h_min, nwinlist);
	if (h_min >= term.tt_height)
	    return;
    }

    S_window *oldwin = curwin;
    S_wksp *wksp_old = curwksp;

    int ccol = cursorcol;
    int clin = cursorline;

    static int cnt = 0;

    cnt++;
//  dbgpr("%03d -----------\n", cnt);
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

    extern Short screensize;
    extern Uchar *image;
    if (screensize) sfree (image);
    image = (Uchar *)NULL;

    d_init(YES,NO);     /* YES for clearmem, NO clearscr */

    /* Determine if all windows have the same orientation, eg:
     * all touch the bottom margin or all touch the right margin.
     * If so, adjust the space alternatively between them.
     * Otherwise, adjust only window touching the border.
     */
    int alternate_h = 0;    /* alternate vertical windows */
    int alternate_w = 0;    /* alternate horizontal windows */

    int adj_winnum = cnt % nwinlist;  /* which win num to adjust */

    /* -noresizeall:  option specifes only adjust border windows */
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
	Nlines wlin_orig = -1;
	S_window *botw;

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

//#ifdef OUT
	/* may need to adjust cursor if bottom win top line changed */

	if (h_chg > 0) {
	    for (i=0; i<nwinlist; i++) {
		if (winlist[i]->bmarg == (ASlines) bm_orig) {
		    botw = winlist[i];
		    wlin_orig = botw->wksp->wlin;
		}
	    }
	}
//#endif

	if (alternate_h) {
	    /* todo: alternate which window to adjust */
	    alt_h_winresize((ASlines) bm_orig, (ASlines) h_chg, adj_winnum);
	    // adj_h_borders (bm_orig, h_chg);
	}
	else {
	    /* adj windows touching bottom margin */
	    adj_h_borders ((ASlines) bm_orig, (ASlines) h_chg);
	}

//#ifdef OUT
	if (h_chg > 0) {
	    //dbgpr("wlin_orig=%d after=%d\n", wlin_orig, botw->wksp->wlin);
	    if (wlin_orig >0 && (wlin_orig != botw->wksp->wlin)) {
		//dbgpr("need to adj cursor: %d\n", wlin_orig - botw->wksp->wlin);
		clin += (int)(wlin_orig - botw->wksp->wlin);
	    }
	}
//#endif

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

#ifdef OUT
// now handled in e.m.c, via key=CCRESIZE
    ungetch('\015');    /* force redraw */
    ungetch('d');
    ungetch('e');
    ungetch('r');
    ungetch(CCCMD);
#endif

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

/*  todo:  if marked area is no longer visible,
 *  call unmark();
 */

    return;
}

void
debugWindow(S_window *w, char *winname)
{
    dbgpr("%12s lmarg=%3d tmarg=%3d rmarg=%3d bmarg=%3d\n",
	winname, w->lmarg, w->tmarg, w->rmarg, w->bmarg);
    dbgpr("%12s ltext=%3d ttext=%3d rtext=%3d btext=%3d\n",
	" ", w->ltext, w->ttext, w->rtext, w->btext);
    dbgpr("%12s ledit=%3d tedit=%3d redit=%3d bedit=%3d\n",
	" ", w->ledit, w->tedit, w->redit, w->bedit);

    return;
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

    dbgpr("utility windows: COLS=%d LINES=%d, cursorcol=%d cursorline=%d\n",
      COLS, LINES, cursorcol, cursorline);
    debugWindow (&wholescreen, "wholescreen");
//  debugWindow (&enterwin, "enterwin");
//  debugWindow (&infowin, "infowin");
#ifdef MOUSE_BUTTONS
    if (optshowbuttons == YES) {
      debugWindow (&buttonwin, "buttonwin");
    }
#endif

    /* edit windows */
    int i;
    int w, h;
    char tmp[16];
    Nlines ln_eof;
    S_window *wp;
    Flag fit_inwin = 0;
    for (i=0; i<nwinlist; i++) {
	sprintf(tmp, "win %d", i);
	wp = winlist[i];
	debugWindow(wp, tmp);
	dbgpr("%12s wksp: clin=%d  ccol=%d  wlin=%d, wcol=%d\n",
	    " ",
	    wp->wksp->clin,
	    wp->wksp->ccol,
	    wp->wksp->wlin,
	    wp->wksp->wcol);

	if (curwin == wp)
	    dbgpr("%12s curwin = winlist[%d]\n", " ", i);

	w = wp->rmarg - wp->lmarg;
	h = wp->bmarg - wp->tmarg;
	ln_eof = la_lsize(&wp->wksp->las);
	fit_inwin = (ln_eof < (wp->bmarg - wp->tmarg)) ? 1 : 0;

	dbgpr("%12s file: %s lines=%d w=%d h=%d fit_inwin=%d\n", " ",
	   names[wp->wksp->wfile], ln_eof, w,h, fit_inwin);

	/*debugLMchars(wp);*/
	/*debugMrk();*/
    }
    dbgpr("%12s ", "w");
    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	dbgpr(" %d", (int) (wp->rmarg - wp->lmarg));
    }
    dbgpr("\n");

    dbgpr("%12s ", "h");
    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	dbgpr(" %d", (int) (wp->bmarg - wp->tmarg));
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

    Nlines ln_eof;
    Flag fit_inwin;

    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	if (bm_orig == wp->bmarg) {
	    if (wp->btext == 0 && h_chg < 0)  /* maintain at least 1 line in window */
		continue;

	      /* number of lines in file */
	    ln_eof = la_lsize(&wp->wksp->las);
	      /* did eof fit in the original window */
	    fit_inwin = (ln_eof < (wp->bmarg - wp->tmarg)) ? 1 : 0;

	    wp->bmarg = (ASlines) (wp->bmarg + h_chg);
	    wp->btext = (ASlines) (wp->bmarg - wp->tmarg - 2);
	    wp->bedit = wp->btext;

	    /* if file line 1 is not window line 1, and
	     * the file did not originally fit in the window
	     * but now does, move file line 1 to window line 1.
	     */
	    if (h_chg > 0 && !fit_inwin && wp->wksp->wlin > 0 &&
		    (ln_eof < (wp->bmarg - wp->tmarg))) {
		wp->wksp->wlin = 0;
		//dbgpr("moved line 1 to win line 1\n");
	    }

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
	    if (w_chg < 0 && ((wp->rmarg - wp->lmarg) < 4))
		continue;
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
    if (wp->bedit < 1 && h_chg < 0)
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

/*
 *   Handle drag on a top margin or left margin of curwin.
 *   We know Button1 was pressed, we have
 *   multiple windows, and press occurred either
 *   on curwin.tmarg or curwin.lmarg
 */
int
moveBorder(MEVENT *ev)
{

/** /
dbgpr("moveBorder y:  ev=(%d,%d) cursorline=%d cursorcol=%d \
tmarg=%d, ttext=%d, bmarg=%d, btext=%d\n",
  ev->y, ev->x, cursorline, cursorcol,
  curwin->tmarg, curwin->ttext, curwin->bmarg, curwin->btext);
/ **/

/** /
dbgpr("moveBorder x:  ev=(%d,%d) cursorline=%d cursorcol=%d \
lmarg=%d, ltext=%d, rmarg=%d, rtext=%d\n",
  ev->y, ev->x, cursorline, cursorcol,
  curwin->lmarg, curwin->ltext, curwin->rmarg, curwin->rtext);
/ **/

    /* if we've recovered from a crash, or replayed a session
     * our full window size may be less than the terminal window size
     * and resizing is not a good idea.
     */
    if (didReplayResize && ((startup_h > term.tt_height)
	    || (startup_w > term.tt_width))) {
	mesg(ERRALL+1, "Border resizing not allowed after a replay.");
	return 0;
    }

    /* are the boundaries ok */
    {
	if (ev->y < 2)  return 0;
	if (ev->y >= curwin->bmarg) return 0;
	if (ev->x  < curwin->lmarg) return 0;
	if (ev->x  > curwin->rmarg) return 0;
	// dbgpr("movingBorder:  boudaries ok\n");
    }

    //S_window __attribute__((unused)) *wp;
    int i;
    int rc = 1;     /* Yes, we have a border move */

    S_window *twin, *bwin, *lwin, *rwin, *savwin;
    savwin = curwin;   /* restore on any return() if changed */

    int ccol = cursorcol;
    int clin = cursorline;

    /* moving a horizontal or vertical border */
    int haveHmove = 0;
    int haveVmove = 0;

    if (ev->y == curwin->tmarg) {  /* a horizontal border */
	if ((i = isTMmoveable(curwin)) >= 0) {
	    twin = winlist[i];
	    bwin = curwin;
	    haveHmove = 1;
	    rc = 0;
	}
    }

    if (!haveHmove ) {
	 /* if press is on lm, find win whose rm matches the lm of curwin */
	if (ev->x == curwin->lmarg && (ev->x >= 2) ) {
	    if ((i = isLMmoveable(curwin)) >= 0 ) {
		lwin = winlist[i];
		rwin = curwin;
		haveVmove = 1;
		rc = 0;
	    }
	}
    }

    if (!haveHmove && !haveVmove) {
	dbgpr("No windows found with movable borders ev->(y,x)=(%d,%d)\n",ev->y, ev->x);
	return 0;
    }

    int rc1;

    if (haveHmove) {
	Nlines __attribute__((unused)) wlin_orig = bwin->wksp->wlin;
	int begy = ev->y;

	rc1 = doHborder(ev, bwin, twin);

	//dbgpr("rc1=%d from doHborder()\n", rc1);

	/* may need to adjust cursor if line 1 was moved */
	if (rc1 && (wlin_orig != bwin->wksp->wlin)) {
	    clin = adjCursor(bwin, wlin_orig, clin, begy);
	}
    }
    else {
	rc1 = doVborder(ev, rwin, lwin);
	//dbgpr("rc1=%d from doVborder()\n", rc1);
    }

    if (rc1 == 0) return 0;   /* no move occurred */

    /*
     *  Without a doubt there are extra fresh(), putup(), and drawborder()
     *  calls below.  The exact "abracadabra" to only call them once
     *  is still a work in progress....
     */

    /* restore the curwin variable before redrawing borders */
    curwin = savwin;

    for (i=0; i<nwinlist; i++) {
	switchwindow (winlist[i]);
	limitcursor();
	poscursor(0, 0);
	putupwin();
	if (winlist[i] != curwin)
	    drawborders (winlist[i], WIN_INACTIVE | WIN_DRAWSIDES);
	else
	    drawborders (winlist[i], WIN_ACTIVE | WIN_DRAWSIDES);
	fresh();
	d_put(0);
    }

    /* seems we need to redraw borders again */
    for (i=0; i<nwinlist; i++) {
	if (curwin != winlist[i]) {
	    switchwindow(winlist[i]);
	    poscursor(0,0);
	    putupwin();
	    drawborders (winlist[i], WIN_INACTIVE | WIN_DRAWSIDES);
	}
	d_put(0);
	fresh();
	fflush(stdout);
    }

    //dbgpr("curwin=%d savwin=%d\n", WinNumber(curwin), WinNumber(savwin));

    curwin = savwin;
    switchwindow(curwin);
    poscursor(0,0);
    limitcursor();
    putupwin();
    drawborders (curwin, WIN_ACTIVE | WIN_DRAWSIDES);
    //fresh();
    //d_put(0);
    fflush(stdout);

    //  switchwindow(curwin);

    //dbgpr("ccol=%d vs rtext=%d, clin=%d vs btext=%d ttext=%d\n",
    //ccol, curwin->rtext, clin, curwin->btext, curwin->ttext);

    if (ccol > curwin->rtext) ccol = curwin->rtext;
    if (clin > curwin->btext) clin = curwin->btext;
    poscursor(ccol, clin);
    d_put(0);
    fresh();
    fflush(stdout);

    /* as per limitcursor() in e.t.c */
    for (i=0; i<nwinlist; i++) {
	winlist[i]->wksp->ccol = min (winlist[i]->wksp->ccol, winlist[i]->rtext);
	winlist[i]->wksp->clin = min (winlist[i]->wksp->clin, winlist[i]->btext);
	winlist[i]->altwksp->ccol = min (winlist[i]->altwksp->ccol, winlist[i]->rtext);
	winlist[i]->altwksp->clin = min (winlist[i]->altwksp->clin, winlist[i]->btext);
    }

    /* force a redraw */
    ungetch('\015');
    ungetch('d');
    ungetch('e');
    ungetch('r');
    ungetch(CCCMD);

    rc = 1;     /* a border move occurred */

    infoinit();
    unmark();

    addResizeKeyfile(); // save resize info in keystroke file

    return rc;

}

/*
 * adjust windows touching top/bottom margins
 */
void
MoveHorBorder (S_window *botw, S_window *topw, ASlines y_new)
{
    /* adjust borders of top and bot windows */

//  S_window *topw;
    ASlines h_chg;
    h_chg = (ASlines) (y_new - botw->tmarg);

//    topw = winlist[w_no];

//  ASlines bm_top = topw->bmarg;
//  ASlines owlin = (ASlines) botw->wksp->wlin;

    /* Don't allow a window to be less than 1 line in height */
//  if (topw->bmarg < 3 && h_chg < 0)
//      return;

    topw->bmarg = (ASlines) (topw->bmarg + h_chg);
    topw->btext = (ASlines) (topw->bmarg - topw->tmarg - 2);
    topw->bedit = topw->btext;

    botw->tmarg = (ASlines) (botw->tmarg + h_chg);
    botw->ttext = (ASlines) (botw->tmarg + 1);
    botw->btext = (ASlines) (botw->bmarg - botw->tmarg - 2);
    botw->bedit = botw->btext;
    botw->tedit = 0;

//  botw->wksp->wlin += h_chg;
//  if (botw->wksp->wlin < 0) botw->wksp->wlin = 0;

    Nlines ln_eof = la_lsize(&botw->wksp->las);
    if (botw->wksp->wlin > ln_eof) {
	//dbgpr("wlin > ln_eof, wlin set to 0\n";
	botw->wksp->wlin = 0;
    }

    /*  if a small file fits in the bottom window and
     *  the top border has been moved so that line 1 is
     *  no longer visible, adjust wlin so the window
     *  starts with line 1
     */
    if (botw->wksp->wlin > 0) {
	//dbgpr("cmp ln_eof %d vs %d\n", ln_eof, (botw->bmarg - botw->tmarg));
	if (ln_eof < (Nlines) (botw->bmarg - botw->tmarg)) {
	    botw->wksp->wlin = 0;
	    //dbgpr("--file fits in botw, setting wlin to 0\n");
	}
    }

//  botw->wksp->clin = (ASlines) (botw->wksp->clin + h_chg);

// dbgpr("\nbot win, wlin goes from  %d to %d\n", owlin, botw->wksp->wlin );

    return;
}

/*
 * adjust borders of left and right touching windows
 */
void
MoveVerBorder (S_window *rwin, S_window *lwin, AScols x_new)
{

    Scols w_chg;

    w_chg = x_new - rwin->lmarg;

    rwin->lmarg = (AScols) (rwin->lmarg + w_chg);
    rwin->ltext = (AScols) (rwin->lmarg + 1);
    rwin->rtext = (AScols) (rwin->rmarg - rwin->lmarg - 2);

    lwin->rmarg = (AScols) (lwin->rmarg + w_chg);
    lwin->rtext = (AScols) (lwin->rmarg - lwin->lmarg - 2);
    lwin->redit = lwin->rtext;

  //dbgpr("\ntop rwin, lm goes from %d to %d\n", i, lmarg, rwin->lmarg );

    return;
}

#ifdef OUT
int
AllVertWins()
{
return 1;
    int rc = 1;     /* assume yes, all vertical */
    int i;

    for (i=1; i<nwinlist; i++) {
	if (winlist[i]->tmarg != winlist[0]->tmarg) {
	    rc = 0; /* nope */
	    break;
	}
    }
    return rc;
}

int
AllHorWins()
{
return 0;
    int rc = 1;     /* assume yes, all horizontal */
    int i;

    for (i=1; i<nwinlist; i++) {
	if (winlist[i]->lmarg != winlist[0]->lmarg) {
	    rc = 0; /* nope */
	    break;
	}
    }
    return rc;
}
#endif /* OUT */

void
clearImageArray (int beg_l, int end_l, int wid)
{
    char *cp0, *cp1;
    Uint cnt;

    /* fill the contents of image[] with blanks
     * from beg_l to end_l
     */

    /* determine how many bytes to clear */

    cp0 = (char *)image + wid * beg_l;
    cp1 = (char *)image + wid * end_l;
    cnt = (Uint) (cp1 - cp0);

    fill (cp0, cnt, ' ');

//dbgpr("clearImageArray:  beg_l=%d end_l=%d wid=%d cnt=%d\n",
//beg_l, end_l, wid, cnt);

    return;
}


int
xlateBorderChar(int border_ch)      /* 129-142, see e.t.h */
{
    if (border_ch < 127)
	return border_ch;

    switch (border_ch) {
	case LMCH:
	case RMCH:
	    return '|';
	case MLMCH:
	    return '<';
	case MRMCH:
	    return '>';
	case TMCH:
	case BMCH:
	    return '-';
	case TLCMCH:
	case TRCMCH:
	case BLCMCH:
	case BRCMCH:
	case TTMCH:
	case BTMCH:
	    return '+';
	case ELMCH:
	    return ';';
	case INMCH:
	    return '.';

	default:
	    break;
    }

    dbgpr("xlate: unknown %d\n", border_ch);

    return '?';
}


void
hiliteactive()
{
    int i;
    int w = term.tt_width;
    Uchar border[w];
    Uchar *cp = (Uchar *)image + w * curwin->tmarg;

//  int clin = cursorline;
//  int ccol = cursorcol;

    for (i=1; i<w; i++) {
	border[i] = (Uchar) xlateBorderChar(cp[i]);
    }
    border[i] = '\0';
//dbgpr("border:(%s)\n", border+1);

    int len = curwin->rmarg - curwin->lmarg - 1;
    mvcur(-1,-1, curwin->tmarg, curwin->lmarg+1);
//  mvcur(-1,-1, 0, 0);
//  tputs(bold_str, 1, Pch);
    tputs(hilite_str, 1, Pch);
    fwrite(border+1, sizeof (char), (size_t) len, stdout);
    tputs(sgr0, 1, Pch);
    fflush(stdout);

    return;
}


/*  doHborder - move a horizontal border between top/bot windows
 *
 *  returns 1 if a move occurred, caller must redraw windows
 *  returns 0 on err, no further action required concerning border moves.
 */

int
doHborder(MEVENT *ev, S_window *bwin, S_window *twin)
{

    //dbgpr("doHborder, ev->x=%d ev->y=%d\n", ev->x, ev->y);

    int i;
    int rc = 1;     /* Yes, we have a border move */

    MEVENT evt;
    int c, begy, begx;
    begy = ev->y;   /* same as bwin->tmarg */
    begx = ev->x;   /* same as bwin->lmarg */
    int y_new /*x_new*/;
    int y_last, __attribute__((unused)) x_last;
    y_last = begy;
    x_last = begx;

    int w = term.tt_width;
    int __attribute__((unused)) h = term.tt_height;
    Uchar *cp;
    int bline = ev->y;      /* border line */

    cp = (Uchar *)image + w * bline;
    Uchar h_border[w];

    /* curwin beg/end points */
    int lm = (int) bwin->lmarg;
    /* border width (bwin may not start at 0) */
    int bwid = (int) (bwin->rmarg - bwin->lmarg - 1);

    /* make a copy of the topmost border, which are '+' and '-' chars */
    //for (i=0; i < w; i++) {     /* omit margin tics, '-' and '+'  */
    for (i=0; i < bwid; i++) {     /* omit margin tics, '-' and '+'  */
	//h_border[i] = cp[i+1] == TMCH ? '-' : '+';  /* see e.t.h */
	h_border[i] = (Uchar) xlateBorderChar(cp[lm+i+1]);
    }
    h_border[--i] = '\0';

    //dbgpr("h_border: %s\n", h_border);
    //for(i=0; i<15; i++)
    //  dbgpr("(%o)(%c)\n", h_border[i], h_border[i]);


    /*  Restrict motion to exclude screen borders, and
     *  moving past the top/left edge of the adjacent window.
     *  Also, the resize needs to leave at least 1 line in a window.
     */
    int ymin, ymax;

    /* leave at least 1 line of top and bot win */
    ymin = twin->ttext;
    ymax = bwin->ttext + bwin->btext;

    /* highlight the current border */
    mvcur(-1, -1, ev->y, lm+1);
    tputs(bold_str,1,Pch);
//  fwrite(h_border, sizeof (char), (size_t) w-2, stdout);
    fwrite(h_border, sizeof (char), (size_t) bwid, stdout);
    tputs(sgr0, 1, Pch);
    mvcur(-1, -1, ev->y, ev->x);
    fflush(stdout);

    //dbgpr("ymin=%d ymax=%d\n", ymin, ymax);

    /* for highlighting horizontal tics as
     * border is moved up/down
     */
    char h_tics[w+1];   /* may not need full width */
    for (i=0; i<w-2; i++) h_tics[i] = '-';
    h_tics[i] = '\0';
    int h_len = (int) strlen(h_tics);
    char row_buf[w+1], *hbuf;

    int firsttime = 1;

    /* collect mouse move events, already have a press-1 */
    while(1) {
	c = wgetch(stdscr);
	//dbgpr("doHborder: got (%03o)\n", c);
	switch(c) {
	    case KEY_MOUSE:
		break;
	    case ERR:
	    case 'q':       /* esc from loop from any bad coding below */
		return 0;
	    default:  /* some other char */
		ungetch(c);
		return 0;
	}

	if (getmouse(&evt) != OK) {
	    dbgpr("doHborder:  getmouse err waiting for release event\n");
	    return 0;
	}

	//dbgpr("event: %s\n", mouse_decode(&evt));

#if NCURSES_MOUSE_VERSION == 1
	if (evt.bstate & BUTTON1_RELEASED) {
	    /* a click on the original border w/o movement */
	    /* restore original horizontal border */
	    //mvcur(-1, -1, evt.y, 1);
	    //fwrite(h_border, sizeof (char), (size_t) w-2, stdout);
	    mvcur(-1, -1, evt.y, lm+1);
	    fwrite(h_border, sizeof (char), (size_t) bwid, stdout);
	    mvcur(-1, -1, evt.y, evt.x);
	    dbgpr("click w/o move at (%d,%d)\n", evt.y, evt.x);
	    fflush(stdout);
	    return 0;
	}
#endif

#if NCURSES_MOUSE_VERSION > 1
	if (evt.bstate & REPORT_MOUSE_POSITION) {
#else
	if (evt.bstate & BUTTON4_PRESSED) {
#endif
	    if (evt.y <= ymin || evt.y >= ymax) {
		//dbgpr("evt.y(%d) <= ymin(%d) or >= ymax(%d), continue\n",
		//evt.y, ymin, ymax);
		continue;
	    }

	    if (firsttime) {
		hbuf = (char *)image + w * evt.y;
		//snprintf(row_buf, (size_t)h_len+1, "%s", hbuf+lm+1);
		snprintf(row_buf, (size_t)h_len+1, "%s", hbuf+1);
	    //  dbgpr("row_buf: (%s)\n", row_buf);
		firsttime = 0;
	    }
	    else {  /* repaint saved text */
		mvcur(-1, -1, y_last, lm+1);
		//tputs(row_buf, 1, Pch);
		if (y_last != begy) {
		    fwrite(row_buf, sizeof(char), (size_t) bwid, stdout);
		}
		else {  /* rewrite original border line in bold */
		    tputs(bold_str,1,Pch);
		    fwrite(h_border, sizeof (char), (size_t) bwid, stdout);
		    tputs(sgr0, 1, Pch);
		}
		//fflush(stdout);
	    }

	    hbuf = (char *)image + w * evt.y;
	    //snprintf(row_buf, (size_t)h_len+1, "%s", hbuf+1);
	    snprintf(row_buf, (size_t)h_len+1, "%s", hbuf+lm+1);
	 // dbgpr("row_buf: (%s)\n", row_buf);

	    mvcur(-1, -1, evt.y, lm+1);
	    //tputs(brace_p,1,Pch);
	    tputs(hilite_str,1,Pch);
	    //tputs(h_tics, 1, Pch);
	    fwrite(h_tics, sizeof (char), (size_t) bwid, stdout);
	    tputs(sgr0, 1, Pch);
	    mvcur(-1, -1, evt.y, evt.x); /* back to mouse orig x */
	    fflush(stdout);
	    y_last = evt.y;

	//  dbgpr("button4_pressed, y_new is %d\n", evt.y);

	    continue;
	}

#if NCURSES_MOUSE_VERSION > 1
	if (evt.bstate & BUTTON1_RELEASED) {
	    break;
	}
#else
	if (evt.bstate & BUTTON4_RELEASED) {
	    break;
	}
#endif
    }

    //dbgpr("button release, move hor border to %d\n", evt.y);

    if (evt.y <= (twin->tmarg+1) ) {
	dbgpr("Can't move past top margin of the adjacent window\n");
	mesg(ERRALL+1, "Can't move past the top margin of the adjacent window");
	rc = 0;
	return 0;
    }

    if (evt.y == bwin->tmarg) {
	dbgpr("no change in border position\n");
	/* rewrite original border */
	mvcur(-1, -1, evt.y, 1);
	fwrite(h_border, sizeof (char), (size_t) w-2, stdout);
	return 0;
    }

    /* shouldn't need this... */
    if (evt.y >= ymax) evt.y = ymax - 1;

    y_new = evt.y;
    //dbgpr("release:  y_new=%d, ymax=%d\n", y_new, ymax);

//  Nlines wlin_orig = bwin->wksp->wlin;
    MoveHorBorder(bwin, twin, (ASlines) y_new);

    if (!CheckWindowValues()) {
    //  dbgpr("window parameters ok\n");
	;
    }

    /*  Fill the contents of image[] with blanks from the
     *  topmost change to the bmarg of the bottom window.
     *  If border moved down, start at begy.
     */
    int from = begy < evt.y ? begy : evt.y;
    clearImageArray(from, (int)bwin->bmarg, w);

    //dbgpr("doHborder, end, rc=%d\n");

    return rc;

}

/*  doVborder - move a vertical border between left/rt windows
 *
 *  returns 1 if a move occurred, caller must redraw windows
 *  returns 0 on err, no further action required concerning border moves.
 */

int
doVborder(MEVENT *ev, S_window *rwin, S_window *lwin)
{
    int i;
    int rc = 1;     /* Yes, we have a border move */
    //int __attribute__((unused)) w_no;

    /*  lwin and rwin point to adjacent windows
     */

    MEVENT evt;
    int c, begy, begx;
    begy = ev->y;   /* same as curwin->tmarg */
    begx = ev->x;   /* same as curwin->lmarg */
    int __attribute__((unused)) y_new, x_new;
    int __attribute__((unused)) y_last, x_last;
    y_last = begy;
    x_last = begx;

    int w = term.tt_width;
    int h = term.tt_height;
#ifdef OUT
    /* after a replay/recovery, our screensize may be larger
     * than our max window size.  Should probably just
     * post a can't do error.
     */
    int w = (int) (wholescreen.rmarg - wholescreen.lmarg + 1);
    int h = (int) (wholescreen.bmarg - wholescreen.tmarg + 1);
#endif /* OUT */

//  Uchar __attribute__((unused)) buf[w+1];
    Uchar *cp;
    int bline = ev->y;      /* border line */

    cp = (Uchar *)image + w * bline;
//  Uchar __attribute__((unused)) h_border[w];

    Uchar v_border[h];

    for (i=rwin->ttext; i < rwin->bmarg; i++) {     /* omit top/bot tics */
	/* v_border[0] not used */
	cp = (Uchar *)image + w * i;
	v_border[i] = (Uchar) xlateBorderChar(cp[ev->x]);
    }
    v_border[i] = '\0';
    //dbgpr("v_border: %s\n", v_border+1);

    /*  Restrict motion to exclude screen borders, and
     *  moving past the top/left edge of the adjacent window.
     *  Also, the resize needs to leave at least 1 line in a window.
     */
    int xmin, xmax;

    /* leave at least 1 column of rwin and lwin */
    xmax = rwin->ltext + rwin->rtext;
    xmin = lwin->ltext + 1;   /* start of left window */

    /* highlight the current vertical column */
    for (i=rwin->ttext; i<rwin->bmarg; i++) {
	mvcur(-1, -1, i, ev->x);
	//futs(hilite_str,1,Pch);
	tputs(bold_str,1,Pch);
	fputc('|', stdout);
	tputs(sgr0, 1, Pch);
    }
    mvcur(-1, -1, ev->y, ev->x);
    fflush(stdout);

//  dbgpr("xmin=%d xmax=%d\n", xmin, xmax);

    /* for highlighting vertical tics as
     * border is moved left/right
     */
    char v_tics[h+1];
    for (i=0; i<h-2; i++) v_tics[i] = '|';
    v_tics[i] = '\0';

    Uchar col_buf[h+1];

    int firsttime = 1;

    /* collect mouse move events, already have a press-1 */
    while(1) {
	c = wgetch(stdscr);
	//dbgpr("moveBorder: got (%03o)\n", c);

	switch(c) {
	    case KEY_MOUSE: /* what we expect */
		break;
	    case ERR:
	    case 'q':       /* esc from loop from any bad coding below */
		return 0;
	    default:  /* some other char */
		ungetch(c);
		return 0;
	}

	if (getmouse(&evt) != OK) {
	    dbgpr("doVborder:  getmouse err waiting for release event\n");
	    return 0;
	}

	//dbgpr("event: %s\n", mouse_decode(&evt));


#if NCURSES_MOUSE_VERSION == 1
	if (evt.bstate & BUTTON1_RELEASED) {
	    /* a click on the original border w/no movement */

	    /* restore original vertical border */
	    for (i=rwin->ttext; i<rwin->bmarg; i++) {
		mvcur(-1, -1, i, evt.x);
		fputc(v_border[i], stdout);
	    }

	    mvcur(-1, -1, evt.y, evt.x);
	    dbgpr("click w/o move at (%d,%d)\n", evt.y, evt.x);
	    fflush(stdout);
	    return 0;
	}
#endif
	if (evt.bstate & BUTTON1_CLICKED) /* not possible ? */
	    return 1;

#if NCURSES_MOUSE_VERSION > 1
	if (evt.bstate & REPORT_MOUSE_POSITION) {
#else
	if (evt.bstate & BUTTON4_PRESSED) {
#endif

	    if (evt.x < xmin || evt.x >= xmax)
		continue;

		/*  As the border is moved left or right, save
		 *  the chars pointed at by the cursor (evt.x) from
		 *  line 1 to the window bottom.  This 1-char wide column
		 *  will be re-written as the cursor moves.
		 */
	    if (firsttime) {
		    /* note: col_buf[0] is not used */
		for (i=rwin->ttext; i<rwin->bmarg; i++) {
		    cp = (Uchar *)image + w * i;
		    col_buf[i] = cp[evt.x];
		}
		col_buf[i] = '\0';
		//dbgpr("col_buf=(%s) firsttime\n", col_buf+1);
		firsttime = 0;
	    }
	    else {  /* rewrite saved column of chars */
		if (evt.x != begx) {
		    if (x_last != begx) { /* not at orig border */
			for (i=rwin->ttext; i<rwin->bmarg; i++) {
			    mvcur(-1, -1, i, x_last);
			    fputc(col_buf[i], stdout);
			}
		    }
		    else {
			for (i=rwin->ttext; i<rwin->bmarg; i++) {
			    mvcur(-1, -1, i, begx);
			    //fputs(hilite_str,1,Pch);
			    tputs(bold_str,1,Pch);
			    fputc('|', stdout);
			    //fputc(v_tics[i], stdout);
			    tputs(sgr0, 1, Pch);
			}
		    }
		    mvcur(-1, -1, evt.y, evt.x); /* restore yx position */
		}
		else {  /* hilite the original vert border */
		    for (i=rwin->ttext; i<rwin->bmarg; i++) {
			mvcur(-1, -1, i, x_last);
			fputc(col_buf[i], stdout);
		    }

		    mvcur(-1, -1, evt.y, begx); /* restore yx position */

		    for (i=rwin->ttext; i<rwin->bmarg; i++) {
			mvcur(-1, -1, i, begx);
			//fputs(hilite_str,1,Pch);
			tputs(bold_str,1,Pch);
			//fputc('|', stdout);
			fputc(v_tics[i], stdout);
			tputs(sgr0, 1, Pch);
		    }
		    mvcur(-1, -1, evt.y, begx); /* restore y position */
		    fflush(stdout);
		}
	    }

	    /* save 1 char from each line, top to bottom */
	    if (evt.x != begx) {    /* unless x = original vert border */
		for (i=rwin->ttext; i<rwin->bmarg; i++) {
		    cp = (Uchar *)image + w * i;
		    col_buf[i] = cp[evt.x];
		}
	    }
	    col_buf[i] = '\0';
	    //dbgpr("col_buf=(%s)\n", col_buf+1);

	    /* hilite the x position, top to bottom */
	    for (i=rwin->ttext; i<rwin->bmarg; i++) {
		mvcur(-1, -1, i, evt.x);
		tputs(hilite_str,1,Pch);
		fputc('|', stdout);
		//fputc(v_tics[i], stdout);
		tputs(sgr0, 1, Pch);
	    }

	    x_last = evt.x;
	    mvcur(-1, -1, evt.y, evt.x);
	    fflush(stdout);
	    //  dbgpr("button4_pressed, x_new is %d\n", evt.x);
	    continue;
	}

#if NCURSES_MOUSE_VERSION > 1
	if (evt.bstate & BUTTON1_RELEASED) {
	    break;
	}
#else
	if (evt.bstate & BUTTON4_RELEASED) {
	    break;
	}
#endif
    }

    //dbgpr("button release, move vert border to %d\n", evt.x);

    /* need this??? */
    if (evt.x < xmin) evt.x = xmin;
    if (evt.x >= xmax) evt.x = xmax-1;

    if (evt.x <= (lwin->lmarg+1) ) {
	dbgpr("Can't move past left margin of the adjacent window\n");
	mesg(ERRALL+1, "Can't move past the left margin of the adjacent window");
	return 1;
    }

    if (evt.x == begx) {
	/* restore original vertical border */
	for (i=rwin->ttext; i<rwin->bmarg; i++) {
	    mvcur(-1, -1, i, evt.x);
	    fputc(v_border[i], stdout);
	}
	dbgpr("no change in horizontal border position\n");
	mvcur(-1, -1, begy, begx);
	fflush(stdout);
	return 1;
    }

    x_new = evt.x;

    MoveVerBorder(rwin, lwin, (AScols) x_new);

    if (!CheckWindowValues()) {
    //  dbgpr("window parameters ok\n");
	;
    }

    clearImageArray(1, rwin->bmarg, w);

    //dbgpr("doVborder, end, rc=%d\n");

    return rc;
}


/* returns 1 if cursor is positioned on a "movable" border.
 *
 */

Flag
atMoveableBorder(int y, int x)
{
       /* todo, more conditions apply */
    if ((y == curwin->tmarg && y > 0) || (x == curwin->lmarg && x > 0)) {
	return 1;
    }

    return 0;
}


int
adjCursor (S_window *bwin, Nlines wlin_orig, int clin, int begy)
{

    int y_new = bwin->tmarg;

    /* adjust cursor to remain on original line */

    //dbgpr("wlin=(%d) clin=%d begy=%d y_new=%d tmarg=%d, wlin_orig=%d\n",
    //    curwksp->wlin, clin, begy, y_new, curwin->tmarg, wlin_orig);

    if (wlin_orig != bwin->wksp->wlin) {
	int h_chg = abs( (int)(wlin_orig - bwin->wksp->wlin) );
	//int  clin_old = clin;  /* dbg */
	if (y_new < begy)
	    clin += h_chg;
	else
	    clin -= h_chg;
	//dbgpr(" cursor fix:, clin changed from %d to %d\n", clin_old, clin);
    }

    return clin;
}



void
hiliteTopBorder(S_window *wp) /* active window */
{

    if (didReplayResize) return;
    if (wp->tmarg == 0) return;
    if (isTMmoveable(wp) < 0) return;

    int w = (int) (wp->rmarg - wp->lmarg);
    int x, y;

    x = (int) (wp->lmarg + w/2);
    y = (int) wp->tmarg;

    //dbgpr("hiliteTopBorder:  x=%d y=%d\n", x, y);

    mvcur(-1, -1, y, x);
    //tputs(hilite_str,1,Pch);
    tputs(gray255,1,Pch);
    tputs("==", 1, Pch);
    tputs(sgr0, 1, Pch);
    mvcur(-1, 1, cursorline + wp->ltext, cursorcol + wp->ltext);
    fflush(stdout);

    return;
}

void
hiliteLeftBorder(S_window *wp) /* active window */
{

    if (didReplayResize) return;
    if (wp->lmarg == 0) return;
    if (isLMmoveable(wp) < 0) return;

    int x, y;

    x = (int) (wp->lmarg);
    y = (int) (wp->tmarg + (wp->bmarg - wp->tmarg) / 2);

    //dbgpr("hiliteLeftBorder:  x=%d y=%d\n", x, y);

    mvcur(-1, -1, y, x);
    //tputs(hilite_str,1,Pch);
    tputs(gray255,1,Pch);
    /* two vertical bars */
    fputc('|', stdout);
    mvcur(-1, -1, y-1, x);
    fputc('|', stdout);
    tputs(sgr0, 1, Pch);
    mvcur(-1, 1, cursorline + wp->ltext, cursorcol + wp->ltext);
    fflush(stdout);

    return;
}


/* returns -1 if win TM not moveable
 * returns index into winlist[] if it is
 */

int
isTMmoveable(S_window *win)
{
    int i;
    S_window *wp;
    int w_no = -1;

    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	    /* share top/bot margins, and same width */
	if ((wp->bmarg == win->tmarg) &&
		(wp->lmarg == win->lmarg) &&
		(wp->rmarg == win->rmarg)) {
	    w_no = i;
	    break;
	}
    }
    //dbgpr("isTMmoveable: w_no=%d\n", w_no);

    return w_no;
}

/* returns -1 if win LM not moveable
 * returns index into winlist[] if it is
 */

int
isLMmoveable(S_window *win)
{
    int i;
    S_window *wp;
    int w_no = -1;

    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	    /* share lt/rt margins, and same height */
	if ((wp->rmarg == win->lmarg) &&
		(wp->tmarg == win->tmarg) &&
		(wp->bmarg == win->bmarg)) {
	    w_no = i;
	    break;
	}
    }
    //dbgpr("isLMmoveable: w_no=%d\n", w_no);

    return w_no;
}

/* returns -1 if win RM not moveable
 * returns index into winlist[] if it is
 */
int
isRMmoveable(S_window __attribute__((unused)) *wp)
{
    return -1;
}

/* returns -1 if win BM not moveable
 * returns index into winlist[] if it is
 */

int
isBMmoveable(S_window __attribute__((unused)) *wp)
{
    return -1;
}


/* for tips on how to "release" an inactive file
 * see dlfile() and the delete file command in e.nm.c
 */

void
debug_fileflags()
{
    int i;

    int first_user_fd = FIRSTFILE + NTMPFILES + 1;

    dbgpr("first_user_fd=%d\n", first_user_fd);

    for (i = FIRSTFILE + NTMPFILES; i < MAXFILES; i++) {
	char bits[17];
	if (names[i] == NULL || *names[i] == '\0')
	    break;

	int j;
	short fflag = fileflags[i];

	for (j=0; j<16; j++) {
	    bits[16-j-1] = (fflag&1) ? '1' : '0';
	    //fflag = (fflag>>1);
	    fflag >>= 1;
	}
	bits[16] = '\0';
	dbgpr("filename:  names[%d]=%s fileflags=%o bits=(%s)\n", i, names[i], fileflags[i], bits);
	dbgpr("      INUSE=%d\n", (fileflags[i] & INUSE) ? 1:0);
	dbgpr(" DWRITEABLE=%d\n", (fileflags[i] & DWRITEABLE) ? 1:0);
	dbgpr(" FWRITEABLE=%d\n", (fileflags[i] & FWRITEABLE) ? 1:0);
	dbgpr("  CANMODIFY=%d\n", (fileflags[i] & CANMODIFY) ? 1:0);
	dbgpr("    INPLACE=%d\n", (fileflags[i] & INPLACE) ? 1:0);
	dbgpr("      SAVED=%d\n", (fileflags[i] & SAVED) ? 1:0);
	dbgpr("        NEW=%d\n", (fileflags[i] & NEW) ? 1:0);
	dbgpr("    DELETED=%d\n", (fileflags[i] & DELETED) ? 1:0);
	dbgpr("    RENAMED=%d\n", (fileflags[i] & RENAMED) ? 1:0);
	dbgpr("     UPDATE=%d\n", (fileflags[i] & UPDATE) ? 1:0);
	dbgpr("    SYMLINK=%d\n", (fileflags[i] & SYMLINK) ? 1:0);
	dbgpr(" FILELOCKED=%d\n", (fileflags[i] & FILELOCKED) ? 1:0);
	dbgpr("   CANTLOCK=%d\n", (fileflags[i] & CANTLOCK) ? 1:0);
	dbgpr("   inWindow=%d\n", isFdinWindow(i));
	if (fileflags[i] & INUSE)
	    dbgpr("la_modified=%d\n", la_modified(&fnlas[i]));
	if (!la_modified(&fnlas[i])
		&& (i > first_user_fd)
		&& (!isFdinWindow(i))
		&& (fileflags[i] & UPDATE)
		&& !(fileflags[i] & RENAMED)
		&& !(fileflags[i] & FILELOCKED))
	    dbgpr(" *could remove, not mod,first,inWin,and UPDATE set\n");
    }

    return;
}

/*  Is fd displayed in any window or the alt file
 *  of any window
 */

Flag
isFdinWindow(int fd)
{
    int i;
    for (i=0; i<nwinlist; i++) {
	if (winlist[i]->wksp->wfile == fd)
	    return YES;
	if (winlist[i]->altwksp && winlist[i]->altwksp->wfile == fd)
	    return YES;
    }

    return NO;
}


/* case 3, keystroke file */

/* record a resize event in the keystroke file */

void
addResizeKeyfile()
{

    /*   update keystroke file with new window size info
     */

    /* Start with "{CCRESIZE} h=nn w=nn nwin=nn\n"
     * Subsequent *lines* will contain info for each window.
     */

    /* Write {CCRESIZE} h=nn w=nn nwin=nn\n to keys file, and flushkeys */
    fprintf(keyfile, "%c h=%d w=%d nwin=%d\n",
	CCRESIZE, term.tt_height, term.tt_width, nwinlist);

    int i;
    S_window *wp;

    for (i=0; i<nwinlist; i++) {
	wp = winlist[i];
	fprintf(keyfile, "win %d tm=%d bm=%d lm=%d rm=%d ",
	    i, wp->tmarg, wp->bmarg, wp->lmarg, wp->rmarg);

	/*  these can be calculated, but since we have the values
	 *  use them for simplicity
	 */
	fprintf(keyfile, "tt=%d bt=%d lt=%d rt=%d ",
	    wp->ttext, wp->btext, wp->ltext, wp->rtext);

	fprintf(keyfile, "te=%d be=%d le=%d re=%d ",
	    wp->tedit, wp->bedit, wp->ledit, wp->redit);

	/* wksp values */
	fprintf(keyfile, "clin=%d ccol=%d wlin=%d wcol=%d\n",
	    (int) wp->wksp->clin, (int) wp->wksp->ccol,
	    (int) wp->wksp->wlin, (int) wp->wksp->wcol);
    }

    flushkeys();
//  dbgpr("writing keyfile: CCRESIZE h=%d w=%d nwin=%d\n",
//     term.tt_height, term.tt_width, nwinlist);

    if ((term.tt_height > resized_max_h) || (term.tt_width > resized_max_w))
	updateKeyfile(term.tt_height, (int)term.tt_width);

    return;
}


void
replayResize(FILE *fp)
{

    int w, h;
    int nwin, wnum;
    int tm, bm, lm, rm;          /* margins */
    int tt, bt, lt, rt;          /* text positions */
    int te, be, le, re;          /* edit values */
    int clin, ccol, wlin, wcol;  /* wksp vars*/

    int __attribute__((unused)) rc, i;

    /* 1st read win info into local buf[]; may need to
     * 'adjust' an item
     */
    char buf[256], *s;
    fgets(buf, (int) sizeof(buf), fp);
    s = buf;

    //dbgpr("%o %o %c%c%c%c\n", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);

    rc = sscanf(s, " h=%d w=%d nwin=%d\n", &h, &w, &nwin);
//  dbgpr("----\nbuf=%s", s);
//  dbgpr("rc=%d h=%d w=%d nwin=%d nwinlist=%d\n", rc, h, w, nwin, nwinlist);

    /* update window values */

    S_window *wp;

    for (i=0; i<nwin; i++) {
	fgets(buf, (int) sizeof(buf), fp);
	if(feof(fp)) break;

	s = buf;
//      dbgpr("buf=%s", s);

	rc = sscanf(s, "win %d", &wnum);
	s += wnum > 9 ? 6 : 5;  /* skip "win n[n]" */

	wp = winlist[wnum];

	if (wp == NULL) {   /* should NOT see this */
	    dbgpr("wp is NULL wnum=%d nwinlist=%d\n", wnum, nwinlist);
	    continue;
	}

	rc = sscanf(s, " tm=%d bm=%d lm=%d rm=%d tt=%d bt=%d lt=%d rt=%d \
		 te=%d be=%d le=%d re=%d \
		 clin=%d ccol=%d wlin=%d wcol=%d\n",
	     &tm, &bm, &lm, &rm, &tt, &bt, &lt, &rt, &te, &be, &le, &re,
	     &clin, &ccol, &wlin, &wcol);

	wp->tmarg = (ASlines) tm;
	wp->bmarg = (ASlines) bm;
	wp->lmarg = (AScols) lm;
	wp->rmarg = (AScols) rm;

	wp->ttext = (ASlines) tt;
	wp->btext = (ASlines) bt;
	wp->ltext = (AScols) lt;
	wp->rtext = (AScols) rt;

	wp->tedit = (ASlines) te;
	wp->bedit = (ASlines) be;
	wp->ledit = (AScols) le;
	wp->redit = (AScols) re;

	wp->wksp->clin = (ASlines) clin;
	wp->wksp->ccol = (AScols) ccol;
	wp->wksp->wlin = (ANlines) wlin;
	wp->wksp->wcol = (ANcols) wcol;

    /** /
	dbgpr("rc=%d win %d tm=%d bm=%d lm=%d tm=%d ", rc, wnum, tm, bm, lm, rm);
	dbgpr("tt=%d bt=%d lt=%d rt=%d ", tt, bt, lt, rt);
	dbgpr("te=%d be=%d le=%d re=%d ", te, be, le, re);
	dbgpr("clin=%d ccol=%d wlin=%d wcol=%d\n", clin, ccol, wlin, wcol);
    / **/
    }

    setResizeWindows(h, w);

    return;
}


/*
 *    Called when a replay sees a CCRESIZE ecmd.
 *    Resize the main and utility windows if changed, and redraw.
 *    The edit windows already contain new values.
 */
void
setResizeWindows (int h, int w)
{
    int h_chg = (h - term.tt_height);
    int w_chg = (w - term.tt_width);
    int i;

    S_window *oldwin = curwin;
//  S_wksp *wksp_old = curwksp;

    int ccol = cursorcol;
    int clin = cursorline;


//  dbgpr("setResizeWindow: h=%d w=%d LINES=%d COLS=%d, h_chg=%d w_chg=%d term(h=%d,w=%d)\n",
//      h, w, LINES, COLS, h_chg, w_chg, term.tt_height, term.tt_width);
//  dbgpr("ccol=%d clin=%d\n");


//  debugAllWindows();

    if (h_chg || w_chg ) {
	blanks = realloc (blanks, (size_t)w);
	fill (blanks, (Uint) w, ' ');

	/* clear utility windows */
	ClearUtilityWindows();

	term.tt_width = (short) w;
	term.tt_height = (char) h;  /* todo, chg to short in e.tt.h/tcap.c */

	/* curses needs to adjust new sizes, reset LINES and COLS */
	resizeterm(h, w);
	refresh();
	/* my guess is that ncurses isn't seeing the SIGWINCH signal
	 * and is not updating these
	 */
	if (COLS != w) COLS = w;
	if (LINES != h) LINES = h;

	didReplayResize = YES;
    }
//dbgpr("After resizeterm: COLS=%d, LINES=%d\n", COLS, LINES);


    extern Short screensize;
    extern Uchar *image;
    if (screensize) sfree (image);
    image = (Uchar *)NULL;

    d_init(YES,NO);     /* YES for clearmem, NO clearscr */

    if (h_chg) {
	ASlines bmarg;
	//bmarg = (ASlines) (wholescreen.bmarg + h_chg);
	bmarg = (ASlines) (h - 1);

	wholescreen.bmarg = bmarg;
	wholescreen.btext = bmarg;
	wholescreen.bedit = bmarg;

	//enterwin.tmarg = (ASlines) (enterwin.tmarg + h_chg);
	enterwin.tmarg = (ASlines) (h - 1 - NINFOLINES - nButtonLines);
	enterwin.bmarg = enterwin.tmarg;
	enterwin.ttext = enterwin.tmarg;

	//infowin.tmarg = (ASlines) (infowin.tmarg + h_chg);
	infowin.tmarg = (ASlines) (h - NINFOLINES - nButtonLines);
	infowin.bmarg = infowin.tmarg;
	infowin.ttext = infowin.tmarg;

	if (nButtonLines) {
	    //buttonwin.tmarg = (ASlines) (buttonwin.tmarg + h_chg);
	    buttonwin.tmarg = (ASlines) (h - nButtonLines);
	    buttonwin.ttext = buttonwin.tmarg;
	    //buttonwin.bmarg = (ASlines) (buttonwin.bmarg + h_chg);
	    buttonwin.bmarg = (ASlines) (h - 1);
	    buttonwin.btext = buttonwin.bmarg; /* not used */
	}
    }

    if (w_chg) {
	AScols rmarg;

	//rmarg = (AScols) (wholescreen.rmarg + w_chg);
	rmarg = (AScols) (w - 1);

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

    /* Move the cursor to its original position; or if its
     * no longer on the screen, move it to just within
     * the current window's borders.
     */
    if (ccol > curwin->rtext) ccol = curwin->rtext;
    if (clin > curwin->btext) clin = curwin->btext;

    poscursor(ccol, clin);
    d_put(0);

    /* as per limitcursor() in e.t.c */
    for (i=0; i<nwinlist; i++) {
	winlist[i]->wksp->ccol = min (winlist[i]->wksp->ccol, winlist[i]->rtext);
	winlist[i]->wksp->clin = min (winlist[i]->wksp->clin, winlist[i]->btext);
	winlist[i]->altwksp->ccol = min (winlist[i]->altwksp->ccol, winlist[i]->rtext);
	winlist[i]->altwksp->clin = min (winlist[i]->altwksp->clin, winlist[i]->btext);
    }

    return;
}

void
updateKeyfile(int h, int w)
{
    long fpos;
    char buf[256], *s;

    if ((h <= resized_max_h) && (w <= resized_max_w))
	return;

    resized_max_h = max (h, resized_max_h);
    resized_max_w = max (w, resized_max_w);

    // save current file location
    fpos = ftell(keyfile);
    fseek(keyfile, 0, 0);   // beg of keyfile

    if (fgets(buf, sizeof(buf), keyfile) == NULL) {
	dbgpr("can't update keyfile with new values (%d,%d)\n", h, w);
	fseek(keyfile, fpos, 0);
	return;
    }

    /*   1st line example:
     *   "version=20 ichar=  term=xterm-256color h=43 w=95 ....."
     *
     *   note:  the extra "."'s provide space for larger width values.
     */

    if ((s = strstr(buf, "h=")) != NULL) {
	sprintf(s, "h=%d w=%d", resized_max_h, resized_max_w);
	// remove the \0 introduced by sprintf()
	s = index(s, '\0');
	*s = ' ';
	fseek(keyfile, 0, 0); // back to beg to rewrite 1st line
	fputs(buf, keyfile);
//      dbgpr("keystroke file update:  h,w reszized to %d,%d\n", resized_max_h, resized_max_w);
    }

    fseek(keyfile, fpos, 0); /* restore file position */

    return;
}
