#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation

File xl.c - Terminal dependent code for AnnArbor Ambassador and XL
#endif

#define AAXL   /* for Ambassador XL terminal */
#define AAAS "aaas"   /* TERM aaas - Don't load keys from tables */
#ifndef MKAAAS /* mkaaas.c defines it - for making tabset file aaas */
#undef  CTRL(x) ((x) & 31)
#define CTRL(x) ((x) & 31)

extern char *getenv();
extern Cmdret help_std();

/* Terminal loads in order sent */
char *i_keys[];          /* Initial key load - Ambassador */
char *i_keysxl[];        /* Initial key load - XL */
char *i_modes[];         /* Initial mode load - Ambassador and XL */
char *x_keys[];          /* Exit key load - Ambassador */
char *x_keysxl[];        /* Exit key load - Ambassador and XL */

Uchar lexa1s0[128];
Uchar lexa1s1[128];

#define AMBASECASE 01
#define GURUECASE  07
#define AMBASXL    11
#define GURUXL     17
/*==========================================================================*/

/*>>>>> Initialze "e", standard (almost) keys <<<<<<<<<<<<<<<<<<<<<<<*/
kini_a1 ()
{
    int n;
    char *envstring;

    if (envstring = getenv("EKEYS"))
	fwrite (envstring, strlen (envstring), 1, stdout);
    else {
	if (strncmp(kname,AAAS,4) == 0) return;
	ldtb_a1(i_keys);
#ifdef AAXL
	ldtb_a1(i_keysxl);
#endif AAXL
    }
}


/*==========================================================================*/

#include <ctype.h>
ini0_a1 ()
{
#   define LINES_DEFAULT 48
#   define COLS_DEFAULT  80
    Reg1 char *cp;
    Reg2 char *numstr;
    int number;
    extern Flag smoothscroll, singlescroll;
#ifdef TERMCAP
    char tcbuf[1024];
#endif

if(getenv("NOESCCMD"))      /* temp, for those who don't want ESC=CMD*/
    lexa1s0[033] = CCINSMODE;

    /*
     *  For insert/delete line to function the most efficiently
     *  (see e.d.c) turn the following flags off.
     */
    smoothscroll = singlescroll = NO;

    /* Decide on the number of lines */
    term.tt_height = LINES_DEFAULT;
    term.tt_width = COLS_DEFAULT;
    for (cp = tname; *cp && !isdigit (*cp); cp++)
	continue;
    if (   (cp = s2i (numstr = cp, &number))
	&& cp != numstr
       )
	term.tt_height = number;
#ifdef TERMCAP
    /* try to get screen width from termcap */
    getcap(tname); /* at least required for "so" and "se" modes */
    if ( tgetent(tcbuf, tname)) {
	term.tt_width = tgetnum("co");
	term.tt_height = tgetnum("li");
    }
#endif
/** printf("\nscreen size = %d x %d\n",term.tt_height,term.tt_width); /**/
    return;
#undef LINES_DEFAULT
#undef COLS_DEFAULT
}

/*==========================================================================*/
ini1_a1 ()
{

    /* Initialize the characteristics */
    if (getenv("EKEYS") == 0)
	    ldtb_a1(i_modes);
}





/*==========================================================================*/


/*>>>>> Output string array <<<<<<<<<<<<<<<<<<<<<<<*/
ldtb_a1(keys)  char *keys[];
{
    char **p;
    char *cp;

    for (p=keys; *p; p++) {
	for (cp = *p; *cp; cp++) {
	    putc(*cp, stdout);
	}
    }
}



/*==========================================================================*/


#ifdef COMMENT
--------
Taken from file term/standard.c
    Standard stuff in support of
    terminal-dependent code and data declarations
Modified, at the time AA Xl terminal support added, thus;
  1. BACKSPACE and MOVELEFT switched.
  2. Escape code (\033) reserved form future use.
  3. Del defines interrupt. Was backspace. See "in_a1".
#endif

