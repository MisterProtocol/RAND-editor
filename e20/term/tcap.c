#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif



#ifdef OUT
#ifndef LIBTINFO  /* using old copy of termcap library */
#define tparm tparam
#endif
#endif /* OUT */


/****************************************/
/**** 0 = terminal from termcap ****/

extern char *tgoto ();
extern Cmdret help_std ();
int punt_tcap ();

#ifdef LMCLDC
char *ldtab[NSPCHR];
/*
**      Line Drawing Definitions used in Tcap.c to draw boxes
**              (thanks to Eric Negaard@aerospace)
**
**      Gs - Introduce Line Drawing     Bj - Bottom join
**      Ge - End Line Drawing           Tj - Top join
**      Tl - Top left corner            Lj - Left join
**      Tr - Top right corner           Rj - Right join
**      Bl - Bottom left corner         Vl - Vertical line
**      Br - Bottom right corner        Hl - Horizontal line
**      Cj - Center join (cross)        Xc - Other unique character
*/
#endif /* LMCLDC */

#undef UP
#undef HO

char *BC;       /* bc - cursor left                     */
char *ND;       /* nd - cursor right                    */
char *DO;       /* do - cursor down                     */
char *UP;       /* up - cursor up                       */
char *CR;       /* cr - carriage return                 */
char *NL;       /* nl - new line                        */
char *CL;       /* cl - clear screen                    */
char *CD;       /* cd - clear to end of display         */
char *HO;       /* ho - home cursor                     */
char *CM;       /* cm - general cursor movement         */
char *TI;       /* ti - allow cm (?)                    */
char *TE;       /* te - return terminal to normal       */
char *KS;       /* ks - start keypad-xmit mode          */
char *KE;       /* ke - end keypad-xmit mode            */
char *VS;       /* vs - start visual/open mode          */
char *VE;       /* ve - end visual/open mode            */
char PC;        /* pc - pad character                   */
char *AL;       /* al - add (insert) line               */
char *DL;       /* dl - delete line                     */
char *ALm;      /* AL - add (insert) line (multiple)    */
char *DLm;      /* DL - delete line (multiple)          */
char *IC;       /* ic - insert character                */
char *DC;       /* dc - delete character                */
char *CE;       /* ce - clear to eol                    */
char *CS;       /* cs - set scrolling region            */
char *SR;       /* sr - scroll up (reverse)             */
#ifdef LMCLDC
char *GS;       /* Gs - start Graphics                  */
char *GE;       /* Ge - end Graphics                    */
#endif /* LMCLDC */
#ifdef LMCHELP
char *SO;       /* so - begin standout mode             */
char *SE;       /* se - end standout mode               */
#endif /* LMCHELP */
#ifdef LMCVBELL
char *VB;       /* vb - visual bell                     */
#endif /* LMCVBELL */

int pch (int);
int lt_tcap (void);
int rt_tcap (void);
int dn_tcap (void);
int up_tcap (void);
int cr_tcap (void);
int nl_tcap (void);
int clr_tcap (void);
int clr1_tcap (void);
int hm_tcap (void);
int bsp_tcap (void);
int addr_tcap (int, int);

int
pch(int ch) { putchar(ch); return (0); }

int
lt_tcap () { tputs(BC, 1, pch); return (0); }
int
rt_tcap () { tputs(ND, 1, pch); return (0); }
int
dn_tcap () { tputs(DO, 1, pch); return (0); }
int
up_tcap () { tputs(UP, 1, pch); return (0); }
int
cr_tcap () { tputs(CR, 1, pch); return (0); }
int
nl_tcap () { tputs(CR, 1, pch); tputs(DO, 1, pch); return (0); }
int
clr_tcap () { tputs(CL, 1, pch); return (0); }
int
clr1_tcap () { punt_tcap (); tputs(CD, 1, pch); return (0); }
int
hm_tcap () { tputs(HO, 1, pch); return (0); }
int
bsp_tcap () { tputs(BC, 1, pch); putchar(' '); tputs(BC, 1, pch); return (0); }
int
addr_tcap (int lin, int col) { tputs(tgoto(CM, col, lin), 1, pch); return (0); }

