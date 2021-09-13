#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

/*todo fix keys: enter dollar
/*     more speed up features
*/

/*


		+--------+--------+--------+--------+
		| ONLINE | UC LCK | REFRSH | CENTER |
		+--------+--------+--------+--------+
		|  BREAK | CONFIG | CLEAR  | MARGIN |
		+--------+--------+--------+--------+


		+--------+--------+--------+--------+
		| FORMAT | WPMODE |  SAVE  |  USE   |
		+--------+--------+--------+--------+
		|  FONT  | WINDOW | CH WIN |  DO    |
		+--------+--------+--------+--------+

		+--------+--------+--------+--------+
		| PRINT  |        |  PICK  |  PUT   |
		+--------+--------+--------+--------+
		| CANCEL |  OPEN  |  CLOSE | RESTOR |
		+--------+--------+--------+--------+


		+--------+--------+--------+---------+
		| INSERT |  QUOTE |        | REPLCE  |
		+--------+--------+--------+---------+
		| DEL CH | TABSET |  HOME  |  GO TO  |
		+--------+--------+--------+---------+


		+--------+--------+--------+
		|        |        |        |
		| +PAGE  | +LINE  | +SRCH  |
		|        |        |        |
		+--------+--------+--------+
		|        |        |        |
		| -PAGE  | -LINE  | -SRCH  |
		|        |        |        |
		+--------+--------+--------+
		|        |   up   |        |
		|  LEFT  | arrow  |  RIGHT |
		|        |        |        |
		+--------+--------+--------+
		|  left  |  down  | right  |
		| arrow  | arrow  | arrow  |
		|        |        |        |
		+--------+--------+--------+

		 <ENTER>   <EXIT>
*/

# define Block

# define  MI_PAGE   CCMIPAGE
# define  MI_SRCH   CCMISRCH
# define  LEFT      CCUNAS1 /*unassigned*/
# define  RIGHT     CCUNAS1 /*unassigned*/
# define  UPARROW   CCMOVEUP
# define  RIGHTARROW CCMOVERIGHT
# define  LEFTARROW CCMOVELEFT
# define  DOWNARROW CCMOVEDOWN
# define  PICK      CCPICK
# define  PUT       CCMARK
# define  INSERT    CCINSMODE
# define  QUOTE     CCCTRLQUOTE
# define  BLANK1    CCPLLINE
# define  REPLCE    CCREPLACE
# define  MARGIN    ('$')
# define  FONT      CCUNAS1  /*unassigned*/
# define  WINDOW    CCUNAS1  /*unassigned*/
# define  CH_WIN    CCCHWINDOW
# define  DO        CCUNAS1  /*unassigned*/
# define  CANCEL    CCUNAS1  /*unassigned*/
# define  OPEN      CCOPEN
# define  CLOSE     CCCLOSE
# define  RESTOR    CCUNAS1  /*unassigned*/
# define  REFRSH    CCUNAS1  /*unassigned*/
# define  CENTER    CCUNAS1  /*unassigned*/
# define  FORMAT    CCUNAS1  /*unassigned*/
# define  WPMODE    CCUNAS1  /*unassigned*/
# define  SAVE      CCUNAS1  /*unassigned*/
# define  USE       CCSETFILE
# define  DEL_CH    CCDELCH
# define  TABSET    CCTABS
# define  HOME      CCHOME
# define  GOTO      CCUNAS1  /*unassigned*/
# define  PL_PAGE   CCPLPAGE
# define  PL_SRCH   CCPLSRCH
# define  BREAK     CCCMD
# define  ESCAPE    CCUNAS1  /*unassigned*/
# define  C_A       CCLWINDOW
# define  C_B       CCSETFILE
# define  C_C       CCINT
# define  C_D       CCMOVEDOWN
# define  C_E       CCUNAS1  /*unassigned*/
# define  C_F       CCUNAS1  /*unassigned*/
# define  C_G       CCUNAS1  /*unassigned*/
# define  C_H       CCBACKSPACE
# define  C_I       CCTAB
# define  C_J       CCCMD    /*LINEFEED*/
# define  C_K       CCUNAS1  /*unassigned*/
# define  C_L       CCMOVELEFT
# define  C_M       CCRETURN  /*also RETURN and ENTER*/
# define  C_N       CCUNAS1  /*unassigned*/
# define  C_O       CCUNAS1  /*unassigned*/
# define  C_P       CCPICK
# define  C_Q       CCUNAS1  /*unassigned*/
# define  C_R       CCMOVERIGHT
# define  C_S       CCRWINDOW
# define  C_T       CCUNAS1  /*unassigned*/
# define  C_U       CCMOVEUP
# define  C_V       CCUNAS1  /*unassigned*/
# define  C_W       CCUNAS1  /*unassigned*/
# define  C_X       CCUNAS1  /*unassigned*/
# define  C_Y       CCUNAS1  /*unassigned*/
# define  C_Z       CCCHWINDOW
# define  C32       CCUNAS1  /*unassigned*/
# define  C33       CCUNAS1  /*unassigned*/
# define  C34       CCCTRLQUOTE
# define  C35       CCUNAS1  /*unassigned*/
# define  C36       CCUNAS1  /*unassigned*/
# define  C37       CCUNAS1  /*unassigned*/
# define  MTAB      CCBACKTAB
# define IMPOSSIBLE CCUNAS1  /*unassigned*/
# define BLANK2     CCMILINE



