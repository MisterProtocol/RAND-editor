#ifdef COMMENT
Copyright abandoned, 1983, The Rand Corporation

File:           aanew.c
Terminal:       new Ann Arbor 40 line  (Model Q2878)
Terminal ID:    TERM=aa1
Author:         cbush
Date:           ? 1983

This file is almost a clone of annarbor.c.  The only distinguishing
difference is border bullets default off for aa1 and on for aa.

#endif


S_term t_aa1 = {
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
/* tt_wl      */    1,
/* tt_cwr     */    1,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    NO,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    40,
};


