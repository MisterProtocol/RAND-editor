#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


/****************************************/
/* 0 = ann arbor old and new */

extern Cmdret help_std();

int
in_aa (lexp, count)
Uchar *lexp;
int *count;
{
    register int nr;
    register Uchar *cp;
    register Uint chr;
    static Uchar xlt1[64] = {
	CCCMD       , /* 000 enter parameter         */
	CCCMD       , /* 001 ^a                      */
	CCUNAS1     , /* 002 -- not assigned --      */
	CCUNAS1     , /* 003 -- not assigned --      */
	CCUNAS1     , /* 004 -- not assigned --      */
	CCUNAS1     , /* 005 -- not assigned --      */
	CCUNAS1     , /* 006 -- not assigned --      */
	CCUNAS1     , /* 007 -- not assigned --      */
	CCUNAS1     , /* 010 -- not assigned --      */
	CCUNAS1     , /* 011 -- not assigned --      */
	CCUNAS1     , /* 012 -- not assigned --      */
	CCUNAS1     , /* 013 -- not assigned --      */
	CCUNAS1     , /* 014 -- not assigned --      */
	CCUNAS1     , /* 015 -- not assigned --      */
	CCUNAS1     , /* 016 -- not assigned --      */
	CCUNAS1     , /* 017 -- not assigned --      */
	CCUNAS1     , /* 020 -- not assigned --      */
	CCCMD       , /* PF12                        */
	CCUNAS1     , /* 022 -- not assigned --      */
	CCUNAS1     , /* 023 -- not assigned --      */
	CCUNAS1     , /* 024 -- not assigned --      */
	CCUNAS1     , /* 025 -- not assigned --      */
	CCUNAS1     , /* 026 -- not assigned --      */
	CCUNAS1     , /* 027 -- not assigned --      */
	CCUNAS1     , /* 030 -- not assigned --      */
	CCUNAS1     , /* 031 -- not assigned --      */
	CCINT       , /* 032 interrupt               */
	CCTABS      , /* 033 set tabs                */
	CCLWINDOW   , /* 034 window left             */
	CCRWINDOW   , /* 035 window right            */
	CCCHWINDOW  , /* 036 change window           */
	CCSETFILE   , /* 037 set file                */
	CCMARK      , /* 040 mark a spot             */
	CCMILINE    , /* 041 minus a line            */
	CCREPLACE   , /* 042 replace                 */
	CCPLLINE    , /* 043 plus a line             */
	CCPICK      , /* 044 pick                    */
	CCCLOSE     , /* 045 delete                  */
	CCOPEN      , /* 046 insert                  */
	CCMIPAGE    , /* 047 minus a page            */
	CCHOME      , /* 050 home cursor             */
	CCPLPAGE    , /* 051 plus a page             */
	CCMISRCH    , /* 052 minus search            */
	CCMOVEUP    , /* 053 move up 1 lin           */
	CCPLSRCH    , /* 054 plus search             */
	CCMOVELEFT  , /* 055 move left               */
	CCMOVEDOWN  , /* 056 move down 1 line        */
	CCMOVERIGHT , /* 057 move right              */
	CCINSMODE   , /* 060 insert mode             */
	CCUNAS1     , /* 061 -- not assigned --      */
	CCDELCH     , /* 062 character delete        */

	CCUNAS1     , /* 063 -- not assigned --      */
	CCUNAS1     , /* 064 -- not assigned --      */
	CCUNAS1     , /* 065 -- not assigned --      */
	CCUNAS1     , /* 066 -- not assigned --      */
	CCUNAS1     , /* 067 -- not assigned --      */
	CCUNAS1     , /* 070 -- not assigned --      */
	CCUNAS1     , /* 071 -- not assigned --      */
	CCUNAS1     , /* 072 -- not assigned --      */
	CCUNAS1     , /* 073 -- not assigned --      */
	CCUNAS1     , /* 074 -- not assigned --      */
	CCUNAS1     , /* 075 -- not assigned --      */
	CCUNAS1     , /* 076 -- not assigned --      */
	CCUNAS1     , /* 077 -- not assigned --      */
    };

    cp = lexp;
    nr = *count;
    while (nr-- > 0) {
#ifdef UNSCHAR
	if ((chr = *cp) >= 0240)
#else
	if ((chr = *cp & 0377) >= 0240)
#endif
	    *cp++ = chr & 0177;
	else {
	    if (chr >= 0200)
		*cp++ = chr & 037;
	    else
		*cp++ = xlt1[chr & 077];
	}
    }

    nr = *count;
    *count = 0;
    return nr;
}

lt_aa () { P ('h' & 31); }
rt_aa () { P ('_' & 31); }
dn_aa () { P (012); }
up_aa () { P (016); }
cr_aa () { P (015); }
nl_aa () { P (015); P (012); }
clr_aa () { P (014);P (013);P (013); }
hm_aa () { P (013); }
bsp_aa () { P (036); }
addr_aa (lin, col)
{
    P (017);
    P (col ? 16 * (col / 10) + col % 10 : 0);
    P ((lin < 20) ? 0100 + lin : 0140 + (lin - 20));
}
extern int kini_nocbreak();


aa_kini()
{

	kini_nocbreak();

	/*
	 * turn off bullets for the newer model
	 * annarbor terminals (Model Q2878)
	 */
	if( !strcmp( tname, "aa1" ))
	    term.tt_bullets = NO;

	return;
}


S_kbd kb_aa = {
/* kb_inlex */  in_aa,
/* kb_init  */  aa_kini,
/* kb_end   */  nop,
};

S_term t_aa = {
/* tt_ini0    */    nop,
/* tt_ini1    */    nop,
/* tt_end     */    nop,
/* tt_left    */    lt_aa,
/* tt_right   */    rt_aa,
/* tt_dn      */    dn_aa,
/* tt_up      */    up_aa,
/* tt_cret    */    cr_aa,
/* tt_nl      */    nl_aa,
/* tt_clear   */    clr_aa,
/* tt_home    */    hm_aa,
/* tt_bsp     */    bsp_aa,
/* tt_addr    */    addr_aa,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xlate0,
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
/* tt_wl      */    1,
/* tt_cwr     */    1,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    YES,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    40,
};


