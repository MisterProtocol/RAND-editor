#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


int in_a1();

#ifdef BLAND

/*
 *  Rand's old function key assignments - If you want this behavior, add
 *  a "define BLAND" at the top of this file, or in ../../include/localenv.h
 *  and recompile ../e18/e.tt.c.
 */

old_kini_a1 ()
{
#ifndef RANDold
    static char nonrepeat[] =
    ",-./013579(@?>=:/.-,ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_";
    static char repeat[] =
    "2468<";
#else   RANDold
    static char nonrepeat[] =
    ",-./046789(@?>=:/.-,ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_";
    static char repeat[] =
    "1235<";
#endif  RANDold

    /* Initialize the keys */
    kinix_a1 (nonrepeat, NO);
    kinix_a1 (repeat, YES);

    return;
}

kinix_a1 (str, repeat)
char *str;
{
    Reg1 char *cp;

#ifdef CBREAK
    cbreakflg = NO;
#endif CBREAK
    for (cp = str; *cp; cp++) {
	fputs ("\033P`", stdout);
	putchar (*cp);
	if (repeat)
	    putchar ('x');
	fputs ("~V", stdout);
	putchar (*cp);
	fputs ("\033\\", stdout);
    }
    return;
}


#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1

static
char lexa1[32] = {
    CCCMD       , /* <BREAK > enter parameter         */
    CCLWINDOW   , /* <cntr A> window left             */
    CCSETFILE   , /* <cntr B> set file                */
    CCINT       , /* <cntr C> interrupt               */
    CCDELCH     , /* <cntr D> character delete        */
    CCMIPAGE    , /* <cntr E> minus a page            */
    CCREPLACE   , /* <cntr F> replace                 */
    CCMARK      , /* <cntr G> mark a spot             */
    CCMOVELEFT  , /* <cntr H> move left               */
    CCTAB       , /* <cntr I> tab                     */
    CCMOVEDOWN  , /* <cntr J> move down 1 line        */
    CCMOVEUP    , /* <cntr K> move up 1 lin           */
    CCMOVERIGHT , /* <cntr L> forward move            */
    CCRETURN    , /* <cntr M> return                  */
    CCCLOSE     , /* <cntr N> delete                  */
    CCOPEN      , /* <cntr O> insert                  */
    CCPICK      , /* <cntr P> pick                    */
    CCMILINE    , /* <cntr Q> minus a line            */
    CCPLPAGE    , /* <cntr R> plus a page             */
    CCRWINDOW   , /* <cntr S> window right            */
    CCMISRCH    , /* <cntr T> minus search            */
    CCBACKTAB   , /* <cntr U> tab left                */
    CCUNAS1     , /* <cntr V> -- not assigned --      */
    CCPLLINE    , /* <cntr W> plus a line             */
    CCUNAS1     , /* <cntr X> -- not assigned --      */
    CCPLSRCH    , /* <cntr Y> plus search             */
    CCCHWINDOW  , /* <cntr Z> change window           */
    CCINSMODE   , /* <escape> insert mode             */
    CCCTRLQUOTE , /* <cntr \> knockdown next char     */
    CCTABS      , /* <cntr ]> set tabs                */
    CCHOME      , /* <cntr ^> home cursor             */
    CCBACKSPACE , /* <cntr _> backspace and erase     */
};

