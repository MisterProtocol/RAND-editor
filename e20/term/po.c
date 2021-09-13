#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

/****************************************/
/**** Perkin Elmer 1251 and owl Terminals  ***/

#ifdef NOTYET
int
in_po (lexp, count)
char *lexp;
int *count;
{
    register int nr;
    register int chr;
    register char *icp;
    register char *ocp;
    static char xlt[32] = {
	CCCMD			, /* 000 <BREAK > enter parameter 	*/
	CCLPORT 		, /* 001 <cntr A> port left		*/
	CCSETFILE		, /* 002 <cntr B> set file		*/
	CCINT			, /* 003 <cntr C> interrupt	    *** was chgwin */
	CCOPEN			, /* 004 <cntr D> insert			*/
	CCMISRCH		, /* 005 <cntr E> minus search		*/
	CCCLOSE 		, /* 006 <cntr F> delete			*/
	CCMARK			, /* 007 <cntr G> mark a spot	    *** was PUT */
	CCMOVELEFT		, /* 010 <cntr H> backspace		*/
	CCTAB			, /* 011 <cntr I> tab			*/
	CCMOVEDOWN      	, /* 012 <cntr J> move down 1 line        */
	CCHOME			, /* 013 <cntr K> home cursor		*/
	CCPICK			, /* 014 <cntr L> pick			*/
	CCRETURN        	, /* 015 <cntr M> return                  */
	CCMOVEUP		, /* 016 <cntr N> move up 1 lin		*/
	CCINSMODE		, /* 017 <cntr O> insert mode		*/
	CCREPLACE       	, /* 020 <cntr P> replace             *** was GOTO */
	CCMIPAGE        	, /* 021 <cntr Q> minus a page            */
	CCPLSRCH		, /* 022 <cntr R> plus search		*/
	CCRPORT 		, /* 023 <cntr S> port right		*/
	CCPLLINE		, /* 024 <cntr T> minus a line		*/
	CCDELCH 		, /* 025 <cntr U> character delete	*/
	CCUNAS1                 , /* 026 <cntr V> -- not assigned --  *** was SAVE */
	CCMILINE        	, /* 027 <cntr W> plus a line             */
	CCUNAS1 		, /* 030 <cntr X> -- not assigned --  *** was EXEC */
	CCPLPAGE        	, /* 031 <cntr Y> plus a page             */
	CCCHPORT		, /* 032 <cntr Z> change port	    *** was WINDOW */
	CCUNAS1                 , /* 033 <ESC > was set tabs            */
	CCCTRLQUOTE		, /* 034 <cntr \> knockdown next char	*/
	CCBACKTAB		, /* 035 <cntr ]> tab left		*/
	CCBACKSPACE		, /* 036 <cntr ^> backspace and erase	*/
	CCMOVERIGHT               /* 037 <cntr _> forward move          */
    };

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--) {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) != 033) {
	    *ocp++ = chr >= ' ' ? chr : xlt[chr];
	}
	else {
	    if (nr < 3) {
		icp--;
		goto nomore;
	    }
	    nr -= 2;
	    chr = *icp++ & 0177;
	    icp++;
     /*       if (chr == 0175  || chr == 0176)
		*ocp++ = CCINSMODE;
	    else {
		if (nr < 2) {
		    icp -= 2;
		    nr++;
		    goto nomore;
		}
	*/
		nr--;
		icp++; /*this character will always be 015; skip over it*/
		if ('A' <= chr && chr <= 'Z') {
		    static char xlt[] = {
			CCUNAS2     , /* A -- not assigned --  */
			CCUNAS2     , /* B -- not assigned --  */
			CCUNAS2     , /* C -- save         --  */
			CCSETFILE   , /* D -- use          --  */
			CCUNAS2     , /* E -- refresh      --  */
			CCUNAS2     , /* F -- window       --  */
			CCCHPORT    , /* G -- change window--  */
			CCUNAS2     , /* H -- do           --  */
			CCUNAS2     , /* I -- not assigned --  */
			CCUNAS2     , /* J -- not assigned --  */
			CCPICK      , /* K -- pick         --  */
			CCMARK      , /* L -- put          --  */
			CCCTRLQUOTE , /* M -- quote        --  */
			CCTABS      , /* N -- sr tab       --  */
			CCUNAS2     , /* O -- not assigned --  */
			CCUNAS2     , /* P -- restore      --  */
			CCPLPAGE    , /* Q -- +page        --  */
			CCPLLINE    , /* R -- +line        --  */
			CCPLSRCH    , /* S -- +srch        --  */
			CCMIPAGE    , /* T -- -page        --  */
			CCMILINE    , /* U -- -line        --  */
			CCMISRCH    , /* V -- -srch        --  */
			CCLPORT     , /* W -- left         --  */
			CCRPORT     , /* X -- right        --  */
			CCCMD       , /* Y -- enter        --  */
			CCREPLACE   , /* Z -- goto         --  */
		    };
		    *ocp++ = xlt[chr - 'A'];
		}
		else
		    *ocp++ = CCUNAS2;
	/*    }         */
	}
    }
 nomore:
    {
    int conv;
    *count = nr;     /* number left over - still raw */
    conv = ocp - lexp;
    while (nr-- > 0)
	*ocp++ = *icp++;
    return conv;
    }
}
#endif

lt_po () { P ('h' & 31); }
rt_po () { P (033); P('C'); }
dn_po () { P (012); }
up_po () { P (033); P('A'); }
cr_po () { P (015); }
nl_po () { P (015); P (012); }
clr_po () { P (033); P('K'); }
hm_po () { P (033); P('H'); }
bsp_po () { P ('h' & 31); P (' '); P ('h' & 31); }
addr_po (lin, col) { P (033); P ('X'); P (040 + lin);
	P (033); P ('Y'); P (040 + col); }

xlate_po (chr)
#ifdef UNSCHAR
Uchar chr;
#else
unsigned int chr;
#endif
{
#ifndef UNSCHAR
    chr &= 0377;
#endif
    if (chr == ESCCHAR || chr == BULCHAR) {
	P (033); P(0177);
    } else
	P (stdxlate[chr - FIRSTSPCL]);
}

S_term t_po = {
/* tt_ini0    */    nop,
/* tt_ini1    */    nop,
/* tt_end     */    nop,
/* tt_left    */    lt_po,
/* tt_right   */    rt_po,
/* tt_dn      */    dn_po,
/* tt_up      */    up_po,
/* tt_cret    */    cr_po,
/* tt_nl      */    nl_po,
/* tt_clear   */    clr_po,
/* tt_home    */    hm_po,
/* tt_bsp     */    bsp_po,
/* tt_addr    */    addr_po,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xlate_po,
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
/* tt_ndn     */    1,
/* tt_nup     */    2,
/* tt_nnl     */    2,
/* tt_nbsp    */    3,
/* tt_naddr   */    6,
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

