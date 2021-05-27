#ifdef COMMENT
	Proprietary Rand Corporation, 1981.
	Further distribution of this software
	subject to the terms of the Rand
	license agreement.
#endif

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1
extern Cmdret help_std();

in_vt52 (lexp, count)
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
	    nr--;
	    chr = *icp++ & 0177;
	    switch (chr) {
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

	    case 'P':
		*ocp++ = CCCLOSE;
		break;

	    case 'Q':
		*ocp++ = CCPICK;
		break;

	    case 'R':
		*ocp++ = CCOPEN;
		break;

	    case '?':
		if (nr < 2)
		{
		    icp -= 2;
		    nr++;
		    goto nomore;
		}
		nr--;
		chr = *icp++ & 0177;
		if ('n' <= chr && chr <= 'y') Block
		{
		    static char xlt[] =
		    {
			CCDELCH        , /* n  . */
			CCUNAS1        , /* o    */
			CCINSMODE      , /* p  0 */
			CCMISRCH       , /* q  1 */
			CCPLSRCH       , /* r  2 */
			CCMARK         , /* s  3 */
			CCMIPAGE       , /* t  4 */
			CCPLPAGE       , /* u  5 */
			CCREPLACE      , /* v  6 */
			CCMILINE       , /* w  7 */
			CCPLLINE       , /* x  8 */
			CCSETFILE      , /* y  9 */
		    };
		    *ocp++ = xlt[chr - 'n'];
		}
		else if (chr = 'M')
		    *ocp++ = CCCMD;
		else
		    *ocp++ = CCUNAS1;
		break;
	    case 'a':                    /* characters following esc */
		*ocp++ = CCABORT;
		break;
	    case 'b':
		*ocp++ = CCBOX;
		break;
	    case 'f':
		*ocp++ = CCFILL;
		break;
	    case 'j':
		*ocp++ = CCJOIN;
		break;
	    case 'k':
		*ocp++ = CCJUSTIFY;
		break;
	    case 'q':
		*ocp++ = CCQUIT;
		break;
	    case 's':
		*ocp++ = CCSPLIT;
		break;
	    case 'x':
		*ocp++ = CCEXIT;
		break;
	    case 'z':
		*ocp++ = CCSTOP;
		break;
	    case 033:
		*ocp++ = CCCTRLQUOTE;
		break;
	    case '.':
		*ocp++ = CCRWINDOW;
		break;
	    case ',':
		*ocp++ = CCLWINDOW;
		break;
	    default:
		*ocp++ = CCUNAS1;
		break;
	    }
	}
	else                       /* control characters */
	    if ('\0' <= chr && chr <= '\036') Block
	    {
		static char xlt[] =
		{
		    CCCMD,          /* ^@ */
		    CCCCASE,        /* ^A */
		    CCLWORD,        /* ^B */
		    CCCLEAR,        /* ^C */
		    CCCENTER,       /* ^D */
		    CCERASE,        /* ^E */
		    CCCAPS,         /* ^F */
		    CCHOME,         /* ^G */
		    CCBACKSPACE,    /* ^H */
		    CCTAB,          /* ^I */
		    CCRETURN,       /* ^J */
		    CCAUTOFILL,     /* ^K */
		    CCBLOT,         /* ^L */
		    CCRETURN,       /* ^M */
		    CCRWORD,        /* ^N */
		    CCOVERLAY,      /* ^O */
		    CCCOVER,        /* ^P */
		    CCBACKTAB,      /* ^Q */
		    CCREDRAW,       /* ^R */
		    CCUNAS1,        /* ^S */
		    CCTABS,         /* ^T */
		    CCHELP,         /* ^U */
		    CCTRACK,        /* ^V */
		    CCCHWINDOW,     /* ^W */
		    CCEXIT,         /* ^X */
		    CCCLRTABS,      /* ^Y */
		    CCSTOPX,        /* ^Z */
		    CCUNAS1,        /* ^[    must be left unassigned */
		    CCINT,          /* ^\ */
		    CCUNAS1,        /* ^] */
		    CCCTRLQUOTE     /* ^^ */
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


xl_vt52 (chr)

#ifdef UNSCHAR
	Uchar chr;
#else
	Uint chr;
#endif
{
	static char esc[] = {033, 'F', 'a', 033, 'G'};

#ifndef UNSCHAR
	chr &= 0377;
#endif
	if (chr == ESCCHAR)
		fwrite (esc, sizeof esc, 1, stdout);
	else if (chr == BULCHAR)
		P('+');
	else
		xlate0 (chr);
}

ki_vt52 ()
{
    static char ini[] = {033, '=', 033, 'G'};
    fwrite (ini, sizeof ini, 1, stdout);
}

ke_vt52 ()
{
    static char end[] = {033,'>'};
    fwrite (end, sizeof end, 1, stdout);
}

rt_vt52 ()  { fwrite ("\033C", 2, 1, stdout); delay (2); }
lt_vt52 ()  { fwrite ("\010", 1, 1, stdout); delay (2); }
dn_vt52 ()  { fwrite ("\033B", 2, 1, stdout); delay (2); }
up_vt52 ()  { fwrite ("\033A", 2, 1, stdout); delay (2); }
cr_vt52 ()  { P (015); delay (2); }
nl_vt52 ()  { P (015); delay (2); P (012); delay (2); }
clr_vt52 () { fwrite ("\033H\033J", 4, 1, stdout); delay (1000); }
hm_vt52 ()  { fwrite ("\033H", 2, 1, stdout); delay (2); }
bsp_vt52 () { fwrite ("\010 \010", 3, 1, stdout); delay (5); }
ad_vt52 (lin, col) { printf ("\033Y%c%c", lin + ' ', col + ' '); delay (5); }

S_kbd kb_vt52 = {
/* kb_inlex */  in_vt52,
/* kb_init  */  ki_vt52,
/* kb_end   */  ke_vt52,
};

S_term t_vt52 = {
/* tt_ini0    */    nop,
/* tt_ini1    */    nop,
/* tt_end     */    nop,
/* tt_left    */    lt_vt52,
/* tt_right   */    rt_vt52,
/* tt_dn      */    dn_vt52,
/* tt_up      */    up_vt52,
/* tt_cret    */    cr_vt52,
/* tt_nl      */    nl_vt52,
/* tt_clear   */    clr_vt52,
/* tt_home    */    hm_vt52,
/* tt_bsp     */    bsp_vt52,
/* tt_addr    */    ad_vt52,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xl_vt52,
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
/* tt_nleft   */    1,
/* tt_nright  */    2,
/* tt_ndn     */    2,
/* tt_nup     */    2,
/* tt_nnl     */    2,
/* tt_nbsp    */    3,
/* tt_naddr   */    4,
/* tt_nlad    */    0,
/* tt_ncad    */    0,
/* tt_wl      */    3,
/* tt_cwr     */    3,
/* tt_pwr     */    3,
/* tt_axis    */    0,
/* tt_bullets */    YES,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
};
