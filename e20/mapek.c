#include <stdio.h>

extern void exit();

#ifdef INFO

S_looktbl keysyms[] = {
    "+line",   CCPLLINE,
    "+page",   CCPLPAGE,
    "+sch",    CCPLSRCH,
    "+tab",    CCTAB,
    "+word",   CCRWORD,
    "-line",   CCMILINE,
    "-page",   CCMIPAGE,
    "-sch",    CCMISRCH,
    "-tab",    CCBACKTAB,
    "-word",   CCLWORD,
    "bksp",    CCBACKSPACE,
    "blot",    CCBLOT,
    "box",     CCBOX,
    "caps",    CCCAPS,
    "ccase",   CCCCASE,
    "cchar",   CCCTRLQUOTE,
    "center",  CCCENTER,
    "chwin",   CCCHWINDOW,
    "clear",   CCCLEAR,
    "close",   CCCLOSE,
    "cltabs",  CCCLRTABS,
    "cmd",     CCCMD,
    "cover",   CCCOVER,
    "dchar",   CCDELCH,
    "down",    CCMOVEDOWN,
    "dword",   CCDWORD,
    "edit",    CCSETFILE,
    "erase",   CCERASE,
    "esc",     CCINSMODE,
    "exit",    CCEXIT,
    "fill",    CCFILL,
    "home",    CCHOME,
    "insmd",   CCINSMODE,
    "int",     CCINT,
    "join",    CCJOIN,
    "justify", CCJUSTIFY,
    "left",    CCMOVELEFT,
    "mark",    CCMARK,
    "null",    CCNULL,
    "open",    CCOPEN,
    "overlay", CCOVERLAY,
    "pick",    CCPICK,
    "play",    CCPLAY,
    "quit",    CCQUIT,
    "range",   CCRANGE,
    "record",  CCRECORD,
    "redraw",  CCREDRAW,
    "replace", CCREPLACE,
    "ret",     CCRETURN,
    "right",   CCMOVERIGHT,
    "split",   CCSPLIT,
    "srtab",   CCTABS,
    "stopx",   CCSTOPX,
    "tab",     CCTAB,
    "track",   CCTRACK,
    "undef",   CCUNAS1,
    "up",      CCMOVEUP,
    "wleft",   CCLWINDOW,
    "wp",      CCAUTOFILL,
    "wright",  CCRWINDOW,
    0
 };




/* input control character assignments */

#define CCCMD           000 /* ^@ enter parameter       */
#define CCLWINDOW       001 /* ^A window left           */
#define CCSETFILE       002 /* ^B set file              */
#define CCINT           003 /* ^C interrupt         *** was chgwin */
#define CCOPEN          004 /* ^D insert                */
#define CCMISRCH        005 /* ^E minus search          */
#define CCCLOSE         006 /* ^F delete                */
#define CCMARK          007 /* ^G mark a spot       *** was PUT */
#define CCMOVELEFT      010 /* ^H backspace             */
#define CCTAB           011 /* ^I tab                   */
#define CCMOVEDOWN      012 /* ^J move down 1 line      */
#define CCHOME          013 /* ^K home cursor           */
#define CCPICK          014 /* ^L pick                  */
#define CCRETURN        015 /* ^M return                */
#define CCMOVEUP        016 /* ^N move up 1 lin         */
#define CCINSMODE       017 /* ^O insert mode           */
#define CCREPLACE       020 /* ^P replace           *** was GOTO */
#define CCMIPAGE        021 /* ^Q minus a page          */
#define CCPLSRCH        022 /* ^R plus search           */
#define CCRWINDOW       023 /* ^S window right          */
#define CCPLLINE        024 /* ^T minus a line          */
#define CCDELCH         025 /* ^U character delete      */
#define CCRWORD         026 /* ^V move right one word   */
#define CCMILINE        027 /* ^W plus a line           */
#define CCLWORD         030 /* ^X move left one word    */
#define CCPLPAGE        031 /* ^Y plus a page           */
#define CCCHWINDOW      032 /* ^Z change window     *** was WINDOW */
#define CCTABS          033 /* ^[ set tabs              */
#define CCCTRLQUOTE     034 /* ^\ knockdown next char   */
#define CCBACKTAB       035 /* ^] tab left              */
#define CCBACKSPACE     036 /* ^^ backspace and erase   */
#define CCMOVERIGHT     037 /* ^_ forward move          */
#define CCDEL          0177 /* <del>    -- not assigned --  *** was EXIT */