static Uchar lexa1s0[128] = {
    CCCMD       , /* <BREAK > */
    CCCMD       , /* <cntr A> */
    CCLWORD     , /* <cntr B> */
    CCMOVELEFT  , /* <cntr C> */  /* non-standard */
    CCMILINE    , /* <cntr D> */
    CCMIPAGE    , /* <cntr E> */
    CCPLLINE    , /* <cntr F> */
    CCHOME      , /* <cntr G> */
    CCBACKSPACE , /* <cntr H> */  /* non-standard */
    CCTAB       , /* <cntr I> */
    CCMOVEDOWN  , /* <cntr J> */
    CCMOVEUP    , /* <cntr K> */
    CCMOVERIGHT , /* <cntr L> */
    CCRETURN    , /* <cntr M> */
    CCRWORD     , /* <cntr N> */
    CCOPEN      , /* <cntr O> */
    CCPICK      , /* <cntr P> */
    CCUNAS1     , /* <cntr Q> */  /* reserved for XON/XOFF flow control */
    CCPLPAGE    , /* <cntr R> */
    CCUNAS1     , /* <cntr S> */  /* reserved for XON/XOFF flow control */
    CCMISRCH    , /* <cntr T> */
    CCMARK      , /* <cntr U> */
    CCCLOSE     , /* <cntr V> */
    CCDELCH     , /* <cntr W> */
    CCUNAS1     , /* <cntr X> */
    CCPLSRCH    , /* <cntr Y> */
    CCINSMODE   , /* <cntr Z> */
    CCCMD       , /* <escape> */
    CCINT       , /* <cntr \> */
    CCREPLACE   , /* <cntr ]> */
    CCERASE     , /* <cntr ^> */
    CCSETFILE   , /* <cntr _> */
    32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,
    48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,
    64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,
    80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,
    96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
    112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,
    CCINT       , /* <del   > */
};

static Uchar lexa1s1[128] = {
    CCUNAS1     , /* <BREAK > */
    CCSETFILE   , /* <cntr A> */
    CCSPLIT     , /* <cntr B> */
    CCCTRLQUOTE , /* <cntr C> */
    CCUNAS1     , /* <cntr D> */
    CCERASE     , /* <cntr E> */
    CCUNAS1     , /* <cntr F> */
    CCUNAS1     , /* <cntr G> */
    CCLWINDOW   , /* <cntr H> */
    CCUNAS1     , /* <cntr I> */
    CCJOIN      , /* <cntr J> */
    CCUNAS1     , /* <cntr K> */
    CCRWINDOW   , /* <cntr L> */
    CCUNAS1     , /* <cntr M> */
    CCDWORD     , /* <cntr N> */
    CCUNAS1     , /* <cntr O> */
#ifdef RECORDING
    CCPLAY      , /* <cntr P> */
#else
    CCUNAS1     , /* <cntr P> */
#endif
    CCUNAS1     , /* <cntr Q> */
#ifdef RECORDING
    CCRECORD    , /* <cntr R> */
#else
    CCREPLACE   , /* <cntr R> */
#endif
    CCUNAS1     , /* <cntr S> */
    CCTABS      , /* <cntr T> */
    CCBACKTAB   , /* <cntr U> */
    CCUNAS1     , /* <cntr V> */
    CCCHWINDOW  , /* <cntr W> */
    CCUNAS1     , /* <cntr X> */
    CCUNAS1     , /* <cntr Y> */
    CCUNAS1     , /* <cntr Z> */
    CCUNAS1     , /* <escape> */
    CCUNAS1     , /* <cntr \> */
    CCUNAS1     , /* <cntr ]> */
    CCUNAS1     , /* <cntr ^> */
    CCUNAS1     , /* <cntr _> */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 32-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 40-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 48-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 56-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 64-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 72-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 80-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 88-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 96-  */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 104- */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,/* 112- */
CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1,CCUNAS1 /* 120- */
};



/*==========================================================================*/

in_a1 (lexp, count)
char *lexp;
int *count;
{
	register int nr;
	register Uint chr;
	Reg3 char *icp;
	Reg4 char *ocp;
	static int state = 0;

	icp = ocp = lexp;
	nr = *count;
	for (; nr > 0; nr--)
	{
		chr = *icp++ & 0177;
		if (state == 0)
		{
			if (chr == CTRL ('X'))
			    state = 1;
			else
			    *ocp++ = lexa1s0[chr];
		}
		else    /* state 1 - ctrl x seen */
		{
			*ocp++ = lexa1s1[chr];
			state = 0;
		}
	}

	*count = 0;
	return (ocp - lexp);
}