int il_tcap (int);
int dl_tcap (int);
int ic_tcap (void);
int dc_tcap (void);
int cle_tcap (void);
#ifdef LMCHELP
int so_tcap (void);
int soe_tcap (void);
#endif /* LMCHELP */
#ifdef LMCVBELL
int vb_tcap (void);
#endif /* LMCVBELL */
Flag vsc_tcap (int, int, int);
int ini1_tcap (void);
int end_tcap (void);
int punt_tcap (void);
#ifdef LMCLDC
int tcap_xl (int);
#endif /* LMCLDC */
Small getcap (char *);


/* ARGSUSED */
/*il_tcap (num) { tputs(AL, 1, pch); return 1; }*/
int
il_tcap (num)
register int num;
{
    /* char *tparm(); */
    if (ALm) {
#if SYSV || SOLARIS
	tputs(tparm(ALm, num), 1, pch);
#else
	printf( ALm, num );
#endif
	return num;
    }
    tputs(AL, 1, pch);
    return 1;
}
int
dl_tcap (num)
register int num;
{
    /* char *tparm(); */
    if (DLm) {
#if SYSV || SOLARIS
	tputs(tparm(DLm, num), 1, pch);
#else
	printf( DLm, num );
#endif
	return num;
    }
    tputs(DL, 1, pch);
    return 1;
}

/* ARGSUSED */
/*dl_tcap (num) { tputs(DL, 1, pch); return 1; }*/
/* ARGSUSED */
int
/*ic_tcap (int num) { tputs(IC, 1, pch); return 1; }*/
ic_tcap () { tputs(IC, 1, pch); return 1; }
/* ARGSUSED */
int
/*dc_tcap (int num) { tputs(DC, 1, pch); return 1; }*/
dc_tcap () { tputs(DC, 1, pch); return 1; }
int
cle_tcap () { tputs(CE, 1, pch); return 1; }
#ifdef LMCHELP
int
so_tcap () { tputs(SO, 1, pch); return (0); }
int
soe_tcap () { tputs(SE, 1, pch); return (0); }
#endif /* LMCHELP */
#ifdef LMCVBELL
int
vb_tcap () { tputs (VB, 1, pch); return (0); }
#endif /* LMCVBELL */
Flag
vsc_tcap (int top, int bottom, int num)
{
    tputs(tgoto (CS, bottom, top), 1, pch);
    if (num > 0) {
	tputs(tgoto (CM, 0, top), 1, pch);
	do {
	    tputs(SR, 1, pch);
	} while (--num);
    } else {
	tputs(tgoto (CM, 0, bottom), 1, pch);
	do {
	    tputs(DO, 1, pch);
	} while (++num);
    }
    tputs(tgoto (CS, term.tt_height - 1, 0), 1, pch);
    return NO;      /* i.e. can't be sure where the cursor is */
}

int
ini1_tcap ()
{
    tputs(TI, 1, pch);          /* initialize cm */
    tputs(KS, 1, pch);          /* set keypad transmit mode */
    tputs(VS, 1, pch);          /* set visual mode */
    return (0);
}

int
end_tcap () {
    if (CS && SR) {
	tputs(tgoto (CS, term.tt_height - 1, 0), 1, pch);
	tputs(tgoto (CM, icol, ilin), 1, pch);
    }
    tputs(VE, 1, pch);
    tputs(KE, 1, pch);
    tputs(TE, 1, pch);
    return (0);
}

int
punt_tcap () { tputs(tgoto (CM, icol, ilin), 1, pch); return (0); }

#ifdef LMCLDC
int
tcap_xl (chr)
#ifdef UNSCHAR
Uchar chr;
#else
int chr;
#endif
{
	if (chr >= FIRSTSPCL && chr <= LASTGRAF) {
		if (!line_draw) {
			line_draw = YES;
			tputs (GS, 1, pch);
		}
		P (*ldtab [chr - FIRSTSPCL]);
	} else {
		if (line_draw) {
			line_draw = NO;
			tputs (GE, 1, pch);
		}
		if (chr >= FIRSTSPCL)
			P (*ldtab [chr - FIRSTSPCL]);
		else if (chr)
			P (chr);
	}
	return (0);
}
#endif /* LMCLDC */

