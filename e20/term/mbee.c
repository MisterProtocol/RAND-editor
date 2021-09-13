/*
**      E editor handler for the Beehive MicroBee terminal.
**
**      Note: Assumed switch positions:
**
**              No auto-echo, Roll mode, No auto-linefeed,
**              No Lower case inhibit, CR is termination char.
**
**      Lyle McElhaney, Littleton, Co.
*/

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1

extern Cmdret help_std();

in_mbee (lexp, count)
char *lexp;
int *count;
{
    Reg1 int chr;
    Reg2 int nr;
    Reg3 char *icp;
    Reg4 char *ocp;

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--)
    {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) >= 040)
	{
	    if (chr == 0177)         /* DEL KEY KLUDGE FOR NOW */
		*ocp++ = CCBACKSPACE;
	    else
		*ocp++ = chr;
	}
	else if (chr == 033)
	{
	    if (nr < 2)
	    {
		icp--;
		goto nomore;
	    }
	    chr = *icp++ & 0177; nr--;
	    switch (chr) {
	    case 033:
		*ocp++ = CCCTRLQUOTE;
		break;
	    case '0':
		*ocp++ = CCAUTOFILL;
		break;
	    case '(':
		*ocp++ = CCCCASE;
		break;
	    case '#':
		*ocp++ = CCREDRAW;
		break;
	    case ',':
		*ocp++ = CCSTOPX;
		break;
	    case '!':
		*ocp++ = CCJOIN;
		break;
	    case '>':
		*ocp++ = CCBACKTAB;
		break;
	    case 'A':
		*ocp++ = CCMOVEUP;
		break;
	    case 'B':
		*ocp++ = CCMOVEDOWN;
		break;
	    case 'C':
		*ocp++ = CCMOVERIGHT;
		break;
	    case 'D':
		*ocp++ = CCMOVELEFT;
		break;
	    case 'H':
		*ocp++ = CCHOME;
		break;
	    case 'K':
		*ocp++ = CCCLEAR;
		break;
	    case 'L':
		*ocp++ = CCOPEN;
		break;
	    case 'M':
		*ocp++ = CCCLOSE;
		break;
	    case 'N':
		*ocp++ = CCSPLIT;
		break;
	    case 'P':
		*ocp++ = CCDELCH;
		break;
	    case 'Q':
		*ocp++ = CCINSMODE;
		break;
	    case 'W':
		*ocp++ = CCFILL;
		break;
	    case 'd':
		*ocp++ = CCJUSTIFY;
		break;
	    case 'g':
		*ocp++ = CCCENTER;
		break;
	    case 'j':
		*ocp++ = CCBOX;
		break;
	    default:
		*ocp++ = CCUNAS1;
		break;
	    }
	}
	else if (chr == 027)
	    *ocp++ = CCINT;

	else if (chr == 02)
	{
	    if (nr < 4)
	    {
		icp--;
		goto nomore;
	    }
	    nr --; if ((*icp++ & 0177) == 033) {
		nr --; chr = *icp++ & 0177;
		if ('p' <= chr && chr <= 0177) Block {
		    static char xlt[] = {
			CCHELP,     /* fk1  */
			CCCHWINDOW, /* fk2  */
			CCPICK,     /* fk3  */
			CCREPLACE,  /* fk4  */
			CCSETFILE,  /* fk5  */
			CCMARK,     /* fk6  */
			CCPLLINE,   /* fk7  */
			CCMILINE,   /* fk8  */
			CCPLPAGE,   /* fk9  */
			CCMIPAGE,   /* fk10 */
			CCLWINDOW,  /* fk11 */
			CCRWINDOW,  /* fk12 */
			CCPLSRCH,   /* fk13 */
			CCMISRCH,   /* fk14 */
			CCLWORD,    /* fk15 */
			CCRWORD     /* fk16 */
		    };
		    *ocp++ = xlt [chr - 'p'];
		    icp++; nr--; /* blow away cr */
		}
	    }
	    else
		*ocp++ = CCUNAS1;
	}
	else                       /* control characters */
	    if ('\0' <= chr && chr <= '\037') Block
	    {
		static char xlt[] =
		{
		    CCCMD,          /* ^@ */
		    CCCCASE,        /* ^A */
		    CCNULL,         /* ^B   preempted by function keys */
		    CCCLEAR,        /* ^C */
		    CCCENTER,       /* ^D */
		    CCERASE,        /* ^E */
		    CCCAPS,         /* ^F */
		    CCHOME,         /* ^G */
		    CCBACKSPACE,    /* ^H */
		    CCTAB,          /* ^I */
		    CCNULL,         /* ^J   to accomodate cr-lf lovers */
		    CCAUTOFILL,     /* ^K */
		    CCBLOT,         /* ^L */
		    CCRETURN,       /* ^M */
		    CCRWORD,        /* ^N */
		    CCOVERLAY,      /* ^O */
		    CCCOVER,        /* ^P */
		    CCUNAS1,        /* ^Q */
		    CCREDRAW,       /* ^R */
		    CCUNAS1,        /* ^S */
		    CCTABS,         /* ^T */
		    CCHELP,         /* ^U */
		    CCTRACK,        /* ^V */
		    CCNULL,         /* ^W    preempted by "send" key */
		    CCEXIT,         /* ^X */
		    CCCLRTABS,      /* ^Y */
		    CCSTOPX,        /* ^Z */
		    CCNULL,         /* ^[    preempted by esc sequences */
		    CCINT,          /* ^\ */
		    CCUNAS1,        /* ^] */
		    CCCTRLQUOTE,    /* ^^ */
		    CCRANGE         /* ^_ */
		};
		*ocp++ = xlt [chr - '\0'];  /* Yeh, I know... */
	    }
	    else
		*ocp++ = CCUNAS1;
    }
