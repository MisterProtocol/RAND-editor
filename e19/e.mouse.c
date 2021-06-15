#ifdef COMMENT
--------
file e.mouse.c
    NCURSES mouse suport
#endif


#include "localenv.h"
#include "e.h"
#include "e.m.h"
#include "e.cm.h"
#include "e.tt.h"
#include "ncurses.h"
#include "term.h"
#include "ctype.h"

#ifdef NCURSES

extern Flag optskipmouse;
extern Flag cmdmode;
extern Small nwinlist;
extern struct loopflags loopflags;
extern char *highlight_info_str;
extern Flag highlight_mode;
int findWin(int y, int x);

#define UCHAR(c)    ((unsigned char)(c))
/*#define CTRL(x) ((x) & 0x1f)*/

void debug_inputkey();
void initCurses();
void exitCurses();
void testMouse();
/*void dbg_showMarkInfo();*/
int toggle_mouse();

static const char *mouse_decode();
Flag initMouseDone = NO;
Flag initCursesDone = NO;
Flag bs_flag = NO;
Flag mouse_enabled = YES;

/* variables for user to set custom rgb values for highlighting */
int opt_bg_r;
int opt_bg_g;
int opt_bg_b;
Flag bg_rgb_options = NO;

int opt_fg_r;
int opt_fg_g;
int opt_fg_b;
Flag fg_rgb_options = NO;

/* Color pairs, Alternate method of setting color */
int opt_setab = 0;  /* -setab=N  */
int opt_setaf = 0;  /* -setaf=N  */

/* screen mode variables */
char *smso;       /* enter standout */
char *rmso;       /* exit standout */
char *bold_str;   /* start bold */
char *setaf;      /* set foreground color */
char *setaf_p;    /* set foreground color, output of tparm() */
char *setab;      /* set background color */
char *setab_p;    /* set background color, output of tparm() */
char *sgr0;       /* reset all modes */
int  n_colors;    /* terminal colors */
char *hilite_str; /* set to a standout mode, bold, bgcolor, rev vid */

/*  5/6/21, we're now combining fg and bg colors in highlight mode */
char *fgbg_pair;  /* combined setaf_p and setab_p */