S_term t_tcap = {
/* tt_ini0    */    nop,
/* tt_ini1    */    ini1_tcap,
/* tt_end     */    end_tcap,
/* tt_left    */    lt_tcap,
/* tt_right   */    rt_tcap,
/* tt_dn      */    dn_tcap,
/* tt_up      */    up_tcap,
/* tt_cret    */    cr_tcap,
/* tt_nl      */    nl_tcap,
/* tt_clear   */    clr_tcap,
/* tt_home    */    hm_tcap,
/* tt_bsp     */    bsp_tcap,
/* tt_addr    */    addr_tcap,
/* tt_lad     */    bad,
/* tt_cad     */    bad,
/* tt_xlate   */    xlate1,
/* tt_insline */    il_tcap,
/* tt_delline */    dl_tcap,
/* tt_inschar */    ic_tcap,
/* tt_delchar */    dc_tcap,
/* tt_clreol  */    cle_tcap,
/* tt_vscroll */    vsc_tcap,
/* tt_deflwin */    (int (*) ()) 0,
/* tt_erase   */    (int (*) ()) 0,
#ifdef LMCHELP
/* tt_so      */    so_tcap,
/* tt_soe     */    soe_tcap,
/* tt_help    */    help_std,
#endif
#ifdef LMCVBELL
/* tt_vbell   */    vb_tcap,
#endif /* LMCVBELL */
/* tt_nleft   */    0,
/* tt_nright  */    0,
/* tt_ndn     */    0,
/* tt_nup     */    0,
/* tt_nnl     */    0,
/* tt_nbsp    */    0,
/* tt_naddr   */    0,
/* tt_nlad    */    0,
/* tt_ncad    */    0,
/* tt_wl      */    1,
/* tt_cwr     */    1,
/* tt_pwr     */    1,
/* tt_axis    */    0,
#ifndef LMCLDC
/* tt_bullets */    NO,
#else /* LMCLDC */
/* tt_bullets */    YES,
#endif /* LMCLDC */
/* tt_prtok   */    YES,
/* tt_width   */    80,
/* tt_height  */    24,
};

#define NG  -2
#define UNKNOWN -1
/* #ifndef CURSES */
/* #define OK  0	/ * Already defined in curses.h */
/* #endif / * CURSES */
#ifdef SYSV
#undef TIOCGWINSZ	/* For ISC -- heh */
#endif

