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
#include "ctype.h"
#include <string.h>

#ifdef NCURSES

extern Flag optuseextnames;
extern Flag optskipmouse;
extern Flag optshowbuttons;
extern Flag cmdmode;
extern Small nwinlist;
extern struct loopflags loopflags;
extern char *highlight_info_str;
extern Flag highlight_mode;
int findWin(int y, int x);
extern void HiLightLine(int, Flag);
extern Scols inf_area;
extern Small infoarealen;

extern int savetty(void);
extern int resetty(void);

#define UCHAR(c)    ((unsigned char)(c))
/*#define CTRL(x) ((x) & 0x1f)*/

void debug_inputkey(int, char *);
void initCurses(void);
void initCursesColor(void);
void exitCurses(void);
void testMouse(void);
int toggle_mouse(char *);
void doMouseEvent(void);

static const char *mouse_decode(MEVENT const *);
Flag initMouseDone = NO;
Flag initCursesDone = NO;
Flag bs_flag = NO;
Flag mouse_enabled = YES;
int mouseFuncKey;   /* set to an E key code for processing in e.m.c */
void getMouseFuncKey(int, int);
extern char *getEkeyname(int c);
void doChgWin(int, int, int);
int from_topline, from_botline; /* previous start, end of highlighted lines */
void calcRange2Clear(int, int, int *, int *);
extern void doMouseReplay (int, int);

#ifdef MOUSE_BUTTONS
int inButtonRow (int);
void HiLightButton (int, int);
void getMouseButtonEvent (void);
void buttoninit (void);
Flag have_record_button = NO;

/* custom button definitions */
#define MAX_BUTTONS 20      /* per row */
mouse_button_table my_button_row1[MAX_BUTTONS];
mouse_button_table my_button_row2[MAX_BUTTONS];
int my_row1_cnt, my_row2_cnt;

void add_mybutton(int, char *, int, int, mouse_button_table *);
int my_lookup (char *, S_looktbl *);
int get_mybuttons (char *);
extern S_looktbl keysyms[];
Flag useMyButtons = 0;  /* set when using custom buttons */

#ifdef BUTTON_FONT
void initButtonRow (int, int, mouse_button_table *);
void overlayButtons (void);
void makeButtonOverlayB (int, int, int);
char *button_font;
int btn_font_n = 0;     /* -buttonfont=N (1-255) */
#endif /* BUTTON_FONT */

#endif /* MOUSE_BUTTONS */

#ifdef USE_MOUSE_POSITION
int doMotion (MEVENT *, int, int);
int endMotion (int, int);
void doDragHiLite (int, int, int, int);
int miny, maxy;
extern Uchar *image;

/* report position only when mouse is pressed */
/**/
char *mouse_init = "\033[?1002h";
char *mouse_stop = "\033[?1002l";
/**/

/* SGR mouse mode, report position only when mouse is pressed */
/** /
char *mouse_init = "\033[?1006;1002h";
char *mouse_stop = "\033[?1006;1002l";
/ **/

extern char *optmouse_init;
extern char *optmouse_stop;

int init_mousemotion;  /* whether we need to output mouse_init, or depend on XM entry */

#ifdef OUT
/* this reports position whenever mouse is moved, even if not pressed */
char *mouse_init = "\033[?1003h";
#endif

#endif /* USE_MOUSE_POSITION */

char *clear_screen_esc;


/* variables for user to set custom rgb values for highlighting */
int opt_bg_r;   /* -bg=r,g,b */
int opt_bg_g;
int opt_bg_b;
Flag bg_rgb_options = NO;

int opt_fg_r;   /* -fg=r,g,b */
int opt_fg_g;
int opt_fg_b;
Flag fg_rgb_options = NO;

/* Color pairs, Alternate method of setting color */
int opt_setab = 0;  /* -setab=N  */
int opt_setaf = 0;  /* -setaf=N  */
Flag setaf_set = NO;
Flag setab_set = NO;

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
char *brace_p;    /* color for bracket matching */
char *font_red;   /* to highlight errors */


#ifdef OUT
char *XM;   /* terminfo entry to initialize mouse mode */
#endif

/*  5/6/21, we're now combining fg and bg colors in highlight mode */
char *fgbg_pair;  /* combined setaf_p and setab_p */


/*
 *  Initiallize Curses to support mouse click.
 */
void
initCurses()
{

    if( initCursesDone ) {
    /* dbgpr("initCurses:  already initialized\n"); */
       return;
    }

    init_mousemotion = 1;    /* yes, output mouse_init string */


/** /
dbgpr("initCurses:  optuseextnames=%d\n", optuseextnames);
/ **/

    if (optuseextnames == NO) {  /* default is NO */
	use_extended_names(0);   /* default is 1, examine non-std entries like XM */
    }

/*  savetty(); */

    initscr();

/**
    dbgpr("initCurses(), initializing curses.\n");
    dbgpr("h=%d, w=%d, maxx=%d maxy=%d\n",
	term.tt_height, term.tt_width, getmaxx(stdscr), getmaxy(stdscr));
**/

    extern int ESCDELAY;
    ESCDELAY = 0;

    initCursesDone = YES;

/*  clear(); */
/*  cbreak();*/
    raw();
    keypad(stdscr, TRUE); /* want *single* value of mouse/function key pressed */
    noecho();
    nonl();    /* want RETURN key to send ^M not ^J */

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

    if( !optskipmouse ) {
	initMouseDone = YES;

#ifdef USE_MOUSE_POSITION

	mouseinterval(0);
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

	if (optmouse_init) {    /* eg, -mouseinit="E[?1006;1002h" */
	    mouse_init = optmouse_init;
	    if (*mouse_init == 'E') {
		*mouse_init = '\033';
	    }
       /** /dbgpr("mouse_init = \\E%s\n", mouse_init + 1 );   / **/

	    /*  if optmouse_stop is not supplied, assume it is a copy
	     *  of optmouse_init but change 'h' to 'l'
	     */
	    if (!optmouse_stop) {
		mouse_stop = strdup(mouse_init);
		char *s = mouse_stop;
		for (s = mouse_stop; *s; s++) {
		    if (*s == 'h') *s = 'l';
		}
	    /*  dbgpr("mouse_stop = \\E%s\n", &(mouse_stop[1])); */
	    }
	}
	if (optmouse_stop) {    /* eg, -mouseinit="E[?1006;1002l" */
	    mouse_stop = optmouse_stop;
	    if (*mouse_stop == 'E') {
		*mouse_stop = '\033';
	    }
	/*  dbgpr("mouse_stop = \\E%s\n", &(mouse_stop[1])); */
	}

/** /
dbgpr("init_mousemotion=%d, optuseext=%d\n", init_mousemotion, optuseextnames);
/ **/
	if (init_mousemotion) {
	    printf("%s", mouse_init);
	}

#else
	mousemask(ALL_MOUSE_EVENTS,  NULL);
	mouseinterval(200);
#endif /* USE_MOUSE_POSITION */
    }

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

    /* Get esc to clear screen, we'll use in e.c, dorecov() */
    clear_screen_esc = tigetstr("clear");
    if (clear_screen_esc == (char *) -1)
    {
	clear_screen_esc = "\033[H\033[2J";
    }

    from_topline = from_botline = 1;   /* was 0 */

    return;
}

/* Initiallize Curses Color
*/
void
initCursesColor()
{

/** /
    dbgpr("initColor(), initializing colors.\n");

    if (bg_rgb_options)
      dbgpr("user bg rgb=(%d,%d,%d)\n", opt_bg_r, opt_bg_g, opt_bg_b);
    if (fg_rgb_options)
      dbgpr("user fg rgb=(%d,%d,%d)\n", opt_fg_r, opt_fg_g, opt_fg_b);
    if (opt_setaf || opt_setab)
      dbgpr("user fg setab=%d setaf=%d  \n", opt_setab, opt_setaf );
/ **/

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

/* the nxt line sometimes turned up blue */

/*      font_red = "\033[41m"; */
	font_red = strdup(tparm(setab,COLOR_RED));

/* dbgpr("has colors true, n=%d\n", n_colors); */

	/*if (n_colors >= 256) {*/
	if (n_colors >= 16) {
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


	    if (btn_font_n == 0) {
		if (n_colors > 16)  /* default color yellow/gold */
		    btn_font_n = 222;
		else
		    btn_font_n = 3; /* light gray */
	    }
	}

	setab_p = strdup(tparm(setab,bg_idx));
	setaf_p = strdup(tparm(setaf,fg_idx));

/*      brace_p= strdup(tparm(setab,6));  */
	brace_p= strdup(tparm(setab,COLOR_CYAN));  /* for bracket matching */

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
	fgbg_pair = smso; /*bold_str;*/
	hilite_str = smso; /*bold_str; */
	highlight_info_str = "color"; /*"bold";*/
	sgr0 = rmso;
	n_colors = 0;
	highlight_mode = NO;
	brace_p = hilite_str;
/*      btn_font_n = 7; / * gray */
	btn_font_n = COLOR_WHITE; /* very light gray */
    }