in_a1 (lexp, count)
char *lexp;
int *count;
{
    Reg1 int chr;
    Reg2 int nr;
    Reg3 char *icp;
    Reg4 char *ocp;

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--) {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) != CTRL('V')) {
	    if (   chr == CTRL('H')     /* BS KEY KLUDGE FOR NOW */
		|| chr == 0177          /* DEL KEY KLUDGE FOR NOW */
	       )
		*ocp++ = CCBACKSPACE;
	    else
		*ocp++ = chr >= ' ' ? chr : lexa1[chr];
	}
	else {
	    if (nr < 2) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    if ('%' <= chr && chr <= '_') Block {
		static char xlt[] = {
		    CCPLLINE    , /* %   send      */
		    _BAD_       , /* &   reset     */
		    _BAD_       , /* '   --        */
		    _BAD_       , /* (   break     */
		    _BAD_       , /* ) S break     */
		    _BAD_       , /* *   pause     */
		    _BAD_       , /* +   return    */
		    CCCMD       , /* ,   move up   */
		    CCCMD       , /* - S move up   */
		    CCMARK      , /* .   move down */
		    CCMARK      , /* / S move down */
		    CCINSMODE   , /* 0   0         */
#ifndef  RANDold
		    CCMISRCH    , /* 1   4         */
		    CCMOVEDOWN  , /* 2   2         */
		    CCPLSRCH    , /* 3   6         */
		    CCMOVELEFT  , /* 4   1         */
		    CCHOME      , /* 5   8         */
		    CCMOVERIGHT , /* 6   3         */
		    CCMIPAGE    , /* 7   7         */
		    CCMOVEUP    , /* 8   5         */
		    CCPLPAGE    , /* 9   9         */
#else   RANDold
		    CCMOVELEFT  , /* 1   1         */
		    CCMOVEDOWN  , /* 2   2         */
		    CCMOVERIGHT , /* 3   3         */
		    CCMISRCH    , /* 4   4         */
		    CCMOVEUP    , /* 5   5         */
		    CCPLSRCH    , /* 6   6         */
		    CCMIPAGE    , /* 7   7         */
		    CCHOME      , /* 8   8         */
		    CCPLPAGE    , /* 9   9         */
#endif  RANDold
		    CCINSMODE   , /* :   .         */
		    _BAD_       , /* ;   tab       */
		    CCDELCH     , /* <   enter     */
		    CCBACKTAB   , /* = S tab       */
		    CCCLOSE     , /* >   erase     */
		    CCCLOSE     , /* ? S erase     */
		    CCOPEN      , /* @   edit      */
		    CCMILINE    , /* A   delete    */
		    CCMILINE    , /* B S delete    */
		    CCREPLACE   , /* C   insert    */
		    CCREPLACE   , /* D S insert    */
		    CCPLLINE    , /* E   print     */
		    CCPLLINE    , /* F S print     */
		    CCMIPAGE    , /* G   7 ctrl-sh */
		    _BAD_       , /* H   pf1       */
		    CCCTRLQUOTE , /* I   pf2       */
		    CCINT       , /* J   pf3       */
		    CCTABS      , /* K   pf4       */
		    CCCHWINDOW  , /* L   pf5       */
		    CCSETFILE   , /* M   pf6       */
		    CCLWINDOW   , /* N   pf7       */
		    CCRWINDOW   , /* O   pf8       */
		    CCLWORD     , /* P   pf9       */
		    CCRWORD     , /* Q   pf10      */
		    CCPICK      , /* R   pf11      */
		    CCERASE     , /* S   pf12      */
		    _BAD_       , /* T S pf1       */
		    _BAD_       , /* U S pf2       */
		    CCINT       , /* V S pf3       */
		    CCTABS      , /* W S pf4       */
		    CCCHWINDOW  , /* X S pf5       */
		    CCSETFILE   , /* Y S pf6       */
		    CCLWINDOW   , /* Z S pf7       */
		    CCRWINDOW   , /* [ S pf8       */
		    CCLWORD     , /* \ S pf9       */
		    CCRWORD     , /* ] S pf10      */
		    CCPICK      , /* ^ S pf11      */
		    CCERASE     , /* _ S pf12      */
		};
		*ocp++ = xlt[chr - '%'];
	    }
	    else
		*ocp++ = _BAD_;
       }
    }
nomore:
    Block {
	Reg1 int conv;
	*count = nr;     /* number left over - still raw */
	conv = ocp - lexp;
	while (nr-- > 0)
	    *ocp++ = *icp++;
	return conv;
    }
}

#endif BLAND



/* Ann Arbor Ambassador Keyboard Downloading Table (addresses) */

