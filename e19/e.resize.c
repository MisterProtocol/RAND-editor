/*
 * handle resize windows events
 */

#include "e.h"
#include "e.tt.h"
#include "e.wi.h"
#include "e.m.h"
#include "signal.h"

extern S_term term;
Flag resized;

void addResizeKeyfile(void);
void ResizeWindows (int h, int w);
void debugWindow (S_window *, char *);
void debugAllWindows (void);
Cmdret DoMacro(char *, char *);
int  CheckWindowValues(void);

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
extern Scols inf_file;
extern Scols inf_line;

int COLS, LINES;  /* curses uses these names */
Flag didReplayResize = NO;
extern int startup_h, startup_w;
extern int resized_max_h, resized_max_w;
void setResizeWindows (int h, int w);
void replayResize(int fd);
void updateKeyfile(int h, int w);


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

    dbgpr("ResizeWindow: h=%d w=%d LINES=%d COLS=%d, h_chg=%d w_chg=%d\n",
	h, w, LINES, COLS, h_chg, w_chg);

    debugAllWindows();

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

	/* now adjust vertical size of edit windows */
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

	/* now adjust horizontal size of edit windows */
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

/**/
dbgpr("after\n");
    debugAllWindows();
/**/
    /* ??? todo, make sure curwin is winlist[0] */

    resized = YES;

    infoinit();

    Small chgborders_save = chgborders;
    chgborders = 0;  /* ? */
    for (i=0; i<nwinlist; i++) {
	switchwindow(winlist[i]);
	limitcursor();  /* see e.t.c */
	poscursor(0, 0);
	putupwin();
	if (curwin == winlist[i]) {
	//  chgborders = 1;
	    drawborders (winlist[i], WIN_ACTIVE | WIN_DRAWSIDES);
	//  chgborders = 0;
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

//#if 0 //#ifdef OUT
    /* seems we need to redraw borders again */
    for (i=0; i<nwinlist; i++) {
	if (curwin != winlist[i]) {
	    drawborders (winlist[i], WIN_INACTIVE | WIN_DRAWSIDES);
	}
	fresh();
	d_put(0);
    }
    drawborders (curwin, WIN_ACTIVE|WIN_DRAWSIDES);
//#endif

    switchwindow(curwin);

    /* Move the cursor to its original position; or if its
     * no longer on the screen, move it to just within
     * the current window's borders.
     */
    if (ccol > curwin->rtext) ccol = curwin->rtext;
    if (clin > curwin->btext) clin = curwin->btext;
    poscursor(ccol, clin);
    //fresh();
    d_put(0);

    /* The REDRAW macro is auto-defined by AddDefaultMacros() */
    DoMacro("$REDRAW", "2");
    resized = YES;


#ifdef OUT
    /* until a better redraw method ... */
    (*term.tt_addr) (term.tt_height-2, 1);
    fprintf(stdout, "  *** Hit <RETURN> to continue.");
    poscursor(ccol, clin);
    fflush(stdout);
#endif

  //dbgpr("end of ResizeWindows()\n");

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

    dbgpr("\n");

    dbgpr("%12s %s\n", "win values", CheckWindowValues() ? "errors" : "ok");

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
replayResize(int fd)
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
    int n;
    char c;

    n = 0;
    while((read(fd, &c, 1)) == 1) {
    //  dbgpr("adding (%o)(%c) to buf[%d]\n", c, c, n);
	buf[n++] = c;
	if ((c == '\n') || (n > 100)) {     /* n should never be this large */
	    buf[n] = '\0';
	    break;
	}
    }

//  dbgpr("%o %o %c%c%c%c\n", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5]);

    s = buf;
    rc = sscanf(s, " h=%d w=%d nwin=%d\n", &h, &w, &nwin);

    if (rc != 3) {
	dbgpr("rc(%d) != 3 in sscan of buf=(%s)\n", rc, buf);
	mesg(ERRALL + 1, "Format error in replay file.");
	return;
    }

//  dbgpr("----\nbuf=%s", s);
//  dbgpr("rc=%d h=%d w=%d nwin=%d nwinlist=%d\n", rc, h, w, nwin, nwinlist);

    if (nwin != nwinlist)
	dbgpr("**** nwin=%d nwinlist=%d\n", nwin, nwinlist);

//return;

    /* update window values */

    S_window *wp;

    for (i=0; i<nwin; i++) {
	n = 0;
	while((read(fd, &c, 1)) == 1) {
	    buf[n++] = c;
	    if ((c == '\n') || (n > 130)) {     /* n should never be this large */
		buf[n] = '\0';
		break;
	    }
	}

	s = buf;
	//dbgpr("buf=%s\n", s);

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

	if (rc != 16) {
	    dbgpr("rc(%d) != 16 in sscan of buf=(%s)\n", rc, s);
	    mesg(ERRALL + 1, "Format error in replay file.");
	    return;
	}

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
  //S_wksp *wksp_old = curwksp;

    int ccol = cursorcol;
    int clin = cursorline;


//  dbgpr("setResizeWindow: h=%d w=%d LINES=%d COLS=%d, h_chg=%d w_chg=%d term(h=%d,w=%d)\n",
//      h, w, LINES, COLS, h_chg, w_chg, term.tt_height, term.tt_width);
//  dbgpr("ccol=%d clin=%d\n");


//  debugAllWindows();

    if (h_chg || w_chg ) {
	blanks = realloc (blanks, (size_t)w);
	fill (blanks, (Uint) w, ' ');

	term.tt_width = (short) w;
	term.tt_height = (char) h;  /* todo, chg to short in e.tt.h/tcap.c */

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
	enterwin.tmarg = (ASlines) (h - 1 - NINFOLINES);
	enterwin.bmarg = enterwin.tmarg;
	enterwin.ttext = enterwin.tmarg;

	//infowin.tmarg = (ASlines) (infowin.tmarg + h_chg);
	infowin.tmarg = (ASlines) (h - NINFOLINES);
	infowin.bmarg = infowin.tmarg;
	infowin.ttext = infowin.tmarg;

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
    }

/** /
dbgpr("after\n");
    debugAllWindows();
/ **/

    infoinit(); /* redraw the info line */

    Small chgborders_save = chgborders;
    chgborders = 0;  /* ? */
    for (i=0; i<nwinlist; i++) {
     // dbgpr("win[%d] = %o\n", i, winlist[i]);
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
    fflush(stdout);

    /* as per limitcursor() in e.t.c */
    for (i=0; i<nwinlist; i++) {
	winlist[i]->wksp->ccol = min (winlist[i]->wksp->ccol, winlist[i]->rtext);
	winlist[i]->wksp->clin = min (winlist[i]->wksp->clin, winlist[i]->btext);
	winlist[i]->altwksp->ccol = min (winlist[i]->altwksp->ccol, winlist[i]->rtext);
	winlist[i]->altwksp->clin = min (winlist[i]->altwksp->clin, winlist[i]->btext);
    }

    resized = YES;

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

