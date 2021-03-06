#ifdef COMMENT
--------
file e.m.c
    mainloop
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.cm.h"
#include "e.inf.h"
#include "e.m.h"
#include "e.tt.h"
#include "e.e.h"

extern char *re_comp();         /* Added 10/18/82 MAB */

#ifdef COMMENT
void
mainloop ()
.
    Get keys from the input and process them.
#endif
void
mainloop ()
{
#ifdef LMCCMDS
char *nix = "\0";
char *abortstg = "ab\0";
char *quitstg = "q\0";
char *pickstg = "pick\0";
char *boardstg = "l\0";
#endif LMCCMDS
	char *s2, *tmpb, b[BUFSIZ]; /* added 10/18/82 MAB */

    for (;;) {
funcdone:
	loopflags.clear = YES;
newnumber:
	if (loopflags.clear && !loopflags.hold) {
	    mesg (TELALL + 1, loopflags.beep ? "\007" : "");
	    loopflags.beep = NO;
	}
	Block {
	    Reg1 Nlines nlines;
	    char ich[8];
	    nlines = curwksp->wlin + cursorline + 1;
	    if (infoline != nlines) {
#ifdef LA_LONGFILES
		sprintf (ich, "%-5D", nlines);
#else  LA_LONGFILES
		sprintf (ich, "%-5d", nlines);
#endif LA_LONGFILES
		info (inf_line, 5, ich);
		infoline = nlines;
	    }
	}

	if (curfile != infofile && (fileflags[curfile] & INUSE)) {
	    info (inf_file, strlen (names[infofile]), names[curfile]);
	    infofile = curfile;
	}

	if (loopflags.flash) {
	    setbul (YES);
	    loopflags.flash = NO;
	}

contin:
	keyused = YES;
	if (curwksp->wkflags & RANGESET) {
	    /* Update the RANGE postion display */
	    infoprange (curwksp->wlin + cursorline);
	}

	if (curmark) Block {
	    /* Update the MARK postion display */
	    Reg2 Nlines nlines;
	    Block {
		Reg1 Nlines curline;
		curline = curwksp->wlin + cursorline;
		if ((nlines = curline - (curmark->mrkwinlin + curmark->mrklin)) < 0)
		    nlines = -nlines;
	    }
	    ++nlines;
	    Block {
		Reg1 Ncols ncols;
		if ((ncols = curwksp->wcol + cursorcol
		    - (curmark->mrkwincol + curmark->mrkcol)) < 0)
		    ncols = -ncols;
		if (marklines != nlines) {
#ifdef LA_LONGFILES
		    sprintf (mklinstr, "%D", nlines);
#else  LA_LONGFILES
		    sprintf (mklinstr, "%d", nlines);
#endif LA_LONGFILES
		    marklines = nlines;
		}
		if (markcols != ncols) {
		    if (ncols)
#ifdef LA_LONGLINES
			sprintf (mkcolstr, "x%D", ncols);
#else  LA_LONGLINES
			sprintf (mkcolstr, "x%d", ncols);
#endif LA_LONGLINES
		    else
			mkcolstr[0] = '\0';
		    markcols = ncols;
		}
	    }
	    Block {
		Reg1 char *cp;
		Reg3 int len;
		cp = append (mklinstr, mkcolstr);
		len = strlen (cp);
		info (inf_area, max (len, infoarealen), cp);
		infoarealen = len;
		sfree (cp);
	    }
	}

	if (borderbullets) {
	    dobullets (loopflags.bullet, loopflags.clear);
	    loopflags.bullet = NO;
	}
	loopflags.clear = NO;

	if (cmdmode) {
	    loopflags.hold = NO;
	    goto gotcmd;
	}
#ifdef LMCLDC
	(*term.tt_xlate) (0);   /* reset the graphics if needed */
#endif LMCLDC
	getkey (WAIT_KEY);
	if (loopflags.hold) {
	    loopflags.hold = NO;
	    mesg (TELALL);
	}

	Block {
	    Reg1 Small donetype;
	    if (   !CTRLCHAR
		|| key == CCCTRLQUOTE
		|| key == CCBACKSPACE
		|| key == CCDELCH
	       ) {
		donetype = printchar ();
		goto doneswitch;
	    }
	    Block {
		Reg2 Small cm;
		if (key < MAXMOTIONS && (cm = cntlmotions[key])) {
		    /* in the next several lines, a redisplay is always done,
		    /*  EXCEPT
		    /* +TAB, -TAB, and <- and->arrows within the screen
		    /**/
		    if (   (cm == RT && cursorcol + 1 <= curwin->redit)
			|| (cm == LT && cursorcol - 1 >= curwin->ledit)
			|| cm == TB
			|| cm == BT
		       )
			{}
		    else
			credisplay (NO);
		    if (key == CCRETURN && cursorline == curwin->btext)
			/* Return on bottom line => +lines    */
			vertmvwin (defplline);
		    /*
		     *  e18 mod:  An RT (right arrow) at the right window
		     *  boundary no longer shifts the window left 1.
		     *  Also, an LT (left arrow) no longer shifts the
		     *  window right 1.
		     */
#ifdef LMCSRMFIX
		    if (optstick) {
#endif LMCSRMFIX
			if (cm == RT) {
			    if (cursorcol + 1 <= curwin->redit)
				movecursor (cm, 1);
			}
			else if (cm == LT) {
			    if (cursorcol - 1 >= curwin->ledit)
				movecursor (cm, 1);
			}
			else
			   movecursor (cm, 1);
#ifdef LMCSRMFIX
		    } else {
			if (cm == RT && cursorcol >= curwin->redit) {
			    movewin (curwksp->wlin, curwksp->wcol + defrwin,
				     cursorline, cursorcol + 1 - defrwin, YES);
			} else if (cm == LT && cursorcol <= curwin->ledit) {
			    Ncols df;
			    df = min (deflwin, curwksp->wcol);
			    movewin (curwksp->wlin, curwksp->wcol - df,
				     cursorline, cursorcol - 1 + df, YES);
			} else
			    movecursor (cm, 1);
		    }
#endif LMCSRMFIX

		    if (infoline != curwksp->wlin + cursorline + 1)
			goto newnumber;
		    goto contin;
		}
	    }
	    credisplay (NO);
	    flushkeys ();
	    donetype = CROK;

	    /* margin-stick feature */
	    if (cursorcol > curwin->rtext)
		poscursor (curwin->rtext, cursorline);

	    switch (key) {
		case CCCMD:
		    keyused = YES;
		    goto gotcmd;

		case CCSETFILE:
		    if (curmark)
			goto nomarkerr;
		    switchfile ();
		    goto funcdone;

		case CCCHWINDOW:
		    if (curmark)
			goto nomarkerr;
		    if (nwinlist > 1) {
			chgwindow (-1);
			loopflags.bullet = YES;
		    }
		    goto funcdone;

		case CCOPEN:
		case CCCLOSE:
		case CCPICK:
		case CCERASE:
		    donetype = edkey (key, NO);
		    goto doneswitch;

		case CCMISRCH:
		case CCPLSRCH:
		    dosearch (key == CCPLSRCH ? 1 : -1);
		    goto funcdone;

		case CCINSMODE:
		    tglinsmode ();
		    goto funcdone;

		case CCLWINDOW:
		    horzmvwin (-deflwin);
		    goto funcdone;

		case CCRWINDOW:
		    horzmvwin (defrwin);
		    goto funcdone;

		case CCPLLINE:
		    vertmvwin (defplline);
		    goto funcdone;

		case CCMILINE:
		    vertmvwin (-defmiline);
		    goto funcdone;

		case CCPLPAGE:
		    vertmvwin (defplpage * (1 + curwin->btext));
		    goto funcdone;

		case CCMIPAGE:
		    vertmvwin (-defmipage * (1 + curwin->btext));
		    goto funcdone;

		case CCINT:
		    goto nointerr;

		case CCTABS:
		    sctab (curwksp->wcol + cursorcol, YES);
		    goto funcdone;

		case CCMARK:
		    mark ();
		    goto funcdone;

		case CCREPLACE:
		    replkey ();
		    goto funcdone;

		case CCSPLIT:
		    donetype = split ();
		    goto funcdone;

		case CCJOIN:
		    donetype = join ();
		    goto funcdone;

#ifdef LMCCMDS
		case CCCENTER:
		    command (CMDCENTER, nix);
		    goto funcdone;

		case CCREDRAW:
		    command (CMDREDRAW, nix);
		    goto funcdone;

		case CCFILL:
		    command (CMDFILL, nix);
		    goto funcdone;

		case CCJUSTIFY:
		    command (CMDJUST, nix);
		    goto funcdone;

		case CCSTOPX:
		    command (CMDSTOP, nix);
		    goto funcdone;

		case CCBOX:
		    donetype = command (CMDBOX, nix);
		    goto doneswitch;

		case CCTRACK:
		    donetype = command (CMDTRACK, nix);
		    goto doneswitch;

		case CCRANGE:
		    donetype = command (CMDRANGE, nix);
		    goto doneswitch;

		case CCCLEAR:
		    command (CMDCLEAR, nix);
		    goto funcdone;

		case CCEXIT:
		    command (CMDEXIT, nix);
		    goto funcdone;

		case CCABORT:
		    command (CMDEXIT, abortstg);
		    goto funcdone;

		case CCQUIT:
		    command (CMDEXIT, quitstg);
		    goto funcdone;

		case CCCLRTABS:
		    command (CMD_TABS, nix);
		    goto funcdone;
#ifdef LMCHELP
		case CCHELP:
		    command (CMDHELP, nix);
		    goto funcdone;
#endif LMCHELP
		case CCCOVER:
		    donetype = command (CMDCOVER, pickstg);
		    goto doneswitch;

		case CCOVERLAY:
		    donetype = command (CMDOVERLAY, pickstg);
		    goto doneswitch;

		case CCBLOT:
		    donetype = command (CMDBLOT, pickstg);
		    goto doneswitch;
#ifdef LMCCASE
		case CCCCASE:
		    donetype = chgcase (NO);
		    goto doneswitch;

		case CCCAPS:
		    donetype = chgcase (YES);
		    goto doneswitch;
#endif LMCCASE
#ifdef LMCAUTO
		case CCAUTOFILL:
		    infoauto(NO);
		    goto funcdone;
#endif LMCAUTO
#endif LMCCMDS
		case CCLWORD:
		    donetype = mword(-1, 1);
		    goto funcdone;

		case CCRWORD:
		    donetype = mword(1, 1);
		    goto funcdone;

#ifdef LMCDWORD
		case CCDWORD:
		    donetype = dodword (YES);
		    goto funcdone;
#endif LMCDWORD

		case CCUNAS1:
		case CCDEL:
		    goto notimperr;

		case CCDELCH:
		case CCMOVELEFT:
		case CCTAB:
		case CCMOVEDOWN:
		case CCHOME:
		case CCRETURN:
		case CCMOVEUP:
		default:
		    goto badkeyerr;
#ifdef LMCCMDS
		case CCNULL:
		    goto funcdone;
#endif LMCCMDS
	    }
gotcmd:
	    param ();

	    if (cmdmode && key != CCRETURN)
		goto notcmderr;
	    switch (key) {
		case CCCMD:
		    goto funcdone;

		case CCLWINDOW:
		case CCRWINDOW:
		    switch (paramtype) {
		    case 0:
			horzmvwin (key == CCRWINDOW
				   ? (Ncols) cursorcol : - curwksp->wcol);
			break;

		    case 1:
			horzmvwin (key == CCRWINDOW
				   ? (Ncols) parmlines : (Ncols) -parmlines);
			break;

		    case 2:
			goto norecterr;

		    default:
			goto notinterr;
		    }
		    goto funcdone;

		case CCSETFILE:
		    goto notimperr;

		case CCINT:
		    goto funcdone;

		case CCMISRCH:
		case CCPLSRCH:
		    if (paramtype == 0)
			getarg ();
		    if (*paramv == 0)
			goto noargerr;
					/* Begin adding 10/18/82 MAB */
		    if (patmode){
			for(s2 = paramv, tmpb = b; *s2; tmpb++, s2++){
			    if (*s2 == ESCCHAR && (s2[1]|0140) == 'j'){
				    mesg (ERRALL + 1, "use ^ or $ rather than ^J");
				    goto funcdone;
			    }
			    else
				    *tmpb = *s2;
			}
			if (tmpb[-1] == '\\')
			    tmpb[-1] = '$';
			*tmpb = '\0';
			if (b[0] == '^' && b[1] == '\0'){
			    b[1] = '.';
			    b[2] = '\0';
			}
			else if (b[0] == '$' && b[1] == '\0'){
			    b[0] = '.';
			    b[1] = '$';
			    b[2] = '\0';
			}
			if ((s2 = re_comp(b)) != (char *) 0){
			    mesg (ERRALL + 3, b, ": ", s2);
			    goto funcdone;
			}
		    }
					/* End adding 10/18/82 MAB */
		    if (searchkey)
			sfree (searchkey);
		    searchkey = append (paramv, "");
		    dosearch (key == CCPLSRCH ? 1 : -1);
		    goto funcdone;

		case CCBACKSPACE:
		    /* donetype can only be 0 */
		    Block {
			Reg2 Ncols  k;
			k = curwksp->wcol;
			if (insmode)
			    donetype = ed (OPCLOSE, QCLOSE,
					   curwksp->wlin + cursorline, k,
					   (Nlines) 1, (Ncols) cursorcol, YES);
			else {
			    savecurs ();
			    donetype = ed (OPERASE, QERASE,
					   curwksp->wlin + cursorline, k,
					   (Nlines) 1, (Ncols) cursorcol, YES);
			    restcurs ();
			}
		    }
		    goto doneswitch;

		case CCDELCH:
		    /* donetype can only be 0 */
		    Block {
			Reg2 Ncols k;
			getline (curwksp->wlin + cursorline);
			if ((k = ncline - 1 - curwksp->wcol - cursorcol) > 0) {
			    donetype = ed (OPERASE, QERASE,
					   curwksp->wlin + cursorline,
					   curwksp->wcol + cursorcol,
					   (Nlines) 1, k, YES);
			    goto doneswitch;
			}
		    }
		    goto funcdone;

		case CCOPEN:
		case CCCLOSE:
		case CCPICK:
		case CCERASE:
		    donetype = edkey (key, YES);
		    goto doneswitch;

		case CCMARK:
		    if (paramtype != 0)
			goto notimperr;
		    unmark ();
		    goto funcdone;

		case CCMOVELEFT:
		case CCMOVEDOWN:
		case CCMOVEUP:
		case CCMOVERIGHT:

		case CCTAB:
		case CCHOME:
		case CCBACKTAB:
		    switch (paramtype) Block {
			Reg2 lns;
		    case 0:
			switch (key) {
			case CCHOME:
			    movecursor (cntlmotions[CCHOME], 1);
			    key = CCMOVEDOWN;
			    goto llcor;

			case CCMOVEDOWN:
			    if ((lns = la_lsize (curlas) -
				       (curwksp->wlin + cursorline)) > 0)
				lns = min (lns, curwin->btext - cursorline);
			    else
		    llcor:      lns = curwin->btext - cursorline;
			    break;

			case CCMOVEUP:
			    lns = cursorline;
			    break;

			case CCBACKTAB:
			case CCTAB:
			    goto notimperr;

			case CCMOVELEFT:
			    lns = cursorcol;
			    break;

			case CCMOVERIGHT:
			    getline (curwksp->wlin + cursorline);
			    if ((lns = ncline - 1 - (curwksp->wcol + cursorcol))
				  > 0)
				lns = min (lns, curwin->rtext - cursorcol);
			    else
				lns = curwin->rtext - cursorcol;
			    break;
			}
			if (lns <= 0)
			    goto funcdone;
			goto multmove;

		    case 1:
			if (parmlines <= 0)
			    goto notposerr;
			lns = parmlines;
 multmove:              movecursor (cntlmotions[key], lns);
			break;

		    case 2:
			goto norecterr;

		    default:
			goto notinterr;
		    }
		    goto funcdone;

		case CCRETURN:
#ifdef LMCCMDS
		    donetype = command (0, nix);
		    goto doneswitch;
#ifdef LMCHELP
		case CCHELP:
		    command (CMDHELP, boardstg);
		    goto funcdone;
#endif LMCHELP
#ifdef LMCAUTO
		case CCAUTOFILL:
		    infoauto(YES);
		    goto funcdone;
#endif LMCAUTO
#else  LMCCMDS
		    donetype = command ();
		    goto doneswitch;
#endif LMCCMDS
		case CCMIPAGE:
		case CCPLPAGE:
		    switch (paramtype) {
		    case 0:
			gotomvwin (key == CCPLPAGE ? la_lsize (curlas) : 0);
			break;

		    case 1:
			vertmvwin ((key == CCPLPAGE ? parmlines : -parmlines )
				    * (1 + curwin->btext));
			break;

		    case 2:
			goto norecterr;

		    default:
			goto notinterr;
		    }
		    goto funcdone;

		case CCMILINE:
		case CCPLLINE:
		    switch (paramtype) {
		    case 0:
			vertmvwin (cursorline - (key == CCPLLINE
				   ? 0 : curwin->btext));
			break;

		    case 1:
			vertmvwin (key == CCPLLINE ? parmlines : -parmlines);
			break;

		    case 2:
			goto norecterr;

		    default:
			goto notinterr;
		    }
		    goto funcdone;

		case CCCHWINDOW:
		    if (curmark)
			goto nomarkerr;
		    switch (paramtype) {
		    case 0:
			goto notimperr; /* should be backwards move */

		    case 1:
			if (parmlines <= 0)
			    goto notposerr;
			chgwindow (parmlines - 1);
			loopflags.bullet = YES;
			break;

		    case 2:
			goto norecterr;

		    default:
			goto notinterr;
		    }
		    goto funcdone;

		case CCTABS:
		    if (paramtype == 0)
			sctab (curwksp->wcol + cursorcol, NO);
		    else
			goto notimperr;
		    goto funcdone;

		case CCINSMODE:
		    goto badkeyerr;

		case CCLWORD:
		case CCRWORD:
		    switch (paramtype) {
		    case 0:
			goto notimperr;

		    case 1:
			if (parmlines <= 0)
			    goto notposerr;
			mword((key == CCLWORD) ? -1 : 1, parmlines);
			break;

		    case 2:
			goto norecterr;

		    default:
			goto notinterr;
		    }
		    goto funcdone;

#ifdef LMCDWORD
		case CCDWORD:
		    donetype = dodword (NO);
		    goto funcdone;
#endif LMCDWORD
#ifdef LMCCMDS
		case CCCOVER:
		    donetype = command (CMDINSERT, pickstg);
		    goto doneswitch;

		case CCOVERLAY:
		    donetype = command (CMDUNDERLAY, pickstg);
		    goto doneswitch;

		case CCBLOT:
		    donetype = command (CMD_BLOT, pickstg);
		    goto doneswitch;

		case CCFILL:
		    linewidth = cursorcol;
		    command (CMDFILL, nix);
		    goto funcdone;

		case CCJUSTIFY:
		    linewidth = cursorcol;
		    command (CMDJUST, nix);
		    goto funcdone;

		case CCCENTER:
		    linewidth = cursorcol;
		    command (CMDCENTER, nix);
		    goto funcdone;

		case CCEXIT:
		    command (CMDEXIT, nix);
		    goto funcdone;

		case CCRANGE:
		    donetype = command (CMDQRANGE, nix);
		    goto doneswitch;

		case CCREDRAW:
		case CCBOX:
		case CCABORT:
		case CCCLRTABS:
		case CCCLEAR:
		case CCSTOPX:
		case CCQUIT:
#endif LMCCMDS
		case CCSPLIT:
		case CCJOIN:
		case CCUNAS1:
		case CCREPLACE:
		case CCDEL:
		default:
		    goto notimperr;
	    }

doneswitch:
	    switch (donetype) {
	    case NOMEMERR:
		mesg (ERRALL + 1, "You have run out of memory.  Exit now!");
		break;

	    case NOSPCERR:
		mesg (ERRALL + 1, "No disk space.  Get help!");
		break;

	    case TOOLNGERR:
		mesg (ERRALL + 1, "Can't make file that long");
		break;

	    case NOTSTRERR:
		mesg (ERRALL + 1, "Argument must be a string.");
		break;

	    case NOWRITERR:
		mesg (ERRALL + 1, "You cannot modify this file!");
		break;

	    noargerr:
	    case NOARGERR:
		mesg (ERRALL + 1, "Invalid argument.");
		break;

	    nomarkerr:
	    case NDMARKERR:
		mesg (ERRALL + 1, "Area must be marked");
		break;

	    case NOMARKERR:
		mesg (ERRALL + 1, "Can't do that with marks set");
		break;

	    norecterr:
	    case NORECTERR:
		mesg (ERRALL + 1, "Can't do that to a rectangle");
		break;

	    case NOTRECTERR:
		mesg (ERRALL + 1, "Can only do that to a rectangle");
		break;

	    case NORANGERR:
		mesg (ERRALL + 1, "Range not set");
		break;

	    case NOBUFERR:
		mesg (ERRALL + 1, "Nothing in that buffer.");
		break;

	    case CONTIN:
		goto contin;

	    case MARGERR:
		mesg (ERRALL + 1, "Cursor stuck on right margin.");
		break;

	    notinterr:
	    case NOTINTERR:
		mesg (ERRALL + 1, "Argument must be numeric.");
		break;

	    notposerr:
	    case NOTPOSERR:
		mesg (ERRALL + 1, "Argument must be positive.");
		break;

	    notimperr:
	    case NOTIMPERR:
		mesg (ERRALL + 1, "That key sequence is not implemented.");
		break;
	    }
	    continue;

	    notcmderr:
	    mesg (ERRALL + 1, "Can't do that in Command Mode");
	    continue;

	    nointerr:
	    mesg (ERRALL + 1, "No operation to interrupt");
	    continue;

	    badkeyerr:
	    mesg (ERRALL + 1, "Bad key error - editor error");
	    continue;
	}
    }
    /* never returns */
    /* NOTREACHED */
}


