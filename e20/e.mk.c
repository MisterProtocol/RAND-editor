#ifdef COMMENT
--------
file e.mk.c
    cursor marking
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.tt.h"
#include "e.inf.h"
#include "e.m.h"
#include <stddef.h>

extern Uchar *image;

/* highlight modes, see cmd: set highlight */
extern char *smso, *rmso, *bold_str, *setab, *setaf, *sgr0, *setab_p, *setaf_p, *hilite_str;
extern char *brace_p;   /* hilight mode for brace matching */
extern char *fgbg_pair;
extern char *font_red;

void highlightarea(Flag, Flag);
void HiLightLine(int, Flag);
void HiLightRect(int, int, int, int);

Flag highlight_mode = YES;
Flag redrawflg; /* set after CMD: redraw */

int Pch(int ch) { putchar(ch); return (0); }



#ifdef COMMENT
void
mark ()
.
    Mark this spot.
#endif
void
mark ()
{
    if (prevmark == 0)
	info (inf_mark, 4, "MARK");
    if (curmark)
	loopflags.flash = YES;
    markprev ();
    exchmark (YES);
    return;
}

#ifdef COMMENT
void
unmark ()
.
    Remove marks, if any.
#endif
void
unmark ()
{
    if (curmark) {
	highlightarea(NO, NO);  /* 4/23/2021 */
	info (inf_mark, 4, "");
	info (inf_area, infoarealen, "");
    }
    curmark = prevmark = 0;
    infoarealen = 0;
    marklines = 0;
    markcols = 0;
    mklinstr[0] = 0;
    mkcolstr[0] = 0;

    return;
}

#ifdef COMMENT
Nlines
topmark ()
.
    Return the top line of a marked area.
#endif
Nlines
topmark ()
{
    return min (curwksp->wlin + cursorline,
		curmark->mrkwinlin + curmark->mrklin);
}

#ifdef COMMENT
Ncols
leftmark ()
.
    Return the leftmost column of a marked area.
#endif
Ncols
leftmark ()
{
    return min (curwksp->wcol + cursorcol,
		curmark->mrkwincol + curmark->mrkcol);
}

#ifdef COMMENT
Flag
gtumark (leftflg)
    Flag leftflg;
.
    Go to upper mark.
    if leftflg is YES, go to upper left corner
    else if lines must be exchanged, then exchange columns also
    else do not exchange columns
#endif
Flag
gtumark (leftflg)
Flag leftflg;
{
    register Short tmp;
    Flag exchlines = NO;
    Flag exchcols  = NO;

    /* curmark is the OTHER corner */
    markprev ();

    if (    curmark->mrkwinlin + curmark ->mrklin
	 < prevmark->mrkwinlin + prevmark->mrklin
       )
	exchlines = YES;
    if (   (   leftflg
	    &&    curmark->mrkwincol + curmark ->mrkcol
	       < prevmark->mrkwincol + prevmark->mrkcol
	   )
	|| (!leftflg && exchlines)
       )
	exchcols = YES;
    if (exchlines || exchcols) {
	if (!exchcols) {
	    /* exchange the cols so mark will set them back */
	    tmp = (int)curmark->mrkwincol;
	    curmark->mrkwincol = prevmark->mrkwincol;
	    prevmark->mrkwincol = tmp;
	    tmp = curmark->mrkcol;
	    curmark->mrkcol = prevmark->mrkcol;
	    prevmark->mrkcol = tmp;
	}
	else if (!exchlines) {
	    /* exchange the lines so mark will set them back */
	    tmp = (int)curmark->mrkwinlin;
	    curmark->mrkwinlin = prevmark->mrkwinlin;
	    prevmark->mrkwinlin = tmp;
	    tmp = curmark->mrklin;
	    curmark->mrklin = prevmark->mrklin;
	    prevmark->mrklin = (ASlines)tmp;
	}
	return exchmark (NO) & WINMOVED;
    }
    return 0;
}

#ifdef COMMENT
Small
exchmark (puflg)
    Flag puflg;
.
    Exchange the two marked positions.
