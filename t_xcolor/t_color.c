#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <ctype.h>

#define CTRL(x) ((x) & 0x1f)

WINDOW *w_info, *mk_win, *w_howto;
void doMouse(), postmove(), premove(), docmd(), ShowMsg(), showhowto();
void addtext();
void ShowColors();
void ShowColors2();

char msgbuf[128];

/* try the
chgat() functions
*/

/*
attr_t b_yellow = COLOR_PAIR(COLOR_YELLOW)|A_BOLD;
*/

/*int COLOR_PAIR (int PAIR_NUMBER); */

int
main()
{
    int max_w, max_h;
    int c, i;

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE); /* want *single* value of mouse/function key pressed */

    /* Keep it simple for now */
    /*mousemask(BUTTON1_PRESSED|BUTTON1_RELEASED|BUTTON1_CLICKED, NULL); */
    mousemask(ALL_MOUSE_EVENTS, NULL);

    start_color();

    int ncolors = COLORS;
    int npairs = COLOR_PAIRS;


    int r,g,b;

    NCURSES_COLOR_T fg, bg;
    (void) pair_content(0, &fg, &bg);
    printw("pair 0 contains (%d,%d), ncolors=%d npairs=%d\n", (int) fg, (int) bg, ncolors, npairs);
    fflush(stdout);

    init_pair(1, COLOR_BLACK, COLOR_WHITE);  /* pair number,  fg,  bg  */
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_BLACK, COLOR_CYAN);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_WHITE, COLOR_CYAN);
    init_pair(9, COLOR_BLUE,COLOR_WHITE);
    init_pair(0xFF, COLOR_BLACK, COLORS > 15 ? 15 : COLOR_WHITE);
    /*init_pair(9, COLOR_BLUE, COLOR_CYAN);*/

    wmove(stdscr,1,0);

    int rc, rc1;

    /*rc = init_color(14, 170, 0, 170);*/
/*  rc = init_color(14, 0xf5, 0xf5, 0xdc); */     /* beige */
/*  rc = init_color(14, 1000,1000,0); */
    rc = init_color(14, 915,793,335);
    rc1 = init_pair(6, COLOR_BLACK, 14);
    printw(" rc=%d after init_color(14, beige)\n", rc);
    printw("rc1=%d after init_pair(6, BLACK, 14)\n", rc);


/*
    rc = init_color(14, 1000,1000,0);
    init_pair(6, COLOR_BLACK, 14);
    printw("rc=%d after init_color(9, 1000,1000,0)\n", rc);
*/
    wrefresh(stdscr);
    sleep(5);


    wbkgd(stdscr, COLOR_PAIR(1));

  /*wbkgd(stdscr, COLOR_PAIR(0xFF));*/

    /* returns the win dimensions, could also use LINES and COLS */
    getmaxyx(stdscr, max_h, max_w);

    /* windows */

    w_info  = newwin(1, max_w, max_h-4, 1);
    w_howto = newwin(1, max_w, max_h-1, 1);
    wbkgd(w_info, COLOR_PAIR(2));
    wbkgd(w_howto, COLOR_PAIR(3));

    keypad(w_info, TRUE); /* trw: returns *single* value of key pressed!!! */
    showhowto();
    int cnt = 0;

    while (1) {
	mvwprintw(w_info, 0, 1, "cmd:");
	wrefresh(w_info);
	c = wgetch(w_info);
	if( c == KEY_MOUSE ) {
	    doMouse(c);
	    continue;
	}
	if (c == 'q' || c == EOF)
	  break;
	mvwprintw(w_info, 0, 10, "got (%o)(%c), cnt %d\n", c, (char)c, cnt++ );
	wrefresh(w_info);
	docmd(c);
    }
    endwin();
}