#ifdef COMMENT
Cmdret
edkey (key, cmdflg)
    Char key;
    Flag cmdflg;
.
    Common code for OPEN, CLOSE, and PICK keys.
    If cmdflg != 0, then do CMD OPEN, CMD CLOSE, CMD PICK.
#endif
Cmdret
edkey (key, cmdflg)
Char key;
Flag cmdflg;
{
    Small opc;
    Small buf;

    switch (key) {
    case CCOPEN:
	opc = OPOPEN;
	buf = 0;
	break;

    case CCPICK:
	opc = OPPICK;
	buf = QPICK;
	break;

    case CCCLOSE:
	opc = OPCLOSE;
	buf = QCLOSE;
	break;

    case CCERASE:
	opc = OPERASE;
	buf = QERASE;
	break;
    }

    if (curmark) {
	if (!cmdflg)
	    return edmark (opc, buf);
	else
	    return NOMARKERR;
    }
    else {
	if (!cmdflg)
	    return ed (opc, buf,
		       curwksp->wlin + cursorline, (Ncols) 0,
		       (Nlines) 1, (Ncols) 0, YES);
	else {
	    switch (paramtype) {
	    case 0:
		if (key == CCOPEN)
		    return NOTIMPERR;
		return ed (OPINSERT, buf,
			   curwksp->wlin + cursorline,
			   curwksp->wcol + cursorcol,
			   (Nlines) 0, (Ncols) 0, YES);

	    case 1:
		if (parmlines <= 0)
		    return NOTPOSERR;
		return ed (opc, buf,
			   curwksp->wlin + cursorline, (Ncols) 0,
			   parmlines, (Ncols) 0, YES);

	    case 2:
		if (parmlines <= 0 || parmcols <= 0)
		    return NOTPOSERR;
		return ed (opc, buf,
			   curwksp->wlin + cursorline,
			   curwksp->wcol + cursorcol,
			   parmlines, (Ncols) parmcols, YES);
	    }
	    return NOTINTERR;
	}
    }
}