Small
getcap(term)
char *term;
{
    char tcbuf[1024];
    char *cp;
    extern char *tgetstr();
#ifdef LMCLDC
    static char dot[] = ".";
    static char dit[] = ";";
#endif /* LMCLDC */
#ifdef TIOCGWINSZ
    struct  winsize winsize;      /* 4.3 BSD window sizing        */
#endif

    switch (tgetent(tcbuf, term)) {
    case -1:
    case 0:
	return UNKNOWN;
    }
    cp = salloc(256, YES);

#ifdef  TIOCGWINSZ
    if (ioctl(0, TIOCGWINSZ, (char *) &winsize) == 0) {
	    /*
	     *  4/2/87 - There's a sun bug that's setting
	     *  all hi bits of ws_col when >= 128 cols...
	     *  So mask if off for now
	     */
	t_tcap.tt_height = (char) (winsize.ws_row & 0377);
	t_tcap.tt_width = winsize.ws_col & 0377;
    } else t_tcap.tt_width = 0;
    if(t_tcap.tt_width<=0 || t_tcap.tt_height<=0)
#endif

#ifdef OUT
    if (   (t_tcap.tt_width = tgetnum("co")) <= 0
	|| (t_tcap.tt_height = tgetnum("li")) <= 0
       )
	return NG;
#endif /* OUT */

/* 12/28/92:  minimum window size requirements */
#define E_MIN_WIDTH     3
#define E_MIN_HEIGHT    5

    if ((t_tcap.tt_width = tgetnum("co")) < E_MIN_WIDTH )
	 t_tcap.tt_width = E_MIN_WIDTH;
    if ((t_tcap.tt_height = tgetnum("li")) < E_MIN_HEIGHT )
	t_tcap.tt_height = E_MIN_HEIGHT;

    if (tgetflag ("xv"))
	t_tcap.tt_width--;   /* vt100 brain damage */
/* if no home command, fake it with cursor movement. */
    if ((HO = tgetstr("ho", &cp)) == NULL)
	t_tcap.tt_home = punt_tcap;
/* can't do without either clear or clear to end of display */
    if ((CL = tgetstr("cl", &cp)) == NULL) {
	if ((CD = tgetstr("cd", &cp)) == NULL)
	    return NG;
/* use home/clear-to-end if there is no straight clear */
	t_tcap.tt_clear = clr1_tcap;
    }
/* got to have cursor addressing */
    if ((CM = tgetstr("cm", &cp)) == NULL)
	    return NG;
    t_tcap.tt_naddr = strlen(tgoto(CM, 10, 10));
/* set up backspace; if none, fake with cm */
    if ((BC = tgetstr("bc", &cp)) == NULL) {
	if (tgetflag("bs")) {
	    BC = "\b";
	} else {
	    t_tcap.tt_left = punt_tcap;
	    t_tcap.tt_nleft = t_tcap.tt_naddr;
	    t_tcap.tt_nbsp  = 2 * t_tcap.tt_naddr + 1;
	    goto endbc;
	}
    }
    t_tcap.tt_nleft = strlen(BC);
    t_tcap.tt_nbsp = 2*strlen(BC)+1;
endbc:
/* what's this? a null string for right cursor? */
    if ((ND = tgetstr("nd", &cp)) == NULL) {
	t_tcap.tt_right = punt_tcap;
	t_tcap.tt_nright = t_tcap.tt_naddr;
/* I overrule it:
	ND = "";       */
    } else
	t_tcap.tt_nright = strlen(ND);
/* fake up with cm if needed */
    if ((UP = tgetstr("up", &cp)) == NULL) {
	t_tcap.tt_up = punt_tcap;
	t_tcap.tt_nup = t_tcap.tt_naddr;
    }
    else
	t_tcap.tt_nup = strlen(UP);
#ifdef COMMENT
/* with strange flavors of c/r and n/l, fake them too. */
    if (tgetflag("nc")) {
	t_tcap.tt_cret = punt_tcap;
	t_tcap.tt_nl = punt_tcap;
	t_tcap.tt_nnl = t_tcap.tt_naddr;
    }
    else
#endif /* COMMENT */
	if ((CR = tgetstr("cr", &cp)) == NULL)
	    CR = "\r";
/* ditto down */
    if (   (DO = tgetstr("do", &cp)) == NULL
	&& (DO = tgetstr("nl", &cp)) == NULL
       ) {
	t_tcap.tt_dn = punt_tcap;
	t_tcap.tt_ndn = t_tcap.tt_naddr;
	t_tcap.tt_nl = punt_tcap;
	t_tcap.tt_nnl = t_tcap.tt_naddr;
    }
    else {
	t_tcap.tt_ndn = strlen(DO);
	t_tcap.tt_nnl = strlen(CR)+strlen(DO);
    }
/* unless both il and dl work, remove them */
    AL = tgetstr("al", &cp);
    DL = tgetstr("dl", &cp);
    if (   (AL = tgetstr("al", &cp)) == NULL
	|| (DL = tgetstr("dl", &cp)) == NULL
       ) {
	    t_tcap.tt_insline = (int (*) ()) 0;
	    t_tcap.tt_delline = (int (*) ()) 0;
    }

/* multiple insert/delete line capabilities */
    ALm = tgetstr("AL", &cp);       /* AL, DL are not std. termcap entries! */
    DLm = tgetstr("DL", &cp);
    if (   (ALm = tgetstr("AL", &cp)) != NULL
	&& (DLm = tgetstr("DL", &cp)) != NULL
       ) {
	    extern Flag singlescroll;

	    singlescroll = NO;
    }

/* zot insert and delete char if they aren't there. */
    IC = tgetstr("ic", &cp);
    DC = tgetstr("dc", &cp);
    if (   (IC = tgetstr("ic", &cp)) == NULL
	|| (DC = tgetstr("dc", &cp)) == NULL
       ) {
	    t_tcap.tt_inschar = (int (*) ()) 0;
	    t_tcap.tt_delchar = (int (*) ()) 0;
    }
/* clear clreol unless it does exist */
    if ((CE = tgetstr("ce", &cp)) == NULL)
	t_tcap.tt_clreol = (int (*) ()) 0;
/* we can vscroll if we have set-region and reverse scroll!! */
    if (   (CS = tgetstr("cs", &cp)) == NULL
	|| (SR = tgetstr("sr", &cp)) == NULL
       )
	t_tcap.tt_vscroll = (int (*) ()) 0;

#ifdef SOLARIS
    /*
     * Trw:  For some reason, CS and SR are both set even
     * when I point at a termcap file that doesn't define them!
     * Maybe some termio/termcap interaction I'm not yet
     * familiar with (my BSD background....).
     *
     * So, we'll force the use of ALm and DLm if defined
     */

    if( ALm && DLm ) {
	t_tcap.tt_vscroll = (int (*) ()) 0;
	CS = SR = NULL;
    }
#endif


/* set/reset for cm. Set pad char if there is one */
    TI = tgetstr("ti", &cp);
    TE = tgetstr("te", &cp);
    KS = tgetstr("ks", &cp);
    KE = tgetstr("ke", &cp);
    VS = tgetstr("vs", &cp);
    VE = tgetstr("ve", &cp);
    Block {
	Reg1 char *pc;
	if ((pc = tgetstr("pc", &cp)))
		PC = *pc;
    }
#ifdef LMCLDC
/* set up the line-drawing for windows. */
    if (    (GE = tgetstr("Ge", &cp)) != NULL
	&&  (GS = tgetstr("Gs", &cp)) != NULL
	) {
	ldtab[BULCHAR- FIRSTSPCL] = tgetstr ("Cj", &cp);
	ldtab[  LMCH - FIRSTSPCL] =
	ldtab[  RMCH - FIRSTSPCL] = tgetstr ("Vl", &cp);
	ldtab[ MLMCH - FIRSTSPCL] = tgetstr ("Rj", &cp);
	ldtab[ MRMCH - FIRSTSPCL] = tgetstr ("Lj", &cp);
	ldtab[  TMCH - FIRSTSPCL] =
	ldtab[  BMCH - FIRSTSPCL] = tgetstr ("Hl", &cp);
	ldtab[TLCMCH - FIRSTSPCL] = tgetstr ("Tl", &cp);
	ldtab[TRCMCH - FIRSTSPCL] = tgetstr ("Tr", &cp);
	ldtab[BLCMCH - FIRSTSPCL] = tgetstr ("Bl", &cp);
	ldtab[BRCMCH - FIRSTSPCL] = tgetstr ("Br", &cp);
	ldtab[ TTMCH - FIRSTSPCL] = tgetstr ("Bj", &cp);
	ldtab[ BTMCH - FIRSTSPCL] = tgetstr ("Tj", &cp);
	ldtab[ INMCH - FIRSTSPCL] = dot;
	ldtab[ ELMCH - FIRSTSPCL] = dit;
	if ((ldtab[ESCCHAR-FIRSTSPCL] = tgetstr ("Xc", &cp)) == NULL)
		ldtab [ESCCHAR-FIRSTSPCL] = ldtab [BULCHAR-FIRSTSPCL];
	t_tcap.tt_prtok = NO;
	t_tcap.tt_xlate = tcap_xl;
    }
#endif /* LMCLDC */
#ifdef LMCHELP
/* standout mode, if you will */
/* no standout if terminal has magic cookies... */
    if (   (SO = tgetstr("so", &cp)) != NULL && tgetnum("sg") == 0)
	SE = tgetstr("se", &cp);
    else
	t_tcap.tt_so = t_tcap.tt_soe = (int (*) ()) 0;
#endif /* LMCHELP */
#ifdef LMCVBELL
/* visual bell */
    if (   (VB = tgetstr ("vb", &cp)) == NULL)
	t_tcap.tt_vbell = (int (*) ())0;
#endif /* LMCVBELL */
/* set wrap variables. */
    t_tcap.tt_wl = 0;   /* termcap doesn't guarantee what it will do */
 /* t_tcap.tt_pwr = tgetflag("am") ? (tgetflag("xn") ? 4 : 1) : 3;  **/
    t_tcap.tt_pwr = 0;  /* might need delay, so we punt */
    t_tcap.tt_cwr = 0;  /* termcap doesn't guarantee what it will do */
    t_tcap.tt_axis = 0;
    t_tcap.tt_nlad = 0;
    t_tcap.tt_ncad = 0;

    return (0); /* OK */
}