nomore:
    Block {
	int conv;
	*count = nr;     /* number left over - still raw */
	conv = ocp - lexp;
	while (nr-- > 0)
	    *ocp++ = *icp++;
	return conv;
    }
}


xl_mbee (chr)

#ifdef UNSCHAR
	Uchar chr;
#else
	Uint chr;
#endif
{
	static char spch[] = {033, 'R', 0, 033, 'S'};
	static char chrs[] = {
		'j',            /* control-quote        */
		'h',            /* bullet               */
		'd',            /* left margin          */
		'd',            /* right margin         */
		'T',            /* left margin w/ more  */
		'X',            /* right margin w/ more */
		'`',            /* top margin           */
		'`',            /* bottom margin        */
		'@',            /* top left corner      */
		'D',            /* top right corner     */
		'H',            /* bottom left corner   */
		'L',            /* bottom right corner  */
		'\\',           /* top tab margin       */
		'P',            /* bottom tab margin    */
		'|',            /* empty left margin    */
		'.'             /* inactive margin      */
	};

#ifndef UNSCHAR
	chr &= 0377;
#endif
    if (chr == ESCCHAR)
	fwrite ("\0336\07", 3, 1, stdout);
    else {
	spch [2] = chrs [chr - FIRSTSPCL];
	fwrite (spch, sizeof spch, 1, stdout);
    }
}

rt_mbee ()  { fwrite ("\033C", 3, 1, stdout); }
lt_mbee ()  { fwrite ("\033D", 3, 1, stdout); }
dn_mbee ()  { fwrite ("\033B", 3, 1, stdout); }
up_mbee ()  { fwrite ("\033A", 3, 1, stdout); }
cr_mbee ()  { P (015); }
nl_mbee ()  { fwrite ("\015\012", 2, 1, stdout); }
clr_mbee () { fwrite ("\033E\033H", 4, 1, stdout); }
hm_mbee ()  { fwrite ("\033H", 2, 1, stdout); }
bsp_mbee () { fwrite ("\010 \010", 3, 1, stdout); }
ad_mbee (lin, col) { printf ("\033F%c%c", lin + ' ', col + ' '); }

S_kbd kb_mbee = {
/* kb_inlex */  in_mbee,
/* kb_init  */  nop,
/* kb_end   */  nop,
};

S_term t_mbee = {
/* tt_ini0    */    nop,
/* tt_ini1    */    nop,
/* tt_end     */    nop,
/* tt_left    */    lt_mbee,
/* tt_right   */    rt_mbee,
/* tt_dn      */    dn_mbee,
/* tt_up      */    up_mbee,
/* tt_cret    */    cr_mbee,
/* tt_nl      */    nl_mbee,
/* tt_clear   */    clr_mbee,
/* tt_home    */    hm_mbee,
/* tt_bsp     */    bsp_mbee,
/* tt_addr    */    ad_mbee,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xl_mbee,
/* tt_insline */    (int (*) ()) 0,
/* tt_delline */    (int (*) ()) 0,
/* tt_inschar */    (int (*) ()) 0,
/* tt_delchar */    (int (*) ()) 0,
/* tt_clreol  */    (int (*) ()) 0,
/* tt_defwin  */    (int (*) ()) 0,
/* tt_deflwin */    (int (*) ()) 0,
/* tt_erase   */    (int (*) ()) 0,
#ifdef LMCHELP
/* tt_so      */    (int (*) ()) 0,
/* tt_soe     */    (int (*) ()) 0,
/* tt_help    */    help_std,
#endif
#ifdef LMCVBELL
/* tt_vbell   */    (int (*) ()) 0,
#endif LMCVBELL
/* tt_nleft   */    2,
/* tt_nright  */    2,
/* tt_ndn     */    2,
/* tt_nup     */    2,
/* tt_nnl     */    2,
/* tt_nbsp    */    3,
/* tt_naddr   */    4,
/* tt_nlad    */    0,
/* tt_ncad    */    0,
/* tt_wl      */    1,
/* tt_cwr     */    1,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    YES,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
};