#ifdef COMMENTS
in_a1 (lexp, count)
char *lexp;
int *count;
{
    register int nr;
    register Uint chr;
    Reg3 char *icp;
    Reg4 char *ocp;

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--) {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) >= 32) {
	    if (chr == 0177)            /* DEL KEY KLUDGE FOR NOW */
		*ocp++ = CCINT;
	    else
		*ocp++ = chr;
	}
	else if (chr == CTRL ('X')) {
	    if (nr < 2) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    switch (chr) {
	    case CTRL ('a'):
		*ocp++ = CCSETFILE;
		break;
	    case CTRL ('b'):
		*ocp++ = CCSPLIT;
		break;
	    case CTRL ('c'):
		*ocp++ = CCCTRLQUOTE;
		break;
	    case CTRL ('e'):
		*ocp++ = CCERASE;
		break;
	    case CTRL ('h'):
		*ocp++ = CCLWINDOW;
		break;
	    case CTRL ('j'):
		*ocp++ = CCJOIN;
		break;
	    case CTRL ('l'):
		*ocp++ = CCRWINDOW;
		break;
	    case CTRL ('r'):
		*ocp++ = CCREPLACE;
		break;
	    case CTRL ('t'):
		*ocp++ = CCTABS;
		break;
	    case CTRL ('u'):
		*ocp++ = CCBACKTAB;
		break;
	    case CTRL ('w'):
		*ocp++ = CCCHWINDOW;
		break;
	    default:
		*ocp++ = CCUNAS1;
		break;
	    }
	}
	else
	    *ocp++ = lexa1[chr];
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
#endif



/*==========================================================================*/


lt_a1 () { P ('h' & 31); }
rt_a1 () { fwrite ("\033[C", 3, 1, stdout); }
dn_a1 () { P (012); }
up_a1 () { fwrite ("\033[A", 3, 1, stdout); }
cr_a1 () { P (015); }
nl_a1 () { P (015); P (012); }
clr_a1 () { fwrite ("\033[;H\033[2J", 8, 1, stdout);  }
hm_a1 () { fwrite ("\033[;H", 4, 1, stdout); }
bsp_a1 () { P ('h' & 31); P (' '); P ('h' & 31); }
addr_a1 (lin, col) { printf ("\033[%d;%dH", lin + 1, col + 1); }
il_a1 (num) { printf ("\033[%dL", num); return num; }
dl_a1 (num) { printf ("\033[%dM", num); return num; }
cle_a1 () { printf ("\033[K"); return; }
so_a1 () { printf ("\033[7m");  }
soe_a1 () { printf ("\033[m");  }
cgr_a1 () { printf ("\033[;H\0339"); } /* change graphic rendition */
erase_a1 (num)
Scols num;
{
    printf ("\033[%dX", num);
}


/*==========================================================================*/
kend_a1()
{
    register char *download;

    if (download = getenv ("DOWNLOAD"))
	fwrite (download, strlen (download), 1, stdout);
    else {
#ifdef DBS
	printf("\033[>30h");
#endif
	if (strncmp(kname,AAAS,4) == 0) return;

	ldtb_a1(x_keys);
#ifdef AAXL
	ldtb_a1(x_keysxl);
#endif
    }
}

