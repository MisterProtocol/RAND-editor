#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

/*     Date:     Sun, 4 Jan 81 20:49-EST
 *     From:     Szurko at UDel
 *     Subject:  e.c100.c
 *     To:       day@rand-unix
 *
 *     Concept 100 driver
 */
/* From University of Delaware - reformatted by dave yost */

/*  Keyboard input analyzer  */
in_c100 (lexp, count)
char  *lexp;
int  *count;
{
    register char  *ip, *op;
    register int  nleft;
    char  c, *sp;
    int  nconv;

    ip = op = lexp;
    for (nleft = *count; nleft > 0; ) {
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

	case  '\10':                               /*  BACKSPACE  */
	case  '\27':
	    *op++ = CCBACKSPACE;
	    break;

	case  '\11':                               /*  TAB  */
	    *op++ = CCTAB;
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

	case  '\33':
	    /*  ESC is leadin for 2 character sequences  */
	    if (nleft < 2)
		goto  nomore;
	    switch (c = *ip++ & 0177) {
	    case  ';':                         /*  UP ARROW  */
		*op++ = CCMOVEUP;
		break;

	    case  '<':                         /*  DOWN ARROW  */
		*op++ = CCMOVEDOWN;
		break;

	    case  '>':                         /*  LEFT ARROW  */
		*op++ = CCMOVELEFT;
		break;

	    case  '=':                         /*  RIGHT ARROW  */
		*op++ = CCMOVERIGHT;
		break;

	    case  '?':                         /*  HOME  */
		*op++ = CCHOME;
		break;

	    case  '.':                         /*  -PAGE  */
		*op++ = CCMIPAGE;
		break;

	    case  '\\':                        /*  -SCROLL  */
		*op++ = CCMILINE;
		break;

	    case  '[':                         /*  +SCROLL  */
		*op++ = CCPLLINE;
		break;

	    case  '-':                         /*  +PAGE  */
		*op++ = CCPLPAGE;
		break;

	    case  ']':
		*op++ = CCTABS;
		break;

	    default:
		*op++ = CCUNAS1;
		break;
	    }
	    nleft -= 2;
	    continue;

	case  '\34':
	    /*  check for <ctrl>-\ X <ctrl>M where X is a single  */
	    /*  letter function code.                             */
	    if (nleft < 3)
		goto  nomore;
	    c = *ip++ & 0177;
	    if ((*ip++ & 0177) != '\15') {
		*op++ = CCUNAS1;
		break;
	    }
	    switch (c) {
	    case  '5':                         /*  F1  */
		*op++ = CCINSMODE;
		break;

	    case  '6':                         /*  F2  */
		*op++ = CCSETFILE;
		break;

	    case  '7':                         /*  F3  */
		*op++ = CCINT;
		break;

	    case  '8':                         /*  F4  */
		*op++ = CCREPLACE;
		break;

	    case  '9':                         /*  F5  */
		*op++ = CCUNAS1;
		break;

	    case  ':':                         /*  F6  */
		*op++ = CCMISRCH;
		break;

	    case  ';':                         /*  F7  */
		*op++ = CCPLSRCH;
		break;

	    case  '<':                         /*  F8  */
		*op++ = CCUNAS1;
		break;

	    case  '=':                         /*  F9  */
		*op++ = CCCLOSE;
		break;

	    case  '>':                         /*  F10  */
		*op++ = CCOPEN;
		break;

	    case  '?':                         /*  F11  */
		*op++ = CCPICK;
		break;

	    case  '@':                         /*  F12  */
		*op++ = CCCMD;
		break;

	    case  'A':                         /*  F13  */
		*op++ = CCDELCH;
		break;

	    case  'B':                         /*  F14  */
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

ini1_c100 () { fputs("\33U\33X", stdout); }
end_c100 () { fputs("\33x\33u", stdout); }
lt_c100()  { P ('\10'); }
rt_c100()  { fputs("\33=", stdout); }
dn_c100()  { fputs("\33<", stdout); }
up_c100()  { fputs("\33;", stdout); }
cr_c100()  { P ('\15'); }
nl_c100()  { fputs("\15\12", stdout); }
clr_c100() { P ('\14'); }
hm_c100()  { fputs("\33?", stdout); }
bsp_c100() { fputs("\10 \10", stdout); }
addr_c100(lin, col)
int  lin, col;
{
    fputs("\33a", stdout);
    P (lin + 040);
    P (col + 040);
}
xl_c100 (chr)
#ifdef UNSCHAR
Uchar chr;
#else
Uint chr;
#endif
{
#ifndef UNSCHAR
    chr &= 0377;
#endif
    if (chr == '_')
	fputs(" \10_", stdout);
    else {
	if (chr >= FIRSTSPCL)
	    chr = stdxlate[chr - FIRSTSPCL];
	P (chr);
    }
}
extern int kini_nocbreak();

S_kbd kb_c100 = {
/* kb_inlex */  in_c100,
/* kb_init  */  kini_nocbreak,
/* kb_end   */  nop,
};

S_term  t_c100 = {
/* tt_ini0    */    nop,
/* tt_ini1    */    ini1_c100,
/* tt_end     */    end_c100,
/* tt_left    */    lt_c100,
/* tt_right   */    rt_c100,
/* tt_dn      */    dn_c100,
/* tt_up      */    up_c100,
/* tt_cret    */    cr_c100,
/* tt_nl      */    nl_c100,
/* tt_clear   */    clr_c100,
/* tt_home    */    hm_c100,
/* tt_bsp     */    bsp_c100,
/* tt_addr    */    addr_c100,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xl_c100,
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
/* tt_bullets */    NO,
/* tt_prtok   */    NO,
/* tt_width   */    80,
/* tt_height  */    24
 };