#endif
Small
exchmark (puflg)
Flag puflg;
{
    register struct markenv *tmp;
    Short retval = 0;

    if (curmark)
	retval = movewin (curmark->mrkwinlin,
			   curmark->mrkwincol,
			    curmark->mrklin,
			     curmark->mrkcol, puflg);
    tmp = curmark;
    curmark = prevmark;
    prevmark = tmp;
    return retval;
}

#ifdef COMMENT
void
markprev ()
.
    Copy the current position into the prevmark structure.
#endif
void
markprev ()
{
    static struct markenv mk1, mk2;

    if (prevmark == 0)
	prevmark = curmark == &mk1 ? &mk2 : &mk1;
    prevmark->mrkwincol = curwksp->wcol;
    prevmark->mrkwinlin = curwksp->wlin;
    prevmark->mrkcol = cursorcol;
    prevmark->mrklin = (ASlines)cursorline;
    return;
}



/*
 *    Redraw the marked area of the screen in a
 *    highlighted screen mode; setmode = YES/NO
 */

void
highlightarea(Flag setmode, Flag redrawflg)
{

    if (highlight_mode == NO)  /* CMD: set [no]hightlight */
	return;

    int i;
    int top_line, bot_line;

    static Flag firsttime = YES;
    static int last_top = -1;
    static int last_bot = -1;
    static int last_col = -1;
    static int last_marklines = -1;
    static int last_cursorline = -1;

    if (setmode == NO) {
/** /
dbgpr("clearing mark from last_top=%d to last_bot=%d\n",
 last_top, last_bot);
/ **/
	for (i = last_top; i <= last_bot && i > 0; i++) {
	    HiLightLine(i, NO);
	}
	last_top = last_bot = last_marklines = last_cursorline = last_col = -1;
	poscursor(cursorcol, cursorline);
	d_put(0);
	return;
    }

/** /
dbgpr("\nhighlightarea: nwinlist=%d, redrawflg=%d, mode=%d\n", nwinlist, redrawflg, setmode);
dbgpr("curwin->tmarg=%d, curwin->bmarg=%d, curwin->lmarg=%d, curwin->rmarg=%d\n",
curwin->tmarg, curwin->bmarg, curwin->lmarg, curwin->rmarg);

dbgpr("curwin->ttext=%d, curwin->btext=%d, curwin->ltext=%d, curwin->rtext=%d\n",
curwin->ttext, curwin->btext, curwin->ltext, curwin->rtext);

dbgpr(" curmark->mrkwinlin=(%d), mrkwincol=(%d), mrklin=(%d), mrkcol=(%d) curwksp->wlin=(%d), curwksp->wcol=(%d)\n",
    curmark->mrkwinlin, curmark->mrkwincol, curmark->mrklin, curmark->mrkcol, curwksp->wlin, curwksp->wcol);
if (prevmark) {
 dbgpr(" prevmark->mrkwinlin=(%d), prevmark->mrkwincol=(%d), prevmark->mrklin=(%d), prevmark->mrkcol=(%d)\n\n",
    prevmark->mrkwinlin, prevmark->mrkwincol, prevmark->mrklin, prevmark->mrkcol);
}
/ **/

/** /
dbgpr("topmark()=%d marklines=%d markcols=%d cursorline=%d cursorcol=%d curwin->btext=%d\n",
topmark(), marklines, markcols, cursorline, cursorcol, curwin->btext);
dbgpr(" curwksp->wlin=(%d), curmark->mrkwinlin=(%d), mrklin=(%d)\n",
    curwksp->wlin, curmark->mrkwinlin, curmark->mrklin);
/ **/

/** /
dbgpr("highlightarea:  curmark:  mrkwincol=%d, mrkwinlin=%d, mrkcol=%d, mrklin=%d wcol=%d\n\n",
    curmark->mrkwincol, curmark->mrkwinlin, curmark->mrkcol, curmark->mrklin, curwksp->wcol);
/ **/

    if (firsttime) {  /* hilite just one line */
	top_line = cursorline + curwin->ttext;  /* omit top border */
	bot_line = top_line;
	last_top = top_line;
	last_bot = bot_line;;
	last_marklines = (int)marklines;
	last_cursorline = cursorline;
	last_col = cursorcol;
	firsttime = 0;
	if (markcols == 0) {    /* added for dragging */
	    HiLightLine(top_line, YES);
/*          dbgpr("firsttime, returning after making top_line\n"); */
	    return;
	}
	else {
/*          dbgpr("firsttime, continue to mark rectangle\n"); */
	}
    }

    /*   no action needed when cursor motion is suppressed, eg,
     *   moving up at line 1 of a file, or left at col 1
     */
    if (markcols == 0) {  /* added for draging */
	if (marklines == last_marklines && cursorline == last_cursorline
		&& cursorcol == last_col && redrawflg == NO && loopflags.hold == NO) {
      /** / dbgpr("***** no highlighting needed...\n"); / **/
	    return;
	}
    }

/** /
dbgpr("--before redraw=%d last_top=%d, last_bot=%d, last_col=%d, last_marklines=%d firsttime=%d infoline=%d\n",
  redrawflg, last_top, last_bot, last_col, last_marklines, firsttime, infoline);
/ **/
    if (redrawflg) {    /* eg, CMD: redraw */
	if (cursorcol == curmark->mrkcol) {
	    for (i = last_top; i <= last_bot; i++)
	      HiLightLine(i, YES);
	}
	else {
	    HiLightRect(last_top, last_bot, curmark->mrkcol, cursorcol);
	}
	return;
    }


    /*  clear previous mark, no need to clear entire window */
/*  for (i = curwin->ttext; i < curwin->bmarg; i++) */
/** /
dbgpr("-- clear last mark:  last_top=%d last_bot=%d ttext=%d bmarg-1=%d\n",
last_top, last_bot, curwin->ttext, curwin->bmarg-1);
/ **/
    for (i = last_top; i <= last_bot; i++) {
       HiLightLine(i, NO);
    }

    /*  If scrolling up from the top line in a window when the
     *  mark began before the top line, set a flag to clear the
     *  extra line.
     */
    int topmark_on_previous = 0;

    /*  Determine the range to highlight, top_line -> bot_line.
     *  There are 3 cases:
     *    1.  marklines == 1, => current cursor line
     *    2.  mark started on a screen before current screen
     *    3.  mark started on the current screen
     */

    if (marklines == 1) {
	top_line = bot_line = cursorline + curwin->ttext;
    //  dbgpr("case 1:  marklines == 1, top_line=%d, bot_line=%d\n", top_line, bot_line);
    }
    else if (topmark() < curwksp->wlin) { /* mark started on a prev screen */
	top_line = curwin->ttext;
	bot_line = top_line + cursorline; /* cursor is at bottom of the mark */
    //  dbgpr("case 2, previous: topmark()=%d < curwksp->wlin=%d cursorline=%d ttext=%d\n",
    //      topmark(), curwksp->wlin, cursorline, curwin->ttext);
	topmark_on_previous++;
    }
    else {  /* topmark() started on current screen */
	top_line = (int) (topmark() - curwksp->wlin);
	top_line += curwin->ttext;
	bot_line = (int) (top_line + marklines - 1);
	if (bot_line >= curwin->bmarg) {
	    dbgpr("bot_line(%d) >= bmarg(%d)\n", bot_line, curwin->bmarg);
	    bot_line = curwin->bmarg - 1;
	}
	//dbgpr("case 3, curwin:  topmark()=%d >= curwksp->wlin=%d\n", topmark(), curwksp->wlin);
    }
//  dbgpr("top_line=%d bot_line=%d cursorline=%d marklines=%d ttext=%d bmarg=%d\n",
//      top_line, bot_line, cursorline, marklines, curwin->ttext, curwin->bmarg);

/** /
dbgpr("top_line=%d last_top=%d, bot_line=%d, last_bot=%d, last_col=%d, firsttime=%d infoline=%d\n",
  top_line, last_top, bot_line, last_bot, last_col, firsttime, infoline);
dbgpr("marklines=%d last_marklines=%d cursorline=%d, last_cursorline=%d\n\n",
    marklines, last_marklines, cursorline, last_cursorline);
/ **/

    /*  10/2022:  Old Message:
     *  The above code sometimes generates values for top_line and bot_line
     *  that are outside the page boundaries (seen after many +pages,
     *  jumps to EOF and paging back).  The variables curwksp->wlin and
     *  curmark->mrkwinlin were used when the the cursor moves up/down
     *  over a page boundary, but they can be much larger than the page
     *  size when moving several pages.  Todo, rethink this...
     */

    if (bot_line >= curwin->bmarg) {
/**/ dbgpr("***bot_line out of range (%d), bmarg=%d\n",
	bot_line, curwin->bmarg);
/**/
	bot_line = curwin->bmarg - 1;
    }

    if (top_line <= 0) {
/**/dbgpr("*** top_line out of range (%d)\n", top_line); /**/
	top_line = curwin->ttext;
    }

    savecurs();

/** /
dbgpr("top_line=%d, bot_line=%d, last_top=%d, last_bot=%d\n",
top_line, bot_line, last_top, last_bot);
/ **/

    if (markcols == 0) { /* entire lines */
	for (i = top_line; i <= bot_line; i++) {
	   HiLightLine(i, setmode);
	}
    }
    else {  /* rectangles */
	HiLightRect(top_line, bot_line, curmark->mrkcol, cursorcol);
    }

    restcurs();

    last_top = top_line;
    last_bot = bot_line;;
    if (cursorline == 0 && topmark_on_previous)
	last_bot++; /* need to clear one more line if user scrolls up */
    last_marklines = (int)marklines;
    last_cursorline = cursorline;
    last_col = cursorcol;

/** /
dbgpr("setting last_top=top_line = %d, last_bot=bot_line = %d\n----\n",
top_line, bot_line );
/ **/

    if (setmode == YES )
	firsttime = NO;    /* so next time we will check if the marked area has shrunk */
    else
	firsttime = YES;   /* doing an unmark, so nexttime we don't check if area has shrunk */

    return;
}