int
in_2intext(lexp, count)
char *lexp;
int *count;
{
    register int nr;
    register int chr;
    register char *icp;
    register char *ocp;
    static char xlt[32] = {
	BREAK ,/* 000 <break >   */  /*$$$*/
	C_A , /* 001 <cntr A>   */
	C_B , /* 002 <cntr B>   */
	C_C , /* 003 <cntr C>   */
	C_D , /* 004 <cntr D>   */
	C_E , /* 005 <cntr E>   */
	C_F , /* 006 <cntr F>   */
	C_G , /* 007 <cntr G>   */
	C_H , /* 010 <cntr H>   */
	C_I , /* 011 <cntr I>   */
	C_J , /* 012 <cntr J>   */
	C_K , /* 013 <cntr K>   */
	C_L , /* 014 <cntr L>   */
	C_M , /* 015 <cntr M>   */
	C_N , /* 016 <cntr N>   */
	C_O , /* 017 <cntr O>   */
	C_P , /* 020 <cntr P>   */
	C_Q , /* 021 <cntr Q>   */
	C_R , /* 022 <cntr R>   */
	C_S , /* 023 <cntr S>   */
	C_T , /* 024 <cntr T>   */
	C_U , /* 025 <cntr U>   */
	C_V , /* 026 <cntr V>   */
	C_W , /* 027 <cntr W>   */
	C_X , /* 030 <cntr X>   */
	C_Y , /* 031 <cntr Y>   */
	C_Z , /* 032 <cntr Z>   */
	C33, /* 033 <escape>   */
	C34, /* 034 <cntr \>   */
	C35 , /* 035 <cntr ]>   */
	C36 , /* 036 <cntr ^>   */
	C37 , /* 037 <cntr ->   */
    };

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--) {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) != '\033' && (chr != '$')) {
	    *ocp++ = (chr >= ' ' ? chr : xlt[chr]);
	}
	else {
	    if (nr < 2) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    if ( 'n' > chr || chr > 'v') {
		if(nr < 2) {
		    icp -= 2;
		    nr++;
		    goto nomore;
		 }
		 else {
		    /*this character always 015; skip over it*/
		    nr--;
		    icp++;
		 }
	    }
	    if (chr == '@')     *ocp++ = REFRSH;
	    else if ('a' <= chr && chr <= 'v') Block {
		static char xlt[] = {
		    CENTER            , /* a*/
		    FORMAT            , /* b*/
		    WPMODE            , /* c*/
		    SAVE              , /* d*/
		    USE               , /* e*/
		    IMPOSSIBLE        , /* f*/
		    BLANK1            , /* g*/
		    PICK              , /* h*/
		    PUT               , /* i*/
		    INSERT            , /* j*/
		    QUOTE             , /* k*/
		    BLANK2            , /* l*/
		    REPLCE            , /* m*/
		    MARGIN            , /* n*/
		    FONT              , /* o*/
		    WINDOW            , /* p*/
		    CH_WIN            , /* q*/
		    DO                , /* r*/
		    CANCEL            , /* s*/
		    OPEN              , /* t*/
		    CLOSE             , /* u*/
		    RESTOR            , /* v*/
		};
		*ocp++ = xlt[chr - 'a'];
	    }
	    else if ('P' <= chr && chr <= 'Y') Block {
		static char xlt[] = {
		    DEL_CH       , /*P*/
		    TABSET       , /*Q*/
		    HOME         , /*R*/
		    GOTO         , /*S*/
		    PL_PAGE      , /*T*/
		    PL_SRCH      , /*U*/
		    MI_PAGE      , /*V*/
		    MI_SRCH      , /*W*/
		    LEFT         , /*X*/
		    RIGHT        , /*Y*/
		};
		*ocp++ = xlt[chr-'P'];
	    }
	    else if ('A' <= chr && chr <= 'E') Block {
		static char xlt[] = {
		    UPARROW      , /*A*/
		    DOWNARROW   , /*B*/
		    RIGHTARROW    , /*C*/
		    LEFTARROW    , /*D*/
		    MTAB         , /*E*/
		 };
		*ocp++ = xlt[chr-'A'];
	    }
	    else  *ocp++ = IMPOSSIBLE;
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

# define FWRITE(x) fwrite((x) , sizeof (x),1,stdout);
# define MULTC 026
addr_2intext (lin, col)
{
 static char addr[] = {MULTC,'H'};
 FWRITE(addr);putchar(041+lin);putchar(041+col);
}

c_addr_2intext(column)
{
 static char c_addr[] = {MULTC,'G'};
 FWRITE(c_addr);putchar(041+column);
}

end_2intext()
{
 static char end[] = {033,'P', 'm', 'a'+100,033,'\\'};
 FWRITE(end);
 /*reset_intext();*/
}


ini_2intext()
{
 static char ini[] = {014,033,'P', 'm', MULTC+0100,033,'\\'};
 /*reset_intext();*/
 FWRITE(ini);
}

reset_intext()
{
 static char reset[] = {'\033','c'};
 FWRITE(reset);
}

up_2intext ()  { static char up[] = {MULTC,'A'}; FWRITE(up); }
dn_2intext ()  { static char dn[] = {MULTC,'B'}; FWRITE(dn); }
rt_2intext ()  { static char rt[] = {MULTC,'C'}; FWRITE(rt); }
lt_2intext ()  { static char lt[] = {MULTC,'D'}; FWRITE(lt); }
nl_2intext ()  { static char nl[] = {MULTC,'E'}; FWRITE(nl); }
cr_2intext ()  { putchar('\015');       }
clr_2intext () { putchar('\014');       }
hm_2intext ()  { addr_2intext(0,0);     }
bsp_2intext () { putchar('\010');       }

xl_2intext (chr)
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
   /*if (   chr >= FIRSTMCH
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
   */
    /* the above commented out to make always slow nzs june 20 1981*/
       putchar (stdxlate[chr - FIRSTSPCL]);
}
extern int kini_nocbreak();

