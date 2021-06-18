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

#include <ncurses.h>
#include <term.h>
extern Uchar *image;

/* highlight modes, see cmd: set highlight */
extern char *smso, *rmso, *bold_str, *setab, *setaf, *sgr0, *setab_p, *setaf_p, *hilite_str;
extern char *fgbg_pair;

void highlightarea(Flag setmode, Flag redrawflg);
void HiLightLine(int line, Flag setmode);
void HiLightRect(int from, int to, int mrkcol, int ccol);
/*void dbg_showMarkInfo();*/
Flag highlight_mode = YES;
Flag redrawflg; /* set after CMD: redraw */

int Pch(int ch) { putchar(ch); return (0); }

extern void markprev ();

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
	    tmp = curmark->mrkwincol;
	    curmark->mrkwincol = prevmark->mrkwincol;
	    prevmark->mrkwincol = tmp;
	    tmp = curmark->mrkcol;
	    curmark->mrkcol = prevmark->mrkcol;
	    prevmark->mrkcol = tmp;
	}
	else if (!exchlines) {
	    /* exchange the lines so mark will set them back */
	    tmp = curmark->mrkwinlin;
	    curmark->mrkwinlin = prevmark->mrkwinlin;
	    prevmark->mrkwinlin = tmp;
	    tmp = curmark->mrklin;
	    curmark->mrklin = prevmark->mrklin;
	    prevmark->mrklin = tmp;
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
    prevmark->mrklin = cursorline;
    return;
}



#ifdef OUT
/*
 */
void
testImageLine(int linenum, int *count) {
    int w = term.tt_width;
    char buf[w];

    char *cp = (char *)image + w;

    snprintf(buf, w-1, "%s", cp+1); /* skip left border char */
 /* dbgpr("image line 1 len=(%d):\n%s\n", strlen(buf), buf); */

    cp = (char *)image + w*2;
    snprintf(buf, w-1, "%s", cp+1);
/*  dbgpr("image line 2 len=(%d):\n%s\n", strlen(buf), buf); */

    cp = (char *)image + w*3;
    snprintf(buf, w-1, "%s", cp+1);
/*  dbgpr("image line 3 len=(%d):\n%s\n", strlen(buf), buf); */
}
#endif /* OUT */

/*
 *    Redraw the marked area of the screen in a
 *    highlighted screen mode; setmode = YES/NO
 */

void
highlightarea(Flag setmode, Flag redrawflg) {

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
	for (i = last_top; i<= last_bot; i++) {
	    HiLightLine(i, NO);
	}
	last_top = last_bot = last_marklines = last_cursorline = last_col = -1;
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
/*dbg_showMarkInfo("hilite");*/

    if (firsttime) {  /* hilite just one line */
	top_line = cursorline + curwin->ttext;  /* omit top border */
	bot_line = top_line;
	last_top = top_line;
	last_bot = bot_line;;
	last_marklines = marklines;
	last_cursorline = cursorline;
	last_col = cursorcol;
	HiLightLine(top_line, YES);
      /*dbgpr("firsttime\n"); */
	firsttime = 0;
	return;
    }

    /*   no action needed when cursor motion is suppressed, eg,
     *   moving up at line 1 of a file, or left at col 1
     */
    if (marklines == last_marklines && cursorline == last_cursorline
	    && cursorcol == last_col && redrawflg == NO) {
  /** / dbgpr("no highlighting needed...\n"); / **/
	return;
    }

/*
dbgpr("--before redraw=%d last_top=%d, last_bot=%d, last_col=%d, last_marklines=%d firsttime=%d infoline=%d\n",
  redrawflg, last_top, last_bot, last_col, last_marklines, firsttime, infoline);
*/
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


    /*  clear previous mark */
    /*for (i = last_top; i<= last_bot; i++)*/
    for (i = curwin->ttext; i < curwin->bmarg; i++)
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
	    bot_line = curwin->btext + 1;
/** / dbgpr("1a: cursor=bottom, topmark() on prev page, top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
	}
	else {
	    top_line = curwin->ttext + topmark() - curwksp->wlin;
	    /*bot_line = top_line + marklines - 1;*/
	    bot_line = curwin->btext + 1;
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
	    bot_line = top_line + marklines - 1;
	}
	if( bot_line > curwin->btext ) {
	    bot_line = curwin->btext + 1;
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
		bot_line = top_line + marklines - 1;
	    }
	    else {
		top_line = curwin->ttext + cursorline;
		bot_line = top_line + marklines - 1;
	    }
