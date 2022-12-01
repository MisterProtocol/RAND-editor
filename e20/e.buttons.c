#ifdef COMMENT
--------
file e.buttons.c
    clickable button suport
#endif

#include "e.h"
#include "e.tt.h"
#include <ctype.h>
#include <string.h>


extern char *smso;       /* enter standout */
extern char *rmso;       /* exit standout */
extern char *sgr0;       /* reset all modes */
extern void getMouseFuncKey(int, int);
extern int mouseFuncKey;   /* set to an E key code for processing in e.m.c */

extern Flag optshowbuttons;
extern Flag cmdmode;
//extern char *highlight_info_str;
//extern Flag highlight_mode;

#ifdef MOUSE_BUTTONS
int inButtonRow (int);
void HiLightButton (int, int);
void getMouseButtonEvent (void);
void buttoninit (void);
Flag have_record_button = NO;

typedef struct button_table {
    short bnum;
    short begx;
    short endx;
    int   ecmd;
    char *label;
    char *cmd_str;
} mouse_button_table;

/* custom button definitions */
#define MAX_BUTTONS 20      /* per row */
mouse_button_table my_button_row1[MAX_BUTTONS];
mouse_button_table my_button_row2[MAX_BUTTONS];
int my_row1_cnt, my_row2_cnt;

void add_mybutton(int, char *, int, char *, int, mouse_button_table *);
int my_lookup (char *, S_looktbl *);
int get_mybuttons (char *);
extern S_looktbl keysyms[];
Flag useMyButtons = 0;  /* set when using custom buttons */

#ifdef BUTTON_FONT
void initButtonRow (int, int, mouse_button_table *);
void overlayButtons (void);
void makeButtonOverlayB (int, int, int);
extern char *button_font;
extern int btn_font_n;     /* -buttonfont=N (1-255) */
#endif /* BUTTON_FONT */

//#endif /* MOUSE_BUTTONS */


//#ifdef MOUSE_BUTTONS

int
inButtonRow (int y) {
    if (y == buttonwin.bmarg || y == buttonwin.bmarg - 2)
	return 1;
    return 0;
}


mouse_button_table button_table_row1[] = {
/*n   beg    end  ecmd       label */
{ 0,     1,   3,  CCCMD,         "cmd" ,NULL},

{ 1,     5,   7,  CCINSMODE,     "ins" ,NULL},

{ 2,     9,  12,  CCMARK,        "mark" ,NULL},
{ 3,    13,  16,  CCUNMARK,       "/-mk" ,NULL},

{ 4,    18,  21,  CCPLSRCH,      "+sch" ,NULL},
{ 5,    22,  26,  CCMISRCH,      "/-sch" ,NULL},

{ 6,    28,  31,  CCCAPS,        "caps"  ,NULL},

{ 7,    33,  37,  CCCCASE,       "ccase"  ,NULL},

{ 8,    39,  41,  CCSPLIT,       "spl" ,NULL},
{ 9,    42,  46,  CCJOIN,        "/join" ,NULL},

{10,    48,  51,  CCDELCH,       "delC" ,NULL},

/*
{11,    55,  56,  CCMOVEUP,      "up" ,NULL},
{12,    57,  59,  CCMOVEDOWN,    "/dn",NULL},
*/

{11,    53,  56,  CCPLWIN,       "+Win" ,NULL},
{12,    57,  59,  CCMIWIN,       "/-W",NULL},

{14,    61,  62,  CCREGEX,        "re",NULL},

{13,    64,  67,  CCREDRAW,       "redr",NULL},

};