#define A1_Send         '%'
#define A1_Reset        '&'
#define A1_Setup        '\''
#define A1_Break        '('
#define A1_Break_Sh     ')'
#define A1_Pause        '*'
#define A1_Return       '+'
#define A1_MoveUp       ','
#define A1_MoveUp_Sh    '-'
#define A1_MoveDn       '.'
#define A1_MoveDn_Sh    '/'
#define A1_0_Sh         '0'
#define A1_1_Sh         '1'
#define A1_2_Sh         '2'
#define A1_3_Sh         '3'
#define A1_4_Sh         '4'
#define A1_5_Sh         '5'
#define A1_6_Sh         '6'
#define A1_7_Sh         '7'
#define A1_8_Sh         '8'
#define A1_9_Sh         '9'
#define A1_Period       ':'
#define A1_Tab          ';'
#define A1_Enter        '<'
#define A1_Tab_Sh       '='
#define A1_Erase        '>'
#define A1_Erase_Sh     '?'
#define A1_Edit         '@'
#define A1_Delete       'A'
#define A1_Delete_Sh    'B'
#define A1_Insert       'C'
#define A1_Insert_Sh    'D'
#define A1_Print        'E'
#define A1_Print_Sh     'F'
#define A1_7_Ctl_Sh     'G'
#define A1_PF1          'H'
#define A1_PF2          'I'
#define A1_PF3          'J'
#define A1_PF4          'K'
#define A1_PF5          'L'
#define A1_PF6          'M'
#define A1_PF7          'N'
#define A1_PF8          'O'
#define A1_PF9          'P'
#define A1_PF10         'Q'
#define A1_PF11         'R'
#define A1_PF12         'S'
#define A1_PF1_Sh       'T'
#define A1_PF2_Sh       'U'
#define A1_PF3_Sh       'V'
#define A1_PF4_Sh       'W'
#define A1_PF5_Sh       'X'
#define A1_PF6_Sh       'Y'
#define A1_PF7_Sh       'Z'
#define A1_PF8_Sh       '['
#define A1_PF9_Sh       '\\'
#define A1_PF10_Sh      ']'
#define A1_PF11_Sh      '^'
#define A1_PF12_Sh      '_'

struct a1_foo {
	char    key_addr;       /* dowload address from above */
	char   *key_value;      /* all control characters! */
	short   repeat;         /* non-zero means make autorepeat */
}
	ak_load[] =  {

A1_Send         ,         0,    0,
A1_Reset        ,         0,    0,
A1_Setup        ,         0,    0,
A1_Break        ,       "A",    0,              /* Command */
A1_Break_Sh     ,       "A",    0,              /* Command */
A1_Pause        ,       "S",    0,              /* ^s stop output */
A1_Return       ,         0,    0,
A1_MoveUp       ,       "A",    0,              /* Command */
A1_MoveUp_Sh    ,       "A",    0,              /* Command */
A1_MoveDn       ,       "U",    0,              /* Mark */
A1_MoveDn_Sh    ,       "U",    0,              /* Mark */
A1_0_Sh         ,       "[",    0,              /* Insert mode */
A1_1_Sh         ,       "T",    0,              /* Minus search */
A1_2_Sh         ,       "J",    1,              /* Down arrow */
A1_3_Sh         ,       "Y",    0,              /* Plus search */
A1_4_Sh         ,       "H",    1,              /* Left arrow */
A1_5_Sh         ,       "G",    0,              /* Home */
A1_6_Sh         ,       "L",    1,              /* Right arrow */
A1_7_Sh         ,       "E",    0,              /* -Page */
A1_8_Sh         ,       "K",    1,              /* Up arrow */
A1_9_Sh         ,       "R",    0,              /* +Page */
A1_Period       ,       "[",    0,              /* Insert Mode */
A1_Tab          ,       "I",    0,              /* Tab */
A1_Enter        ,       "W",    1,              /* Del Char */
A1_Tab_Sh       ,      "XU",    0,              /* -Tab */
A1_Erase        ,       "V",    0,              /* Close */
A1_Erase_Sh     ,       "V",    0,              /* Close */
A1_Edit         ,       "O",    0,              /* Open */
A1_Delete       ,       "D",    0,              /* -Line */
A1_Delete_Sh    ,       "D",    0,              /* -Line */
A1_Insert       ,       "]",    0,              /* Replc */
A1_Insert_Sh    ,       "]",    0,              /* Replc */
A1_Print        ,       "F",    0,              /* +Line */
A1_Print_Sh     ,       "F",    0,              /* +Line */
A1_7_Ctl_Sh     ,         0,    0,              /* Debugging maybe? */
A1_PF1          ,         0,    0,              /* ???? */
A1_PF2          ,      "XC",    0,              /* Control-Char */
A1_PF3          ,      "\\",    0,              /* Interrupt */
A1_PF4          ,         0,    0,              /* S/R Tab */
A1_PF5          ,      "XW",    0,              /* Change Window */
A1_PF6          ,       "_",    0,              /* Alternate file */
A1_PF7          ,      "XH",    0,              /* Scroll Left */
A1_PF8          ,      "XL",    0,              /* Scroll Right */
A1_PF9          ,       "B",    1,              /* -word */
A1_PF10         ,       "N",    1,              /* +word */
A1_PF11         ,       "P",    0,              /* pick */
A1_PF12         ,       "^",    0,              /* erase */
A1_PF1_Sh       ,         0,    0,              /* ???? */
A1_PF2_Sh       ,      "XC",    0,              /* Control-Char */
A1_PF3_Sh       ,      "\\",    0,              /* Interrupt */
A1_PF4_Sh       ,         0,    0,              /* S/R Tab */
A1_PF5_Sh       ,      "XW",    0,              /* Change Window */
A1_PF6_Sh       ,       "_",    0,              /* Alternate file */
A1_PF7_Sh       ,      "XH",    0,              /* Scroll Left */
A1_PF8_Sh       ,      "XL",    0,              /* Scroll Right */
A1_PF9_Sh       ,       "B",    0,              /* -word */
A1_PF10_Sh      ,       "N",    0,              /* +word */
A1_PF11_Sh      ,       "P",    0,              /* pick */
A1_PF12_Sh      ,       "^",    0,              /* erase */
0               ,         0,    0               /* End of table flag */
};