/** / dbgpr("4: mrkwinlin = wlin, top_line=%d, bot_line=%d\n", top_line, bot_line); / **/
	}
	/* topmark() on current screen */
	else if (curwksp->wlin < topmark()) {
	    top_line = topmark() - curwksp->wlin + 1;
	    bot_line = top_line + marklines - 1;
	    if (bot_line > curwin->btext+1)
		bot_line = curwin->btext+1;
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

    if (bot_line > curwin->btext+1) {
/** /dbgpr("***bot_line out of range (%d)\n", bot_line); / **/
	bot_line = curwin->btext + 1;
    }
    if (top_line <= 0) {
/** /dbgpr("*** top_line out of range (%d)\n", top_line); / **/
	top_line = curwin->ttext;
    }

    savecurs();

    /*if (cursorcol == curmark->mrkcol) { */
    if (markcols == 0) {
	/* entire lines */
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
    last_marklines = marklines;
    last_cursorline = cursorline;
    last_col = cursorcol;

/** / dbgpr("setting last_top=top_line = %d, last_bot=bot_line = %d\n", top_line, bot_line ); / **/

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
HiLightLine(int line, Flag setmode) {

    int w = term.tt_width;
    char buf[w+1];

    char *cp = (char *)image + w*line;

    /*  use curwin->lmarg and curwin->rtext to account for any windows,
     *  omit left/rt border chars
     */
    int wid = curwin->rmarg - curwin->lmarg;

/**
if (setmode == YES)
dbgpr("HiLi: wid=%d lmarg=%d ltext=%d, rmarg=%d, rtext=%d mrkwincol=%d mrkcol=%d wksp->wcol=%d wksp->wlin=%d\n",
wid, curwin->lmarg, curwin->ltext, curwin->rmarg, curwin->rtext,
curmark->mrkwincol, curmark->mrkcol, curwksp->wcol, curwksp->wlin);
 **/

    snprintf(buf, wid, "%s", cp+curwin->ltext);
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
HiLightRect(int from, int to, int mrkcol, int curcol) {

    int w = term.tt_width;
    char buf[w+1];
    char *cp;

    int beg_mark;
    int end_mark;
    int mark_len;
    int line;


    if( curcol > mrkcol ) {
	beg_mark = mrkcol;
	end_mark = curcol;
    }
    else {
	beg_mark = curcol;
	end_mark = mrkcol;
    }

    mark_len = end_mark - beg_mark;

    /*  use curwin->lmarg and curwin->rtext to account for any windows,
     *  omit left/rt border chars
     */
    int wid = curwin->rmarg - curwin->lmarg;
    beg_mark += curwin->ltext;

/**
dbgpr("HiLi: wid=%d lmarg=%d ltext=%d, rmarg=%d, rtext=%d mrkwincol=%d mrkcol=%d wksp->wcol=%d wksp->wlin=%d\n",
wid, curwin->lmarg, curwin->ltext, curwin->rmarg, curwin->rtext,
curmark->mrkwincol, curmark->mrkcol, curwksp->wcol, curwksp->wlin);
 **/

    for (line = from; line <= to; line++ ) {
	cp = (char *)image + w*line;

	/* clear any full line highlighting */
	mvcur(-1,-1, line, curwin->ltext);
	snprintf(buf, wid, "%s", cp+curwin->ltext);
	puts(buf);

#ifdef OUT
/*   hmm, since we've redrawn the line above
 *   we should only need to overlay the marked area
 *   yes, seems to work ok
 */
	/* left side of marked area */
	if( beg_mark > 1 ) {
	    snprintf(buf, beg_mark+1, "%s", cp+1); /* omit left border char */
	    mvcur(-1,-1, line, 1);
	    puts(buf);
	    /* dbgpr("left side=(%s)\n", buf); */
	}
#endif /* OUT */
	/* marked area */
	snprintf(buf, mark_len+1, "%s", cp + beg_mark );
	mvcur(-1,-1, line, beg_mark);

	tputs(hilite_str, 1, Pch);      /* user selected mode, see cmd: set highlight */
	puts(buf);
	tputs(sgr0, 1, Pch);

	/* dbgpr("marked area=(%s)\n", buf); */

#ifdef OUT
	/* right side of marked area */
	snprintf(buf, w - end_mark - 1, "%s", cp + 1 + end_mark); /* omit right border char */
	mvcur(-1,-1, line, end_mark + 1);
	puts(buf);
	/*dbgpr("right side=(%s)\n", buf);*/
#endif /* OUT */
    }
    fflush(stdout);

/*
dbgpr("HiLightRect: from=%d, to=%d beg_mark=%d end_mark=%d mark_len=%d\n",
  from, to, beg_mark, end_mark, mark_len);
*/
    return;
}