/* Initiallize Curses to support mouse click.
*/
void initCurses()
{
    if( initCursesDone ) {
    /* dbgpr("initCurses:  already initialized\n"); */
       return;
    }

    initscr();
/**
    dbgpr("initCurses(), initializing curses.\n");
    dbgpr("h=%d, w=%d, maxx=%d maxy=%d\n",
	term.tt_height, term.tt_width, getmaxx(stdscr), getmaxy(stdscr));

    if (bg_rgb_options)
      dbgpr("user bg rgb=(%d,%d,%d)\n", opt_bg_r, opt_bg_g, opt_bg_b);
    if (fg_rgb_options)
      dbgpr("user fg rgb=(%d,%d,%d)\n", opt_fg_r, opt_fg_g, opt_fg_b);
    if (opt_setaf || opt_setab)
      dbgpr("user fg setab=%d setaf=%d  \n", opt_setab, opt_setaf );
**/

    initCursesDone = YES;

/*  clear(); */
    noecho();
/*  cbreak();*/
    raw();
    nonl(); /* want RETURN key to send ^M not ^J */
    keypad(stdscr, TRUE); /* want *single* value of mouse/function key pressed */

    if( !optskipmouse ) {
	/* Keep it simple for now */
    /*  mousemask(BUTTON1_PRESSED|BUTTON1_RELEASED|BUTTON1_CLICKED, NULL); */
	mousemask(ALL_MOUSE_EVENTS, NULL);

	/* default is 166, which is a bit quick on my linux box */
	/* todo, make the delay an option */
	mouseinterval(225);
	initMouseDone = YES;
    }

    /*  A non-blocking call to getch() here, seems to prevent getch() from clearing
     *  the screen the next time getch() is called.  Before adding this, the initial display was
     *  blank!
     *
     *  Credit for this sage piece of info goes to Michael Burr in a Dec. 2013
     *  post at stackoveflow.com, see:
     *  https://stackoverflow.com/questions/19748685/curses-library-why-does-getch-clear-my-screen
     */
    nodelay(stdscr,TRUE);
    getch();
    nodelay(stdscr, FALSE);

#ifdef OUT
/*testMouse();*/
#endif

    /*  Test for terminal property "kbs" being '^H'.  If it is, ^H won't
     *  properly be interpreted as CCLEFT.  We'll need to fudge the input.
     */
    char *kbs_str;

    kbs_str = tigetstr("kbs");
    if (kbs_str != (char *) -1)
    {
	int len;

	len = strlen(kbs_str);
	if (len == 1 && *kbs_str == '\b') {
	    bs_flag = 1;
	}
    }


    /* test, todo, chk for errs */
    smso = tigetstr("smso");           /* rev video */
    rmso = tigetstr("rmso");           /* end rev video */
    bold_str = tigetstr("bold");       /* bold font */

    setab = setaf = "\0\0"; /* for dbgpr below */

    /*
     * initialize highlight bg/fg colors
     */

    int bg_idx = COLOR_WHITE;
/*  int fg_idx = COLOR_BLUE; */
    int fg_idx = COLOR_BLACK;
    highlight_info_str = "color";

    if (has_colors()) {
	start_color();
	n_colors = tigetnum("colors");
	    /* todo, what if terminal uses setb/setf instead of setab/setaf */
	setaf= tigetstr("setaf");          /* fg color */
	setab= tigetstr("setab");          /* bg color */
	sgr0 = tigetstr("sgr0");           /* cancel graphics */

	if (n_colors >= 256) {
	    int r1;

	    /* User can specify a bg color option either
	     * via -bg=r,g,b or -setab=N [1-255]
	     */
	    if ( bg_rgb_options ) {
		r1 = init_color(36, opt_bg_r, opt_bg_g, opt_bg_b);      /* -bg=r,g,b */
		bg_idx = 36;
		/*setab_p = strdup(tparm(setab,36));*/
		if (r1 == ERR)
		    dbgpr("initCurses, bg: init_color(%d,%d,%d) returned ERR\n",
			opt_bg_r, opt_bg_g, opt_bg_b);
	    }
	    else if (opt_setab) {
		bg_idx = opt_setab;
		/*setab_p = strdup(tparm(setab,opt_setab));*/
	    }

	    /* User can specify a fg color option either
	     * via -fg=r,g,b or -setaf=N [1-255]
	     */
	    if ( fg_rgb_options ) {
		r1 = init_color(37, opt_fg_r, opt_fg_g, opt_fg_b);      /* -fg=r,g,b */
		fg_idx = 37;
		/*setaf_p = strdup(tparm(setaf,37));*/
		if (r1 == ERR)
		    dbgpr("initCurses, fg: init_color(%d,%d,%d) returned ERR\n",
			opt_fg_r, opt_fg_g, opt_fg_b);
	    }
	    else if (opt_setaf) {
		fg_idx = opt_setaf;
		/*setaf_p = strdup(tparm(setaf,opt_setaf));*/
	    }
	}

	setab_p = strdup(tparm(setab,bg_idx));
	setaf_p = strdup(tparm(setaf,fg_idx));

	/* todo, best to learn how to use sgr to set both fg/bg colors in one 'call' */

	fgbg_pair = malloc(strlen(setab_p) + strlen(setaf_p) + 1);
	strcpy(fgbg_pair, setab_p);
	strcat(fgbg_pair, setaf_p);
	hilite_str = fgbg_pair;
    }
    else {
	   /* init these nxt 3 strings to prevent null ptr if we
	    * reference a routine that uses one of them.
	    */
	fgbg_pair = bold_str;
	hilite_str = bold_str;
	highlight_info_str = "bold";
	sgr0 = rmso;

	n_colors = 0;
	highlight_mode = NO;
    }

/**
if (n_colors) {
  dbgpr("screen modes: smso=(\\E%s), rmso=(\\E%s) bold=(\\E%s) kbs=%s colors=%d\nsetab_p=(\\E%s)\nsetaf_p=(\\E%s)\n",
  smso+1, rmso+1, bold_str+1, kbs_str, n_colors, setab_p+1, setaf_p+1);
}
 **/
    return;
}


