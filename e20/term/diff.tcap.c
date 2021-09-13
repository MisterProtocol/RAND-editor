9d8
< extern Cmdret help_std ();
11,26d9
< #ifdef LMCLDC
< char *ldtab[NSPCHR];
< /*
< **      Line Drawing Definitions used in Tcap.c to draw boxes
< **              (thanks to Eric Negaard@aerospace)
< **
< **      Gs - Introduce Line Drawing     Bj - Bottom join
< **      Ge - End Line Drawing           Tj - Top join
< **      Tl - Top left corner            Lj - Left join
< **      Tr - Top right corner           Rj - Right join
< **      Bl - Bottom left corner         Vl - Vertical line
< **      Br - Bottom right corner        Hl - Horizontal line
< **      Cj - Center join (cross)        Xc - Other unique character
< */
< #endif LMCLDC
< 
30,64c13,30
< char *BC;       /* bc - cursor left                     */
< char *ND;       /* nd - cursor right                    */
< char *DO;       /* do - cursor down                     */
< char *UP;       /* up - cursor up                       */
< char *CR;       /* cr - carriage return                 */
< char *NL;       /* nl - new line                        */
< char *CL;       /* cl - clear screen                    */
< char *CD;       /* cd - clear to end of display         */
< char *HO;       /* ho - home cursor                     */
< char *CM;       /* cm - general cursor movement         */
< char *TI;       /* ti - allow cm (?)                    */
< char *TE;       /* te - return terminal to normal       */
< char *KS;       /* ks - start keypad-xmit mode          */
< char *KE;       /* ke - end keypad-xmit mode            */
< char *VS;       /* vs - start visual/open mode          */
< char *VE;       /* ve - end visual/open mode            */
< char PC;        /* pc - pad character                   */
< char *AL;       /* al - add (insert) line               */
< char *DL;       /* dl - delete line                     */
< char *IC;       /* ic - insert character                */
< char *DC;       /* dc - delete character                */
< char *CE;       /* ce - clear to eol                    */
< char *CS;       /* cs - set scrolling region            */
< char *SR;       /* sr - scroll up (reverse)             */
< #ifdef LMCLDC
< char *GS;       /* Gs - start Graphics                  */
< char *GE;       /* Ge - end Graphics                    */
< #endif LMCLDC
< #ifdef LMCHELP
< char *SO;       /* so - begin standout mode             */
< char *SE;       /* se - end standout mode               */
< #endif LMCHELP
< #ifdef LMCVBELL
< char *VB;       /* vb - visual bell                     */
< #endif LMCVBELL
---
> char *BC;
> char *ND;
> char *DO;
> char *UP;
> char *CR;
> char *NL;
> char *CL;
> char *CD;
> char *HO;
> char *CM;
> char *TI;
> char *TE;
> char PC;
> char *AL;
> char *DL;
> char *CE;
> char *CS;
> char *SR;
83,86d48
< /* ARGSUSED */
< ic_tcap (num) { tputs(IC, 1, pch); return 1; }
< /* ARGSUSED */
< dc_tcap (num) { tputs(DC, 1, pch); return 1; }
88,94d49
< #ifdef LMCHELP
< so_tcap () { tputs(SO, 1, pch); }
< soe_tcap () { tputs(SE, 1, pch); }
< #endif LMCHELP
< #ifdef LMCVBELL
< vb_tcap () { tputs (VB, 1, pch); }
< #endif LMCVBELL
109d63
<     tputs(tgoto (CS, term.tt_height - 1, 0), 1, pch);
113,119c67
< ini1_tcap ()
< {
<     tputs(TI, 1, pch);          /* initialize cm */
<     tputs(KS, 1, pch);          /* set keypad transmit mode */
<     tputs(VS, 1, pch);          /* set visual mode */
< }
< 
---
> ini1_tcap () { tputs(TI, 1, pch); }
125,126d72
<     tputs(VE, 1, pch);
<     tputs(KE, 1, pch);
131,157d76
< #ifdef LMCLDC
< tcap_xl (chr)
< #ifdef UNSCHAR
< Uchar chr;
< #else
< int chr;
< #endif
< {
< 	if (chr >= FIRSTSPCL && chr <= LASTGRAF) {
< 		if (!line_draw) {
< 			line_draw = YES;
< 			tputs (GS, 1, pch);
< 		}
< 		P (*ldtab [chr - FIRSTSPCL]);
< 	} else {
< 		if (line_draw) {
< 			line_draw = NO;
< 			tputs (GE, 1, pch);
< 		}
< 		if (chr >= FIRSTSPCL)
< 			P (*ldtab [chr - FIRSTSPCL]);
< 		else if (chr)
< 			P (chr);
< 	}
< }
< #endif LMCLDC
< 
177,178c96,97
< /* tt_inschar */    ic_tcap,
< /* tt_delchar */    dc_tcap,
---
> /* tt_inschar */    (int (*) ()) 0,
> /* tt_delchar */    (int (*) ()) 0,
183,190d101
< #ifdef LMCHELP
< /* tt_so      */    so_tcap,
< /* tt_soe     */    soe_tcap,
< /* tt_help    */    help_std,
< #endif
< #ifdef LMCVBELL
< /* tt_vbell   */    vb_tcap,
< #endif LMCVBELL
204d114
< #ifndef LMCLDC
206,208d115
< #else   LMCLDC
< /* tt_bullets */    YES,
< #endif  LMCLDC
225,228d131
< #ifdef LMCLDC
<     static char dot[] = ".";
<     static char dit[] = ";";
< #endif LMCLDC
241,243c144
<     if (tgetflag ("xv"))
< 	t_tcap.tt_width--;   /* vt100 brain damage */
< /* if no home command, fake it with cursor movement. */
---
> 
246c147
< /* can't do without either clear or clear to end of display */
---
> 
250d150
< /* use home/clear-to-end if there is no straight clear */
253c153
< /* got to have cursor addressing */
---
> 
257c157
< /* set up backspace; if none, fake with cm */
---
> 
269,276c169,173
< endbc:
< /* what's this? a null string for right cursor? */
<     if ((ND = tgetstr("nd", &cp)) == NULL) {
< 	t_tcap.tt_right = punt_tcap;
< 	t_tcap.tt_nright = t_tcap.tt_naddr;
< /* I overrule it:
< 	ND = "";       */
<     } else
---
>  endbc:
> 
>     if ((ND = tgetstr("nd", &cp)) == NULL)
> 	ND = "";
>     else
278c175
< /* fake up with cm if needed */
---
> 
285c182
< /* with strange flavors of c/r and n/l, fake them too. */
---
> 
293c190
< /* ditto down */
---
> 
306c203
< /* unless both il and dl work, remove them */
---
> 
308a206
> 
315,324c213
< /* zot insert and delete char if they aren't there. */
<     IC = tgetstr("ic", &cp);
<     DC = tgetstr("dc", &cp);
<     if (   (IC = tgetstr("ic", &cp)) == NULL
< 	|| (DC = tgetstr("dc", &cp)) == NULL
<        ) {
< 	    t_tcap.tt_inschar = (int (*) ()) 0;
< 	    t_tcap.tt_delchar = (int (*) ()) 0;
<     }
< /* clear clreol unless it does exist */
---
> 
327c216
< /* we can vscroll if we have set-region and reverse scroll!! */
---
> 
332c221
< /* set/reset for cm. Set pad char if there is one */
---
> 
335,338d223
<     KS = tgetstr("ks", &cp);
<     KE = tgetstr("ke", &cp);
<     VS = tgetstr("vs", &cp);
<     VE = tgetstr("ve", &cp);
344,383c229
< #ifdef LMCLDC
< /* set up the line-drawing for windows. */
<     if (    (GE = tgetstr("Ge", &cp)) != NULL
< 	&&  (GS = tgetstr("Gs", &cp)) != NULL
< 	) {
< 	ldtab[BULCHAR- FIRSTSPCL] = tgetstr ("Cj", &cp);
< 	ldtab[  LMCH - FIRSTSPCL] =
< 	ldtab[  RMCH - FIRSTSPCL] = tgetstr ("Vl", &cp);
< 	ldtab[ MLMCH - FIRSTSPCL] = tgetstr ("Rj", &cp);
< 	ldtab[ MRMCH - FIRSTSPCL] = tgetstr ("Lj", &cp);
< 	ldtab[  TMCH - FIRSTSPCL] =
< 	ldtab[  BMCH - FIRSTSPCL] = tgetstr ("Hl", &cp);
< 	ldtab[TLCMCH - FIRSTSPCL] = tgetstr ("Tl", &cp);
< 	ldtab[TRCMCH - FIRSTSPCL] = tgetstr ("Tr", &cp);
< 	ldtab[BLCMCH - FIRSTSPCL] = tgetstr ("Bl", &cp);
< 	ldtab[BRCMCH - FIRSTSPCL] = tgetstr ("Br", &cp);
< 	ldtab[ TTMCH - FIRSTSPCL] = tgetstr ("Bj", &cp);
< 	ldtab[ BTMCH - FIRSTSPCL] = tgetstr ("Tj", &cp);
< 	ldtab[ INMCH - FIRSTSPCL] = dot;
< 	ldtab[ ELMCH - FIRSTSPCL] = dit;
< 	if ((ldtab[ESCCHAR-FIRSTSPCL] = tgetstr ("Xc", &cp)) == NULL)
< 		ldtab [ESCCHAR-FIRSTSPCL] = ldtab [BULCHAR-FIRSTSPCL];
< 	t_tcap.tt_prtok = NO;
< 	t_tcap.tt_xlate = tcap_xl;
<     }
< #endif LMCLDC
< #ifdef LMCHELP
< /* standout mode, if you will */
< /* no standout if terminal has magic cookies... */
<     if (   (SO = tgetstr("so", &cp)) != NULL && tgetnum("sg") == 0)
< 	SE = tgetstr("se", &cp);
<     else
< 	t_tcap.tt_so = t_tcap.tt_soe = (int (*) ()) 0;
< #endif LMCHELP
< #ifdef LMCVBELL
< /* visual bell */
<     if (   (VB = tgetstr ("vb", &cp)) == NULL)
< 	t_tcap.tt_vbell = (int (*) ())0;
< #endif LMCVBELL
< /* set wrap variables. */
---
> 
