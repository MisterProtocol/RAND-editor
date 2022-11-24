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
#include <ctype.h>

Cmdret edkey (Char, Flag);
extern Cmdret chgcase (Flag);


#ifdef NCURSES
extern void debug_inputkey ();
extern void testart();      /* what is this? */
extern Flag entering;
extern Flag debug_d_write;
extern Flag optskipmouse;
extern Flag finished_replay;
extern char *getEkeyname(int);
extern int toggle_mouse(char *);
extern Flag redrawflg;
extern void doMouseReplay(void);
extern void highlightarea(Flag, Flag);
extern void HiLightBracePairs(Flag);
extern int bracematching;
extern int brace_marked;
extern void hiliteTopBorder(S_window *);
extern void hiliteLeftBorder(S_window *);
extern Flag borderChanged;

#ifdef MOUSE_BUTTONS
extern int mouseFuncKey;
extern void getMouseButtonEvent(void);
extern void doMouseEvent(void);
#endif

#ifdef TAGS
extern int dotag(char *);
extern void PushTagCmds(void);
extern Flag editTagFile;
#endif

#endif /* NCURSES */

#ifdef COMMENT
void
mainloop ()
.
    Get keys from the input and process them.
#endif
void
mainloop ()
{
	char *nix = "\0";
	char *abortstg = "ab\0";
	char *quitstg = "q\0";
	char *pickstg = "pick\0";
	char *boardstg = "l\0";
	char *s2, *tmpb, b[BUFSIZ]; /* added 10/18/82 MAB */

    for (;;) {
	key = (Char) 0;
funcdone:
	loopflags.clear = YES;
newnumber:
	if (loopflags.clear && !loopflags.hold) {
	    mesg (TELALL + 1, loopflags.beep ? "\007" : "");
	    loopflags.beep = NO;
	}
	Block {
	    Reg1 Nlines nlines;
	    char ich[32];
	    nlines = curwksp->wlin + cursorline + 1;
	    if (infoline != nlines) {
#ifdef LA_LONGFILES
	    /*  sprintf (ich, "%-6ld", nlines); */
		snprintf (ich, 32, "%ld", nlines);
#else /* LA_LONGFILES */
		snprintf (ich, 32, "%-6d", nlines);
#endif /* LA_LONGFILES */
/** /
dbgpr("mainloop:  nlines=%d, cursorline=%d, curwksp->wlin=%d\n",
nlines, cursorline, curwksp->wlin);
/ **/
		info (inf_line, 9, ich);
		infoline = nlines;
	    }
	}

	if (curfile != infofile && (fileflags[curfile] & INUSE)) {
	    info (inf_file, (Scols)strlen (names[infofile]), names[curfile]);
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

/** /
dbgpr("mainloop: cursorline=%d curwksp->wlin=%d curmark->mrkwinlin=%d curmark->mrklin=%d\n",
    cursorline, curwksp->wlin, curmark->mrkwinlin, curmark->mrklin);
dbgpr("mainloop: infomark:  nlines=%d ncols=%d, marklines=%d markcols=%d\n",
nlines, ncols, marklines, markcols);
/ **/

		if (marklines != nlines) {
#ifdef LA_LONGFILES
		    sprintf (mklinstr, "%ld", nlines);
#else /* LA_LONGFILES */
		    sprintf (mklinstr, "%d", nlines);
#endif /* LA_LONGFILES */
		    marklines = nlines;
		}
		if (markcols != ncols) {
		    if (ncols)
#ifdef LA_LONGLINES
			sprintf (mkcolstr, "x%ld", ncols);
#else /* LA_LONGLINES */
			sprintf (mkcolstr, "x%d", ncols);
#endif /* LA_LONGLINES */
		    else
			mkcolstr[0] = '\0';
		    markcols = ncols;
/**
dbgpr("mainloop:  curmark, marklines=%d markcols=%d, mklinstr=(%s) mkcolstr=(%s)\n",
marklines, markcols, mklinstr, mkcolstr);
/ **/
		}
	    }
	    Block {
		Reg1 char *cp;
		Reg3 int len;
		cp = append (mklinstr, mkcolstr);
		len = (int) strlen (cp);
		info (inf_area, max (len, infoarealen), cp);
		infoarealen = len;
		sfree (cp);
	    }

	    /* display marked area */
		/* don't hilite if playing a macro or replaying a crash */
	    if (!playing && !replaying) {
		highlightarea(YES,redrawflg);
		/* In some cases, we're off by 1 line somewhere in highligharea() */
		mvcur(-1, -1, curwin->ttext+cursorline, curwin->ltext+cursorcol);
	    }

	}
	else if (bracematching || brace_marked) {
	    HiLightBracePairs(YES);
	}

	if (borderbullets) {
	    dobullets (loopflags.bullet, loopflags.clear);
	    loopflags.bullet = NO;
	}
	loopflags.clear = NO;

/* test, show a little hilite on active border margin */
if (borderChanged || redrawflg) {
    hiliteTopBorder(curwin);  /* testing */
    hiliteLeftBorder(curwin);  /* testing */
    borderChanged = 0;
}
	redrawflg = 0;  /* set in e.cm.c, case CMDREDRAW */

	if (cmdmode) {
	    loopflags.hold = NO;
	    goto gotcmd;
	}
#ifdef LMCLDC
	(*term.tt_xlate) (0);   /* reset the graphics if needed */
#endif /* LMCLDC */


/**
dbgpr("e.m.c, mainloop(), waiting for mGetkey\n");
 **/

	mGetkey (WAIT_KEY, NULL);   /* returns value in global 'key' */

	/** /
	if( key < ' ' || key >= 0177 ) {
	  dbgpr("e.m.c: mGetkey returned key=(%d)(%04o)(%s)\n\n", key, key, getEkeyname(key));
	}
	else {
	  dbgpr("e.m.c, mGetkey returned key=(%d)(%04o)('%c') line=%d col=%d\n\n",
	    key, key, (char)key, cursorline, cursorcol);
	}
	/ **/

/* /
dbgpr("mainloop:  curwin=(%o) wholescreen=(%o) enterwin=(%o) infowin=(%o)\n",
    curwin, &wholescreen, &enterwin, &infowin);
dbgpr("mainloop:  curwksp=(%o) wholescreen.wksp=(%o) enterwin.wksp=(%o) infowin.wksp=(%o)\n",
    curwksp, wholescreen.wksp, enterwin.wksp, infowin.wksp);
/ */

/* begin curses changes */
	if( key == CCMOUSE ) {
	    /*debug_inputkey(KEY_MOUSE, "e.m.c mainloop(), mGetkey()"); */
	    /* handle mouse event */
	    doMouseEvent();
#ifdef MOUSE_BUTTONS
	    if (mouseFuncKey != -1) {
		key = mouseFuncKey;
		if (recording)  /* testing */
		    RecordChar((unsigned Short) key);
	    }
	    else
#endif /* MOUSE_BUTTONS */
	    continue;
	}
	/* use F6 to enable/disable the mouse so one can use copy/paste
	 * to insert text into E
	 */
	else if( key == CCMOUSEONOFF ) {
	/*  dbgpr("mainloop, got key CCMOUSEONOFF, toggle mouse\n");*/
	    toggle_mouse("");
	    continue;
	}

/* end curses changes */

	if (loopflags.hold) {
	    loopflags.hold = NO;
	    /*mesg (TELALL);*/
	    mesg (TELALL+1, "");
	}

    /** /dbgpr("mainloop, got key %o\n", key);   / **/


	Block {
	    Reg1 Small donetype;
	    if (   !CTRLCHAR                /*  (key < 040 || 0177 <= key) */
		|| key == CCCTRLQUOTE
		|| key == CCBACKSPACE
		|| key == CCDELCH
	       ) {
		/*dbgpr("e.m.c: before printchar() key=%d (%c)\n", key, (char)key);*/
		donetype = printchar ();
		goto doneswitch;
	    }
	    Block {
		/*dbgpr("e.m.c: skipping printchar() key=%d (%c)\n", key, (char)key);*/
		Reg2 Small cm;
		if (key < MAXMOTIONS && (cm = cntlmotions[key])) {
		    /* in the next several lines, a redisplay is always done,
		     *  EXCEPT
		     * +TAB, -TAB, and <- and->arrows within the screen
		     **/
		    if (   (cm == RT && cursorcol + 1 <= curwin->redit)
			|| (cm == LT && cursorcol - 1 >= curwin->ledit)
			|| cm == TB
			|| cm == BT
		       )
			{}
		    else
			credisplay (NO);
		    if (key == CCRETURN) {
			if (autofill) Block {
			    Ncols nc, df;
			    Nlines nl;
			    nl = (cursorline == curwin->btext) ? defplline : 0;
			    nc = autolmarg - curwksp->wcol;
			    for (df = 0; nc < 0; nc += deflwin, df += deflwin);
			    cursorline -= (Slines) nl;
			    cursorcol = (Scols) nc;
			    if (nl || df)
			      movewin(curwksp->wlin + nl, curwksp->wcol - df,
				cursorline, cursorcol, YES);
			}
			else if (cursorline == curwin->btext) {
			    /* Return on bottom line => +lines    */
			    vertmvwin (defplline);
			}
			flushkeys();
		    }
		    /*
		     *  e18 mod:  An RT (right arrow) at the right window
		     *  boundary no longer shifts the window left 1.
		     *  Also, an LT (left arrow) no longer shifts the
		     *  window right 1.
		     */
		    if (optstick) {
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
		    } else {
			if (cm == RT && cursorcol >= curwin->redit) {
			    movewin (curwksp->wlin, curwksp->wcol + defrwin,
				     cursorline, (Scols)(cursorcol + 1 - defrwin), YES);
			} else if (cm == LT && cursorcol <= curwin->ledit) {
			    Ncols df;
			    df = min (deflwin, curwksp->wcol);
			    movewin (curwksp->wlin, curwksp->wcol - df,
				     cursorline, (Scols)(cursorcol - 1 + df), YES);
			} else
			    movecursor (cm, 1);
		    }

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
		case CCCMD0:
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

		case CCPUT:         /* convert to a CMD: pick which is a put */
		/*  dbgpr("CCPUT line 395\n"); */
		    key = CCPICK;
		    paramtype = 0;  /* no cmdline arguments */
		    donetype = edkey (key, YES);
		    goto doneswitch;

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

#ifdef TAGS
		case CCTAG:
		    //dbgpr("key CCTAG, calling dotag(NULL)\n");
		    dotag(NULL);
		    if (editTagFile)
			PushTagCmds();
		    goto funcdone;
#endif

		case CCBRACE:
		    opstr = ""; /* for dbg in doSetBraceMode() */
		    doSetBraceMode("toggle");
		    if (brace_marked)
			HiLightBracePairs(NO);  /* to clear mark */
		    goto funcdone;

		case CCREGEX:
		    /* toggle patmode */
		    if (patmode)
			command(CMD_PATTERN, nix);
		    else
			command(CMDPATTERN, nix);
		    goto funcdone;

		case CCPLWIN:   /* +w button */
		    command(CMDWINDOW, nix);
		    goto funcdone;

		case CCMIWIN:   /* -w button */
		    command(CMD_WINDOW, nix);
		    goto funcdone;

		case CCMICLOSE: /* -clos button */
		    command(CMD_CLOSE, nix);
		    goto funcdone;

		case CCMIERASE: /* -eras button */
		    command(CMD_ERASE, nix);
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
#ifdef RECORDING
		    if (playing) {
			playing = 0;
			mesg (ERRALL + 1, "Playback cancelled.");
			loopflags.hold = YES;
			d_put(0);
			goto funcdone;
		    }
#endif /* RECORDING */
		    goto nointerr;

		case CCTABS:
		    sctab (curwksp->wcol + cursorcol, YES);
		    goto funcdone;


		case CCMARK:
		    mark ();
		    goto funcdone;

		case CCUNMARK:
		    unmark ();
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
#endif /* LMCHELP */
#ifdef RECORDING
		case CCRECORD:
		    donetype = SetRecording(CCRECORD);
		    goto doneswitch;

		case CCPLAY:
		    donetype = PlayRecording(1);
		    goto doneswitch;
#endif /* RECORDING */

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
#endif /* LMCCASE */
		case CCAUTOFILL:
		    infoauto(NO);
		    goto funcdone;

		case CCLWORD:
		    donetype = mword(-1, 1);
		    goto funcdone;

		case CCRWORD:
		    donetype = mword(1, 1);
		    goto funcdone;

#ifdef LMCDWORD
		case CCDWORD:
		    donetype = dodword (YES);
		    goto doneswitch;
#endif /* LMCDWORD */

		case CCMOUSEONOFF:
		    toggle_mouse("");
		    goto doneswitch;

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
		case CCNULL:
		    goto funcdone;
	    }
gotcmd:
	    param ();
/** /
dbgpr("after param(): key=(%03o)(%d)(%c) cmdmode=%d,paramtype=%d paramv=(%s)\n",
key, key, key, cmdmode, paramtype, paramv);
/ **/
	    if (cmdmode && key != CCRETURN)
		goto notcmderr;

#ifdef MOUSE_BUTTONS
	    /* check if a mouse button is pressed while in CMD mode */
	    if( key == CCMOUSE ) {
		getMouseButtonEvent();
		if (mouseFuncKey != -1) {
		    key = mouseFuncKey;
		}
/** /
dbgpr("after param(), mouse->button: key=(%04o)(%s) cmdmode=%d, paramv=(%s) paramtype=(%d)\n",
key, getEkeyname(key), cmdmode, paramv, paramtype);
/ **/
	    }
#endif /* MOUSE_BUTTONS */

	    flushkeys();  /* added 6/21/2021 */

	    switch (key) {
		case CCCMD:
		case CCCMD0:
		    goto funcdone;

		case CCLWINDOW:
		case CCRWINDOW:
		    switch (paramtype) {
		    case 0:
			if (key == CCRWINDOW) {
				Ncols n;
			    GetLine (cursorline + curwksp->wlin);
			    /*
			     *  shift window so the end of line is displayed
			     *  (w/little xtra space); don't do anything if
			     *  entire line is already visible.
			     */
			    if (curwin->rmarg + curwksp->wcol > ncline - 1)
				break;
			    n = ncline - curwksp->wcol - curwin->rmarg + 5;
			    horzmvwin ((Ncols) n);
			    /* 11/21/91: position cursor at end of line */
			    GetLine (cursorline + curwksp->wlin);
			    n = ncline - 1 - curwksp->wcol - curwksp->ccol;
			    movecursor (cntlmotions[CCMOVERIGHT], n>0 ? n : -n);
			}
			else {
			    horzmvwin ((Ncols) -curwksp->wcol);
			    /* 11/21/91: position cursor at beginning of line */
			    movecursor (cntlmotions[CCMOVELEFT], cursorcol);
			}
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
	/*      case CCTAGSRCH:  */

//dbgpr("e.m.c: got (%o)=CC{PL,MI,TAG}SRCH paramtype=%d paramv=(%s)\n",
//key, paramtype, paramv);

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
#ifdef OUT
		    { int i;
		      int len = (int)strlen(searchkey);
		      dbgpr("srch,len=%d:");
		      for(i=0; i<len;i++) {
			if (isprint(searchkey[i]))
			  dbgpr("%c", searchkey[i]);
			else
			  dbgpr("(%o)", searchkey[i]);
		      }
		      dbgpr("\n");
		    }
#endif /* OUT */
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
			GetLine (curwksp->wlin + cursorline);
			if ((k = ncline - 1 - curwksp->wcol - cursorcol) > 0) {
			    donetype = ed (OPERASE, QERASE,
					   curwksp->wlin + cursorline,
					   curwksp->wcol + cursorcol,
					   (Nlines) 1, k, YES);
			    goto doneswitch;
			}
		    }
		    goto funcdone;

		case CCPUT:         /* convert to a CMD: pick which is a put */
		    key = CCPICK;
		 /* dbgpr("CCPUT line 749\n"); */
		    paramtype = 0;
		    /* fall through */
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
			int lns;
		    case 0:
			switch (key) {
			case CCHOME:
			    movecursor (cntlmotions[CCHOME], 1);
			    key = CCMOVEDOWN;
			    goto llcor;

			case CCMOVEDOWN:
			    if ((lns = (int)(la_lsize (curlas) -
				       (curwksp->wlin + cursorline))) > 0)
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
			    GetLine (curwksp->wlin + cursorline);
			    if ((lns = (int)(ncline - 1 - (curwksp->wcol + cursorcol)))
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
			lns = (int)parmlines;
 multmove:              movecursor (cntlmotions[key], lns);
			break;

		    case 2:
			goto norecterr;

		    default:
			goto notinterr;
		    }
		    goto funcdone;

		case CCRETURN:
		    donetype = command (0, nix);
		    goto doneswitch;

#ifdef LMCHELP
		case CCHELP:
		    command (CMDHELP, boardstg);
		    goto funcdone;
#endif /* LMCHELP */
#ifdef RECORDING
		case CCRECORD:
		    donetype = UnSetRecording();
		    goto doneswitch;

		case CCPLAY:
		    if (paramtype == 1)
			PlayRecording ((int)parmlines);
		    else
			goto notinterr;
		    goto funcdone;
#endif /* RECORDING */
		case CCAUTOFILL:
		    infoauto(YES);
		    goto funcdone;

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
			vertmvwin ((Nlines) cursorline - (key == CCPLLINE
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
			chgwindow ((Small)(parmlines - 1));
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
			mword((key == CCLWORD) ? -1 : 1, (int)parmlines);
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
		    goto doneswitch;
#endif /* LMCDWORD */
		case CCCOVER:
		    donetype = command (CMDINSERT, pickstg);
		    goto doneswitch;

		case CCOVERLAY:
		    donetype = command (CMDOVERLAY, pickstg);
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
#ifdef TAGS
		case CCTAG:
//dbgpr("e.m.c: calling command(CMDTAG, nix) from <cmd>tag xxx\n");
		    command (CMDTAG, nix);
		    //if (editTagFile)
		    //    PushTagCmds();
		    goto funcdone;
#endif

		case CCREDRAW:
		case CCBOX:
		case CCABORT:
		case CCCLRTABS:
		case CCCLEAR:
		case CCSTOPX:
		case CCQUIT:
		case CCSPLIT:
		case CCJOIN:
		case CCUNAS1:
		case CCREPLACE:
		case CCDEL:
		default:
		//  dbgpr("key=(%03o)(%d) not implemented, entering=%d cmdmode=%d\n",
		//      key,key,entering,cmdmode);
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

#ifdef FILELOCKING
	    case NOLOCKERR:
		mesg (ERRALL + 1, "Can't lock file, someone else must be editing it!");
		break;
#endif /* FILELOCKING */

	    noargerr:
	    case NOARGERR:
		mesg (ERRALL + 1, "Invalid argument.");
		break;

	    case NDMARKERR:
		mesg (ERRALL + 1, "Area must be marked");
		break;

	    nomarkerr:
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
	//  dbgpr("bad key error: key=(%o)(%d)\n", key, key);
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

/** /
dbgpr("edkey: key=(%04o)(%s) cmdflg=%d, paramtype=%d parmlines=%d, parmcols=%d\n",
    key, getEkeyname(key), cmdflg, paramtype, parmlines, parmcols);
/ **/

    switch (key) {
    case CCOPEN:
	opc = OPOPEN;
	buf = 0;
	break;

    case CCPICK:
    case CCPUT:         /* added 8/30/2021 */
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