static const char *
mouse_decode(MEVENT const *ep)
{
    static char buf[80];

    snprintf(buf, sizeof buf, "id %2d at (%2d, %2d, %d) state %4lx = {",
	ep->id, ep->x, ep->y, ep->z, (unsigned long) ep->bstate);

    /* manual says only one bit is set in bstate */

    if (ep->bstate & REPORT_MOUSE_POSITION) {
	strcat(buf, "moved");
    }
    else if (ep->bstate & BUTTON1_RELEASED) {
	strcat(buf, "release-1");
    }
    else if (ep->bstate & BUTTON1_PRESSED) {
	strcat(buf, "press-1");
    }
    else if (ep->bstate & BUTTON1_CLICKED) {
	strcat(buf, "click-1");
    }
    else if (ep->bstate & BUTTON1_DOUBLE_CLICKED) {
	strcat(buf, "double-click-1");
    }
    else if (ep->bstate & BUTTON2_RELEASED) {
	strcat(buf, "release-2");
    }
    else if (ep->bstate & BUTTON2_PRESSED) {
	strcat(buf, "press-2");
    }
    else if (ep->bstate & BUTTON2_CLICKED) {
	strcat(buf, "click-2");
    }
    else if (ep->bstate & BUTTON2_DOUBLE_CLICKED) {
	strcat(buf, "double-click-2");
    }
    else {
	strcat(buf, "other mouse event");
    }
    strcat(buf, "}");
    return (buf);
}

void
debug_inputkey(int c, char *source)
{
    MEVENT event;

    if (c == KEY_MOUSE) {
	if(getmouse(&event) == OK) {
	    dbgpr("%s:%s\n", source, mouse_decode(&event));
	}
    }
    else if (isprint(c)) {
	dbgpr("%s:isprint key: %o %c\n", source, UCHAR(c), UCHAR(c));
    }
    else if (iscntrl(c)) {
	dbgpr("%s:iscntrl key: %o %c\n", source, UCHAR(c), UCHAR(c));
    }
    else {
	dbgpr("%s:Other key:  %o %x %d\n", source, UCHAR(c), UCHAR(c), UCHAR(c));
    }

    return;
}


/* add to exit routines */
void exitCurses()
{
    endwin();
/*  dbgpr("exitCurses()\n"); */
    return;
}


#ifdef OUT
/* can call this at various places to
 * find out where it stops working
 */

void testMouse() {

    int c;
    MEVENT event;

    while(1) {
	c = getch();
	if (c == KEY_MOUSE) {
	    if(getmouse(&event) == OK) {
		event.id = c;
		printw("%s\n", mouse_decode(&event));
	    }
	}
	else if (isprint(c)) {
	    printw("key: %o %c\n", UCHAR(c), UCHAR(c));
	}
	else if (iscntrl(c)) {
	    printw("ctrl key: %o %c\n", UCHAR(c), UCHAR(c));
	    if (c == CTRL('x')) /* ^x */    /* EXIT */
		break;
	}
	else {
	    printw("Other:  %o %c\n", UCHAR(c), UCHAR(c));
	}
    }
    return;
}