#define CCSTOP         0200 /* *@ stop replay           */
#define CCERASE        0201 /* *A erase                 */
#define CCUNAS1        0202 /* *B -- not assigned --    */
#define CCSPLIT        0203 /* *C split                 */
#define CCJOIN         0204 /* *D join                  */
#define CCEXIT         0205
#define CCABORT        0206
#define CCREDRAW       0207
#define CCCLRTABS      0210
#define CCCENTER       0211
#define CCFILL         0212
#define CCJUSTIFY      0213
#define CCCLEAR        0214
#define CCTRACK        0215
#define CCBOX          0216
#define CCSTOPX        0217
#define CCQUIT         0220
#define CCCOVER        0221
#define CCOVERLAY      0222
#define CCBLOT         0223
#define CCHELP         0224
#define CCCCASE        0225
#define CCCAPS         0226
#define CCAUTOFILL     0227
#define CCRANGE        0230
#define CCNULL         0231
#define CCDWORD        0232
#define UNASS          0233
#define CCRECORD       0234
#define CCPLAY         0235
#define CCMOUSE        0236
#define CCLAST         0236
#endif /* INFO */

#define CCLAST         0236

struct keymap {
    char *keysym;
};

struct keymap low_keys[] = {
	{"<cmd>"},
	{"<wleft>"},
	{"<edit>"},
	{"<int>"},
	{"<open>"},
	{"<-sch>"},
	{"<close>"},
	{"<mark>"},
	{"<left>"},
	{"<tab>"},
	{"<down>"},
	{"<home>"},
	{"<pick>"},
	{"<ret>"},
	{"<up>"},
	{"<ins>"},
	{"<repl>"},
	{"<-page>"},
	{"<+sch>"},
	{"<wright>"},
	{"<+line>"},
	{"<dchar>"},
	{"<+word>"},
	{"<-line>"},
	{"<-word>"},
	{"<+page>"},
	{"<chwin>"},
	{"<srtab>"},
	{"<cchar>"},
	{"<-tab>"},
	{"<bksp>"},
	{"<right>"}
};


struct keymap hi_keys[] = {     /* 0200 - 0236 */
	{"<stop>"},
	{"<erase>"},
	{"<undef>"},
	{"<split>"},
	{"<join>"},
	{"<exit>"},
	{"<abort>"},
	{"<redraw>"},
	{"<clrtabs>"},
	{"<center>"},
	{"<fill>"},
	{"<just>"},
	{"<clear>"},
	{"<track>"},
	{"<box>"},
	{"<stopx>"},
	{"<quit>"},
	{"<cover>"},
	{"<overlay>"},
	{"<blot>"},
	{"<help>"},
	{"<ccase>"},
	{"<caps>"},
	{"<autofill>"},
	{"<range>"},
	{"<null>"},
	{"<dword>"},
	{"<unass>"},
	{"<record>"},
	{"<play>"},
	{"<mouse>"}
};


int
main()
{
	register int c;
	int y, x;

/* test */
#ifdef OUT
int i;
for(i = 0; i < (CCLAST - 0200); i++) {
  printf("%d=%s\n",i,hi_keys[i]);
}
exit(1);
#endif

	/*
	 * skip up to first \r (or \n with new ascii format for 1st line)
	 */
	do
	    c = fgetc( stdin );
	while( c != '\r' && c != '\n' && c != EOF );

	if( c == EOF ) {
	    fprintf( stderr, "unexpected end of file!\n" );
	    exit(1);
	}

	while(( c = fgetc( stdin )) != EOF ) {
	    c &= 0377;
	    if( c < 040 ) {
		printf( "%s", low_keys[c] );
		if( c == '\r' )
		    putchar( '\n' );
	    }
	    else if( c < 0200 )
		putchar( c );
	    else if( c <= CCLAST ) {

		/*fprintf(stderr, "c=(%o)idx=(%d)\n", c, c-0200);*/

		if( c == 0236 ) { /* CCMOUSE */
		    fscanf(stdin, "%3d%3d", &y, &x);
		    printf("<CCMOUSE(%d,%d)>", y,x);
		}
		else {
		  printf( "%s", hi_keys[c - 0200]);
		}

	    }
	    else
		fprintf( stderr, "key %o unrecognized\n", c );
	}

	exit(0);
}