S_kbd kb_2intext = {
/* kb_inlex */  in_2intext,
/* kb_init  */  kini_nocbreak,
/* kb_end   */  nop,
};

S_term t_2intext = {
/* tt_ini0    */    nop,
/* tt_ini1    */    ini_2intext,
/* tt_end     */    end_2intext,
/* tt_left    */    lt_2intext,
/* tt_right   */    rt_2intext,
/* tt_dn      */    dn_2intext,
/* tt_up      */    up_2intext,
/* tt_cret    */    cr_2intext,
/* tt_nl      */    nl_2intext,
/* tt_clear   */    clr_2intext,
/* tt_home    */    hm_2intext,
/* tt_bsp     */    bsp_2intext,
/* tt_addr    */    addr_2intext,
/* tt_lad     */    bad,
/* tt_cad     */    c_addr_2intext,
/* tt_xlate   */    xl_2intext,
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
/* tt_nleft   */    2,
/* tt_nright  */    2,
/* tt_ndn     */    2,
/* tt_nup     */    2,
/* tt_nnl     */    2,
/* tt_nbsp    */    1,
/* tt_naddr   */    4,
/* tt_nlad    */    0,
/* tt_ncad    */    3,
/* tt_wl      */    2,
/* tt_cwr     */    2,
/* tt_pwr     */    1,
/* tt_axis    */    0,
/* tt_bullets */    NO,
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
};
