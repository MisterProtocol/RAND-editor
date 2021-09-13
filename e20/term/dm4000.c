#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


/***********************************/
/* 7 = _dm4000 (one-of-a-kind)      */

lt_dm4000 () { P ('h' & 31); }
rt_dm4000 () { P ('\\' & 31); }
dn_dm4000 () { P (012); }
up_dm4000 () { P ('Z' & 31); }
cr_dm4000 () {
    if (olin == 23) {
	P('L' & 31); P(' '); P(' ' + 23);
    } else {
	P(015); P('Z' & 31);
    }
}

nl_dm4000 () { P (015); }
clr_dm4000 () { P ('B' & 31); P ('_' & 31); P (0177); }
hm_dm4000 () { P ('B' & 31); }
bsp_dm4000 () { P ('h' & 31); P (' '); P ('h' & 31); }
addr_dm4000 (lin,col) { P ('l' & 31); P (col + ' '); P (lin + ' '); }
ini1_dm4000 () { clr_dm4000(); }
/* lad not used */
/* cad not used */
xl_dm4000 (chr)
#ifdef UNSCHAR
Uchar chr;
#else
Uint chr;
#endif
{
#ifdef UNSCHAR
    chr &= 0377;
#endif
    if (chr == ESCCHAR)
	P ('^');
    else if (chr == BULCHAR)
	P ('@');
    else
	P (stdxlate[chr-FIRSTSPCL]);
}

S_term t_dm4000 = {
/* tt_ini0    */    nop,
/* tt_ini1    */    ini1_dm4000,
/* tt_end     */    nop,
/* tt_left    */    lt_dm4000,
/* tt_right   */    rt_dm4000,
/* tt_dn      */    dn_dm4000,
/* tt_up      */    up_dm4000,
/* tt_cret    */    cr_dm4000,
/* tt_nl      */    nl_dm4000,
/* tt_clear   */    clr_dm4000,
/* tt_home    */    hm_dm4000,
/* tt_bsp     */    bsp_dm4000,
/* tt_addr    */    addr_dm4000,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xl_dm4000,
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
/* tt_nbsp    */    3,
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
/* tt_height  */    24,
};