/*
 *  Download the standard keyboard sequences into the function keys.
 */

std_kini_a1 ()
{
	struct a1_foo *p = ak_load;

	printf("\033P");        /* Start download sequence */
	while (p->key_addr) {
		if (p->key_value)
			a1load(p->key_addr,
				 p->key_value,
				 p->repeat);
		p++;
	}
	printf("\033\\");       /* End command */
	return;
}

a1load(name, sbuf, repeat)
char name;
char *sbuf;
int repeat;
{

	putchar('`');           /* Start string entry */
	putchar(name);          /* Next is "name" of key */
	if (repeat)
	   putchar('x');
	while (*sbuf)
	{
		putchar('~');           /* control char */
		putchar(*sbuf++ | 0x40);
	}
}



lt_a1 () { P ('h' & 31); }
rt_a1 () { fwrite ("\033[C", 3, 1, stdout); }
dn_a1 () { P (012); }
up_a1 () { fwrite ("\033[A", 3, 1, stdout); }
cr_a1 () { P (015); }
nl_a1 () { P (015); P (012); }
clr_a1 () { fwrite ("\033[;H\033[2J", 8, 1, stdout); delay (200); }
hm_a1 () { fwrite ("\033[;H", 4, 1, stdout); }
bsp_a1 () { P ('h' & 31); P (' '); P ('h' & 31); }
addr_a1 (lin, col) { printf ("\033[%d;%dH", lin + 1, col + 1); }
/*
il_a1 (num) { printf ("\033[%dL", num); return num; }
dl_a1 (num) { printf ("\033[%dM", num); return num; }
*/
il_a1 (num)
register Short num;
{
    printf ("\033[%dL", num);
    if (fast && num > 30)
	delay (3*num);
    return num;
}
dl_a1 (num)
register Short num;
{
    printf ("\033[%dM", num);
    if (fast && num > 30)
	delay (3*num);
    return num;
}
cle_a1 () { printf ("\033[K"); return; }

erase_a1 (num)
Scols num;
{
    printf ("\033[%dX", num);
}