S_kbd kb_a1 = {
/* kb_inlex */  in_a1, /* Modified (see BS and left cursor) STANDARD keyboard */
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
/* tt_so      */    so_a1,
/* tt_soe     */    soe_a1,
/* tt_help    */    help_std,
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


#endif MKAAAS
/*==========================================================================*/


/* Ann Arbor Ambassador Keyboard Downloading Table (addresses) */


static char *i_keys[] =  {

    "\033[>37h", /*  XON/XOFF flow ctl on   */
    "\033P",
    "`0x~X~N",  /*  `0  A1_0_Sh         ,del words                  */
    "`1~T",     /*  `1  A1_1_Sh         ,Minus search               */
    "`2x~J",    /*  `2  A1_2_Sh         ,Down arrow                 */
    "`3~Y",     /*  `3  A1_3_Sh         ,Plus search                */
    "`4x~C",    /*  `4  A1_4_Sh         ,Left arrow                 */
    "`5~G",     /*  `5  A1_5_Sh         ,Home                       */
    "`6x~L",    /*  `6  A1_6_Sh         ,Right arrow                */
    "",         /*  `G  A1_7_Ctl_Sh     ,Debugging maybe?           */
    "`7~E",     /*  `7  A1_7_Sh         ,-Page                      */
    "`8x~K",    /*  `8  A1_8_Sh         ,Up arrow                   */
    "`9~R",     /*  `9  A1_9_Sh         ,+Page                      */
    "`A~D",     /*  `A  A1_Delete       ,-Line                      */
    "`B~D",     /*  `B  A1_Delete_Sh    ,-Line                      */
    "`@~O",     /*  `@  A1_Edit         ,Open                       */
    "`<x~W",    /*  `<  A1_Enter        ,Del Char                   */
    "`>~V",     /*  `>  A1_Erase        ,Close                      */
    "\033\134",                          /* dc terminator           */
    "\033P", /* =================== XL DCS control.   */
    "`?~V",     /*  `?  A1_Erase_Sh     ,Close                      */
    "`C~]",     /*  `C  A1_Insert       ,Replc                      */
    "`D~]",     /*  `D  A1_Insert_Sh    ,Replc                      */
    "`.~U",     /*  `.  A1_MoveDn       ,Mark                       */
    "`/~U",     /*  `/  A1_MoveDn_Sh    ,Mark                       */
    "`,~A",     /*  `,  A1_MoveUp       ,Command                    */
    "`-~A",     /*  `-  A1_MoveUp_Sh    ,Command                    */
    "",         /*  `H  A1_PF1          ,????                       */
    "`Qx~N",    /*  `Q  A1_PF10         ,+word                      */
    "`]x~N",    /*  `]  A1_PF10_Sh      ,+word                      */
    "`R~P",     /*  `R  A1_PF11         ,pick                       */
    "`^~P",     /*  `^  A1_PF11_Sh      ,pick                       */
/*  "`S~^",     /*  `S  A1_PF12         ,erase                      */
    "`S~X~E",   /*  `S  A1_PF12         ,erase                      */
/*  "`_~^",     /*  `_  A1_PF12_Sh      ,erase                      */
    "`_~X~E",   /*  `_  A1_PF12_Sh      ,erase                      */
    "",         /*  `T  A1_PF1_Sh       ,????                       */
    "\033\134",                          /* dc terminator           */
    "\033P", /* =================== XL DCS control.   */
    "`I~X~C",   /*  `I  A1_PF2          ,Control-Char               */
    "`U~X~C",   /*  `U  A1_PF2_Sh       ,Control-Char               */
    "`J~\\",    /*  `J  A1_PF3          ,Interrupt                  */
    "`V~\\",    /*  `V  A1_PF3_Sh       ,Interrupt                  */
    "`K~X~T",   /*  `K  A1_PF4          ,S/R Tab                    */
    "`W~X~T",   /*  `W  A1_PF4_Sh       ,S/R Tab                    */
    "`L~X~W",   /*  `L  A1_PF5          ,Change Window              */
    "`X~X~W",   /*  `X  A1_PF5_Sh       ,Change Window              */
/*  "`M~_",     /*  `M  A1_PF6          ,Alternate file             */
    "`M~X~A",   /*  `M  A1_PF6          ,Alternate file             */
/*  "`Y~_",     /*  `Y  A1_PF6_Sh       ,Alternate file             */
    "`Y~X~A",   /*  `Y  A1_PF6_Sh       ,Alternate file             */
    "`N~X~H",   /*  `N  A1_PF7          ,Scroll Left                */
    "`Z~X~H",   /*  `Z  A1_PF7_Sh       ,Scroll Left                */
    "`O~X~L",   /*  `O  A1_PF8          ,Scroll Right               */
    "`[~X~L",   /*  `[  A1_PF8_Sh       ,Scroll Right               */
    "`Px~B",    /*  `P  A1_PF9          ,-word                      */
    "`\\x~B",   /*  `\  A1_PF9_Sh       ,-word                      */
    "\033\134",                          /* dc terminator           */
    "\033P", /* =================== XL DCS control.   */
    "`*~S",     /*  `*  A1_Pause        ,^s stop output             */
    "`:~Z",     /*  `:  A1_Period       ,Insert Mode                */
    "`E~F",     /*  `E  A1_Print        ,+Line                      */
    "`F~F",     /*  `F  A1_Print_Sh     ,+Line                      */
    "",         /*  `&  A1_Reset        ,                           */
    "",         /*  `+  A1_Return       ,                           */
    "",         /*  `%  A1_Send         ,                           */
    "`;x~I",     /*  `;  A1_Tab          ,Tab                        */
    "`=x~X~U",   /*  `=  A1_Tab_Sh       ,-Tab                       */
    "\033\134",
    0 } ;                   /* array terminator        */

#ifdef AAXL
static char *i_keysxl[] =  {
"\033[>37h",            /*  XON/XOFF flow ctl on   */
"\033P>", /* =================== XL DCS control.   */
	/*** unshifted, non-repeatable keys      ***/
"|0;0;0;1|~X~B",        /*  split                  */
"|1|~X~J",              /*  join                   */
"|3|~A",                /*  cmd                    */
"|74|~?",               /*  interrupt              */
"|100|~Z",              /*  insert                 */
"|112|~G",              /*  home                   */
"|124|~X~C",            /*  cntrl char             */
"|125|~X~T",            /*  s/r tab                */
"|126|~X~W",            /*  chg win                */
/*"|127|~_",              /*  alt                    */
"|127|~X~A",            /*  alt                    */
"|128|~X~H",            /*  window left            */
"|129|~X~L",            /*  window right           */
/*"|130|~^",              /*  erase                  */
"|130|~X~E",            /*  erase                  */
"|131|~O",              /*  open                   */
"|132|~P",              /*  pick                   */
"|133|~V",              /*  close                  */
"|135|~U",              /*  mark                   */
"|136|~E",              /*  -page                  */
"|137|~D",              /*  -line                  */
"|138|~T",              /*  -srch                  */
"|141|~R",              /*  +page                  */
"|142|~F",              /*  +line                  */
"|143|~Y",              /*  +srch                  */
"|144|~]",              /*  replc                  */
"\033\134",             /* dc terminator           */
"\033P>", /* =================== XL DCS control.   */
	/*** unshifted, repeatable keys         ****/
"|53;0;0;2|~M",         /*  return                 */
"|54|~J",               /*  line feed              */
"|60|~I",               /*  tab                    */
"|73|~H",               /*  bsp                    */
"|102|~X~N",            /*  delword (NOTE. Mode 27 makes it shifted */
"|107|~J",              /*  down cursor            */
"|110|~X~N",            /*  delword (NOTE. Mode 27 makes it shifted */
"|111|~C",              /*  lt cursor              */
"|113|~L",              /*  rg cursor              */
"|117|~K",              /*  up cursor              */
"|116|~B",              /*  <-word                 */
"|118|~N",              /*  ->word                 */
"\033\134",             /* dc terminator           */
"\033P>", /* =================== XL DCS control.   */
	/*** shifted, non-repeatable keys       ****/
"|0;1;0;1|~X~B",        /*  split                  */
"|1|~X~J",              /*  join                   */
"|3|~A",                /*  cmd                    */
"|74|~?",               /*  interrupt              */
"|100|~Z",              /*  insert                 */
"|112|~G",              /*  home                   */
"|124|~X~C",            /*  cntrl char             */
"|125|~X~T",            /*  s/r tab                */
"|126|~X~W",            /*  chg win                */
/*"|127|~_",              /*  alt                    */
"|127|~X~A",            /*  alt                    */
"|128|~X~H",            /*  window left            */
"|129|~X~L",            /*  window right           */
/*"|130|~^",              /*  erase                  */
"|130|~X~E",            /*  erase                  */
"|131|~O",              /*  open                   */
"|132|~P",              /*  pick                   */
"|133|~V",              /*  close                  */
"|135|~U",              /*  mark                   */
"|136|~E",              /*  -page                  */
"|137|~D",              /*  -line                  */
"|138|~T",              /*  -srch                  */
"|141|~R",              /*  +page                  */
"|142|~F",              /*  +line                  */
"|143|~Y",              /*  +srch                  */
"|144|~]",              /*  replc                  */
"\033\134",             /* dc terminator           */
"\033P>", /* =================== XL DCS control.   */
	/*** shifted, repeatable keys           ****/
"|53;1;0;2|~M",         /*  return                 */
"|54|~J",               /*  line feed              */
"|60|~X~U",             /*  -tab                   */
"|73|~H",               /*  bsp                    */
"|102|~X~N",            /*  delword (NOTE. Mode 27 makes it unshifted */
"|107|~J",              /*  down cursor            */
"|110|~W",              /*  delchr (NOTE. Mode 27 makes it unshifted */
"|111|~C",              /*  lt cursor              */
"|113|~L",              /*  rg cursor              */
"|117|~K",              /*  up cursor              */
"|116|~B",              /*  <-word                 */
"|118|~N",              /*  ->word                 */
"\033\134",             /* dc terminator           */
"\033P>", /* =================== XL DCS control.   */
	/*** misc loads, unused keys, etc        ***/
"|33;30;0;0;1|",        /*  scroll                 */
"|34;29;0;0;1|",        /*  zoom                   */
#ifdef OUT
"|102|0;2;0;0|~@",      /*  disabled key           */
"|102|1;2;0;0|~@",      /*  disabled key           */
#endif OUT
"|106;0;2;0;0|~@",      /*  disabled key           */
"|106;1;2;0;0|~@",      /*  disabled key           */
"|108;0;2;0;0|~@",      /*  disabled key           */
"|108;1;2;0;0|~@",      /*  disabled key           */
"\033\134",             /* dc terminator           */
0 } ;                   /* array terminator        */
#endif AAXL



#ifndef MKAAAS
/*==========================================================================*/

static char *i_modes[] = {
"\033[>",               /* SM - Set Mode                                */
"12;",                  /*  SRM  Send-Receive Mode (B,12)               */
"27;",                  /* zKPCM Keypad Control Mode (A,27)             */
"33;",                  /* zWFM  Wrap Forward Mode (D,33)               */
"34;",                  /* zWBM  Wrap Backwards Mode (D,34)             */
"35;",                  /* zDDM  Del-Character Display mode (D,35)      */
"37;",                  /* zAXM  Auto Xon/Xoff mode (B,37)              */
"h",
"\033[>",               /* RM - Reset Mode                              */
"2;",                   /*  KAM  Keyboard-Action Mode (B,2)             */
"4;",                   /*  IRM  Insertion-replacement mode (-,4)       */
"20;",                  /*  LNM  LF-New-line Mode (D,20)                */
"30;",                  /* zDBM  Destructive Backspace Mode (D,30)      */
"36;",                  /* zSPM  Scroll-page Mode (D,36)                */
"40;",                  /* zHDM  Half-Duplex MOde (B,40)                */
"l",
NULL
};




/*==========================================================================*/


static char *x_keys[] = {
    "\033P",
/* strings */
    "`!"        ,       /* s_trailer Trailer string             */
    "`\""       ,       /* s_res1    [Reserved]                 */
    "`#"        ,       /* s_enq     ENQ string                 */
/* keys  */
    "`%y~[S"     ,      /*  %  k_send      Send                     */
    "`&"         ,      /* `&  k_reset     Reset ???                */
    "`'y~[{D}"   ,      /* `'  k_setup     Setup                    */
    "`(y~[{E}"   ,      /* `(  k_break     Break                    */
    "`)y~[{F}"   ,      /* `)  k_shbreak   Break Shift              */
    "`*y~[{G}"   ,      /* `*  k_pause     Pause                    */
    "",                 /* `+  k_return    Return                   */
    "`,xy~[[T"   ,      /* `,  k_mvup      Move Up         SD       */
    "`-y~[{I}"   ,      /* `-  k_shmvup    Move Up Shift            */
    "`.xy~[[S"   ,      /* `.  k_mvdown    Move Down       SU       */
    "`/y~[{J}"   ,      /* `/  k_shmvdown  Move Down Shift          */
    "`0"         ,      /* `0  k_zero      0 Shift                  */
    "`1~[F"      ,      /* `1  k_one       1 Shift         SSA      */
    "`2x~[[B"    ,      /* `2  k_two       2 Shift                  */
    "`3~[G"      ,      /* `3  k_three     3 Shift                  */
    "`4x~[[D"    ,      /* `4  k_four      4 Shift                  */
    "`5~[[H"     ,      /* `5  k_five      5 Shift                  */
    "`6x~[[C"    ,      /* `6  k_six       6 Shift                  */
    "`7~[[{G}"   ,      /* `7  k_seven     7 Shift                  */
    "`8x~[[A"    ,      /* `8  k_eight     8 Shift                  */
    "`9~[H"      ,      /* `9  k_nine      9 Shift                  */
    "`:."        ,      /* `:  k_period    Period                   */
    "`;~I"       ,      /* `;  k_tab       Tab                      */
    "`<~M"       ,      /* `<  k_enter     Enter                    */
    "`=~[[Z"     ,      /* `=  k_shtab     Tab Shift                */
    "`>~[[K"     ,      /* `>  k_erase     Erase                    */
    "`?~[[J"     ,      /* `?  k_sherase   Erase Shift              */
    "`@~[6"      ,      /* `@  k_edit      Edit                     */
    "`A~[[P"     ,      /* `A  k_delete    Delete                   */
    "`B~[[M"     ,      /* `B  k_shdelete  Delete Shift             */
    "`C~[[@"     ,      /* `C  k_insert    Insert                   */
    "`D~[[L"     ,      /* `D  k_shinsert  Insert Shift             */
    "`Fy~[{K}"   ,      /* `F  k_shprint   Print Shift              */
    "`G~[3{G}"   ,      /* `G  k_ctlsh7    7 Ctrl Shift             */
    ""           ,      /* `H  k_pf1       PF1             (null)   */
    "`I"         ,      /* `I  k_pf2       PF2             (null)   */
    "`J"         ,      /* `J  k_pf3       PF3             (null)   */
    "`K"         ,      /* `K  k_pf4       PF4             (null)   */
    "`L"         ,      /* `L  k_pf5       PF5             (null)   */
    "`M"         ,      /* `M  k_pf6       PF6             (null)   */
    "`N"         ,      /* `N  k_pf7       PF7             (null)   */
    "`O"         ,      /* `O  k_pf8       PF8             (null)   */
    "`P"         ,      /* `P  k_pf9       PF9             (null)   */
    "`Q"         ,      /* `Q  k_pf10      PF10            (null)   */
    "`R"         ,      /* `R  k_pf11      PF11            (null)   */
    "`S"         ,      /* `S  k_pf12      PF12            (null)   */
    ""           ,      /* `T  k_shpf1     PF1   Shift     (null)   */
    "`U"         ,      /* `U  k_shpf2     PF2   Shift     (null)   */
    "`V"         ,      /* `V  k_shpf3     PF3   Shift     (null)   */
    "`W"         ,      /* `W  k_shpf4     PF4   Shift     (null)   */
    "`X"         ,      /* `X  k_shpf5     PF5   Shift     (null)   */
    "`Y"         ,      /* `Y  k_shpf6     PF6   Shift     (null)   */
    "`Z"         ,      /* `Z  k_shpf7     PF7   Shift     (null)   */
    "`["         ,      /* `[  k_shpf8     PF8   Shift     (null)   */
    "`\\"        ,      /* `\  k_shpf9     PF9   Shift     (null)   */
    "`]"         ,      /* `]  k_shpf10    PF10  Shift     (null)   */
    "`^"         ,      /* `^  k_shpf11    PF11  Shift     (null)   */
    "`_"         ,      /* `_  k_shpf12    PF12  Shift     (null)   */
    "\033\134",         /* dc terminator                        */
    0 } ;                   /* array terminator        */

#ifdef AAXL
static char *x_keysxl[] = {
    "\033P>",                 /*  XL DCS control.  */
    "|33;30;0;0;1|",        /* scrool shift                         */
    "|34;29;0;0;1|",        /* zoom   shift                         */
    "\033\134",             /* dc terminator           */
    0 } ;                   /* array terminator        */
#endif AAXL
#endif MKAAAS