#ifdef BUTTON_FONT
    /* use font idx if user specified, otherwise hilite_str */
    button_font = btn_font_n ? strdup(tparm(setab,btn_font_n)) : hilite_str;
#endif /* BUTTON_FONT */

/** /
if (n_colors) {
  dbgpr("screen modes: smso=(\\E%s), rmso=(\\E%s) bold=(\\E%s) colors=%d\nsetab_p=(\\E%s)\nsetaf_p=(\\E%s)\n",
  smso+1, rmso+1, bold_str+1, n_colors, setab_p+1, setaf_p+1);
  dbgpr("hilite_str=%s brace_p=%s\n", hilite_str+1, brace_p+1);
}
/ **/
    return;
}





static const char *
mouse_decode(MEVENT const *ep)
{
    static char buf[80];

    snprintf(buf, sizeof buf, "id %2d at (%2d, %2d, %d) state %4lx = {",
	ep->id, ep->y, ep->x, ep->z, (unsigned long) ep->bstate);

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
    else if (ep->bstate & BUTTON4_RELEASED) {
	strcat(buf, "release-2");
    }
    else if (ep->bstate & BUTTON4_PRESSED) {
	strcat(buf, "press-2");
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
void
exitCurses()
{
    static int endwin_done = 0;

    if (endwin_done)
	return;     /* already done */

    endwin();
/*  resetty(); */

    if (init_mousemotion)
	printf("%s\n", mouse_stop);

/*  printf("\r\n"); */

/** / dbgpr("exitCurses\n"); / **/

    endwin_done = 1;

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

    sprintf(buf, "id %2d at (%2d,%2d) state 0x%08lx = {",
	ep->id, ep->y, ep->x (unsigned long) ep->bstate);

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
    else if (ep->bstate & BUTTON4_PRESSED) {
	strcat(buf, "press-4");
    }
    else if (ep->bstate & BUTTON4_RELEASED) {
	strcat(buf, "release-4");
    }
    else if (ep->bstate & REPORT_MOUSE_POSITION) {
	strcat(buf, "position");
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
    int oldcursorline = cursorline;
    int oldcursorcol = cursorcol;


#ifdef OUT
    int x_rel, y_rel;  /* button release values */
#endif

/*DebugVal = 0;*/

    loopflags.hold = NO;
    mouseFuncKey = -1;  /* check in mainloop() */

    /*
     * todo, examine ev->state to handle specific mouse functions
     */

    if (getmouse(&event) != OK) {
	dbgpr("--getmouse() error\n");
	return;
    }

    /* Since this routine waits to collect a release event
     * after seeing a press, this routine should only see
     * press and mouse_position events
     */


    /* ignore motion w/o a prior press event */

    if (ev->bstate & REPORT_MOUSE_POSITION) {
/** /
dbgpr("doMouseEvent: ignore initial mouse moves (%d,%d) bstate=0x%08lx\n",
    ev->y, ev->x, (long)ev->bstate);
/ **/
	return;
    }


/** /
dbgpr("\ndoMouseEvent: state=%ld %s\n",
  (unsigned long)ev->bstate, mouse_decode(ev) );
/ **/

/* errant call from replaying? */

if( ev->id == -1 && ev->bstate == 0 ) {
/* dbgpr("doMouseEvent:  skipped, id=-1, bstate=0\n"); */
  return;
}

    if( cmdmode ) {
/* /    dbgpr("--skip doMouseEvent, cmdmode\n"); / */
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

/** /
dbgpr("doMouseEv: curwksp->clin=%d, curwksp->ccol=%d, curwksp->wlin=%d, curwksp->wcol=%d\n",
  curwksp->clin, curwksp->ccol, curwksp->wlin, curwksp->wcol);
/ **/

    /*   BUG somewhere, on long lines the ev->x value turns negative!
     *   (Per Mike O'brien, this error doesn't happen on osx)
     *   It's also ok on Fedora 34
     *
     *   Problem fixed 8/2021.  In setitty() the ISTRIP (strip hi bit of char)
     *   was limiting input to 7 chars.
     */
    if( (ev->y < 0) || (ev->x < 0) ) {
	char err_buf[128];

	dbgpr("doMouse:  x or y can't be negative ev->y=(%d) ev->x=(%d)\n", ev->y, ev->x);
	/*sprintf(err_buf, "Internal error, mouse click skipped, y,x is negative:  y=(%d),x=(%d).", ev->y, ev->x);*/
	snprintf(err_buf, sizeof(err_buf),
	    "Internal error, mouse click skipped, y,x is negative:  y=(%d),x=(%d).", ev->y, ev->x);
	mesg (ERRALL+1, &err_buf);
	loopflags.hold = YES;
	d_put(0);
	return;
    }


    /*  Did the press occur outside the current window;
     *  Save the info for later action.
     */
    int newwin_n = -1;
    int need2chgwin = 0;

    from_topline = curwin->ttext;
    from_botline = curwin->ttext;

    if (nwinlist > 1) {
	newwin_n = findWin(ev->y, ev->x);
	if (newwin_n >= 0 && curwin != winlist[newwin_n]) {
	    if (marklines) {
	    /*  mvcur(-1, -1, curwin->ttext+cursorline-1, curwin->ltext+cursorcol); */
		poscursor(cursorcol, cursorline);
		d_put(0);
/** /
dbgpr("Can't chwin w/mark, mvcur(%d,%d)\n",
curwin->ttext+cursorline-1, curwin->ltext+cursorcol);
/ **/
		mesg (ERRALL+1, "Can't change windows while an area is marked.");
		loopflags.hold = YES;
		fresh();
		return;

	    }
	    need2chgwin = 1;
	/*  dbgpr("--press occurred outside current window\n"); */
	}
    }

/* 1st check for mouse wheel movement, if supported */

#ifdef MOUSE_BUTTONS
    /* This is tied to MOUSE_BUTTONS because existing code
     * makes adding wheel support *very* easy.
     */
#if NCURSES_MOUSE_VERSION > 1
    if (event.bstate & BUTTON4_PRESSED) {
	mouseFuncKey = CCMOVEUP;
	return;
    }
    else if (event.bstate & BUTTON5_PRESSED) {
	mouseFuncKey = CCMOVEDOWN;
	return;
    }
#endif /* NCURSES_MOUSE_VERSION */
#endif /* MOUSE_BUTTONS */

    /*   Collect mouse events until BUTTON1_RELEASED in ncurses 6.2 and
     *   BUTTON4_RELEASED in 5.7
     */

    /* first, get 5.7 working */
    if (event.bstate & BUTTON1_PRESSED) {

	/** /
	dbgpr("doMouseEvent:  GOT PRESS-1 (%d,%d) marg: t(%d) l(%d) r(%d) b(%d)\n",
	    event.y, event.x, curwin->tmarg, curwin->lmarg, curwin->rmarg, curwin->bmarg);
	/ **/

#ifdef MOUSE_BUTTONS
    /*  if (optshowbuttons && event.y == buttonwin.bmarg) {  */
	if (optshowbuttons && inButtonRow(event.y)) {
	   /** /
	   dbgpr("skip press-1 in buttonwin, wait for rel-1\n");
	   / **/
	   return;  /* wait for release-1 */
	}
#endif /* MOUSE_BUTTONS */

	Flag marking = 0;
	int c;
	MEVENT evt;
	int begy, begx;
	int firstmove = 1;
	int y0 = event.y;  /* initial press values */
	int x0 = event.x;

	if (event.y == 0) event.y = 1;     /* press was at top margin */
	if (event.x == 0) event.x = 1;     /*   "    "  "  left margin */

	/* This may not be correct for the shared border multiple windows */
	if (event.y == curwin->bmarg) event.y = curwin->btext;
	else if (event.y == curwin->tmarg) event.y = curwin->ttext;

	if (event.x == curwin->rmarg) event.x = curwin->rtext;
	else if (event.x == curwin->lmarg) event.x = curwin->ltext;

	begy = event.y;
	begx = event.x;
	mvcur(-1,-1, begy, begx);


	/* TODO, first check if event is in a window other than curwin */
	poscursor(ev->x - curwin->ltext, ev->y - curwin->ttext);

	static int y_last; /* b1-press values */
	static int x_last;

	while(1) {
	    /*dbgpr("before wgetch(), %d\n", cnt++);*/
	    c = wgetch(stdscr);
	    /*dbgpr("wgetch() got (%o)\n", c);*/

	    if (c == ERR) {
		dbgpr("doMouseEvent:  got ERR from wgetch()\n");
		break;
	    }
	    if (c != KEY_MOUSE) {
		dbgpr("doMouseEvent:  skip non mouse event: (%o)(%c)\n", c, (char)c);
		if (c == '\r') break;
		continue;
	    }
	    if (getmouse(&evt) != OK) {
		dbgpr("doMouseEvent:  getmouse err waiting for up event after a press-1\n");
		return;
	    }
	    /*dbgpr("  bstate=0x%08lx\n", (long)evt.bstate);*/

	    /** /
	    if (evt.bstate & NCURSES_BUTTON_RELEASED ) {
		dbgpr("---A button was released, bstate=0x%08lx\n", (long)evt.bstate);
	    }
	    / **/

	    if (evt.bstate & REPORT_MOUSE_POSITION) { /* NCURSES_MOUSE_VERSION > 1 */
	    /*  dbgpr("doMouseEvent:  got POSITION\n"); */

#if NCURSES_MOUSE_VERSION > 1

		if (need2chgwin) {
		    doChgWin(newwin_n, evt.y, evt.x);
		    need2chgwin = 0;
		}

		if (firstmove) {
		    /* Very slight mouse movements within the same char's y,x value
		     * are causing an unintended line to be marked.  So, delay
		     * marking until initial movement is outside the starting char's y,x
		     */
		    if (evt.y == y0 && evt.x == x0) {
		    /** dbgpr("firstmove, ignore drag w/in same char cell\n"); **/
			continue;
		    }
		    firstmove = 0;
		    marking = 1;

		    /*dbgpr("b1, first\n"); */

		    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */
		    fprintf(keyfile, "%c%03d%03d", CCMOUSE, begy, begx);
		    fprintf(keyfile, "%c", CCMARK);

		    if (curmark) {
			unmark();
		    }

		    mark();
		    miny = maxy = evt.y;
		    y_last = y0;
		    x_last = x0;
		}
		/* ignore movement within same char cell */
		if (evt.y == y_last && evt.x == x_last) {
		/** dbgpr("ignore drag w/in same char cell\n"); **/
		    continue;
		}

		doMotion(&evt, begy, begx);

		y_last = evt.y;
		x_last = evt.x;
#endif
		continue;
	    }

	    if (evt.bstate & BUTTON4_PRESSED) {     /* NCURSES_MOUSE_VERSION = 1 */
		/*dbgpr("doMouseEvent:  got press-4 (%d,%d)\n", evt.y, evt.x);*/

		if (need2chgwin) {
		    doChgWin(newwin_n, evt.y, evt.x);
		    need2chgwin = 0;
		}

		if (firstmove) {
		    /* Very slight mouse movements within the same char's y,x value
		     * are causing an unintended line to be marked.  So, delay
		     * marking until initial movement is outside the starting char's y,x
		     */
		    if (evt.y == y0 && evt.x == x0) {
		    /*  dbgpr("firstmove, ignore drag w/in same char cell\n"); */
			continue;
		    }

		    firstmove = 0;
		    marking = 1;

		    /*dbgpr("b4, first\n");*/

		    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */
		    fprintf(keyfile, "%c%03d%03d", CCMOUSE, begy, begx);
		    fprintf(keyfile, "%c", CCMARK);

		    if (curmark) {
/** /
dbgpr("  curmark=1, from_topline=%d, from_botline=%d\n",
from_topline, from_botline);
/ **/
			unmark();
		    }

		    mark();
		    miny = maxy = evt.y;
		    y_last = y0;
		    x_last = x0;
		}

		/* ignore movement within same char cell */
		if (evt.y == y_last && evt.x == x_last) {
		/*  dbgpr("ignore drag w/in same char cell\n"); */
		    continue;
		}

		doMotion(&evt, begy, begx);

		y_last = evt.y;
		x_last = evt.x;

		continue;
	    }

	    if (evt.bstate & BUTTON4_RELEASED) {
		if (marking) {
		    /** /dbgpr("doMouseEvent:  got release-4 (%d,%d)\n", evt.y, evt.x); / **/
		    /* adjust if released on a margin */
		    if (evt.y == 0) evt.y = 1;
		    if (evt.x == 0) evt.x = 1;
		    endMotion(begy, begx);

		    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */
		    fprintf(keyfile, "%c%03d%03d", CCMOUSE, evt.y, evt.x);
		    fflush(keyfile);
		    fresh();
		}
#ifdef OUT
		y_rel = evt.y;
		x_rel = evt.x;
#endif
		break;
	    }
	    else if (evt.bstate & BUTTON1_RELEASED) {
		/** / dbgpr("doMouseEvent:  got release-1 (%d,%d)\n", evt.y, evt.x); / **/
		/* adjust if released on a margin */
		if (evt.y == 0) evt.y = 1;
		if (evt.x == 0) evt.x = 1;
#if NCURSES_MOUSE_VERSION > 1
		if (marking) {
		    endMotion(begy, begx);

		    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */
		    fprintf(keyfile, "%c%03d%03d", CCMOUSE, evt.y, evt.x);
		    fflush(keyfile);
		    fresh();
		}
#endif
		/*b1_released = 1;*/
#ifdef OUT
		y_rel = evt.y;
		x_rel = evt.x;
#endif
		break;
	    }
	}

	DebugVal = 0;   /* debug TMI, keep poscursors() quiet */

	/**  RETURN **/
	if (marking) return;
    }

#ifdef OUT
/*hmm, maybe we don't need y_rel and x_rel */
/** /
dbgpr("---After release, press_yx(%d,%d) rel_yx(%d,%d)\n",
    ev->y, ev->x, y_rel, x_rel);
/ **/
#endif

    /*
     * Save the mouse event info in the keystroke file.
     */

    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */
    fprintf(keyfile, "%c%03d%03d", CCMOUSE, ev->y, ev->x);
    fflush(keyfile);


#ifdef MOUSE_BUTTONS
/*dobutton: */

    /* Check if a mouse function button was pressed */
    if (optshowbuttons == YES) {
/** /
	dbgpr("doMouseEvent:  ev->y=%d bwin->bmarg=%d bwin->btext=%d\n",
	ev->y,  buttonwin.bmarg, buttonwin.btext);
/ **/
	if (inButtonRow(ev->y)) {
	    if( ev->bstate & BUTTON1_RELEASED ) {
		/** /
		dbgpr("release in button window at ev(%d,%d) col,lin=(%d,%d) curwin=%o\n",
		  ev->y, ev->x, cursorcol, cursorline, curwin);
		/ **/
DebugVal = 0;

		savecurs();
		getMouseFuncKey(ev->x, ev->y);  /* sets global variable: mouseFuncKey */
		restcurs();

		/** /
		dbgpr("after getMouseFuncKey curCoLi=(%d,%d) curwin=%o\n",
		    cursorcol, cursorline, curwin);
		/ **/

		/* move back to curwin's position */
		/* mvcur(-1,-1, curwin->ttext + cursorline, curwin->ltext + cursorcol); */
		poscursor(cursorcol, cursorline);
		d_put(0);
DebugVal = 0;
		return;

	    }
	    return;
	}
    }
#endif /* MOUSE_BUTTONS */

    /*
     * Determine which e window the click occurred in, and use
     * that window's ltext and ttext values for the call to poscursor().
     */

    /* Adjust clicks that are outside the text area of an E window.
     */
    if( ev->y == 0 ) ev->y = 1;     /* click was at top margin */
    if( ev->x == 0 ) ev->x = 1;     /*   "    "  "  left margin */


    int winChanged = 0;


    /* if only one window... */
    if (nwinlist == 1) {

	if( ev->x >= curwin->rmarg )
	    ev->x = curwin->rmarg - 1;

	if( ev->y >= curwin->bmarg ) {
/** /
dbgpr("click outside editwin,ev->y=%d, ttext=%d, tmarg=%d, \
cursorline=%d, oldcursorline=%d\n",
    ev->y, curwin->ttext, curwin->tmarg,cursorline, oldcursorline);
/ **/
	    /* ev->y = curwin->bmarg - 1; */

	    /* ignore click if outside window */
	    ev->y = curwin->ttext + oldcursorline;
	    ev->x = curwin->ltext + oldcursorcol;
	}
    }
    else {
	if (need2chgwin) {  /* win chg w/o mouse movement */
	    doChgWin(newwin_n, ev->y, ev->x);
	    need2chgwin = 0;
	    winChanged++;
	}
    }

/** /
dbgpr("doMouse, end calling poscursor(%d,%d)\n",
ev->x - curwin->ltext, ev->y - curwin->ttext);
/ **/
    poscursor(ev->x - curwin->ltext, ev->y - curwin->ttext);
    d_put(0);

    /* shouldn't be needed, but we're off by one in certain instances */
    mvcur(-1, -1, ev->y, ev->x);

    if (!winChanged) {
	/* set/unset a MARK  */
    /*  if ((ev->bstate & BUTTON2_CLICKED) || (ev->bstate & BUTTON2_PRESSED)) { */
	if (ev->bstate & BUTTON2_RELEASED) {
	    /* dbgpr("b2-rel, toggle mark\n"); */
	    if (curmark) {
		unmark();
	    }
	    else {
		mark();
	    }
	}
    }

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

/*
 * opstr is either on/off, or a toggle if missing
 */

int
toggle_mouse(char *opstr) {

#ifdef USE_MOUSE_POSITION
    mmask_t newmask = ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION;
#else
    mmask_t newmask = ALL_MOUSE_EVENTS;
#endif
    int set_mode = 0; /* 1=enable, 2=disable */

    if (opstr == NULL || *opstr == '\0') {   /* toggle */
	if (mouse_enabled) set_mode = 2;
	else set_mode = 1;
    }
	/* eg, <cmd>mouse on/off<ret>  */
    else if (strncmp("on", opstr, 2) == 0) {
	set_mode = 1;
    }
    else if (strncmp("off", opstr, 3) == 0) {
	set_mode = 2;
    }
    else {
	mesg (ERRALL+4, "\"", opstr, "\"", " is an invalid option to the mouse command.");
	return 1;
    }

    if (set_mode == 1) {
	mouse_enabled = YES;
	mousemask(newmask, NULL);
	if (init_mousemotion)
	    printf("%s", mouse_init);
    }
    else if (set_mode == 2) {
	mouse_enabled = NO;
	mousemask(0, NULL);
	if (init_mousemotion)
	    printf("%s", mouse_stop);
    }

    /* todo, maybe do a mesg(TELALL...), but user will see pointer shape change */

/*
dbgpr("--toggle_mouse, opt=(%s), mouse is now %sabled\n", opstr, (mouse_enabled>0 )? "en" : "dis");
*/
    return 0;
}

#ifdef MOUSE_BUTTONS

int
inButtonRow (int y) {
    if (y == buttonwin.bmarg || y == buttonwin.bmarg - 2)
	return 1;
    return 0;
}


mouse_button_table button_table_row1[] = {
/*n   beg    end  ecmd       label */
{ 0,     1,   3,  CCCMD,         "cmd" },

{ 1,     5,   7,  CCINSMODE,     "ins" },

{ 2,     9,  12,  CCMARK,        "mark" },
{ 3,    13,  16,  CCUNMARK,       "/-mk" },

{ 4,    18,  21,  CCPLSRCH,      "+sch" },
{ 5,    22,  26,  CCMISRCH,      "/-sch" },

{ 6,    28,  31,  CCCAPS,        "caps"  },

{ 7,    33,  37,  CCCCASE,       "ccase"  },

{ 8,    39,  41,  CCSPLIT,       "spl" },
{ 9,    42,  46,  CCJOIN,        "/join" },

{10,    48,  51,  CCDELCH,       "delC" },

/*
{11,    55,  56,  CCMOVEUP,      "up" },
{12,    57,  59,  CCMOVEDOWN,    "/dn"},
*/

{11,    53,  56,  CCPLWIN,       "+Win" },
{12,    57,  59,  CCMIWIN,       "/-W"},

{14,    61,  62,  CCREGEX,        "re"},

{13,    64,  67,  CCREDRAW,       "redr"},

#ifdef OUT
{-1,    0,   0,   0,             ""},   /* dummy entries up to 20 */
{-1,    0,   0,   0,             ""},   /* dummy entries up to 20 */
{-1,    0,   0,   0,             ""},   /* dummy entries up to 20 */
{-1,    0,   0,   0,             ""},   /* dummy entries up to 20 */
{-1,    0,   0,   0,             ""},   /* dummy entries up to 20 */
{-1,    0,   0,   0,             ""},   /* dummy entries up to 20 */
#endif

};



mouse_button_table button_table_row2[] = {
/*n   beg    end  ecmd       label */
{ 1,     1,   3,  CCPLPAGE,      "+Pg" },
{ 2,     4,   7,  CCMIPAGE,      "/-Pg" },

{ 3,     9,  10,  CCPLLINE,      "+L"  },
{ 4,    11,  13,  CCMILINE,      "/-L"  },

{ 5,    15,  17,  CCRWORD,       "+wd"  },
{ 6,    18,  21,  CCLWORD,       "/-wd"  },

{ 7,    23,  24,  CCRWINDOW,     "wR"  },
{ 8,    25,  27,  CCLWINDOW,     "/wL"  },

{ 9,    29,  32,  CCOPEN,        "open" },

{10,    34,  37,  CCCLOSE,       "close"},
{11,    38,  41,  CCMICLOSE,     "/-cl"},

{12,    43,  46,  CCPICK,        "pick" },
{13,    47,  50,  CCPUT,         "/put"  },

{14,    52,  56,  CCERASE,       "erase" },
{15,    57,  60,  CCMIERASE,     "/-er" },

{16,    62,  64,  CCBRACE,       "{}"},

{17,    66,  69,  CCEXIT,        "exit"},

#ifdef OUT
{-1,    0,   0,   0,             ""},
{-1,    0,   0,   0,             ""},
{-1,    0,   0,   0,             ""},
#endif
};



void
getMouseFuncKey(int x, int y)
{

    int k = CCNULL;
    int i, row;
    int tablesize;
    mouse_button_table *tp;

    mouseFuncKey = -1;  /* means no function to handle in mainloop(), e.m.c */

/** /
dbgpr("getMouseFuncKey: (x,y)=(%d,%d), buttonwin.bmarg=%d\n",
 x, y, buttonwin.bmarg);
/ **/

    if (y == buttonwin.bmarg-2) {   /* x is in row 1 */
	if (useMyButtons) {
	    tp = &my_button_row1[0];
	    tablesize = my_row1_cnt;
	}
	else {
	    tp = &button_table_row1[0];
	    tablesize = (int) sizeof(button_table_row1) / sizeof(button_table_row1[0]);
	}
/** /   dbgpr(" checking for x=%d in button row 1 (useMyButtons=%d)\n", x, useMyButtons); / **/
	row = 1;
    }
    else {
	if (useMyButtons) {
	    tp = &my_button_row2[0];
	    tablesize = my_row2_cnt;
	}
	else {
	    tp = &button_table_row2[0];
	    tablesize = (int) sizeof(button_table_row2) / sizeof(button_table_row2[0]);
	}
	row = 2;
/** /   dbgpr(" checking for x=%d in button row 2 (useMyButtons=%d)\n", x, useMyButtons); / **/
    }

    for (i = 0; i < tablesize; i++) {
   /**  dbgpr("check for x=%d between %d and %d\n", x, tp[i].begx, tp[i].endx);  / **/

	if (x >= tp[i].begx && x <= tp[i].endx) {
	   k = tp[i].ecmd;
    /** /  dbgpr(" match, k=%o\n", k);  / **/
	   break;
	}
    }
    if (i < tablesize) {
	mouseFuncKey = k;
	HiLightButton(i, row);
    }

/** /
char *e_keyname = getEkeyname(k);
dbgpr("getMouseFuncKey: i=%d tblsize=%d x=%d set mouseFuncKey=(%03o)(%s)\n",
i, tablesize, x, k, e_keyname);
/ **/

    return;
}


void
buttoninit ()
{
    if (buttonwin.wksp == NULL) {
       dbgpr("buttonwin NULL\n");
       return;
    }

    if (useMyButtons) { /* custom buttons */
	initButtonRow(1, my_row1_cnt, my_button_row1);
	initButtonRow(2, my_row2_cnt, my_button_row2);
    }
    else {      /* default buttons */
	int n;
	n = sizeof(button_table_row1) / sizeof(button_table_row1[0]);
	initButtonRow(1, n, button_table_row1);

	n = sizeof(button_table_row2) / sizeof(button_table_row2[0]);
	initButtonRow(2, n, button_table_row2);
    }



#ifdef BUTTON_FONT
    overlayButtons();
#endif

    return;
}


void
initButtonRow (int row, int tablesize, mouse_button_table *tp)
{

    S_window *oldwin;
    Scols  oldcol;
    Slines oldlin;

    oldwin = curwin;        /* save old window info   */
    oldcol = cursorcol;
    oldlin = cursorline;

//  struct button_table *tp;
//  unsigned int i, tablesize;
    int i;
    char *s;
    int row_y;

    if (row == 1) {
//      tablesize = sizeof(button_table_row1) / sizeof(button_table_row1[0]);
//      tp = &button_table_row1[0];
	row_y = buttonwin.btext - 2;
    }
    else {
//      tablesize = sizeof(button_table_row2) / sizeof(button_table_row2[0]);
//      tp = &button_table_row2[0];
	row_y = buttonwin.btext;
    }


    int x_next = 0;
    int label_widths = 0;
    int n_labels, w, totw;
    int nspaces = 1;  /* default space between labels */
    int marg = 4;     /* 2 spaces before/after each row */
    int indent = 0;   /* center labels */
    int npairs = 0;
    int nslots = 0;   /* where spaces separate labels, excludes label pairs */

    for (i=0; i < tablesize; i++) {
	label_widths += (int) strlen((tp+i)->label);
	if ((tp+i)->label[0] == '/')
	    npairs++;
	/* Make note if we have the [record] button so we don't
	 * treat a mouse click as an error in RecordChar(),
	 * allowing [record] to be used as a toggle
	 */
	if ((tp+i)->ecmd == CCRECORD)
	    have_record_button = 1;
    }
    n_labels = (int) i;
    nslots = n_labels - npairs;


    /* if there's room, separate labels with 4 spaces */
    if (label_widths + (4 * (nslots-1)) + marg <= (int) term.tt_width) {
	nspaces = 4;
    }
    /* if there's room, separate labels with 3 spaces */
    else if (label_widths + (3 * (nslots-1)) + marg <= (int) term.tt_width) {
	nspaces = 3;
    }
    /* if there's room, separate labels with 2 spaces */
    else if (label_widths + (2 * (nslots-1)) <= (int) term.tt_width) {
	nspaces = 2;
    }
    totw = label_widths + (nspaces * (nslots-1));
    indent = ((int)term.tt_width - totw) / 2;

/** /
dbgpr("row %d: n_labels=%d nslots=%d label_widths=%d indent=%d\n",
    row, n_labels, nslots, label_widths, indent);
/ **/

    tp->begx = indent;

    switchwindow (&buttonwin);
    for (i=0; i < tablesize; i++) {

	w = strlen((tp+i)->label);

	if ((tp+i)->begx + w > (int)term.tt_width)
	    break;

	poscursor((tp+i)->begx, row_y);
	for (s = (tp+i)->label; *s; s++) {
	    putch((Uchar)*s, NO);
	}

	(tp+i)->endx = (tp+i)->begx + w;
	x_next = (tp+i)->endx;

	if (i+1 == tablesize) { /* just finished last entry */
/** /
dbgpr("buttoninit: i=%d beg=%d end=%d label=%s\n",
    i, (tp+i)->begx, (tp+i)->endx, (tp+i)->label);
/ **/
	    break;
	}

	if ((tp+i+1)->label[0] != '/' ) {
	    x_next += nspaces;  /* no space between pairs, eg, +L/-L */
	}
	(tp+i+1)->begx = x_next;
/** /
dbgpr("buttoninit: i=%d beg=%d end=%d label=%s\n",
    i, (tp+i)->begx, (tp+i)->endx, (tp+i)->label);
/ **/

    }

#ifdef OUT
/* add . temp to end row */
poscursor(cursorcol+1, cursorline);
putch('.', NO);
#endif /* OUT */

    fflush(stdout);

    switchwindow (oldwin);  /* restore old window info */
    poscursor (oldcol, oldlin);

#ifdef BUTTON_FONT
    makeButtonOverlayB(row, nspaces, indent);
#endif /* BUTTON_FONT */



    fflush(stdout);

    return;
}


/*
 *   visually show a button press (try...)
 */


/* i indexes a button_table_rowX[] entry, in the specified row */
void
HiLightButton(int i, int row) {


    S_window *oldwin;
    Scols  oldcol;
    Slines oldlin;

    struct button_table *tp;
    int row_y;

    oldwin = curwin;        /* save old window info   */
    oldcol = cursorcol;
    oldlin = cursorline;

    if (row == 1) {
	if (useMyButtons) {
	    tp = &my_button_row1[0];
	}
	else {  /* default buttons */
	    tp = &button_table_row1[0];
	}
	row_y = buttonwin.bmarg - 2;
    }
    else {
	if (useMyButtons) {
	    tp = &my_button_row2[0];
	}
	else {
	    tp = &button_table_row2[0];
	}
	row_y = buttonwin.bmarg;
    }

/** /
dbgpr("HiLightButton: i=%d row=%d row_y=%d begx=%d\n",
  i, row, row_y, tp[i].begx);
/ **/
    /* try to simulate a button click */
    switchwindow (&buttonwin);

    curs_set(0);    /* cursor invisible */

    mvcur(-1, -1, row_y, tp[i].begx);
/*  tputs(hilite_str, 1, Pch); */   /* bg highlight */
    tputs(smso, 1, Pch);
    fputs(tp[i].label, stdout);
/*  tputs(sgr0, 1, Pch); */   /* end hilite mode */
    tputs(rmso, 1, Pch);      /* end standout mode */
    fflush(stdout);

    /*usleep(100000);*/   /* .1 seconds */
    napms(75);     /* .1 seconds */

    mvcur(-1, -1, row_y, tp[i].begx);
    fputs(tp[i].label, stdout);
    fflush(stdout);

    curs_set(1);    /* cursor visible */

#ifdef BUTTON_FONT
    overlayButtons();
#endif /* BUTTON_FONT */

    switchwindow (oldwin);  /* restore old window info */

    /*   When invoked this way, some commands need an assist to restore
     *   the cursor position
     */
    switch (tp[i].ecmd) {
	case CCDELCH:
	case CCERASE:
	case CCMARK:
	case CCUNMARK:
	case CCPICK:
	case CCPUT:
	case CCPLPAGE:
	case CCMIPAGE:
	case CCPLLINE:
	case CCMILINE:
	case CCRWORD:
	case CCLWORD:
	case CCRWINDOW:
	case CCLWINDOW:
	case CCMOVEUP:
	case CCMOVEDOWN:
	case CCCAPS:
	case CCCCASE:
	case CCSPLIT:
	case CCJOIN:
	case CCEXIT:
	case CCREGEX:
	case CCPLWIN:
	case CCMIWIN:
	case CCCLOSE:
	case CCMICLOSE:
	case CCMIERASE:
	case CCREPLACE:
	case CCRECORD:
	case CCPLAY:
	case CCDWORD:
	case CCCTRLQUOTE:
	case CCMOUSEONOFF:
	case CCAUTOFILL:
	    mvcur(-1, -1, curwin->ttext+cursorline, curwin->ltext+cursorcol);
	    cursorcol = cursorline = -1;
	    fflush(stdout);
	default:
	    break;
    }

    poscursor (oldcol, oldlin);
    d_put(0);

    return;
}



void
getMouseButtonEvent()
{
    MEVENT event, *ev;
    ev = &event;

    mouseFuncKey = -1;  /* check in mainloop(); and param() */

    if (getmouse(&event) != OK) {
	dbgpr("--getmouse() error\n");
	return;
    }

/** /dbgpr("getMouseButtonEvent:  %s\n", mouse_decode(&event)); / **/

    if (! (event.bstate & BUTTON1_PRESSED)) {
	dbgpr("getMouseButtonEvent:  not button1 pressed\n");
	return;
    }

/** /
    dbgpr("getMouseButtonEvent:  ev->y=%d ev->x=%d bwin->bmarg=%d bwin->btext=%d, cmdmode=%d\n",
    ev->y,  ev->x,  buttonwin.bmarg, buttonwin.btext, cmdmode);
    dbgpr("  bstate=(0x%08lx)\n", (long)ev->bstate);
/ **/

/*  if (!cmdmode || optskipbuttons == YES) */
    if (optshowbuttons == NO)
	return;

#ifdef USE_MOUSE_POSITION
    /* get the release event */
    int c;
    while(1) {
	c = wgetch(stdscr);
	if (c == ERR || c != KEY_MOUSE)
	    return;
	getmouse(&event);
	if (event.bstate & BUTTON1_RELEASED || event.bstate & BUTTON4_RELEASED)
	    break;
    }

#ifdef OUT
    if (!(event.bstate & BUTTON2_RELEASED)) {
   /** /dbgpr("getMouseButtonEvent, wait for rel-2\n"); / **/
	return;
    }
    /* else, we see clicks */
#endif

#endif

    /*
     * Save the mouse event info in the keystroke file.
     */

    /* Write {CCMOUSE}{y}{x} to keys file, and flushkeys */
    fprintf(keyfile, "%c%03d%03d", CCMOUSE, ev->y, ev->x);
    fflush(keyfile);

    /* was a function button "key" pressed,  */
    if( ev->y == buttonwin.bmarg || ev->y == buttonwin.bmarg - 2 ) {
  /** / dbgpr("getMouseButtonEvent:  release in button window at (%d, %d)\n", ev->y, ev->x); / **/
	getMouseFuncKey(ev->x, ev->y);  /* sets global mouseFuncKey */
    }
    return;
}

#endif /* MOUSE_BUTTONS */



#ifdef USE_MOUSE_POSITION
int
doMotion(MEVENT *ev, int begy, int begx)
{
    char adj_cursor = 0;

/** /
dbgpr("doMotion y:  beg=(%d,%d) end=(%d,%d) cursorline=%d cursorcol=%d \
tmarg=%d, ttext=%d, bmarg=%d, btext=%d\n",
  begy, begx, ev->y, ev->x, cursorline, cursorcol,
  curwin->tmarg, curwin->ttext, curwin->bmarg, curwin->btext);
/ **/

/** /
dbgpr("doMotion x:  beg=(%d,%d) end=(%d,%d) cursorline=%d cursorcol=%d \
lmarg=%d, ltext=%d, rmarg=%d, rtext=%d\n",
  begy, begx, ev->y, ev->x, cursorline, cursorcol,
  curwin->lmarg, curwin->ltext, curwin->rmarg, curwin->rtext);
/ **/

    /* don't allow motion outside of curwin's boundaries */
    if (ev->y <= curwin->tmarg) {
	ev->y = curwin->ttext;
	adj_cursor = 1;
    }
    if (ev->y >= curwin->bmarg) {
	/*ev->y = curwin->btext; */
	ev->y = curwin->bmarg - 1;
	adj_cursor = 1;
    }
    if (ev->x <= curwin->ltext) {
	ev->x = curwin->ltext;
	adj_cursor = 1;
    }
#ifdef OUT
    /* this misses the last char on the line */
    if (ev->x >= curwin->rtext) {
	ev->x = curwin->rtext;
	adj_cursor = 1;
    }
#endif /* OUT */
    if (ev->x >= curwin->rmarg) {
	ev->x = curwin->rmarg-1;
	adj_cursor = 1;
    }
    if (adj_cursor) {
	mvcur(-1,-1, ev->y, ev->x);
    /*  dbgpr("  adj_cursor to (%d,%d)\n", ev->y, ev->x); */
    }


/** /
getyx(stdscr, y, x);
    dbgpr("doMotion:  mid=(%d,%d) end=(%d,%d) cursorline=%d cursorcol=%d, getyx=(%d,%d)\n",
      begy, begx, ev->y, ev->x, cursorline, cursorcol, y,x);
/ **/

    /* In E, marking to the right excludes cursor position,
     * and includes it marking left
     */

    int ncols;

    if (ev->x >= begx) {
	/** /
	dbgpr(" ev->x >= begx, calling poscursor(%d,%d) tmarg=%d\n",
	    ev->x - curwin->ltext, ev->y - curwin->tmarg, curwin->tmarg);
	/ **/
	poscursor(ev->x - curwin->ltext, ev->y - curwin->ttext);
    }
    else {
	/** /
	dbgpr(" ev->x < begx, calling poscursor(%d,%d) ttext=%d\n",
	    ev->x - curwin->ltext -1 , ev->y - curwin->tmarg, curwin->ttext);
	/ **/
/*      poscursor(ev->x - curwin->ltext - 1, ev->y - curwin->ttext); */
	poscursor(ev->x - curwin->ltext, ev->y - curwin->ttext);
    }

    doDragHiLite(begy, ev->y, begx, ev->x);


    /* update info display of marked area:  eg, 1x2 */

    char buf[32];
    int nlines;

    nlines = abs(ev->y - begy) + 1;

    if (ev->x == begx) {  /* don't have a rect */
	ncols = 0;
	sprintf(buf, "%d", nlines);
    }
    else {
	int nc = (ev->x > begx) ? ev->x - begx : begx - ev->x;
	ncols = 1 + nc - curwin->ltext;
	sprintf(buf, "%dx%d", nlines, ncols);
    }

    /* pad with spaces, length of inf_area is 9 */
    char ibuf[10];
    int i;

    strncpy(ibuf, buf, 9);
    int len = strlen(ibuf);

    for (i=len; i<9; i++) {
	ibuf[i] = ' ';
    }
    ibuf[9] = '\0';

/*  snprintf(ibuf, 9, "%-9s", buf); */


#ifdef OUT
    info (inf_area, max(strlen(buf), (size_t)infoarealen), buf);
    infoarealen = strlen(buf);  /* a global set by info() routine */
    d_put(0);
    fflush(stdout);
#endif

    curs_set(0);    /* cursor invisible */

    mvcur(-1,-1, infowin.tmarg, inf_area);
    tputs(ibuf, 1, Pch);

    /* mv cursor up 1 line, where E expects it to be */
    mvcur(-1, -1, ev->y, ev->x);
    fflush(stdout);
    curs_set(1);    /* cursor visible */

/** /
    dbgpr("  curmark:  mrkwincol=%d, mrkwinlin=%d, mrkcol=%d, mrklin=%d\n",
	curmark->mrkwincol, curmark->mrkwinlin, curmark->mrkcol, curmark->mrklin);
    dbgpr("doMotion,end: cursorline=%d cursorcol=%d, info_area=(%s) len=%d\n\n",
	cursorline, cursorcol, buf, infoarealen);
/ **/

    return 0;
}

int
endMotion(int begy, int begx)
{

    /* E's (0,0) is curses (1,1) */
    curmark->mrklin = begy - curwin->ttext;
    curmark->mrkcol = begx - curwin->ltext;

/** /
if (DebugVal) {
dbgpr("endMotion:  start=(%d,%d) end=(%d,%d)\n", begy, begx, ev->y, ev->x);
dbgpr("  curmark:  mrkwincol=%d, mrkwinlin=%d, mrkcol=%d, mrklin=%d curwksp->wlin=%d\n",
 curmark->mrkwincol, curmark->mrkwinlin, curmark->mrkcol,
curmark->mrklin,curwksp->wlin);
dbgpr("endMotion,end: cursorline=%d cursorcol=%d, ltext=%d ttext=%d\n\n",
    cursorline, cursorcol, curwin->ltext, curwin->ttext);
}
/ **/

    return 0;
}


/*
 *   mark a portion of a line
 */

void
doDragHiLite(int begy, int endy, int begx, int endx)
{

    int w = term.tt_width;
    char buf[w+1];
    char *cp;

    /* todo, improve on these border cases */
    if (begy == 0) begy = 1;
    if (endy == 0) endy = 1;
/*
    if (begy >= curwin->bmarg) begy = curwin->btext;
    if (endy >= curwin->bmarg) endy = curwin->btext;
*/
    if (begy >= curwin->bmarg) begy = curwin->bmarg - 1;
    if (endy >= curwin->bmarg) endy = curwin->bmarg - 1;

    int beg_mark;
    int mark_len = abs(endx - begx);  /* how many chars to highlight */
    int line;
    int from, to;

    if (begx > endx) {
	beg_mark = endx;
    }
    else {
	beg_mark = begx;
    }

    if (begy > endy) {
	from = endy;
	to = begy;
    }
    else {
	from = begy;
	to = endy;
    }

    /* clear the previously marked area */

    int y0, y1;
    y0 = y1 = 0;

    calcRange2Clear(from, to, &y0, &y1);

/** /
dbgpr("clear (%d to %d) from=%d to=%d from_topline=%d from_botline=%d\n",
y0, y1, from, to, from_topline, from_botline);
/ **/

    /* clear lines previously marked */
    int i;
    for (i = y0; i <= y1; i++)
       HiLightLine(i, NO);


    /*  use curwin->lmarg and curwin->rtext to account for any windows,
     *  omit left/rt border chars
     */
    int wid = curwin->rmarg - curwin->lmarg;
    /* beg_mark += curwin->ltext; */


    /* marked area lies partly to right of our window */
/*  if (beg_mark + mark_len > curwin->rtext) { */
    if (beg_mark + mark_len > curwin->rmarg) {
	mark_len = curwin->rtext - beg_mark;
	if (mark_len <= 0 )
	    mark_len = 1;
    /** /dbgpr("6: mark_len reduced to %d from %d\n", mark_len, markcols); / **/
    }

/** /
dbgpr("doHiLiRect: mrkwincol=%d wksp->wcol=%d mrkcol=%d curwin->ltext=%d\n",
  curmark->mrkwincol, curwksp->wcol, curmark->mrkcol, curwin->ltext);

dbgpr("beg_mark=%d mark_len=%d cursorcol=%d,cursorline=%d from=%d to=%d miny=%d maxy=%d\n\n",
beg_mark, mark_len, cursorcol, cursorline, from, to, miny, maxy);
/ **/

    from_topline = from;
    from_botline = to;

    /* clear previous marks */
/** not working correctly
    for (line = miny; line <= maxy; line++ ) {
	cp = (char *)image + w*line;

	mvcur(-1,-1, line, curwin->ltext);
	snprintf(buf, (size_t) wid, "%s", cp+curwin->ltext);
	puts(buf);
    }
**/

    /* highlight full lines */
    if (begx == endx) {
	for (line = from; line <= to; line++ ) {
	    cp = (char *)image + w*line;
	    mvcur(-1,-1, line, curwin->ltext);
	    snprintf(buf, (size_t) wid, "%s", cp+curwin->ltext);
	    tputs(hilite_str, 1, Pch);
	    puts(buf);
	    tputs(sgr0, 1, Pch);
	}
	return;
    }


    /* highlight rect area */
    for (line = from; line <= to; line++ ) {
	cp = (char *)image + w*line;

	/* clear any full line highlighting */
	mvcur(-1,-1, line, curwin->ltext);
	snprintf(buf, (size_t) wid, "%s", cp+curwin->ltext);
	puts(buf);

	/* get a copy of the marked area */
	snprintf(buf, (size_t) mark_len+1, "%s", cp + beg_mark );
	mvcur(-1,-1, line, beg_mark);
	tputs(hilite_str, 1, Pch);      /* user selected mode, see cmd: set highlight */
	puts(buf);
	tputs(sgr0, 1, Pch);  /* end hilite mode */

	/* dbgpr("marked area=(%s)\n", buf); */
    }

    if (from < miny) miny = from;
    if (to > maxy)  maxy = to;

    fflush(stdout);

    return;
}
#endif /* USE_MOUSE_POSITION */

void
doChgWin(int wn, int y, int x)
{
    poscursor(x - winlist[wn]->ltext, y - winlist[wn]->ltext);
    d_put(0);
    chgwindow(wn);

    /* used in calcRange2Clear() */
    from_topline = curwin->ttext;
    from_botline = from_topline;

    /* check these ... */
    fprintf(keyfile, "%c%c%c", CCCMD, wn,CCCHWINDOW);
    fflush(keyfile);

}


/* determine range of previously highlighted area to clear */

void
calcRange2Clear(int from, int to, int *Fr, int *To)
{

/** /
dbgpr("calc:  from_topline=%d, from_botline=%d, w->ttext=%d, w->tmarg=%d\n",
  from_topline, from_botline, curwin->ttext, curwin->tmarg);
/ **/
    /*
     *  from = screen line of the start of the new highlight area
     *  end  = screen line of the end of the new highlight area
     *  from_topline = previous top of old highlight area (global var)
     *  from_botline = previous bottom of old highlight area (global var)
     */

    int y0, y1;
    y0 = y1 = 0;

    if (from == to) {       /* 1 (or 2) line(s) to clear */
     /* if (from > from_topline && from_topline > 0) { */
	if (from > from_topline) {
	    y0 = from_topline;  /* 2 lines to clear */
	}
	else {
	    y0 = from;  /* 1 line to clear */
	}

	if (to < from_botline) { /* 2 lines to clear */
	   y1 = from_botline;
	}
	else {
	   y1 = to;
	}
    }
    else {
	if (from < from_topline) {
	    y0 = from;
	}
	else {
	    y0 = from_topline;
	}

	if (to > from_botline) {
	    y1 = to;
	}
	else {
	    y1 = from_botline;
	}
    }

    *Fr = y0;
    *To = y1;

    return;
}

#ifdef BUTTON_FONT

static char button_line1[512];
static char button_line2[512];

void
overlayButtons()
{


    /* row 1 buttons */
    mvcur(-1, -1, buttonwin.bmarg - 2 , 0);
    tputs(button_line1, 1, Pch);
    fflush(stdout);

    /* row 2 buttons */
    mvcur(-1, -1, buttonwin.bmarg, 0);
    tputs(button_line2, 1, Pch);
    fflush(stdout);

    int c = cursorcol;
    int l = cursorline;
    cursorcol = cursorline = -1;
    poscursor(c, l);
    d_put(0);

/** /
dbgpr("row1:%s\n", button_line1);
dbgpr("row2:%s\n", button_line2);
dbgpr("overlayButton, return\n");
/ **/

    return;
}

/* highlight button text, for overlay
 */
void
makeButtonOverlayB (int row, int spaces, int indent)
{

    char *s;
    char *font_on, *font_off;

    /* add bold */
    char fontbuf[128];
    snprintf(fontbuf, sizeof(fontbuf), "%s\033[1m", button_font);

    font_on = fontbuf;
    font_off = sgr0;

    size_t /*int*/ len_on  = strlen(font_on);
    size_t /*int*/ len_off = strlen(font_off);

    struct button_table *tp;
    int i, tablesize;

/*dbg*/
/** /
char *cp_orig, *b_line;
int wid = term.tt_width;
/ **/

    if (row == 1) {
	if (useMyButtons) {
	    tablesize = my_row1_cnt;
	    tp = &my_button_row1[0];
	}
	else {
	    tablesize = sizeof(button_table_row1) / sizeof(button_table_row1[0]);
	    tp = &button_table_row1[0];
	}
	s = button_line1;
/** /   cp_orig = (char *)image + wid * (buttonwin.bmarg-2);  / * dbg */
    }
    else {
	if (useMyButtons) {
	    tablesize = my_row2_cnt;
	    tp = &my_button_row2[0];
	}
	else {
	    tablesize = sizeof(button_table_row2) / sizeof(button_table_row2[0]);
	    tp = &button_table_row2[0];
	}
	s = button_line2;
/** /   cp_orig = (char *)image + wid * buttonwin.bmarg;  / * dbg */
    }
/* b_line = s; */
    char *gap;
    int ind=0;
    size_t len_gap;

    /* between each label:  <label><font_off><sp><sp><font_on><label>*/
    if (spaces == 2) {
	gap = "  ";
	ind = indent;
    }
    /* between each label:  <label><sp><font_off><sp><font_on><sp><label> */
    else if (spaces == 3) {
	gap = " ";
	ind = indent - 1;
    }
    /* between each label:  <label><sp><font_off><sp><sp><font_on><sp><label> */
    else if (spaces == 4) {
	gap = "  ";
	ind = indent - 1;
    }
    len_gap = strlen(gap);

    /* copy initial indent */
    int a;
    for (a=0; a<ind; a++) {
	*s++ = ' ';
    }

/** /
dbgpr("makeB:  row=%d indent=%d ind=%d spaces=%d gap=(%s) len_gap=%d\n",
    row, indent, ind, spaces, gap, len_gap);
/ **/

    size_t /*int*/ len;
    for (i=0; i < tablesize; i++) {

	/* add font_on */
	strncpy(s, font_on, len_on);
	s += len_on;

	if (spaces > 2) *s++ = ' ';

	len = strlen((tp+i)->label);
	strncpy(s, (tp+i)->label, len);
	s += len;

	if (i+1 < tablesize ) {
	    /* eg, want +Pg/-Pg to appear together */
	    if ((tp+i+1)->label[0] == '/' ) {
		i++;
		len = strlen((tp+i)->label);
		strncpy(s, (tp+i)->label, len);
		s += len;
	    }
	}

	if (spaces > 2) *s++ = ' ';

	/* add font_off */
	strncpy(s, font_off, len_off);
	s += len_off;

	/* add gap if not last label */
	if (i+1 < tablesize) {
	    /*dbgpr("--adding gap=(%s) len=%d\n", gap, len_gap);*/
	    strncpy (s, gap, len_gap);
	    s += len_gap;
	}

    }

/*  sprintf(s, "%s", font_off); */

/** /
dbgpr("makeB: indent=%d spaces=%d len_gap=%d\n",
  indent, spaces, len_gap);
cp_orig[wid-1] = '\0';
dbgpr("row%d:(%s)\n",row, cp_orig);
dbgpr("row%d:(%s)\n", row, b_line);
/ **/

    return;
}

#endif /* BUTTON_FONT */


/*  my_lookup differs from lookup() (in e.pa.c)
 *  in that we don't care if the S_lookup table
 *  is sorted or not (it's not that long), and case is lower.
 *  return -1 on error, ecmd if found
 */

int
my_lookup( char *label, S_looktbl *table)
{
    S_looktbl *ptr;

    for (ptr = table; ptr->str != 0; ptr++) {
	if (strcmp(label, ptr->str) == 0) {
	    return ptr->val;
	}
    }
    return -1;
}


/* use this buffer to more easily pass error messages to getout()
 */
extern char error_buf[];

/* populate a mouse_button_table row */

void
add_mybutton(int cnt, char *label, int fcode, int row, mouse_button_table *table)
{
    mouse_button_table *row_ptr;

    if (cnt >= MAX_BUTTONS) {
	sprintf(error_buf, "%d is too many buttons for row %d\n",
	     cnt, row);
	return;
    }

    row_ptr = table;

    row_ptr[cnt].ecmd = fcode;
    row_ptr[cnt].label = strdup(label);
    row_ptr[cnt].bnum = cnt;    /* field not used */
    row_ptr[cnt].begx = 0;
    row_ptr[cnt].endx = 0;

    return;
}

/* read the specified -buttonfile=filename and
 * populate the button labels and corresponding e function
 * codes.  See ../examples/ for e_button samples.
 *
 */

int
get_mybuttons(char *filename)
{
    FILE *fp;
    char label[32], ecmd[32];

//  dbgpr("get_mybuttons:  filename=%s\n", filename);

    if ((fp = fopen(filename, "r")) == NULL) {
	sprintf(error_buf, "can't open the button file:  %s\n", filename);
	dbgpr("get_mybuttons:  can't open %s\n", filename);
	return -1;
    }

    char buf[256];
    int row = 1;
    int cmdcode;
    int lineno = 0;
    int row1_width = 0; /* sum of button labels */
    int row2_width = 0;
    int row1_slashes = 0;   /* sum of labels that start with '/' */
    int row2_slashes = 0;

    my_row1_cnt = my_row2_cnt = 0;

    while (fgets(buf, sizeof(buf), fp)) {

	if (feof(fp)) break;

	lineno++;

	if (*buf == '\n' || *buf == '\0') continue;
	if (*buf == '#' || *buf == ' ') continue;

	if (sscanf(buf, "-ROW%d", &row) == 1) {
	    /* dbgpr("ROW is %d\n", row); */
	    if (row < 1 || row > 2) {
		sprintf(error_buf, "buttonfile error: -ROW%d must be -ROW1 or -ROW2: %s\n", row, buf);
	    /*  dbgpr("row must be 1 or 2\n", row); */
		return -1;
	    }
	    continue;
	}

	/* eg:  "insert":<insmd>     # toggle insert mode */
	if (sscanf(buf, "\"%[^:\"]\":<%[^>]", label, ecmd) != 2) {
	    sprintf(error_buf, "buttonfile  error: line %d format error: %s\n", lineno, buf);
	/*  dbgpr("line %d format error: %s\n", lineno, buf); */
	    return -1;
	}

	cmdcode = my_lookup(ecmd, keysyms);
	if (cmdcode < 0) {
	    sprintf(error_buf, "buttonfile error:  <%s> not recognized as an e command\n", ecmd);
	/*  dbgpr("<%s> not recognized as an e command\n", ecmd); */
	    return -1;
	}

	/** /
	dbgpr("get_mybuttons: label=%s ecmd=%s cmdcode=(%d, %04o)\n",
	  label, ecmd, cmdcode, cmdcode);
	/ **/

	if (row == 1) {
	    add_mybutton(my_row1_cnt, label, cmdcode, row, my_button_row1);
	    my_row1_cnt++;
	    row1_width += (int) strlen(label);
	    if( *label == '/' ) {
		row1_slashes++;
	    }
	}
	else {
	    add_mybutton(my_row2_cnt, label, cmdcode, row, my_button_row2);
	    my_row2_cnt++;
	    row2_width += (int) strlen(label);
	    if( *label == '/' ) {
		row2_slashes++;
	    }
	}
    }

    if (my_row1_cnt == 0 || my_row2_cnt == 0) {
	sprintf(error_buf, "buttonfile error: \"-ROW2\" keyword not found.\n");
    /*  dbgpr("Row2: button %d label=%s func=%04o\n"); */
	return -1;
    }

/** /
dbgpr("row1_width=%d r1_slashes=%d r1_cnt=%d COLS=%d\n",
    row1_width, row1_slashes, my_row1_cnt, COLS);
dbgpr("row2_width=%d r2_slashes=%d r2_cnt=%d COLS=%d\n",
    row2_width, row2_slashes, my_row2_cnt, COLS);
/ **/

    /* width of button rows must fit on screen */
    int w;
    w = row1_width - (row1_slashes*2) + (my_row1_cnt*2);
    if (w > COLS) {
	sprintf(error_buf, "buttonfile error:  row1 width (%d) exceeds screen width (%d).\n",
	    w, COLS);
    /*  dbgpr("row1 width (%d) exceeds screen width (%d)\n", w, COLS); */
	return -1;
    }
    w = row2_width - (row2_slashes*2) + (my_row2_cnt*2);
    if (w > COLS) {
	sprintf(error_buf, "buttonfile error:  row2 width (%d) exceeds screen width (%d).\n",
	    w, COLS);
    /*  dbgpr("row2 width (%d) exceeds screen width (%d)\n", w, COLS); */
	return -1;
    }


#ifdef OUT  /* debug */
    int i;
    for (i=0; i < my_row1_cnt; i++) {
	dbgpr("Row1: button %d label=%s func=%04o\n",
	    i, my_button_row1[i].label, my_button_row1[i].ecmd);

    }
    for (i=0; i < my_row2_cnt; i++) {
	dbgpr("Row2: button %d label=%s func=%04o\n",
	    i, my_button_row2[i].label, my_button_row2[i].ecmd);

    }
#endif /* OUT */

    useMyButtons = YES;

    return 1;
}

#endif /* NCURSES */
