#ifdef COMMENT
Copyright abandoned, 1983, The Rand Corporation
#endif


/****************************************/
/**** Sierra - dumb mode ****/

#define PX putterm
lt_dy () { P (8); }
rt_dy () { P (31); }
dn_dy () { P (18); }
up_dy () { P (14); }
cr_dy () { P (015); }
nl_dy () { P (10); }
clr_dy () { P (C('Z')); }
hm_dy () { P (11); }
bsp_dy () { P (30); }
addr_dy (lin, col) { P (25); PX (041 + lin); PX (041 + col); }
ini1_dy () {
    /* 40x80 mode */
 /* PX (167); P ('0'); */
    /* underscore off */
    PX (164); P ('0');
}
end_dy () {
    /* underscore on */
    PX (164); P ('1');
}
lad_dy (lin) { PX (174); PX (041 + lin); }
cad_dy (col) { PX (175); PX (041 + col); }
xl_dy (chr)
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
    static char bd[] = {
	151, 151, 153, 155, 150, 150, 159, 158, 156, 157, 152, 154, 155
    };

#ifndef UNSCHAR
    chr &= 0377;
#endif
    if (   chr >= FIRSTMCH
	&& fast
       )
	PX (bd[chr - FIRSTMCH]);
    else
	P (stdxlate[chr - FIRSTSPCL]);
}
putterm (chr)
int chr;
{
    if ((chr &= 0377) < 128)
	putchar (chr);
    else {
	putchar (chr < 128 + 64
		 ? 2
		 : 3
		);
	putchar ((chr & 63) + 64);
    }
    return;
}
S_term t_dy = {
/* tt_ini0    */    nop,
/* tt_ini1    */    ini1_dy,
/* tt_end     */    end_dy,
/* tt_left    */    lt_dy,
/* tt_right   */    rt_dy,
/* tt_dn      */    dn_dy,
/* tt_up      */    up_dy,
/* tt_cret    */    cr_dy,
/* tt_nl      */    nl_dy,
/* tt_clear   */    clr_dy,
/* tt_home    */    hm_dy,
/* tt_bsp     */    bsp_dy,
/* tt_addr    */    addr_dy,
/* tt_lad     */    lad_dy,
/* tt_cad     */    cad_dy,
/* tt_xlate   */    xl_dy,
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
/* tt_nnl     */    1,
/* tt_nbsp    */    1,
/* tt_naddr   */    3,
/* tt_nlad    */    0,
/* tt_ncad    */    0,
/* tt_wl      */    1,
/* tt_cwr     */    1,
/* tt_pwr     */    4,
/* tt_axis    */    3,
/* tt_bullets */    YES,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    40,
};