/*
 *  Entire line is highlighted
 */

void
HiLightLine(int line, Flag setmode)
{

    int w = term.tt_width;
    char buf[w+1];

    if (line < 0)
	return;


    char *cp = (char *)image + w*line;

    /*  use curwin->lmarg and curwin->rtext to account for any windows,
     *  omit left/rt border chars
     */
    int wid = curwin->rmarg - curwin->lmarg;

/** /
if (setmode == YES)
dbgpr("HiLightLine: setmode=%d line=%d wid=%d lmarg=%d ltext=%d, rmarg=%d, rtext=%d mrkwincol=%d mrkcol=%d wksp->wcol=%d wksp->wlin=%d\n",
setmode, line, wid, curwin->lmarg, curwin->ltext, curwin->rmarg, curwin->rtext,
curmark->mrkwincol, curmark->mrkcol, curwksp->wcol, curwksp->wlin);
/ **/

    snprintf(buf, (size_t)wid, "%s", cp+curwin->ltext);
    mvcur(-1,-1, line, curwin->ltext);


#ifdef OLD
    snprintf(buf, w-1, "%s", cp+1); /* omit left/rt border chars */
    mvcur(-1,-1, line, 1);
#endif

    if (setmode == YES) {
	/* works ok, see e.mouse.c */
	tputs(hilite_str, 1, Pch);      /* user selected mode, see cmd: set highlight */
	puts(buf);
	tputs(sgr0, 1, Pch);
    }
    else {  /* clears any previous mode on this line */
	puts(buf);
    }
    fflush(stdout);

/** / dbgpr("buf=(%s)\n", buf);  / **/


/** /
if (setmode == YES)
  dbgpr("HiLightLine: line=%d, setmode=(%d), buf=\n(%s)\n", line, setmode, buf);
/ **/
}

