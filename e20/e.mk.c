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

extern Uchar *image;

/* highlight modes, see cmd: set highlight */
extern char *smso, *rmso, *bold_str, *setab, *setaf, *sgr0, *setab_p, *setaf_p, *hilite_str;
extern char *brace_p;   /* hilight mode for brace matching */
extern char *fgbg_pair;
extern char *font_red;

void highlightarea(Flag, Flag);
void HiLightLine(int, Flag);
void HiLightRect(int, int, int, int);

/*void HiLightBraces();*/
void HiLightBracePairs(Flag);
int brace_marked;
Flag highlight_mode = YES;
Flag redrawflg; /* set after CMD: redraw */
/*
int FindBraceBack();
int FindBraceForw();
*/
void GetLine();
extern int braceRange;  /* limit match to +/- braceRange lines */
int win_has_eof();

int Pch(int ch) { putchar(ch); return (0); }


/* brace matching/highlighting */
int bracematching;
extern Flag bracematchCoding;

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
dbgpr("topmark=%d marklines=%d markcols=%d cursorline=%d cursorcol=%d curwin->btext=%d\n",
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
/*dbgpr("--last_top=%d last_bot=%d\n", last_top, last_bot); */
    for (i = last_top; i <= last_bot; i++)
       HiLightLine(i, NO);

    /*   The cursor line is always either at the top or bottom
     *   of a marked area
     */

    if (marklines == 1) {
	top_line = bot_line = cursorline + curwin->ttext;
    }
	/* if cursor is at the bottom of a window */
    else if (cursorline == curwin->btext) {
	if (topmark() < curwksp->wlin) {
	    top_line = curwin->ttext;
	    /*bot_line = curwin->btext + 1;*/
	    bot_line = curwin->ttext + curwin->btext;
/** / dbgpr("1a: cursor=bottom, topmark() on prev page, top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
	}
	else {
	    top_line = (int)(curwin->ttext + topmark() - curwksp->wlin);
	    /*bot_line = top_line + marklines - 1;*/
	    /*bot_line = curwin->btext + 1; */
	    bot_line = curwin->ttext + curwin->btext;
	}
/** /  dbgpr("1: cursorline == btext, top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
    }
    else if (cursorline == 0) {    /* cursor at top of window */
	top_line = curwin->ttext;
	/* if cursor at the bottom of a marked area, hilight 1 line */
	if (topmark() + marklines == curwksp->wlin + 1 ) {
	    bot_line = top_line;
	}
	else {
	    bot_line = (int)(top_line + marklines - 1);
	}
	if (bot_line > curwin->ttext + curwin->btext) {
	    bot_line = curwin->ttext + curwin->btext;
	}
/** / dbgpr("2:  cursorline == 0, top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
    }
    else {  /* not top or bottom, and marklines > 1 */
	/* if topmark() is on previous page... */
	if (topmark() < curwksp->wlin) {
	    top_line = curwin->ttext;
	    bot_line = top_line + cursorline;
/** / dbgpr("3: topmark() on prev page, top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
	}
	else if (curmark->mrkwinlin == curwksp->wlin) {
	    if (cursorline > curmark->mrklin) {
		top_line = curwin->ttext + curmark->mrklin;
		bot_line = (int)(top_line + marklines - 1);
	    }
	    else {
		top_line = curwin->ttext + cursorline;
		bot_line = (int)(top_line + marklines - 1);
	    }
/** / dbgpr("4: mrkwinlin = wlin, top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
	}
	/* topmark() on current screen */
	else if (curwksp->wlin < topmark()) {
	    top_line = (int)(curwin->ttext + topmark() - curwksp->wlin);
	    bot_line = (int)(top_line + marklines - 1);
	    if (bot_line > curwin->ttext + curwin->btext)
		bot_line = curwin->ttext + curwin->btext;
/** / dbgpr("5: wlin < topmark(), top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
	}
	else {
/** / dbgpr("6: XXXXX shouldn't see this\n"); / **/
	}
    }

/** /
dbgpr("top_line=%d last_top=%d, bot_line=%d, last_bot=%d, last_col=%d, firsttime=%d infoline=%d\n",
  top_line, last_top, bot_line, last_bot, last_col, firsttime, infoline);
dbgpr("marklines=%d last_marklines=%d cursorline=%d, last_cursorline=%d\n\n",
    marklines, last_marklines, cursorline, last_cursorline);
/ **/

    /*
     *  The above code sometimes generates values for top_line and bot_line
     *  that are outside the page boundaries (seen after many +pages,
     *  jumps to EOF and paging back).  The variables curwksp->wlin and
     *  curmark->mrkwinlin were used when the the cursor moves up/down
     *  over a page boundary, but they can be much larger than the page
     *  size when moving several pages.  Todo, rethink this...
     */

    if (bot_line > curwin->ttext + curwin->btext) {
/** / dbgpr("***bot_line out of range (%d), btext=%d ttext=%d\n",
	bot_line, curwin->btext, curwin->btext);
/ **/
	bot_line = curwin->ttext + curwin->btext + 1;
    }

    if (top_line <= 0) {
/** /dbgpr("*** top_line out of range (%d)\n", top_line); / **/
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
    last_marklines = (int)marklines;
    last_cursorline = cursorline;
    last_col = cursorcol;

/** /
dbgpr("setting last_top=top_line = %d, last_bot=bot_line = %d\n",
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
if (1 || setmode == YES)
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
/*  if (beg_mark + mark_len > curwin->rtext) { */
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


/*
 *   visually mark matching {} pairs
 */
void
HiLightBracePairs(Flag set)      /* 0 when we're only clearing a mark */
{
    int w = term.tt_width;
    char buf[w+1];
    char *cp;

    int line;
    int from, to;
    int lcol, rcol;

    /* save these values to clear the last marked end brace */
    static int last_y = 0;
    static int last_x = 0;
    static int last_wlin = 0;
/*  static int last_ch = '\0'; */
    static char last_ch_type;      /* R=closing, L=opening */

    from = curwin->ttext;
    to = curwin->ttext + curwin->btext;
    lcol = curwin->ltext;
    rcol = curwin->rtext;

/** /
dbgpr("-----\n");
dbgpr("ttext=%d, btext=%d, lcol=%d, rcol=%d\n", from, to, lcol, rcol);

dbgpr("HiLiBrace: curwin->ttext=%d, curwin->btext=%d, curwin->ltext=%d, curwin->rtext=%d\n",
    curwin->ttext, curwin->btext, curwin->ltext, curwin->rtext);

dbgpr("curwin->tmarg=%d, curwin->bmarg=%d, curwin->lmarg=%d, curwin->rmarg=%d\n",
    curwin->tmarg, curwin->bmarg, curwin->lmarg, curwin->rmarg);

dbgpr("curwksp->wlin=%d, curwksp->wcol=%d cursorline=%d cursorcol=%d\n\n",
   curwksp->wlin, curwksp->wcol, cursorline, cursorcol);
/ **/

    /** /
    dbgpr("check unmark %c at (%d,%d) wlin=%d lwlin=%d\n",
	last_ch, last_y, last_x, curwksp->wlin, last_wlin);
    / **/

    int end_ch;

    int cursor_ch;    /* char pointed to by cursor */
    int cursor_screenline = cursorline + curwin->ttext;
    from = cursor_screenline;
    int cursor_screencol = cursorcol + lcol;

/** /
dbgpr("cursorline=%d cursor_screenline=%d cursorcol=%d cursor_screencol=%d\n",
   cursorline, cursor_screenline, cursorcol, cursor_screencol);
/ **/

    /* clear previous highlight ch, if still on current screen */
    if (last_y > 0) {
	if (last_wlin >= curwksp->wlin && last_wlin <= (curwksp->wlin + curwin->btext)) {
	    if (last_wlin < curwksp->wlin) {
		last_y -= (int)((curwksp->wlin - last_wlin));
	    }
	    else if (last_wlin > curwksp->wlin) {
		last_y += (int)((last_wlin - curwksp->wlin));
	    }

	    /* for some reason, redrawing only the rt matching brace
	     * left an earlier portion of the line messed up, so we'll
	     * try redrawing the line upto and including the rt brace
	     */

	    cp = (char *)image + w*last_y;
	/*  int wid = curwin->rtext - curwin->ltext;  */ /* not whole line */
	    int wid = last_x - curwin->ltext + 2;   /* +1 for marg char, +1 for rt brace */
	    mvcur(-1, -1, last_y, curwin->ltext);
	    snprintf(buf, (size_t)wid, "%s", cp+curwin->ltext);
	    tputs(buf,1,Pch);
	    /** /
	    dbgpr("clear, buf='%s'\n", buf);
	    / **/
	    /* restoring the cursor doesn't work well when
	     * one is scrolling using the arrow keys (or LT, RT keys).
	     * Searching for an opening brace, or tabbing works ok
	     * w/o this...
	     */

	    int col = cursorcol;
	    int lin = cursorline;
	    cursorcol = cursorline = -1;

	    poscursor(0,0); /* probably don't need this now... */
	    d_put(0);
	    poscursor(col,lin);
	    d_put(0);

	    mvcur(-1, -1, cursor_screenline, cursor_screencol);
	    last_y = last_x = 0;
	    /** /
	    dbgpr("unmark %c at (%d,%d), then moving cur back to (%d,%d)\n",
		    last_ch, last_y, last_x, cursor_screenline, cursor_screencol );
	    / **/
	    fflush(stdout);
	    brace_marked = 0;
	}
    }

    if (set == NO)   /* we're only clearing prev mark */
	return;

/** /
dbgpr("after unmark, cursorline,cursorcol=(%d,%d) cursor_screenline,cursor_screencol=(%d,%d)\n",
   cursorline, cursorcol, cursor_screenline, cursor_screencol );
/ **/

    /* if we're not pointing at an opening brace, eg '{', return */
    int i;
    cp = (char *)image + cursor_screenline*w;
    i = cursorcol + lcol;
    cursor_ch = *(cp+i);

/** /dbgpr("cursor_ch=%c\n", cursor_ch); / **/

    /* if cursor isn't pointing at an opening brace, return */
    switch (cursor_ch) {
	case '{':
	    end_ch = '}';
	    last_ch_type = 'R';
	    break;
	case '(':
	    end_ch = ')';
	    last_ch_type = 'R';
	    break;
	case '[':
	    end_ch = ']';
	    last_ch_type = 'R';
	    break;
	case '}':
	    end_ch = '{';
	    last_ch_type = 'L';
	    break;
	case ')':
	    end_ch = '(';
	    last_ch_type = 'L';
	    break;
	case ']':
	    end_ch = '[';
	    last_ch_type = 'L';
	    break;
	default:
	    return;
    }

#ifdef OUT
/* if we want to do this, need add vars to keep
 * track of position in order to clear it next time
 */

    /* Cursor is resting on opening brace.
     * Do we want to hilight it in some other way?
     */
    mvcur(-1, -1, cursor_screenline, cursor_screencol);
    if( curs_set(0) == ERR)
	dbgpr("curs_set rc=ERR\n");
    tputs(hilite_str, 1, Pch);
    /*tputs(smso, 1, Pch);*/
    fputc(cursor_ch, stdout);
    tputs(sgr0, 1, Pch);
    fflush(stdout);
#endif

    /*  Keep a tally of left braces found before an ending one.
     *  Have a match when we find an end brace and cnt returns to 0.
     */
    int cnt = 0;
    int found_match = 0;
    char msg[128];
    int ln_eof = (int) (la_lsize(curlas) - curwksp->wlin);

    int match_error = 0; /* set to show where the err is detected */

    if (last_ch_type == 'R') {  /* look forw for closing brace */
/** /
dbgpr("closing brace search, from=%d to=%d ln_eof=%d wlin=%ld, lcol=%d curs_col=%d\n",
from, to, ln_eof, curwksp->wlin, lcol, cursor_screencol);
/ **/
	for (line = from; line <= to; line++) {
	    if (line > ln_eof) {
		snprintf(msg, sizeof(msg),
		    " Matching %c not found thru end of file", end_ch);
		mesg(ERRALL+1, msg);
		return;     /* NOT FOUND */
	    }

       /** / dbgpr("checking line %d for }, cnt=%d\n", line, cnt); / **/
	    cp = (char *)image + w*line;
	    for (i=lcol; i<=rcol; i++) {
		if (*(cp+i) == cursor_ch) {
		    /* omit chars left of cursor or the one cursor is at */
		    if (line == cursor_screenline && i <= (cursor_screencol))
			continue;
		    cnt++;
		 /** / dbgpr("got %c, cnt=%d\n", cursor_ch, cnt);  / **/
		    continue;
		}

		if (*(cp+i) == end_ch) {

	      /** /  dbgpr("got %c at line=%d col=%d, cnt=%d\n", end_ch, line, i, cnt); / **/

		    /* on the cursor line, don't include matches left of the cursor */
		    if (line == cursor_screenline && i <= (cursor_screencol))
			continue;

		    /*  if bracematchCoding mode, and } appears in column 1
		     *  we only have a match if we started in column 1
		     *  and cnt is 0
		     */
		    if (i == lcol && end_ch == '}' && bracematchCoding) {
			    /* { and } both in column 1, ie have match  */
			if (cnt == 0 && cursor_screencol == lcol) {
			    found_match = 1;
			    break;
			}

			snprintf(msg, sizeof(msg),
"Matching %c not found in current function, ending at line %ld.",
end_ch, line + curwksp->wlin);
			mesg(ERRALL+1, msg);
		    /*  return; *//* NO MATCH */
			found_match = 1;
			match_error = 1;
			break;
		    }

		    if (cnt > 0) {
		 /** / dbgpr("got %c at col=%d, cnt=%d\n", end_ch, i, cnt);  / **/
		       cnt--;
		       continue;
		    }

		    /* cnt is 0 */

		    /* if bracematchCoding and cursor_screencol == lcol, no match */
		    if (cursor_screencol == lcol && bracematchCoding && i > lcol
			   && end_ch == '}' ) {
			snprintf(msg, sizeof(msg),
"The matching %c at line %ld, col %d is not at the end of a function.",
end_ch, line + curwksp->wlin, i);
			mesg(ERRALL+1, msg);
		     /*  return;*/   /* NO MATCH */
			found_match = 1;
			match_error = 1;
		    }

		    /** /
		    dbgpr("found %c at line %d col %d, cursor_screenline=%d, cursor_screencol=%d, wlin=%d\n\n",
			end_ch, line, i, cursor_screenline, cursor_screencol, curwksp->wlin);
		    / **/
		    found_match = 1;
		}
		if (found_match) break;
	    }
	    if (found_match) break;
	}
	    /* search forward, start at lines not in current window */
	if (!found_match) {
	    long l_num = -1;
	    int c_num = -1;

	    int rc = FindBraceForw(cursor_ch, end_ch, cnt, &l_num, &c_num);
/** /
dbgpr("rc=%d from FindBraceForw, l_num=%d c_num=%d\n", rc, l_num, c_num);
/ **/
	    /* found match looking forward */
	    if (rc == 1) {
		snprintf(msg, sizeof(msg), " Matching %c found at line %ld col %d.",
		    end_ch, l_num+1, c_num+1);
	  /** / dbgpr("%s\n", msg);    / **/
		mesg(ERRALL + 1, msg);
		/* todo?? hilite match */
		return;  /* MATCH FOUND */
	    }

	    snprintf(msg, sizeof(msg), " Matching %c not found ", end_ch);
	    int len = (int)strlen(msg);


	    if (rc == 2) {  /* bracematchCoding mode, not found */
		snprintf (msg+len, sizeof(msg), "in current function() \
ending at line %ld col %d.", l_num+1, c_num+1);
	    }
	    else if (rc == 3 || win_has_eof()) {
		strcat (msg, "searching to the end of the file.");
	    }
	    else {  /* eof not in win  */
		if (braceRange == 0) {
		    strcat (msg, "searching to the end of file.");
		}
		else {
		    sprintf (msg+len, "in the next %d lines.", braceRange);
		}
	    }

	    mesg(ERRALL + 1, msg);
	    return;     /* NOT FOUND */
	}
    }
    else {  /* look for opening brace */
	from = curwin->ttext + cursorline;
	to = curwin->ttext;
	/** /
	dbgpr("open brace search, from=%d to=%d\n", from, to);
	/ **/
/** /
dbgpr("opening brace search, from=%d to=%d ln_eof=%d wlin=%ld, lcol=%d curs_col=%d\n",
from, to, ln_eof, curwksp->wlin, lcol, cursor_screencol);
/ **/

	for (line = from; line >= to; line--) {
	    /** /
	    dbgpr("checking line (reverse) %d for %c, cnt=%d\n", line, end_ch, cnt);
	    / **/
	    cp = (char *)image + w*line;
	    for (i=rcol; i>=lcol; i--) {
		if (*(cp+i) == cursor_ch) {
		    /* don't include the chars right of cursor, or the one
		     * cursor is pointing at
		     */

		    if (line == cursor_screenline && i >= (cursor_screencol))
			continue;
		    cnt++;
		    /*dbgpr("got %c, cnt=%d\n", cursor_ch, cnt);*/
		    continue;
		}

		if (*(cp+i) == end_ch) {
		    /* on the cursor line, don't include matches right of the cursor */
		    if (line == cursor_screenline && i >= (cursor_screencol))
			continue;
/** /
dbgpr("have %c at i=%d line=%d cnt=%d\n", end_ch, i, line, cnt);
dbgpr("  lcol=%d cursor_screencol=%d\n", lcol, cursor_screencol);
/ **/

		    if (cnt > 0) {
		       /*dbgpr("got %c at col=%d, cnt=%d\n", end_ch, i, cnt);*/
		       cnt--;
		       continue;
		    }

		    /*  if bracematchCoding mode, and { appears in column 1
		     *  we only have a match if we started in column 1
		     *  and cnt is 0
		     */
		    if (end_ch == '{' && bracematchCoding) {
			    /* { and } both in column 1, ie have match  */

/** /
dbgpr("  end_ch is } and braceCoding is on, i=%d cnt=%d\n", i, cnt);
dbgpr("  lcol=%d cursor_screencol=%d\n", lcol, cursor_screencol);
/ **/

			if (i == lcol && cnt == 0 && cursor_screencol == lcol) {
			    dbgpr("x1 found match\n");
			    found_match = 1;
			    break;
			}

			/* also a match if i != lcol && cursor_screencol != lcol */
			if (i != lcol && cursor_screencol != lcol) {
			/*  dbgpr("x2 found match\n"); */
			    found_match = 1;
			    break;
			}


			snprintf(msg, sizeof(msg),
"Matching %c not found in current function, starting at line %ld.",
end_ch, line + curwksp->wlin);
			mesg(ERRALL+1, msg);
		    /*  return;*/ /* NO MATCH */
			found_match = 1;
			match_error = 1;
			break;
		    }

/** /
dbgpr("found %c at line %d col %d, cursor_screenline=%d, cursor_screencol=%d, wlin=%d\n\n",
    end_ch, line, i, cursor_screenline, cursor_screencol, curwksp->wlin);
/ **/
		    found_match = 1;
		}
		if (found_match) break;
	    }
	    if (found_match) break;
	}
	    /* search backward, start at 1st line not in current window */
	if (!found_match) {

	    if (curwksp->wlin > 0) {  /* line 1 of file not in current window */
		long l_num = -1;
		int c_num = -1;
		int rc = FindBraceBack(cursor_ch, end_ch, cnt, &l_num, &c_num);

		/** /
		dbgpr("rc=%d from FindBraceBack, l_num=%d c_num=%d\n", rc, l_num, c_num);
		/ **/

		if (rc == 1) {
		    snprintf(msg, sizeof(msg), " Matching %c found at line %ld col %d.",
			end_ch, l_num+1, c_num+1);
	       /*   dbgpr("%s\n", msg); */
		    mesg(ERRALL + 1, msg);
		    return; /* FOUND MATCH */
		}

		snprintf(msg, sizeof(msg), " Matching %c not found ", end_ch);
		int len = (int)strlen(msg);

		if (braceRange == 0) {
		    strcat(msg, "searching to beginning of the file");
		}
		else {
		    sprintf (msg+len, "in the preceeding %d lines", braceRange);
		}
	    }
	    else {  /* no match, line 1 of file at top of window */
		snprintf(msg, sizeof(msg), " Matching %c not found %s ", end_ch,
		    "searching to beginning of the file");
	    }
	    mesg(ERRALL + 1, msg);
	    return;  /* NOT FOUND */

	}
    }

    /* have a match on the screen */

#ifdef OUT
    if( !found_match ) {
	snprintf(msg, 128, " Matching %c not found ", end_ch);
	int len = strlen(msg);
	if (last_ch_type == 'R') {  /* search forw for closing */
	    if (braceRange == 0) {
		sprintf (msg+len, " searching to the end of the file");
	    }
	    else {
		sprintf (msg+len, " in the following %d lines", braceRange);
	    }
	}
	else {
	    if (braceRange == 0) {
		sprintf (msg+len, " searching to beginning of the file");
	    }
	    else {
		sprintf (msg+len, " in the preceeding %d lines", braceRange);
	    }
	}
/** /   dbgpr(" matching %c not found on this screen", end_ch);  / **/
	mesg(ERRALL + 1, msg);
	return;
    }
#endif /* OUT */

    int match_col = i;

    /* Highlight matching brace */
    mvcur(-1,-1, line, match_col);
/** /
    dbgpr("moving to line %d col %i to highlight %c\n", line, match_col, end_ch);
/ **/

    /* save these in order to clear the mark, next time */
    last_y = line;
    last_x = match_col;
    last_wlin = (int)curwksp->wlin;
/*  last_ch = end_ch; */

    if (match_error)
/*      tputs("\033[41m", 1, Pch); */
	tputs(font_red, 1, Pch);
    else
	tputs(brace_p, 1, Pch);

    fputc(end_ch, stdout);
    tputs(sgr0, 1, Pch);

    fflush(stdout);

    brace_marked = 1;


/*DebugVal=1;*/

    /*  redraw first part of line, something
     *  is getting messed up when closing brace is found
     */
    if (last_ch_type == 'R') {
	cp = (char *)image + w*line;
	int wid = last_x - curwin->ltext + 1;   /* redraw upto the ending match char */
	mvcur(-1, -1, last_y, curwin->ltext);
	snprintf(buf, (size_t)wid, "%s", cp+curwin->ltext);
	tputs(buf,1,Pch);
    /** /
	dbgpr("buf='%s'\n", buf);
    / **/
    }

    /* seem to have trouble restoring cursor
     * to original position; this seems to help
     */
    int col = cursorcol;
    int lin = cursorline;

    cursorcol = cursorline = -1;


    poscursor(0,0);
    d_put(0);
    poscursor(col, lin);
    d_put(0);

    mvcur(-1, -1, cursor_screenline, cursor_screencol);
    fflush(stdout);

/** /
dbgpr(" calling poscursor(%d,%d)  mvcur(-1,-1, %d,%d)\n",
col, lin, cursor_screenline, cursor_screencol);
/ **/

    return;

}


/* returns 1 if matching end brace is found
 * updates lnum and cnum ptrs
 */
int
FindBraceBack(int beg_ch, int end_ch, int cnt, long *lnum, int *cnum)
{

    Nlines wlin = curwksp->wlin;
    Nlines nextline;
    Nlines lastline = 0;      /* first line in file is the lastline we examine */

    nextline = wlin - 1;

	/* limit search range? */
    if (braceRange && (wlin - braceRange > 0)) {
	lastline = wlin - braceRange;
    }

/** /
dbgpr("FindBraceBack:  beg_ch=%c, end_ch=%c, cnt=%d, wlin=%d \
ttext=%d nextline=%d braceRange=%d\n",
    beg_ch, end_ch, cnt, wlin, curwin->ttext, nextline, braceRange);
/ **/

    Ncols i;
    Nlines ln;

#ifdef DBUG
    char buf[1024];
#endif

    /* search backward in file */
    for (ln = nextline; ln >= lastline; ln--) {
	GetLine(ln);

#ifdef DBUG
	dbgpr("ln=%d lcline=%d, ncline=%d\n", ln, lcline, ncline);
	strncpy(buf, cline, ncline);
	buf[ncline] = '\0';
	dbgpr("(%s)\n", buf);
#endif

	for (i=ncline+1; i >= 0; i--) {
	    if (cline[i] == end_ch) {
		if (cnt <= 0) {
		    *lnum = ln; *cnum = (int)i;
		 /* dbgpr("--found %c at ln %d c %d\n", end_ch, ln, i); */
		    return 1;
		}
		cnt--;
	    /*  dbgpr("--got end_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch); */
	    }
	    else if (cline[i] == beg_ch) {
		cnt++;
	    /*  dbgpr("--got beg_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch); */
	    }
	}
    }


    return 0;
}

/* returns 1 if matching end brace is found
 * updates lnum and cnum ptrs
 */
int
FindBraceForw(int beg_ch, int end_ch, int cnt, long *lnum, int *cnum)
{

    Nlines wlin;    /* curwin topline */
    Nlines ln;
    Nlines nextline;
    Nlines lastline = la_lsize(curlas);

    wlin = curwksp->wlin;
    nextline = wlin + curwin->btext + 1; /* nxt line not displayed in curwin */

    if (braceRange) {  /* look only this far ahead */
	if (nextline + braceRange < lastline)
	    lastline = nextline + braceRange;
    }

/** /
dbgpr("FindBraceForw:  beg_ch=%c end_ch=%c, cnt=%d, wlin=%d \
btext=%d ttext=%d nextline=%d lastline=%d braceRange=%d\n",
    beg_ch, end_ch, cnt, wlin, curwin->btext,
curwin->ttext, nextline, lastline, braceRange);
/ **/

/* /lastline = nextline + 5;    / * debugging */

#ifdef DBG
    char buf[1024];
#endif

    Ncols i;

    /* search forwward in file */
    for (ln = nextline; ln <= lastline; ln++) {
	GetLine(ln);

#ifdef DBG
	dbgpr("ln=%d lcline=%d, ncline=%d\n", ln, lcline, ncline);
	strncpy(buf, cline, ncline);
	buf[ncline] = '\0';
	dbgpr("(%s)\n", buf);
#endif

	for (i=0; i <= ncline; i++) {

	    if (cline[i] == end_ch) {

/** /
dbgpr("FindForw:  have end_ch %c at ln %d c %d braceCoding=%d\n",
 end_ch, ln, i, bracematchCoding);
/ **/

		if (cnt <= 0) {
		    *lnum = ln; *cnum = (int)i;
		/*  dbgpr("--found %c at ln %d c %d\n", end_ch, ln, i); */
		    return 1;
		}
		/* stop at end of a function, which we assume
		 * to be a } in column 0
		 */
		if (i == 0 && end_ch == '}' && bracematchCoding) {
		    *lnum = ln; *cnum = (int)i;
		    return 2;
		}

		cnt--;
	    /*  dbgpr("--got end_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch);*/
	    }
	    else if (cline[i] == beg_ch) {
		cnt++;
	    /*  dbgpr("--got beg_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch); */
	    }
	}
    }
    if (ln >= lastline)
	return 3;

    return 0;
}


int
win_has_eof()
{

/** /
dbgpr("win_has:  wlin=%d btext=%d ll=%d",
    curwksp->wlin, curwin->btext, la_lsize(curlas));
/ **/

    if ((curwksp->wlin + curwin->btext) >= la_lsize(curlas))
	return 1;

    return 0;
}

