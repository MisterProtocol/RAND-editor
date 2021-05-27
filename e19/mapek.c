#include <stdio.h>

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
    "cchar",   CCCTRLQUOTE,
    "chwin",   CCCHWINDOW,
    "close",   CCCLOSE,
    "cmd",     CCCMD,
    "dchar",   CCDELCH,
    "down",    CCMOVEDOWN,
    "dword",   CCDWORD,
    "edit",    CCSETFILE,
    "erase",   CCERASE,
    "esc",     CCINSMODE,
    "home",    CCHOME,
    "insmd",   CCINSMODE,
    "int",     CCINT,
    "join",    CCJOIN,
    "left",    CCMOVELEFT,
    "mark",    CCMARK,
    "open",    CCOPEN,
    "pick",    CCPICK,
    "replace", CCREPLACE,
    "ret",     CCRETURN,
    "right",   CCMOVERIGHT,
    "split",   CCSPLIT,
    "srtab",   CCTABS,
    "tab",     CCTAB,
    "undef",   CCUNAS1,
    "up",      CCMOVEUP,
    "wleft",   CCLWINDOW,
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
#define CCRANGE        0230
#define CCNULL         0231
#define CCHELP         0224
#define CCHELP         CCUNAS1
#define CCCCASE        0225
#define CCCAPS         0226
#define CCCCASE        CCUNAS1
#define CCCAPS         CCUNAS1
#define CCAUTOFILL     0227
#define CCAUTOFILL     CCUNAS1
#define CCDWORD        0232

#endif /* INFO */




struct keymap {
    char *keysym;
};

struct keymap low_keys[] = {
	"<cmd>",
	"<wleft>",
	"<edit>",
	"<int>",
	"<open>",
	"<-sch>",
	"<close>",
	"<mark>",
	"<left>",
	"<tab>",
	"<down>",
	"<home>",
	"<pick>",
	"<ret>",
	"<up>",
	"<ins>",
	"<repl>",
	"<-page>",
	"<+sch>",
	"<wright>",
	"<+line>",
	"<dchar>",
	"<+word>",
	"<-line>",
	"<-word>",
	"<+page>",
	"<chwin>",
	"<srtab>",
	"<cchar>",
	"<-tab>",
	"<bksp>",
	"<right>"
};


struct keymap hi_keys[] = {     /* 0200 - 0204 */
	"<stop>",
	"<erase>",
	"<undef>",
	"<split>",
	"<join>"
};



main()
{
	register int c;

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
	    else if( c < 0205 )
		printf( "%s", hi_keys[c - 0200]);
	    else
		fprintf( stderr, "key %o unrecognized\n", c );
	}
}