#include <ctype.h>
ini0_a1 ()
{
#   define LINES_DEFAULT 48
    Reg1 char *cp;
    Reg2 char *numstr;
    int number;
    extern Flag smoothscroll, singlescroll;

    /*
     *  For insert/delete line to function the most efficiently
     *  (see e.d.c) turn the following flags off.
     */
    smoothscroll = singlescroll = NO;

    /* Decide on the number of lines */
    for (cp = tname; *cp && !isdigit (*cp); cp++)
	continue;
    if (   (cp = s2i (numstr = cp, &number))
	&& cp != numstr
       )
	term.tt_height = number;
    else
	term.tt_height = LINES_DEFAULT;
    return;
}

ini1_a1 ()
{
    /* Initialize the characteristics */
    Block {
	Reg1 char **cpp;
	static char *ini[] = {
			      /* A: xxx0 1011 xxxx xxxx */
	"\033[>27;h",         /*  SM ZKPCM */
	"\033[>25;29l",       /*  RM ZMBM,ZRLM */
			      /* B: xxxx xxxx 100x xxxx xxxx 0xxx xxxx xx xx*/
	"\033[>12h",          /*  SM SRM */
	"\033[>40;2;37l",     /*  RM ZHDM,KAM,ZAXM */
			      /* C: xx xx xx x xxxx xxxx xxxx */
			      /* D: 0110 10xx */
	"\033[>33;34;35h",    /*  SM ZWFM,ZWBM,ZDDM */
	"\033[>20;30;36l",    /*  RM LNM,ZDBM,ZSPM */
	NULL
	};

	for (cpp = ini; *cpp; cpp++)
	    fwrite (*cpp, strlen (*cpp), 1, stdout);
    }
    return;
}


kini_a1 ()
{

#ifdef BLAND
	if (strcmp (kname, "aaastandard") != 0) {
	    old_kini_a1();
	    kbd.kb_inlex = in_a1;
	    return;
	}
#endif BLAND
	std_kini_a1();

	return;
}




kend_a1()
{
    register char *download;
    char *getenv();

    if (download = getenv ("DOWNLOAD"))
	fwrite (download, strlen (download), 1, stdout);
    else if (system ("ambas -pop"))
	fatalpr ("\nCan't exec 'ambas' to reset the terminal.\n");
    return;
}

S_kbd kb_a1 = {
/* kb_inlex */  in_std,         /* STANDARD keyboard */
/* kb_init  */  kini_a1,
/* kb_end   */  kend_a1,
};

S_term t_a1 = {
/* tt_ini0    */    ini0_a1,
/* tt_ini1    */    ini1_a1,
/* tt_end     */    nop,
/* tt_left    */    lt_a1,
/* tt_right   */    rt_a1,
/* tt_dn      */    dn_a1,
/* tt_up      */    up_a1,
/* tt_cret    */    cr_a1,
/* tt_nl      */    nl_a1,
/* tt_clear   */    clr_a1,
/* tt_home    */    hm_a1,
/* tt_bsp     */    bsp_a1,
/* tt_addr    */    addr_a1,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xlate0,
/* tt_insline */    il_a1,
/* tt_delline */    dl_a1,
/* tt_inschar */    (int (*) ()) 0,
/* tt_delchar */    (int (*) ()) 0,
/* tt_clreol  */    cle_a1,
/* tt_defwin  */    (int (*) ()) 0,
/* tt_deflwin */    (int (*) ()) 0,
/* tt_erase   */    erase_a1,
#ifdef LMCHELP
/* tt_so      */    (int (*) ()) 0,
/* tt_soe     */    (int (*) ()) 0,
/* tt_help    */    (int (*) ()) 0,
#endif LMCHELP
#ifdef LMCVBELL
/* tt_vbell   */    (int (*) ()) 0,
#endif LMCVBELL
/* tt_nleft   */    1,
/* tt_nright  */    3,
/* tt_ndn     */    1,
/* tt_nup     */    3,
/* tt_nnl     */    2,
/* tt_nbsp    */    3,
/* tt_naddr   */    8,
/* tt_nlad    */    0,
/* tt_ncad    */    0,
/* tt_wl      */    1,
/* tt_cwr     */    1,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    NO,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    48,
};



