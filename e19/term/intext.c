#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


/****************************************/
/**** 5 = INTERACTIVE Systems INtext ****/

int
in_intext (lexp, count)
char *lexp;
int *count;
{
    register int nr;
    register int chr;
    register char *icp;
    register char *ocp;
    static char xlt[32] = {
	CCUNAS1     , /* 000 <break > -- not assigned --      */
	CCUNAS1     , /* 001 <cntr A> -- not assigned --      */
	CCUNAS1     , /* 002 <cntr B> -- not assigned --      */
	CCINT       , /* 003 <cntr C> interrupt               */
	CCUNAS1     , /* 004 <cntr D> -- not assigned --      */
	CCUNAS1     , /* 005 <cntr E> -- not assigned --      */
	CCUNAS1     , /* 006 <cntr F> -- not assigned --      */
	CCUNAS1     , /* 007 <cntr G> -- not assigned --      */
	CCBACKSPACE , /* 010 <cntr H> backspace               */
	CCTAB       , /* 011 <cntr I> tab                     */
	CCMOVEDOWN  , /* 012          downarrow               */
	CCUNAS1     , /* 013 <cntr K> -- not assigned --      */
	CCUNAS1     , /* 014 <cntr L> -- not assigned --      */
	CCRETURN    , /* 015 <cntr M> return                  */
	CCUNAS1     , /* 016 <cntr N> -- not assigned --      */
	CCUNAS1     , /* 017 <cntr O> -- not assigned --      */
	CCOPEN      , /* 020 <cntr P> open                    */
	CCCLOSE     , /* 021 <cntr Q> close                   */
	CCDELCH     , /* 022 <cntr R> del character           */
	CCUNAS1     , /* 023 <cntr S> -- not assigned --      */
	CCUNAS1     , /* 024 <cntr T> -- not assigned --      */
	CCUNAS1     , /* 025 <cntr U> -- not assigned --      */
	CCUNAS1     , /* 026 <cntr V> -- not used     --      */
	CCUNAS1     , /* 027 <cntr W> -- not assigned --      */
	CCUNAS1     , /* 030 <cntr X> -- not assigned --      */
	CCBACKTAB   , /* 031 <cntr Y> -- minus tab    --      */
	CCHOME      , /* 032 <cntr Z> -- home         --      */
	CCUNAS1     , /* 033 <escape> -- not assigned --      */
	CCMOVEUP    , /* 034 <cntr \> -- up arrow     --      */
	CCUNAS1     , /* 035 <cntr ]> -- not assigned --      */
	CCMOVERIGHT , /* 036 <cntr ^> -- right arrow  --      */
	CCMOVELEFT    /* 037 <cntr -> -- left arrow   --      */
    };

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--) {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) != 026) {
	    *ocp++ = chr >= ' ' ? chr : xlt[chr];
	}
	else {
	    if (nr < 2) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    if (chr == 0175  || chr == 0176)
		*ocp++ = CCINSMODE;
	    else {
		if (nr < 2) {
		    icp -= 2;
		    nr++;
		    goto nomore;
		}
		nr--;
		icp++; /*this character will always be 015; skip over it*/
		if ('A' <= chr && chr <= 'Z') Block {
		    static char xlt[] = {
			CCUNAS1     , /* A -- not assigned --  */
			CCUNAS1     , /* B -- not assigned --  */
			CCUNAS1     , /* C -- save         --  */
			CCSETFILE   , /* D -- use          --  */
			CCUNAS1     , /* E -- refresh      --  */
			CCUNAS1     , /* F -- window       --  */
			CCCHWINDOW  , /* G -- change window--  */
			CCUNAS1     , /* H -- do           --  */
			CCUNAS1     , /* I -- not assigned --  */
			CCUNAS1     , /* J -- not assigned --  */
			CCPICK      , /* K -- pick         --  */
			CCMARK      , /* L -- put          --  */
			CCCTRLQUOTE , /* M -- quote        --  */
			CCTABS      , /* N -- sr tab       --  */
			CCUNAS1     , /* O -- not assigned --  */
			CCUNAS1     , /* P -- restore      --  */
			CCPLPAGE    , /* Q -- +page        --  */
			CCPLLINE    , /* R -- +line        --  */
			CCPLSRCH    , /* S -- +srch        --  */
			CCMIPAGE    , /* T -- -page        --  */
			CCMILINE    , /* U -- -line        --  */
			CCMISRCH    , /* V -- -srch        --  */
			CCLWINDOW   , /* W -- left         --  */
			CCRWINDOW   , /* X -- right        --  */
			CCCMD       , /* Y -- enter        --  */
			CCREPLACE   , /* Z -- goto         --  */
		    };
		    *ocp++ = xlt[chr - 'A'];
		}
		else
		    *ocp++ = CCUNAS1;
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

lt_intext () { P (037); }
rt_intext () { P (036); }
dn_intext () { P (012); }
up_intext () { P (034); }
cr_intext () { P (015); }
nl_intext () { P (015); P (012); }
clr_intext () { P (014); }
hm_intext () { P (032); }
bsp_intext () { P (010); }
addr_intext (lin, col) { P (017); P (040 + lin); P (040 + col); }
ini1_intext ()
{
    static char ini[] = {026, 072, 026, 064, 026, 045};
    fwrite (ini, sizeof ini, 1, stdout);
}
end_intext () { P (026);P ('9'); }
xl_intext (chr)
#ifdef UNSCHAR
Uchar chr;
#else
Uint chr;
#endif
{
    /* LMCH   left */
    /* RMCH   right */
    /* MLMCH  more left */
    /* MRMCH  more right */
    /* TMCH   top */
    /* BMCH   bottom */
    /* TLCMCH top left corner */
    /* TRCMCH top right corner */
    /* BLCMCH bottom left corner */
    /* BRCMCH bottom right corner */
    /* TTMCH  top tab */
    /* BTMCH  bottom tab */
    /* ELMCH  empty left */
    /* INMCH  inactive */
    static char bd[] = {7, 7, 0127, 0123, 3, 3, 3, 3, 3, 3, 3, 3, 47};

#ifndef UNSCHAR
    chr &= 0377;
#endif
    if (   chr >= FIRSTMCH
	&& fast
       ) {
	if (chr == INMCH)
	    P ('.');
	else {
	    P (026);
	    P (044);
	    P (bd[chr - FIRSTMCH]);
	}
    }
    else
	P (stdxlate[chr - FIRSTSPCL]);
}
extern int kini_nocbreak();

S_kbd kb_intext = {
/* kb_inlex */  in_intext,
/* kb_init  */  kini_nocbreak,
/* kb_end   */  nop,
};

S_term t_intext = {
/* tt_ini0    */    nop,
/* tt_ini1    */    ini1_intext,
/* tt_end     */    end_intext,
/* tt_left    */    lt_intext,
/* tt_right   */    rt_intext,
/* tt_dn      */    dn_intext,
/* tt_up      */    up_intext,
/* tt_cret    */    cr_intext,
/* tt_nl      */    nl_intext,
/* tt_clear   */    clr_intext,
/* tt_home    */    hm_intext,
/* tt_bsp     */    bsp_intext,
/* tt_addr    */    addr_intext,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xl_intext,
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
/* tt_nbsp    */    1,
/* tt_naddr   */    3,
/* tt_nlad    */    0,
/* tt_ncad    */    0,
/* tt_wl      */    2,
/* tt_cwr     */    2,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    YES,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
};
