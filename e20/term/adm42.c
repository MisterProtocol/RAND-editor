#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

/*     Date:     Sun, 4 Jan 81 20:49-EST
 *     From:     Szurko at UDel
 *     Subject:  e.adm42.c
 *     To:       day@rand-unix
 *
 *     ADM-42 terminal driver
 */
/* From University of Delaware - reformatted by dave yost */

/*  Keyboard input analyzer  */
in_adm42 (lexp, count)
char  *lexp;
int  *count;
{
    register char  *ip, *op;
    register int  nleft;
    char  c, *sp;
    int  nconv;

    ip = op = lexp;
    nleft = *count;

    while (nleft > 0) {
	sp = ip;
	c = *ip++ & 0177;
	switch (c) {
	case  '\3':                                /*  CTRL C  */
	    *op++ = CCINT;
	    break;

	case  '\5':                                /*  CTRL E  */
	    *op++ = CCLWINDOW;
	    break;

	case  '\7':                                /*  CTRL G  */
	    *op++ = CCSETFILE;
	    break;

	case  '\10':                               /*  LEFT ARROW  */
	    *op++ = CCMOVELEFT;
	    break;

	case  '\11':                               /*  TAB  */
	    *op++ = CCTAB;
	    break;

	case '\12':                                /*  DOWN ARROW  */
	    *op++ = CCMOVEDOWN;
	    break;

	case  '\13':                               /*  UP ARROW  */
	    *op++ = CCMOVEUP;
	    break;

	case  '\14':                               /*  RIGHT ARROW  */
	    *op++ = CCMOVERIGHT;
	    break;

	case  '\15':                               /*  CR  */
	    *op++ = CCRETURN;
	    break;

	case  '\20':                               /*  CTRL P  */
	    *op++ = CCCHWINDOW;
	    break;

	case  '\21':                               /*  CTRL  Q  */
	    *op++ = CCCTRLQUOTE;
	    break;

	case  '\23':                               /*  CTRL  S  */
	    *op++ = CCRWINDOW;
	    break;

	case  '\24':                               /*  CTRL T  */
	    *op++ = CCBACKTAB;
	    break;

	case  '\27':                               /*  CTRL W  */
	case  '\177':                              /*  DELETE  */
	    *op++ = CCBACKSPACE;
	    break;

	case  '\33':                               /*  ESCAPE  */
	    *op++ = CCDELCH;
	    break;

	case  '\36':                               /*  HOME  */
	    *op++ = CCHOME;
	    break;

	case  '\37':                               /*  NEW LINE  */
	    *op++ = CCCMD;
	    break;

	case  '\1':
	    /*  check for <ctrl>A X <ctrl>M where X is a single  */
	    /*  letter function code.                            */
	    if (nleft < 3)
		goto  nomore;
	    c = *ip++ & 0177;
	    if ((*ip++ & 0177) != '\15')
		{
		*op++ = CCUNAS1;
		break;
		}
	    switch (c) {
	    case  '@':                         /*  F1  */
		*op++ = CCINSMODE;
		break;

	    case  'A':                         /*  F2  */
		*op++ = CCPLLINE;
		break;

	    case  'B':                         /*  F3  */
		*op++ = CCMILINE;
		break;

	    case  'C':                         /*  F4  */
		*op++ = CCINT;
		break;

	    case  'D':                         /*  F5  */
		*op++ = CCPLSRCH;
		break;

	    case  'E':                         /*  F6  */
		*op++ = CCMISRCH;
		break;

	    case  'F':                         /*  F7  */
		*op++ = CCSETFILE;
		break;

	    case  'G':                         /*  F8  */
		*op++ = CCREPLACE;
		break;

	    case  'H':                         /*  F9  */
		*op++ = CCPICK;
		break;

	    case  'I':                         /*  F10  */
		*op++ = CCOPEN;
		break;

	    case  'J':                         /*  F11  */
		*op++ = CCCLOSE;
		break;

	    case  'K':                         /*  F12  */
		*op++ = CCPLPAGE;
		break;

	    case  'L':                         /*  F13  */
		*op++ = CCMIPAGE;
		break;

	    case  'M':                         /*  F14  */
		*op++ = CCMARK;
		break;

	    default:
		*op++ = CCUNAS1;
	    }

	    nleft -= 3;
	    continue;

	default:
	    if (c >= ' ')
		*op++ = c;
	    else
		*op++ = CCUNAS1;
	}

	nleft--;
	continue;
	}

/*  normal finish -- all input characters converted  */

    *count = 0;
    nconv = op - lexp;
    return nconv;

/*  we come here when there are insufficent characters to parse  */

 nomore:
    *count = nleft;
    nconv = op - lexp;
    ip = sp;

    while (nleft-- > 0)
	*op++ = *ip++;

    return nconv;
}

/*  move cursor one space to the left  */

lt_adm42() { P ('\10'); }
rt_adm42() { P ('\14'); }
dn_adm42() { P ('\12'); }
up_adm42() { P ('\13'); }
cr_adm42() { P ('\15'); }
nl_adm42() { P ('\15'); P ('\12'); }
clr_adm42() { fputs("\36\33;", stdout); }
hm_adm42() { P ('\36'); }
bsp_adm42() { fputs("\10 \10", stdout);}
addr_adm42(lin, col)
register int  lin, col;
{
    fputs("\33=", stdout);
    P (lin + 040);
    P (col + 040);
}

S_kbd kb_adm42 = {
/* kb_inlex */  in_adm42,
/* kb_init  */  nop,
/* kb_end   */  nop,
};

S_term  t_adm42 = {
/* tt_ini0    */    nop,
/* tt_ini1    */    nop,
/* tt_end     */    nop,
/* tt_left    */    lt_adm42,
/* tt_right   */    rt_adm42,
/* tt_dn      */    dn_adm42,
/* tt_up      */    up_adm42,
/* tt_cret    */    cr_adm42,
/* tt_nl      */    nl_adm42,
/* tt_clear   */    clr_adm42,
/* tt_home    */    hm_adm42,
/* tt_bsp     */    bsp_adm42,
/* tt_addr    */    addr_adm42,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xlate1,
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
/* tt_help    */    (int (*) ()) 0,
#endif LMCHELP
#ifdef LMCVBELL
/* tt_vbell   */    (int (*) ()) 0,
#endif LMCVBELL
/* tt_nleft   */    1,
/* tt_nright  */    1,
/* tt_ndn     */    1,
/* tt_nup     */    1,
/* tt_nnl     */    2,
/* tt_nbsp    */    3,
/* tt_naddr   */    4,
/* tt_nlad    */    0,
/* tt_ncad    */    0,
/* tt_wl      */    1,
/* tt_cwr     */    1,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    NO,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
 };