/*
 *   visually mark a portion of a line
 */
void
HiLightRect(int from, int to, int mrkcol, int curcol)
{

    int w = term.tt_width;
    char buf[w+1];
    char *cp;

    int beg_mark;
    int mark_len = (int)markcols;  /* how many chars to highlight */
    int line;
    int adj_markcol = mrkcol;

    /*
     * adj_markcol, initially set to curmark->mrkcol, needs to be
     * adjusted to account for various window shifts.  If the marked area
     * is increasing to the right, the marked area ends one char before
     * the cursor position; if increasing to the left, the marked area
     * starts at the cursor position.
     */

    /* First, the case if the cursor position is 0 when the window is shifted, the mark
     * started somewhere to the left of the visible window  nd nothing
     * needs to be highlighted.
     */
    if (curcol == 0 && markcols <= curwksp->wcol) {
    /** /dbgpr("0: marked area is left of our window, wcol=%d curcol=%d markcols=%d\n",
	    curwksp->wcol, curcol, markcols); / **/
	return;
    }

	/* window not shifted right, or mark began in unshifted window */
    if (curwksp->wcol == curmark->mrkwincol) {  /* both are 0 or equal */
	adj_markcol = mrkcol;
	/** /dbgpr("1: wcol=mrkwincol, adj_markcol=%d\n", adj_markcol); / **/
    }
    else if (mrkcol < curwksp->wcol) {  /* mark began left of our window */
	adj_markcol = 0;
	mark_len = (int)(markcols - curwksp->wcol + mrkcol);
	/** /dbgpr("1a: mrkcol < wksp->wcol, adj_markcol=%d, mrkcol=%d new mark_len=%d\n",
	   adj_markcol, mrkcol, mark_len); / **/
    }
    else if (curmark->mrkwincol == 0 && mrkcol >= curwksp->wcol) {
	adj_markcol = (int)(mrkcol - curwksp->wcol);
	/** /dbgpr("2: mrkwincol=0, adj_markcol=%d\n", adj_markcol); / **/
    }
    else if (curmark->mrkwincol > curwksp->wcol) {
	adj_markcol = (int)(mrkcol + (curmark->mrkwincol - curwksp->wcol));
	/** /dbgpr("3: mrkwincol > wksp->wcol, adj_markcol=%d\n", adj_markcol); / **/
    }
    else if (curwksp->wcol > curmark->mrkwincol) {
	adj_markcol = (int)(mrkcol - (curwksp->wcol - curmark->mrkwincol));
	/** /dbgpr("4: wcol > mrkwincol, adj_markcol=%d\n", adj_markcol); / **/
    }
    else {
	/** /dbgpr("5: no condition matched\n"); / **/
    }

    if (curcol > adj_markcol) {
	beg_mark = adj_markcol;
    }
    else {
	beg_mark = curcol;
    }

    /*  use curwin->lmarg and curwin->rtext to account for any windows,
     *  omit left/rt border chars
     */
    int wid = curwin->rmarg - curwin->lmarg;
    beg_mark += curwin->ltext;


    /* marked area lies partly to right of our window */
    // if (beg_mark + mark_len > curwin->rtext) {
    if (beg_mark + mark_len > curwin->rmarg) {
	mark_len = curwin->rtext - beg_mark;
	if (mark_len <= 0 )
	    mark_len = 1;
    /** /dbgpr("6: mark_len reduced to %d from %d\n", mark_len, markcols); / **/
    }

/** /
dbgpr("HiLiRect: mrkwincol=%d wksp->wcol=%d mrkcol=%d curwin->ltext=%d\n",
  curmark->mrkwincol, curwksp->wcol, curmark->mrkcol, curwin->ltext);

dbgpr("end: beg_mark=%d markcols=%d curcol=%d, adj_markcol=%d from=%d to=%d\n\n",
beg_mark, markcols, curcol, adj_markcol, from, to);
/ **/

    for (line = from; line <= to; line++ ) {
	cp = (char *)image + w*line;

	/* clear any full line highlighting */
	mvcur(-1,-1, line, curwin->ltext);
	snprintf(buf, (size_t)wid, "%s", cp+curwin->ltext);
	puts(buf);

	/* get a copy of the marked area */
	snprintf(buf, (size_t)mark_len+1, "%s", cp + beg_mark );
	mvcur(-1,-1, line, beg_mark);

	tputs(hilite_str, 1, Pch);      /* user selected mode, see cmd: set highlight */
	puts(buf);
	tputs(sgr0, 1, Pch);  /* end hilite mode */

	/** / dbgpr("marked area=(%s)\n", buf); / **/

    }
    fflush(stdout);

    return;
}