static const char *
t_mouse_decode(MEVENT const *ep)
{
    static char buf[80];

    sprintf(buf, "id %2d at (%2d, %2d, %d) state %4lx = {",
	ep->id, ep->x, ep->y, ep->z, (unsigned long) ep->bstate);

    if (ep->bstate & BUTTON1_RELEASED) {
	strcat(buf, "release-1");
    }
    else if (ep->bstate & BUTTON1_PRESSED) {
	strcat(buf, "press-1");
    }
    else if (ep->bstate & BUTTON1_CLICKED) {
	strcat(buf, "click-1");
    }
    else if (ep->bstate & BUTTON1_DOUBLE_CLICKED) {
	strcat(buf, "double-click-1");
    }
    else if (ep->bstate & BUTTON2_RELEASED) {
	strcat(buf, "release-2");
    }
    else if (ep->bstate & BUTTON2_PRESSED) {
	strcat(buf, "press-2");
    }
    else if (ep->bstate & BUTTON2_CLICKED) {
	strcat(buf, "click-2");
    }
    else if (ep->bstate & BUTTON2_DOUBLE_CLICKED) {
	strcat(buf, "double-click-2");
    }
    else {
	strcat(buf, "other mouse event");
    }
    strcat(buf, "}");
    return (buf);
}

void testarg(char *s, int ch)
{
    dbgpr("testarg: %s: got o:%o d:%d c:'%c'\n", s, UCHAR(ch), UCHAR(ch), UCHAR(ch));
    return;
}
#endif /* OUT */

/*
 * goal here is to retrieve mouse location, button info, and
 * implement a few basic functions.  Eg:
 *  - move the e cursor
 *  - chwin if clicked in a diffent e window
 *  - maybe simple rectangle marking
 */