void
docmd(int c) {

    int rc;
    int y0 = 15;
    int x0 = 15;
    int y, x, ymax, xmax;

    if (c == 'c') {  /* create window */
	mk_win = newwin(10, 10, y0, x0);
	wcolor_set(mk_win, 0, NULL);
	wbkgd(mk_win, COLOR_PAIR(2));
	box(mk_win, ' ', ' ');
	{ int i;
	  for (i=0; i<10; i++) {
	    mvwprintw(mk_win, i, 0, "1234567890");
	  }
	}
	wrefresh(mk_win);
	keypad(mk_win, TRUE);
	ShowMsg("mk_win window created");
	/*addtext();*/
    }
    else if (c == '1') {  /* test text fg/bg color */
	ShowColors (msgbuf);
    }
    else if (c == '2') {  /* test text fg/bg color */
	ShowColors2 ();
    }
    else if (c == 'x') {  /* delete window */
	wclear(mk_win);
	rc = delwin(mk_win);
	wmove(stdscr, 10, 10);
	wclear(stdscr);
	wrefresh(stdscr);
	showhowto();
	sprintf(msgbuf, "rc=(%d)  window deleted", rc);
	ShowMsg (msgbuf);
    }
    else if (c == 'l' || c == KEY_LEFT) {  /* move window left */
	premove();

	getbegyx(mk_win, y, x);
	x--;
	mvwin(mk_win, y, x);

	postmove();

	sprintf(msgbuf, "mv left to (%d,%d)", y,x);
	ShowMsg(msgbuf);
    }
    else if (c == 'r' || c == KEY_RIGHT) {  /* move window right */
	premove();

	getbegyx(mk_win, y, x);
	x++;
	mvwin(mk_win, y, x);

	postmove();

	sprintf(msgbuf, "mv right to (%d,%d)", y,x);
	ShowMsg(msgbuf);
    }
    else if (c == 'u' || c == KEY_UP) {  /* move window up */
	premove();

	getbegyx(mk_win, y, x);
	y--;
	mvwin(mk_win, y,x);

	postmove();

	sprintf(msgbuf, "mv up to (%d,%d)", y,x);
	ShowMsg(msgbuf);
    }
    else if (c == 'd' || c == KEY_DOWN) {  /* move window down */
	premove();

	getbegyx(mk_win, y, x);
	y++;
	mvwin(mk_win, y,x);

	postmove();

	sprintf(msgbuf, "mv down to (%d,%d)", y,x);
	ShowMsg(msgbuf);
    }

    else if (c == 'U') {  /* grow window up */
	premove();

	getbegyx(mk_win, y, x);
	y--;    /* 1-line higher */
	mvwin(mk_win, y, x);
	getmaxyx(mk_win, ymax, xmax);
	ymax++; /* 1-line taller */
	rc = wresize(mk_win, ymax, xmax);

	wmove(mk_win, 0, 0);
	winsertln(mk_win);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow up size=(%d,%d), at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
    else if (c == 'D') {  /* grow window down */
	premove();

	getbegyx(mk_win, y, x);
	/*mvwin(mk_win, y, x);*/     /* stay in same starting position */
	getmaxyx(mk_win, ymax, xmax);
	ymax++; /* 1-line taller */
	rc = wresize(mk_win, ymax, xmax);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow down size=(%d,%d), at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
    else if (c == 'L') {  /* grow window left */
	premove();

	getbegyx(mk_win, y, x);
	x--;   /* start 1 left */
	mvwin(mk_win, y, x);
	getmaxyx(mk_win, ymax, xmax);
	xmax++; /* 1 wider */
	rc = wresize(mk_win, ymax, xmax);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow left (%d,%d) at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
    else if (c == 'R') {  /* grow window right */
	premove();

	getbegyx(mk_win, y, x);
	/*  x--; */  /* start 1 left */
	/*mvwin(mk_win, y, x);*/
	getmaxyx(mk_win, ymax, xmax);
	xmax++; /* 1 wider, same height */
	rc = wresize(mk_win, ymax, xmax);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow right (%d,%d) at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
    else if (c == CTRL('t')) {  /* shrink top  */
	premove();

	getbegyx(mk_win, y, x);
	y++;    /* move down 1-line */
	mvwin(mk_win, y, x);
	getmaxyx(mk_win, ymax, xmax);
	ymax--; /* 1-line smaller */
	rc = wresize(mk_win, ymax, xmax);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow up size=(%d,%d), at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
    else if (c == CTRL('b')) {  /* shrink bottom */
	premove();

	getbegyx(mk_win, y, x);
	/*y--; */   /* top stays at cur y */
	mvwin(mk_win, y, x);
	getmaxyx(mk_win, ymax, xmax);
	ymax--;    /* 1-line smaller */
	rc = wresize(mk_win, ymax, xmax);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow up size=(%d,%d), at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
    else if (c == CTRL('l')) {  /* shrink left */
	premove();

	getbegyx(mk_win, y, x);
	x++;    /* 1 char narrower */
	mvwin(mk_win, y, x);
	getmaxyx(mk_win, ymax, xmax);
	xmax--;    /* 1-line smaller */
	rc = wresize(mk_win, ymax, xmax);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow up size=(%d,%d), at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
    else if (c == CTRL('r')) {  /* shrink right */
	premove();

	getbegyx(mk_win, y, x);
	mvwin(mk_win, y, x);
	getmaxyx(mk_win, ymax, xmax);
	xmax--;    /* 1-char narrowerr */
	rc = wresize(mk_win, ymax, xmax);

	postmove();

	sprintf(msgbuf, "rc=(%d) mv grow up size=(%d,%d), at (%d,%d)", rc, ymax,xmax, y,x);
	ShowMsg(msgbuf);
    }
}

void
mybox(WINDOW *w) {
    box(w, ' ', ' ');
    return;
}

void
ShowMsg(char *msg) {

    wclear(w_info);
    mvwprintw(w_info, 0, 30, msg);
    /*wclrtoeol(w_info);*/
    wrefresh(w_info);
    showhowto();
    return;
}

void
postmove() {
    touchwin(stdscr);
    wrefresh(stdscr);
    /*mybox(mk_win);*/
    touchwin(mk_win);
    wrefresh(mk_win);
    return;
}

void
premove() {

    wrefresh(stdscr);
/*
    werase(mk_win);
    wclear(mk_win);
*/
    return;

}


void
showhowto() {

/*
    mvwprintw(w_howto, 0, 1,
      "Cmds: c/x/q (make/del win/quit);     l,r,u,d (move);  L,R,U,D (grow)   ^{l,r,t,b} (shrink)\n");
*/

    mvwprintw(w_howto, 0, 1, "Cmds: c/x/q (make/del win/quit)     ");

/*  wstandout(w_howto); */
/*  wcolor_set(w_howto, 2, NULL);*/
    wattron(w_howto, A_DIM);
    wprintw(w_howto, "l,r,u,d (move)    L,R,U,D (grow)    " );
    wattr_off(w_howto, A_DIM, NULL);
/*  wcolor_set(w_howto, 1, NULL);*/
/*  wstandend(w_howto);*/

    wprintw(w_howto, " ^{l,r,t,b} (shrink)\n");

    wrefresh(w_howto);
    return;
}


void
doMouse(int c) {

    MEVENT event, *ep;
    ep = &event;

    if( c != KEY_MOUSE ) {
       ShowMsg("key not mouse");
       return;
    }


    if(getmouse(&event) == OK) {
	sprintf(msgbuf, "mouse (y,x)=(%d,%d)", ep->y, ep->x);
	ShowMsg(msgbuf);
    }
    else {
	ShowMsg( "getmouse() error" );
    }
    return;
}



/* want this to be outside curses "windows" and on top */
void
addtext() {
    int i;

    wmove(stdscr,10,0);
    for (i=0; i<30; i++) {
	printf("Now is the time for all good men to come to the aid of their country\r\n");
    }
    return;
}

void
ShowColors() {
    attr_t wattr;
    int pair;
    int rc;
    rc = wattr_get(stdscr, &wattr, &pair, NULL);
    wmove(stdscr, 10,0);
    wprintw(stdscr, "wattr=%o, pair=%d\n", wattr, pair);

    wmove(stdscr, 30,0);
    wprintw(stdscr, "Now is the time for all good men\n");
/*  wattrset(stdscr, A_BOLD); */
    wcolor_set(stdscr, 6, NULL);
/*  wattrset(stdscr, COLOR_PAIR(9)); */
    wprintw(stdscr, "Now is the time for all good men\n");
    wprintw(stdscr, "Now is the time for all good men\n");
    wprintw(stdscr, "Now is the time for all good men\n");
    wattrset(stdscr, 0);
    wprintw(stdscr, "Now is the time for all good men\n");
    wrefresh(stdscr);
}

void
ShowColors2() {
    attr_t wattr;
    int pair;
    int rc;
    int i, n;


    wmove(stdscr,1,0);
    for(n=0; n<=7; n++) {
	for (i=0; i<=7; i++) {
	    init_pair(9, n, i);
	    wcolor_set(stdscr, 9, NULL);
	    wprintw(stdscr, "Hello pair=(%d,%d)\n",n,i);
	    wrefresh(stdscr);
	    wattrset(stdscr, A_NORMAL);    /* same as 0 */
	    sleep(2);
	}
    }
    wattrset(stdscr, 0);
}
