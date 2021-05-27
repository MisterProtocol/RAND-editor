#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


/****************************************/
/* heathkit h19 */

in_h19 (lexp, count)
char *lexp;
int *count;
{
    register int nr;
    register int chr;
    register char *icp;
    register char *ocp;
    static char xlt[32] = {
	CCUNAS1     , /* 000 <break > -- not assigned --      */
	CCLWINDOW   , /* 001 <cntr A> window left             */
	CCSETFILE   , /* 002 <cntr B> set file                */
	CCINT       , /* 003 <cntr C> interrupt               */
	CCUNAS1     , /* 004 <cntr D> -- not assigned --      */
	CCMIPAGE    , /* 005 <cntr E> minus a page            */
	CCUNAS1     , /* 006 <cntr F> -- not assigned --      */
	CCUNAS1     , /* 007 <cntr G> -- not assigned --      */
	CCBACKSPACE , /* 010 <cntr H> backspace               */
	CCTAB       , /* 011 <cntr I> tab                     */
	CCUNAS1     , /* 012 <cntr J> -- not assigned --      */
	CCUNAS1     , /* 013 <cntr K> -- not assigned --      */
	CCUNAS1     , /* 014 <cntr L> -- not assigned --      */
	CCRETURN    , /* 015 <cntr M> return                  */
	CCUNAS1     , /* 016 <cntr N> -- not assigned --      */
	CCUNAS1     , /* 017 <cntr O> -- not assigned --      */
	CCUNAS1     , /* 020 <cntr P> -- not assigned --      */
	CCMILINE    , /* 021 <cntr Q> minus a line            */
	CCPLPAGE    , /* 022 <cntr R> plus a page             */
	CCRWINDOW   , /* 023 <cntr S> window right            */
	CCMISRCH    , /* 024 <cntr T> minus search            */
	CCBACKTAB   , /* 025 <cntr U> tab left                */
	CCUNAS1     , /* 026 <cntr V> -- not assigned --      */
	CCPLLINE    , /* 027 <cntr W> plus a line             */
	CCUNAS1     , /* 030 <cntr X> -- not assigned --      */
	CCUNAS1     , /* 031 <cntr Y> -- not assigned --      */
	CCCHWINDOW  , /* 032 <cntr Z> change window           */
	CCUNAS1     , /* 033 <escape> -- not assigned --      */
	CCCTRLQUOTE , /* 034 <cntr \> knockdown next char     */
	CCTABS      , /* 035 <cntr ]> set tabs                */
	CCUNAS1     , /* 036 <cntr ^> -- not assigned --      */
	CCUNAS1     , /* 037 <cntr _> -- not assigned --      */
    };

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--) {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) != 033) {
	    *ocp++ = chr >= ' ' ? chr : xlt[chr];
	}
	else {
	    if (nr < 2) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    if (chr == '?') {
		if (nr < 2) {
		    icp -= 2;
		    nr++;
		    goto nomore;
		}
		nr--;
		chr = *icp++ & 0177;
		if ('n' <= chr && chr <= 'y') Block {
		    static char xlt[] = {
			CCMISRCH,
			CCUNAS1,
			CCPLSRCH,
			CCOPEN,
			CCMOVEDOWN,
			CCCLOSE,
			CCMOVELEFT,
			CCHOME,
			CCMOVERIGHT,
			CCINSMODE,
			CCMOVEUP,
			CCDELCH,
		    };
		    *ocp++ = xlt[chr - 'n'];
		}
		else if (chr == 'M')
		    *ocp++ = CCCMD;
		else
		    *ocp++ = CCUNAS1;
	    }
	    else
		switch (chr) {
		case 'P':
		    *ocp++ = CCSETFILE;
		    break;

		case 'Q':
		    *ocp++ = CCPICK;
		    break;

		case 'R':
		    *ocp++ = CCMARK;
		    break;

		case 'U':
		    *ocp++ = CCREPLACE;
		    break;

		default:
		    *ocp++ = CCUNAS1;
		    break;
		}
	}
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

lt_h19 () { P ('h' & 31); }
rt_h19 () { P (033); P ('C'); }
dn_h19 () { P (033); P ('B'); }
up_h19 () { P (033); P ('A'); }
cr_h19 () { P (015); }
nl_h19 () { P (015); P (012); }
clr_h19 () { P (033); P ('H'); P (033); P ('J'); }
hm_h19 () { P (033); P ('H'); }
bsp_h19 () { P ('h' & 31); P (' '); P ('h' & 31); }
addr_h19 (lin, col) { P (033); P ('Y'); P (lin + 040); P (col + 040); }
ini1_h19 ()
{
    static char ini[] = {033,'=',033,'v'/*, 033,'x','1' */};
    fwrite (ini, sizeof ini, 1, stdout);
}
end_h19 ()
{
    static char end[] = {033,'>',/* 033,'y','1' */};
    fwrite (end, sizeof end, 1, stdout);
 /* term.tt_height = 24; */
}
xl_h19 (chr)
#ifdef UNSCHAR
Uchar chr;
#else
Uint chr;
#endif
{
#ifndef UNSCHAR
    chr &= 0377;
#endif
    if (   chr == ESCCHAR
	|| chr == BULCHAR
       ) Block {
	static char bul[] = {033,'F','i',033,'G'};
	fwrite (bul, sizeof bul, 1, stdout);
    }
    else
	P (stdxlate[chr-FIRSTSPCL]);
}
il_h19 () { fwrite ("\033L", 2, 1, stdout); return 1; }
dl_h19 () { fwrite ("\033M", 2, 1, stdout); return 1; }
cle_h19 () { fwrite ("\033K", 2, 1, stdout); }
extern int kini_nocbreak();

S_kbd kb_h19 = {
/* kb_inlex */  in_h19,
/* kb_init  */  kini_nocbreak,
/* kb_end   */  nop,
};

S_term t_h19 = {
/* tt_ini0    */    nop,
/* tt_ini1    */    ini1_h19,
/* tt_end     */    end_h19,
/* tt_left    */    lt_h19,
/* tt_right   */    rt_h19,
/* tt_dn      */    dn_h19,
/* tt_up      */    up_h19,
/* tt_cret    */    cr_h19,
/* tt_nl      */    nl_h19,
/* tt_clear   */    clr_h19,
/* tt_home    */    hm_h19,
/* tt_bsp     */    bsp_h19,
/* tt_addr    */    addr_h19,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xl_h19,
/* tt_insline */    il_h19,
/* tt_delline */    dl_h19,
/* tt_inschar */    (int (*) ()) 0,
/* tt_delchar */    (int (*) ()) 0,
/* tt_clreol  */    cle_h19,
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
/* tt_wl      */    3,
/* tt_cwr     */    3,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    YES,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
};