void
doMouseEvent()
{
    MEVENT event, *ev;
    ev = &event;
/*  static int last_state; */

    /*
     * todo, examine ev->state to handle specific mouse functions
     */

    if (getmouse(&event) != OK) {
	dbgpr("--getmouse() error\n");
	return;
    }

/*
dbgpr("\ndoMouseEvent: ev->y=(%d), ev->x=(%d), id=%d state=%ld\n",
  (int)ev->y, (int)ev->x, ev->id, (unsigned long)ev->bstate );
*/

/* errant call from replaying? */

if( ev->id == -1 && ev->bstate == 0 ) {
/* dbgpr("doMouseEvent:  skipped, id=-1, bstate=0\n"); */
  return;
}

    if( cmdmode ) {
/*      dbgpr("--skip doMouse, cmdmode\n"); */
	return;
    }

    /* Note the origin for ev->(y,x) is 0,0 */

/*
int cury,curx;
getyx(stdscr,cury,curx);
dbgpr("doMouseEv: before poscursor(), cury=%d curx=%d\n", cury,curx);
dbgpr("doMouseEv: curwin: ltext=%d, ttext=%d, rtext=%d, btext=%d nwinlist=%d\n",
  curwin->ltext, curwin->ttext, curwin->rtext, curwin->btext, nwinlist);
dbgpr("doMouseEv: curwin: lmarg=%d, tmarg=%d, rmarg=%d, bmarg=%d nwinlist=%d\n",
  curwin->lmarg, curwin->tmarg, curwin->rmarg, curwin->bmarg, nwinlist);
*/

/*
dbgpr("doMouseEv: curwksp->clin=%d, curwksp->ccol=%d, curwksp->wlin=%d, curwksp->wcol=%d\n",
  curwksp->clin, curwksp->ccol, curwksp->wlin, curwksp->wcol);
*/

    /*   BUG somewhere, on long lines the ev->x value turns negative!
     *   (Per Mike O'brien, this error doesn't happen on osx)
     */

    if( (ev->y < 0) || (ev->x < 0) ) {
	char err_buf[128];
	dbgpr("doMouse:  x or y can't be negative\n");
	/*sprintf(err_buf, "Internal error, mouse click skipped, y,x is negative:  y=(%d),x=(%d).", ev->y, ev->x);*/
	snprintf(err_buf, sizeof(err_buf),
	    "Internal error, mouse click skipped, y,x is negative:  y=(%d),x=(%d).", ev->y, ev->x);
	mesg (ERRALL+1, &err_buf);
	loopflags.hold = YES;
	d_put(0);
	return;
    }

    /*
     * Save the mouse event info in the keystroke file.
     */

    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */
/*
    char ys[4], xs[4];
    sprintf(ys, "%03d", ev->y);
    sprintf(xs, "%03d", ev->x);
    fprintf(keyfile, "%c%s%s", CCMOUSE, ys, xs);
*/
    fprintf(keyfile, "%c%03d%03d", CCMOUSE, ev->y, ev->x);
    fflush(keyfile);

    /*
     * Determine which e window the click occurred in, and use
     * that window's ltext and ttext values for the call to poscursor().
     */

    /* Adjust clicks that are outside the text area of an E window.
     */
    if( ev->y == 0 ) ev->y = 1;
    if( ev->x == 0 ) ev->x = 1;

    int win_n;

    /* if only one window... */
    if( nwinlist == 1 ) {
	if( ev->y >= curwin->bmarg ) ev->y = curwin->bmarg - 1;
	if( ev->x >= curwin->rmarg ) ev->x = curwin->rmarg - 1;
    }
    else {
	win_n = findWin(ev->y, ev->x);
	if( win_n == -1 ) {
	/* dbgpr("can't find window enclosing (y,x)=(%d,%d)\n", ev->y, ev->x); */
	   return;  /* ??? */
	}

/* dbgpr("doMouse: changing to win %d\n",win_n); */

	/* todo, flush keys,and write {CCCMD}win_n{CCCHWINDOW} to keys file */
	fprintf(keyfile, "%c%c%c", CCCMD, win_n,CCCHWINDOW);
	fflush(keyfile);

	chgwindow(win_n); /* see e.wi.c */
	if( ev->y >= curwin->bmarg ) ev->y = curwin->bmarg - 1;
	if( ev->y == curwin->tmarg ) ev->y = curwin->tmarg + 1;
	if( ev->x >= curwin->rmarg ) ev->x = curwin->rmarg - 1;
	if( ev->x == curwin->lmarg ) ev->x = curwin->lmarg + 1;
    }

/*
dbgpr("doMouse calling poscursor(%d,%d)\n",
ev->x - curwin->ltext, ev->y - curwin->ttext);
*/

    poscursor(ev->x - curwin->ltext, ev->y - curwin->ttext);

    /*  Using wmove(...) causes a problem, sometimes requiring the current line(s)
     *  to be redrawn.  The advice online is not to mix curses calls w/old pgms written
     *  for termcap/terminfo.
     */
    /* wmove(stdscr, ev->y, ev->x); */

    /* set/unset a MARK  */
    if ((ev->bstate & BUTTON2_CLICKED) || (ev->bstate & BUTTON2_PRESSED)) {
	if (curmark) {
	    unmark();
	}
	else {
	    mark();
	}
    }

/*
    if( curmark )
      dbg_showMarkInfo("doMous");
*/

/* Not used yet */
/*  last_state = ev->bstate; */


#ifdef OUT
cury=getcury(stdscr);
curx=getcurx(stdscr);
dbgpr("doMouseEv: after poscursor() and wmove(%d,%d), cury=%d curx=%d\n\n", ev->y,ev->x,cury,curx);
#endif  /* OUT */

    return;
}

/*  return the number of the window that contains y,x
 *  if x or y falls on a border, use the window down or right of the y or x
 */

