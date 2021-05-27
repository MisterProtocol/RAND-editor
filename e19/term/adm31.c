#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


/****************************************/
/* lear siegler adm31 ****/

lt_31 () { P ('h' & 31); }
rt_31 () { P ('l' & 31); }
dn_31 () { P (012); }
up_31 () { P ('k' & 31); }
cr_31 () { P (015); }
nl_31 () { P (015); P (012); }
clr_31 () { P (033); P ('*'); }
hm_31 () { P ('^' & 31); }
bsp_31 () { P ('h' & 31); P (' '); P ('h' & 31); }
addr_31 (lin, col) { P (033); P ('='); P (lin + 040); P (col + 040); }
S_term t_31 = {
/* tt_ini0    */    nop,
/* tt_ini1    */    nop,
/* tt_end     */    nop,
/* tt_left    */    lt_31,
/* tt_right   */    rt_31,
/* tt_dn      */    dn_31,
/* tt_up      */    up_31,
/* tt_cret    */    cr_31,
/* tt_nl      */    nl_31,
/* tt_clear   */    clr_31,
/* tt_home    */    hm_31,
/* tt_bsp     */    bsp_31,
/* tt_addr    */    addr_31,
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
/* tt_help    */    help_std (),
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