mouse_button_table button_table_row2[] = {
/*n   beg    end  ecmd       label */
{ 1,     1,   3,  CCPLPAGE,      "+Pg" ,NULL},
{ 2,     4,   7,  CCMIPAGE,      "/-Pg" ,NULL},

{ 3,     9,  10,  CCPLLINE,      "+L"  ,NULL},
{ 4,    11,  13,  CCMILINE,      "/-L"  ,NULL},

{ 5,    15,  17,  CCRWORD,       "+wd"  ,NULL},
{ 6,    18,  21,  CCLWORD,       "/-wd"  ,NULL},

{ 7,    23,  24,  CCRWINDOW,     "wR"  ,NULL},
{ 8,    25,  27,  CCLWINDOW,     "/wL"  ,NULL},

{ 9,    29,  32,  CCOPEN,        "open" ,NULL},

{10,    34,  37,  CCCLOSE,       "close",NULL},
{11,    38,  41,  CCMICLOSE,     "/-cl",NULL},

{12,    43,  46,  CCPICK,        "pick" ,NULL},
{13,    47,  50,  CCPUT,         "/put"  ,NULL},

{14,    52,  56,  CCERASE,       "erase" ,NULL},
{15,    57,  60,  CCMIERASE,     "/-er" ,NULL},

{16,    62,  64,  CCBRACE,       "{}" ,NULL},

{17,    66,  69,  CCEXIT,        "exit",NULL},

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
       //  dbgpr(" match, k=%o cmdstr=(%s)\n", k, tp[i].cmd_str);
	   break;
	}
    }
    if (i < tablesize) {
	mouseFuncKey = k;
	HiLightButton(i, row);

	/* push keystrokes on input queue if we
	 * have a <cmd>string instead of a CCCODE
	 */
	if (k == CCNULL && tp[i].cmd_str != NULL) {
	    int j = (int) (strlen (tp[i].cmd_str) - 1);  //eg, <cmd>tag +1<ret>
	    while (j > 4) {     // stop before "<cmd>"
		ungetch( tp[i].cmd_str[j--] );
	    }
	    ungetch(CCCMD);
	}
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
//      row_y = buttonwin.btext - 2;
	row_y = buttonwin.bmarg - 2;
    }
    else {
//      tablesize = sizeof(button_table_row2) / sizeof(button_table_row2[0]);
//      tp = &button_table_row2[0];
//      row_y = buttonwin.btext;
	row_y = buttonwin.bmarg;
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
dbgpr("labels: row %d row_y=%d: n_labels=%d nslots=%d label_widths=%d indent=%d\n",
    row, row_y, n_labels, nslots, label_widths, indent);
/ **/

    tp->begx = (short)indent;

    switchwindow (&buttonwin);
    for (i=0; i < tablesize; i++) {

	w = (int)strlen((tp+i)->label);

	if ((tp+i)->begx + w > (int)term.tt_width)
	    break;

	poscursor((tp+i)->begx, row_y);
	for (s = (tp+i)->label; *s; s++) {
	    putch((Uchar)*s, NO);
	}

	(tp+i)->endx = (short) ((tp+i)->begx + w);
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
	(tp+i+1)->begx = (short)x_next;
/** /
dbgpr("buttoninit: i=%d beg=%d end=%d label=%s\n",
    i, (tp+i)->begx, (tp+i)->endx, (tp+i)->label);
/ **/

    }

#ifdef OUT
/* add . temp to end row to see where we are... */
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
add_mybutton(int cnt, char *label, int fcode, char *ecmds, int row, mouse_button_table *table)
{
    mouse_button_table *row_ptr;

    if (cnt >= MAX_BUTTONS) {
	sprintf(error_buf, "%d is too many buttons for row %d\n",
	     cnt, row);
	return;
    }

    row_ptr = table;

//dbgpr("add_mybutton:  fcode=(%d), ecmds=(%s)\n", fcode, ecmds);

    if (ecmds != NULL) {
	row_ptr[cnt].cmd_str = ecmds;    /* keystrokes or NULL */
	row_ptr[cnt].ecmd = CCNULL;
    }
    else {
	row_ptr[cnt].cmd_str = NULL;    /* keystrokes or NULL */
	row_ptr[cnt].ecmd = fcode;
    }

    row_ptr[cnt].label = strdup(label);
    row_ptr[cnt].bnum = (short) cnt;    /* field not used */
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

	/* The rhs can either contain a single e function (eg, "+srch":<+srch>)
	 * or a <cmd> line function:  "Tag+":<cmd>tag +1<ret>
	 */

	char *ecmd_str = NULL;
	cmdcode = -1;

	if (strstr(buf, "<cmd>")) {
	    if (sscanf(buf, "\"%[^:\"]\":%[^\n]", label, ecmd) != 2) {
	       sprintf(error_buf, "buttonfile  error: line %d format error: %s\n", lineno, buf);
	       dbgpr("line %d format error: %s\n", lineno, buf);
	       return -1;
	    }

	    /* change <ret> to \r */
	    char *s = strstr(ecmd, "<ret>");
	    if (s) {
		*s = '\r';  s[1] = '\0';
	    }
	    ecmd_str = malloc(strlen(ecmd)+1);
	    strcpy(ecmd_str, ecmd);
	    //dbgpr("  got <cmd> ecmd_str=(%s) ecmd=(%s)\n", ecmd_str, ecmd);
	}
	else {
	    /* eg:  "insert":<insmd>     toggle insert mode */
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
	}

	/** /
	dbgpr("get_mybuttons: label=%s ecmd=%s cmdcode=(%d, %04o)\n",
	  label, ecmd, cmdcode, cmdcode);
	/ **/

	if (row == 1) {
	    add_mybutton(my_row1_cnt, label, cmdcode, ecmd_str, row, my_button_row1);
	    my_row1_cnt++;
	    row1_width += (int) strlen(label);
	    if( *label == '/' ) {
		row1_slashes++;
	    }
	}
	else {
	    add_mybutton(my_row2_cnt, label, cmdcode, ecmd_str, row, my_button_row2);
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


#ifdef BUTTON_FONT

static char button_line1[512];
static char button_line2[512];

void
overlayButtons()
{
    //int h;


    /* can't use mvcur() as ncurses doesn't know about winsize change */
    /*mvcur(-1, -1, buttonwin.bmarg - 2 , 0); */

    /* row 1 buttons */
    //h = buttonwin.tmarg + 1;
    //(*term.tt_addr) (h, 0);
    mvcur(-1, -1, buttonwin.bmarg - 2 , 0);
    tputs(button_line1, 1, Pch);
    fflush(stdout);
    //dbgpr("overlayButtons:  row1 at %d ", h);

    /* row 2 buttons */
    //h += 2;
    //(*term.tt_addr) (h, 0);
    mvcur(-1, -1, buttonwin.bmarg, 0);
    tputs(button_line2, 1, Pch);
    fflush(stdout);
    //dbgpr("  row2 at %d\n", h);

    int c = cursorcol;
    int l = cursorline;
    cursorcol = cursorline = -1;
    poscursor(c, l);
    d_put(0);


#ifdef OUT
int i;
dbgpr("button_line1:\n");
for(i=0; i<75; i++) {
 c = button_line1[i];
 if (c < ' ') dbgpr("(%o)", c);
 else dbgpr("%c", c);
}
dbgpr("\n");

dbgpr("button_line2:\n");
for(i=0; i<75; i++) {
 c = button_line2[i];
 if (c < ' ') dbgpr("(%o)", c);
 else dbgpr("%c", c);
}
dbgpr("\n");
#endif /* OUT */

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

    if (term.tt_width < 80) {
	mesg(ERRALL+1, "Need at least 80 chars for -showbuttons.");
	return;
    }

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
/** /   cp_orig = (char *)image + wid * (buttonwin.bmarg-2);  / **/
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
/** /    cp_orig = (char *)image + wid * buttonwin.bmarg;  / *dbg */
    }
//b_line = s; /*dbg*/
    char *gap;
    int ind=0;
    size_t len_gap;

    /* clear button_line */
    strncpy(s, blanks, (size_t) term.tt_width);

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
    else /*if (spaces == 4)*/ {
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

    *s++ = ' ';
    *s = '\0';

/*  sprintf(s, "%s", font_off); */

/** /
dbgpr("makeB: indent=%d spaces=%d len_gap=%d\n", indent, spaces, len_gap);
cp_orig[wid-1] = '\0';
b_line[wid-1] = '\0';
dbgpr("row%d:(%s)\n", row, cp_orig);
//dbgpr("row%d:(%s)\n", row, b_line);
/ **/

    return;
}

#endif /* BUTTON_FONT */


#endif /* MOUSE_BUTTONS */