int
findWin(int y, int x) {
    int i;
    S_window *wp;
    int wn = -1;

    for(i=0; i < MAXWINLIST; i++) {
	if( i >= nwinlist ) {
	    break;
	}
	wp = winlist[i];

/*
	dbgpr("winlist[%d], wp->ttext=%03d wp->btext=%03d wp->ltext=%03d wp->rtext=%03d\n",
	     i, wp->ttext, wp->btext, wp->ltext, wp->rtext);
	dbgpr("winlist[%d], wp->tmarg=%03d wp->bmarg=%03d wp->lmarg=%03d wp->rmarg=%03d\n",
	     i, wp->tmarg, wp->bmarg, wp->lmarg, wp->rmarg);
*/

	/* todo, handle clicks at a border... */
	if( (y >= wp->tmarg) && (y < wp->bmarg) && (x > wp->lmarg) && (x <= wp->rmarg) ) {
	    wn = i;
	    /* dbgpr("winlist[%d] contains (y,x) of (%d,%d)\n", i, y,x); */
	    break;
	}
    }
    return wn;
}


/*
 * handle mouse call during a replay/recovery
 *   (no need to check for E windows)
 */
void
doMouseReplay(int y, int x)
{

    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */

    char ys[4], xs[4];
    sprintf(ys, "%03d", y);
    sprintf(xs, "%03d", x);
    fprintf(keyfile, "%c%s%s", CCMOUSE, ys, xs);

    if( y == 0 ) y = 1;
    if( x == 0 ) x = 1;

    /* no need to consider the number of windows, or which is active
     * as an chwin cmd will have already been seen
     */

    if( y >= curwin->bmarg ) y = curwin->bmarg - 1;
    if( x >= curwin->rmarg ) x = curwin->rmarg - 1;

/*
dbgpr("doMouseReplay calling poscursor(%d,%d)\n",
 x - curwin->ltext, y - curwin->ttext);
*/
    poscursor(x - curwin->ltext, y - curwin->ttext);

    return;
}

#ifdef OUT  /* currently not used, see debugging in e.mk.c */
void
dbg_showMarkInfo(char *source)
{
    int m_top;
    int m_left;
    int w_line;
    int w_col;
    int n_lines;
    int n_cols;

    if( curmark == NULL )
	return;

    m_top = topmark();
    m_left = leftmark();
    w_line = curmark->mrkwinlin;
    w_col  = curmark->mrkwincol;
    n_lines = curmark->mrklin;
    n_cols = curmark->mrkcol;

/**
    dbgpr("Mark info:  topmark()=%d, leftmark()=%d, curmark->: mrkwinlin=%d, mrkwincol=%d mrkln=%d, mrkcol=%d\n",
      m_top, m_left, w_line, w_col, n_lines, n_cols);

    dbgpr("Mark info (%s):  marklines=%d, markcols=%d, cursorline=%d, cursorcol=%d\n\n",
	source, marklines, markcols, cursorline, cursorcol);
 **/

    return;

}
#endif /* OUT */

/*
 * opstr is either on/off, or a toggle if missing
 */

int
toggle_mouse(char *opstr) {

    mmask_t newmask;
    Flag ok = YES;  /* mask change needed */

    if (opstr == NULL || *opstr == '\0') {   /* toggle */
	if (mouse_enabled) {
	    mouse_enabled = NO;
	    newmask = 0;
	}
	else {
	    mouse_enabled = YES;
	    newmask = ALL_MOUSE_EVENTS;
	}
	mousemask(newmask, NULL);
	return 0;
    }

    /* eg, <cmd>mouse on/off<ret>  */

    if (strncmp("on", opstr, 2) == 0) {
	mouse_enabled = YES;
	newmask = ALL_MOUSE_EVENTS;
    }
    else if (strncmp("off", opstr, 3) == 0) {
	mouse_enabled = NO;
	newmask = 0;
    }
    else {
	mesg (ERRALL+4, "\"", opstr, "\"", " is an invalid option to the mouse command.");
	ok = NO;
    }

    if (ok) {
	mousemask(newmask, NULL);
    }

    /* todo, maybe do a mesg(TELALL...), but user will see pointer shape change */

/*
dbgpr("--toggle_mouse, opt=(%s), mouse is now %sabled\n", opstr, (mouse_enabled>0 )? "en" : "dis");
*/
    return 0;
}




#endif /* NCURSES */

