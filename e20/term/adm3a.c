#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


/****************************************/
/**** 2 = lear siegler adm3a ****/

lt_3a () { P ('h' & 31); }
rt_3a () { P ('l' & 31); }
dn_3a () { P (012); }
up_3a () { P ('k' & 31); }
cr_3a () { P (015); }
nl_3a () { P (015); P (012); }
clr_3a () { P ('z' & 31); }
hm_3a () { P ('^' & 31); }
bsp_3a () { P ('h' & 31); P (' '); P ('h' & 31); }
addr_3a (lin, col) { P (033); P ('='); P (lin + 040); P (col + 040); }
S_term t_3a = {
/* tt_ini0    */    nop,
/* tt_ini1    */    nop,
/* tt_end     */    nop,
/* tt_left    */    lt_3a,
/* tt_right   */    rt_3a,
/* tt_dn      */    dn_3a,
/* tt_up      */    up_3a,
/* tt_cret    */    cr_3a,
/* tt_nl      */    nl_3a,
/* tt_clear   */    clr_3a,
/* tt_home    */    hm_3a,
/* tt_bsp     */    bsp_3a,
/* tt_addr    */    addr_3a,
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
/* tt_wl      */    0,
/* tt_cwr     */    0,
/* tt_pwr     */    0,
/* tt_axis    */    0,
/* tt_bullets */    NO,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
};

