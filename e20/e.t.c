#ifdef COMMENT
--------
file e.t.c
    terminal-independent input and output routines
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#ifdef GCC
#include <errno.h>
#endif

#include "e.h"
#include "e.inf.h"
#include "e.m.h"
#include "e.tt.h"
#ifdef LMCMARG
#include "e.wi.h"
#endif /* LMCMARG */

#ifdef  SYSSELECT
#include <sys/time.h>
#else /* SYSSELECT */
#include SIG_INCL
#endif /* SYSSELECT */

#ifdef TESTRDNDELAY
# undef EMPTY
# define RDNDELAY
# define O_RDONLY 0
# define O_NDELAY 0
# define F_SETFL  0
# define fcntl(a,b,c) 0;
#endif /* TESTRDNDELAY */

#ifdef RDNDELAY
#include <fcntl.h>
#else /* RDNDELAY */
# define xread(a,b,c) read (a, b, c)
# ifdef EMPTY
#   define xempty(fd) empty(fd)
# else  /* EMPTY */
#   define xempty(fd) 1
# endif /* EMPTY */
#endif /* RDNDELAY */

#define EINTR   4       /* should be #included but for problems ... */

#ifdef  NOCMDCMD
extern  Flag optnocmd;          /* YES = -nocmdcmd; <cmd><cmd> functions disabled */
#endif /* NOCMDCMD */

/* set outside putup (), looked at by putup () */
Flag entfstline = NO;           /* says write out entire first line */
Flag needputup; /* because the last one was aborted */
Flag atoldpos;
Slines  newcurline = -1;        /* what cursorline will be set to after */
Scols   newcurcol  = -1;        /* similar to newcurline                */
Flag    freshputup;             /* ignore previous stuff on these lines */
Flag    nodintrup;              /* disallow display interruption */
Scols putupdelta;               /* nchars to insert (delete) in putup() */

Flag entering = NO;     /* set if e is in the param () routine. */

int fnbcol (char *, Ncols, Ncols);

unsigned Short getkey1 (Flag, struct timeval *);
Flag dintrup (void);
Flag la_int (void);
void exchgcmds (void);

#ifdef NCURSES

/* these are replacement versions of getkey and getkey1 */
unsigned Short mGetkey (Flag, struct timeval *);
unsigned Short mGetkey1(Flag, struct timeval *);
Char MapCursesKey(Char);

/* If !NULL, points to remaining codes to process from a F_KEY mapping/press */
/* which are defined in the user's EKBFILE */
Uchar *FKey_p;

#include "e.keys.h"
#include <ctype.h>

char *getEkeyname(int);
char *getCursesKeyname(int);
char *dbg_KeyName(int);

#ifdef MOUSE_BUTTONS
extern int mouseFuncKey;
extern void getMouseButtonEvent();
#endif /* MOUSE_BUTTONS */

#endif /* NCURSES */

Flag finished_replay = NO;
extern off_t replay_filesize;
extern int replay_stopcount;

extern void scvtab (Ncols, Flag);
extern void putupwin ();
extern void putup ();
extern void multchar ();
extern void offbullets ();
extern void dobullets ();
#ifdef LMCMARG
extern void tm_unbullet (Flag, Ncols, Ncols, int, Flag);
extern void bm_unbullet (Flag, Ncols, Ncols, int, int);
extern void stmarg0 (Ncols, Flag, Flag);
#endif /* LMCMARG */
extern void setbul ();
extern void clrbul ();
extern void savecurs ();
extern void restcurs ();
extern void burncurs ();
extern void poscursor ();
extern void writekeys ();
/*extern void putch ();*/
extern void param ();
extern void exchgcmds ();
extern void getarg ();
extern void limitcursor ();
extern void info ();
extern void credisplay ();
extern void screenexit ();
extern void tglinsmode ();
extern void fsynckeys ();
extern Flag vinsdel (Slines, Slines, Flag);
extern int get_profilekey (Flag);
extern int d_vmove ();
extern void doMouseReplay(int, int);

/*void dbg_winlist();*/


void MesgHack (Small, char *);

void mesg (int, ...);

#define SIGSEGV_PATCH  /* tmp to deal with mem violation */

#ifdef COMMENT
void
putupwin ()
.
    Do a putup() of the whole current window.
#endif
void
putupwin ()
{
    savecurs ();
    putup (0, curwin->btext, 0, MAXWIDTH);
    restcurs ();
    return;
}

#ifdef COMMENT
void
putup (ls, lf, strtcol, ncols)
    Slines  ls;
    Slines  lf;
    Scols   strtcol;    /* starting col for partial line redraw */
    Scols   ncols;      /* number of columns for partial line redraw */
.
    Puts up (displays) lines from 'ls' to 'lf'.
    'ls' and 'lf' are line numbers relative to the top of the current window,
    where 0 would be the first line of the window.
    If 'ls' is negative,
    only puts up line 'lf' and takes it from cline instead of disk.
    Start at column 'strtcol' (rel. to left column of window) on each line.
    If 'ncols' != MAXWIDTH then it is assumed that the line has not gotten
    shorter or longer since last putup, and you know what you are doing,
    and you only want to putup from 'strtcol' to 'strtcol' + 'ncols'.
    Caller must do a poscursor after calling putup.
    If the caller does not want the display of the borders to change, he
    clears the global "chgborders" before calling putup (..); If he wants
    dot borders for this putup, he sets chgborders to 2.
    If the caller sets the global 'freshputup' flag,
    it means that the line on the screen is blank, and we will
    not need to put out any leading or trailing blanks, and we will have
    to redraw the borders.
#endif

void
putup (ls, lf, strtcol, ncols)
Slines  ls;
Slines  lf;
Scols   strtcol;    /* starting col for partial line redraw */
Scols   ncols;      /* number of columns for partial line redraw */
{
    Reg3 Nlines ln;     /* the line worked on within the for loop */
    Reg6 Ncols  fc;
    Reg5 Ncols  lcol;   /* = curwksp->wcol */
    Reg8 Scols  rlimit; /* = curwin->rtext + 1 */
    Reg7 Scols  lstcol=0; /* rightmost col of text on line              */
    Reg4 Ncols  col;    /* variable col, reset to strtcol for each line */
    int     lmc;        /* left  margin char that should be on screen */
    int     rmc;        /* right margin char that should be on screen */
    Flag    usecline;   /* ls was < 0, so use cline */
    Flag    offendflg;  /* off the end of the file */


    if (strtcol > (lcol = curwksp->wcol) + (rlimit = curwin->rtext + 1))
	goto ret;

    /*  Due to a bug while collecting keys in CMD: mode, this routine is
     *  sometimes called with curwin pointing to the enterwin.  This causes
     *  a core dump because some expected members are missing, eg null pointers.
     */
    if (curwin == &enterwin) {
   /* dbgpr("putup:  curwin==&enterwin, returning\n");*/
      goto ret;
    }

/** /
dbgpr("putup0: ls=(%d) lf=(%d) strtcol=(%d) ncols=(%d), lcol=(%d), cursorcol=(%d) cursorlin=(%d) rlimit=(%d), \n",
ls,lf,strtcol,ncols,lcol,cursorcol,cursorline,rlimit);
dbgpr("putup0a:  curwksp->wlin=%d\n", curwksp->wlin);
/ **/

    /* if the caller knows that after the putup he will move the cursor
     * to someplace other than where it is, then he will set newcurline
     * and/or newcurcol to the new position
     * newcurline is always set to -1 on return from putup.
     **/
    if (newcurline == -1)
	newcurline = cursorline;
    if (newcurcol == -1)
	newcurcol = cursorcol;
    atoldpos = NO;
    if (!nodintrup && dintrup ()) {
	needputup = YES;
	goto ret;
    }
    else
	needputup = NO;

    if (lf > curwin->btext)
	lf = curwin->btext;
    if ((usecline = ls < 0))
	ls = lf;
    lmc = lcol == 0 ? LMCH : MLMCH;

    for (ln = ls;  ln <= lf; ln++) {
	rmc = RMCH;
	offendflg = NO;

	if (!usecline) {        /* get the line into cline */
	    if (   ln != lf
		&& !entfstline
		&& ( replaying || (++intrupnum > DPLYPOL) )
		&& !nodintrup
		&& dintrup ()
	       ) {
		needputup = YES;
		break;
	    }
	    GetLine (curwksp->wlin + ln);
	    if (xcline) {
		offendflg = YES;
		lmc = ELMCH;
	    }
	    if (ln >= newcurline)
		atoldpos = YES;
	}

	/* deal with the left border */
	/* if this is the first line of the putup, back up and fix left    */
	/*   border as far up as necessary. */

	if (freshputup) {
	    poscursor (-1, ln);
	    putch ((Uchar)(chgborders == 1 ? lmc : INMCH), NO);
	    if (curwin->lmchars)
	      curwin->lmchars[ln] = lmc;
	} else Block {
	    Reg1 Slines lt;
	    Slines lb;
	    if (ln == ls && lmc != ELMCH)
		lb = 0;
	    else
		lb = ln;
	    for (lt = ln; lt >= lb; lt--) {
/**
dbgpr("e.t.c:270, lt=%d, ln=%d length of lmchars=(%d) curwin=(%o) enterwin=(%o)\n",
lt, ln, strlen(curwin->lmchars), curwin, enterwin );
 **/
		if ((curwin->lmchars[lt] & CHARMASK) == lmc)
		    break;
		if (   chgborders
		    && (   !borderbullets
			|| chgborders == 2
			|| lt != newcurline
		       )
		   ) {
		    poscursor (-1, lt);
		    putch (chgborders == 1 ? lmc : INMCH, NO);
		}
		if (curwin->lmchars)
		  curwin->lmchars[lt] = lmc;
	    }
	}

	/*  determine fc: first non-blank column in cline
	 *  if portion of line to be displayed is blank, then set fc
	 *  to rlimit
	 **/

	/*dbg*/
	/*int fc_rval;*/

	if (   offendflg
	    || (fc = fnbcol (cline, lcol, min (ncline, lcol + rlimit))) < lcol
	   ) {
	    /*fc_rval = fc;*/
	    fc = rlimit;
	}
	else
	    fc -= lcol;

/* /
dbgpr("putup1: rlimit=(%d) fc=(%d), col=(%d) offendflg=(%d) fc_rval=(%d) ncline=(%d)\n",
       rlimit, fc, col, offendflg,fc_rval,ncline);
/ **/

	/* how many leading blanks do we need to put out */
	if (entfstline) {
	    entfstline = NO;
	    col = 0;
	} else {
	    if (freshputup)
		col = fc;
	    else
		col = min (fc, curwin->firstcol[ln]);
	    if (col < strtcol)
		col = strtcol;
	}
	curwin->firstcol[ln] = fc;
	poscursor ((Scols) col, ln);
/* /
dbgpr("putup2: rlimit=(%d) fc=(%d), col=(%d)\n", rlimit, fc, col);
/ */
#ifdef OUT
	if (stdout->_cnt < 100)   /* smooth any outputting breaks  */
	    d_put (0);
#endif /* OUT */
	Block {
	    Reg1 Scols tmp;
	    if ((tmp = fc - col) > 0) {
		multchar (' ', tmp);
		col = fc;
	    }
	}

	/* determine the rightmost col of the new text to put up */
	/* this should do better on lines that go past right border */
/* /
dbgpr("putup3: rlimit=(%d) lstcol=(%d) col=(%d)\n", rlimit,lstcol,col);
/ */
	if (offendflg)
	    lstcol = 0;
	else Block {
	    Reg1 Ncols tmp;
	    tmp = (ncline - 1) - lcol;
	    if (tmp < 0)
		tmp = 0;
	    else if (tmp > rlimit) {
		tmp = rlimit;
		rmc = MRMCH;
	    }
	    lstcol = tmp;
	}
	if (lstcol < col)
	    lstcol = col;
/* /
dbgpr("putup4: rlimit=(%d) lstcol=(%d) col=(%d)\n", rlimit,lstcol,col);
/ */
	Block {
	    Reg1 Scols ricol;   /* rightmost col of text to put up */
/* /
dbgpr("putup5: lstcol=(%d), rlimit=(%d) strtcol=(%d) ncols=(%d)\n",
   rlimit, lstcol, strtcol, ncols);
/ */
	    ricol = min (lstcol, min (rlimit, strtcol + ncols));
/* /
dbgpr("putup6: ricol=(%d), col=(%d) lcol=(%d)\n", ricol, col, lcol);
/ */
	    /* put out the text */
	    if (ricol - col > 0) {
		/*  if (putupdelta && col >= strtcol) {
			d_hmove (putupdelta, (Short) (ricol - col),
				 &cline[lcol + col]);
		    } else
		 */     d_write (&cline[lcol + col], (Short) (ricol - col));
		cursorcol += (ricol - col);
	    }
	}
	/* determine how many trailing blanks we need to put out */
	Block {
	    Reg1 Scols tmp;
	    if (   !freshputup
		&& (tmp = curwin->lastcol[ln] - lstcol) > 0
	       )
		multchar (' ', tmp);
	}
	curwin->lastcol[ln] = (lstcol == fc) ? 0 : lstcol;

	/* what to do about the right border */
	if (freshputup) {
	    poscursor (curwin->rmarg - curwin->ltext, ln);
	    putch (chgborders == 1 ? rmc : INMCH, NO);
	    curwin->rmchars[ln] = rmc;
	} else if ((curwin->rmchars[ln] & CHARMASK) != rmc) {
	    if (   chgborders
		&& (   !borderbullets
		    || chgborders == 2
		    || ln != newcurline
		   )
	       ) {
/* /
dbgpr("(else) freshputup=%d, rmc=%d, curwin->rmchars[ln]=%d\n",
  freshputup,rmc,curwin->rmchars[ln]&CHARMASK);
dbgpr(" chgborders=%d, borderbullets=%d, ln=%d, newcurline=%d\n",
  chgborders, borderbullets, ln, newcurline);
/ */
		poscursor (curwin->rmarg - curwin->ltext, ln);
		putch (chgborders == 1 ? rmc : INMCH, NO);
	    }
	    curwin->rmchars[ln] = rmc;
	}
    }
ret:
    putupdelta = 0;
    newcurline = -1;
    newcurcol = -1;
    return;
}

#ifdef COMMENT
int
fnbcol (line, left, right)
    char *line;
    Ncols left;
    Ncols right;
.
    Look for a non-blank character in line between line[left] and line[right]
    If right < left, or a newline or non-blank is encountered,
	return left - 1
    else
	return index into array line of the non-blank char.
#endif
int
fnbcol (line, left, right)
Reg4 char *line;
Reg5 Ncols left;
Ncols right;
{
    Reg1 char *cp;
    Reg2 char *cpr;
    Reg3 char *cpl;

/*  dbgpr("fnbcol: left=(%d) right=(%d) line[0]=(%c) line[1]=(%c) line[2]=(%c)\n", left,right,line[0],line[1],line[2]); **/

    if (right < left)
	return left - 1;

    /* this code is a little tricky because it looks for a premature '\n' */
    cpl = &line[left];
    cpr = &line[right];
    for (cp = line; ; cp++) {
	if (   *cp == '\n'
	    || cp >= cpr
	   )
	    return left - 1;
	if (   *cp != ' '
	    && cp >= cpl
	   )
	    return cp - line;
    }
    /* returns from inside for loop */
    /* NOTREACHED */
}

#ifdef COMMENT
void
multchar (chr, num)
    Char chr;
    Scols num;
.
    Write 'num' 'chr's to d_write.
    Advance cursorcol by 'num' columns.
#endif
void
multchar (chr, num)
Reg4 Char chr;
Reg1 Scols num;
{
    Reg2 Small cnt;
    Reg3 char *buf;
    char cbuf[3];

    if (num <= 0)
	return;
    cursorcol += num;
    buf = cbuf;
    while (num > 0) {
	cnt = Z;
	if (num > 3) {
	    buf[cnt++] = VCCARG;
	    if (num > 95) {
		buf[cnt++] = 95 + 040;
		num -= 95;
	    }
	    else {
		buf[cnt++] = num + 040;
		num = Z;
	    }
	}
	else {
	    if (num > 2) {
		buf[cnt++] = chr;
		num--;
	    }
	    if (num > 1) {
		buf[cnt++] = chr;
		num--;
	    }
	    num--;
	}
	buf[cnt++] = chr;
	d_write (buf, cnt);
    }
    return;
}

static Scols  oldccol;
static Slines oldcline;

#ifdef COMMENT
void
offbullets ()
.
    Turn off border bullets on the sides of the window.
#endif
void
offbullets ()
{
    Reg2   Slines ocl;

    if (!borderbullets)
	return;
    oldccol = cursorcol;
    ocl = oldcline;
    oldcline = cursorline;
    poscursor (curwin->rtext + 1, ocl);
    putch (curwin->rmchars[ocl], NO);
    poscursor (-1, ocl);
    putch (curwin->lmchars[ocl], NO);
    poscursor (oldccol, oldcline);
}

#ifdef COMMENT
void
dobullets (allbullets, newbullets)
    Flag allbullets;
    Flag newbullets;
.
    Update the border bullets to the current cursor position.
    To minimize terminal cursor addressing, this routine has more
    code than would be necessary without the optimization.
#endif
void
dobullets (allbullets, newbullets)
Reg3 Flag allbullets;
Flag newbullets;
{
    Reg1   Scols  occ;
    Reg2   Slines ocl;

    if (!borderbullets)
	return;
    occ = oldccol;
    oldccol = cursorcol;
    ocl = oldcline;
    oldcline = cursorline;
    if (newbullets || (oldccol != occ)) {
	/* do top */
#ifdef LMCMARG
	if (occ < oldccol)
	    /*tm_unbullet (allbullets, occ, -1, TMCH, BTMCH, NO);*/
	    tm_unbullet (allbullets, occ, -1, TMCH, NO);
#else /* LMCMARG */
	if (!allbullets && occ < oldccol) {
	    poscursor (occ, -1);
	    putch (TMCH, NO);
	}
#endif /* LMCMARG */
	poscursor (oldccol, -1);
	putch (BULCHAR, NO);
#ifdef LMCMARG
	if (occ > oldccol)
	    /*tm_unbullet (allbullets, occ, -1, TMCH, BTMCH, NO);*/
	    tm_unbullet (allbullets, occ, -1, TMCH, NO);
#else /* LMCMARG */
	if (!allbullets && occ > oldccol) {
	    poscursor (occ, -1);
	    putch (TMCH, NO);
	}
#endif /* LMCMARG */
	/* do bottom */
#ifdef LMCMARG
	if (occ < oldccol)
	    /*bm_unbullet (allbullets, occ, curwin->btext + 1, BMCH, TTMCH, NO);*/
	    bm_unbullet (allbullets, occ, curwin->btext + 1, BMCH, TTMCH);
#else /* LMCMARG */
	if (!allbullets && occ < oldccol) {
	    poscursor (occ, curwin->btext + 1);
	    putch (BMCH, NO);
	}
#endif /* LMCMARG */
	poscursor (oldccol, curwin->btext + 1);
	putch (BULCHAR, NO);
#ifdef LMCMARG
	if (occ > oldccol)
	    /*bm_unbullet (allbullets, occ, curwin->btext + 1, BMCH, TTMCH, NO);*/
	    bm_unbullet (allbullets, occ, curwin->btext + 1, BMCH, TTMCH);
#else /* LMCMARG */
	if (!allbullets && occ > oldccol) {
	    poscursor (occ, curwin->btext + 1);
	    putch (BMCH, NO);
	}
#endif /* LMCMARG */
    }

    if (newbullets || (oldcline != ocl)) {
	/* do right side */
	if (!allbullets) {
	    poscursor (curwin->rtext + 1, ocl);
	    putch (curwin->rmchars[ocl], NO);
	}
	poscursor (curwin->rtext + 1, oldcline);
	putch (BULCHAR, NO);
	/* do left side */
	if (!allbullets) {
	    poscursor (-1, ocl);
	    putch (curwin->lmchars[ocl], NO);
	}
	poscursor (-1, oldcline);
	putch (BULCHAR, NO);
    }
    poscursor (oldccol, oldcline);
    d_put (0);
    return;
}

/*
 * trw - redo these two
 */
#ifdef LMCMARG
#ifdef COMMENT
void
tm_unbullet (allbullets, occ, tb, ordch, xordch, igntab)
    Flag allbullets, igntab;
    Ncols occ, tb;
    char ordch, xordch;
.
    tm_unbullet is called to move the top bullets. It essentially
    rewrites the margin character, based on the current tab data.
    If igntab is true, any tab at the character is ignored.
#endif
/*tm_unbullet (allbullets, occ, tb, ordch, xordch, igntab) */
void
tm_unbullet (allbullets, occ, tb, ordch, igntab)
    Flag allbullets, igntab;
    Ncols occ, tb;
    Uchar ordch;
{
    Ncols i;

    if (!allbullets) {
	    /*
	     * want occ > 0, so we don't display the tab mark at the
	     * first position
	     */
	if (!igntab && occ > 0) {
	    for (i = Z; i < ntabs && tabs[i] < curwksp->wcol + occ; i++) ;
	    if (tabs[i] - curwksp->wcol == occ && ntabs > 0 && i < ntabs)
		ordch = ordch + (BTMCH - BMCH);
	}
	poscursor (occ, tb);
	putch (ordch, NO);
    }
}

#ifdef COMMENT
void
bm_unbullet (allbullets, occ, tb, ordch, xordch, igntab)
    Flag allbullets, igntab;
    Ncols occ, tb;
    Uchar ordch, xordch;
.
    bm_unbullet is called to move the bottom bullets. It essentially
    rewrites the margin character, based on current margin data.
    If igntab is true, any tab at the character is ignored.
#endif
/*bm_unbullet (allbullets, occ, tb, ordch, xordch, Flag igntab)*/
void
bm_unbullet (allbullets, occ, tb, ordch, xordch)
    Flag allbullets;
    Ncols occ, tb;
    Uchar ordch, xordch;
{
    /*Ncols i;*/

    if (!allbullets) {
	poscursor (occ, tb);
	putch (occ == linewidth - curwksp->wcol
#ifdef LMCAUTO
	       || (autolmarg && (occ == autolmarg - curwksp->wcol))
#endif /* LMCAUTO */
	       ? xordch : ordch, NO);
    }
}

#ifdef COMMENT
void
scvtab (pos, sc)
    Ncols pos;
    Flag sc;
.
    scvtab is called to set or reset a tab mark on the top
    margins. If sc is YES, the tab is reset.
#endif
void
scvtab (pos, sc)
    Ncols pos;
    Flag sc;

{
    if (pos > 0 ) {
	savecurs();
	stmarg0 (pos, sc, TOPMAR);
	restcurs();
    }
    return;
}

#ifdef COMMENT
void
stmarg0 (pos, igntab, marflag)
    Ncols pos;
    Flag igntab;
    Flag marflag;
.
    stmarg0 rewrites the top and/or bottom edges of a window at column pos. If
    igntab is YES then the existance of a tab on that column is ignored,
    thus effectively resetting it.  Marflag indicates which margins to
    update (1=top, 2=bot, 3=both);
#endif
void
stmarg0 (pos, igntab, marflag)
    Ncols pos;
    Flag igntab;
    Flag marflag;
{
    if (pos >= 0 && pos <= curwin->rtext)
    {
	if (marflag & TOPMAR)
	    /*tm_unbullet (NO, pos, -1, TMCH, TTMCH, igntab);*/
	    tm_unbullet (NO, pos, -1, TMCH, igntab);
	if (marflag & BOTMAR)
	    /*bm_unbullet (NO, pos, curwin->bmarg - 1, BMCH, BTMCH, igntab);*/
	    bm_unbullet (NO, pos, curwin->bmarg - 1, BMCH, BTMCH);
    }
    return;
}

#ifdef COMMENT
void
setmarg (old, new)
    Ncols *old, new;
.
    setmarg moves the left or right margin indicator by first removing the
    old, setting the new, and then setting the old to the new.
#endif
void
setmarg (old, new)
    Ncols *old, new;

{
    Ncols temp;

    savecurs();
    temp = *old;
    *old = new;
/*  stmarg0 (temp, NO, BOTMAR);
    stmarg0 (new, NO, BOTMAR);          */
    stmarg0 (temp, YES, BOTMAR);
    stmarg0 (new, YES, BOTMAR);
    restcurs();
    return;
}
#endif /* LMCMARG */

#ifndef SYSSELECT
#ifdef COMMENT
int
bulalarm ()
.
    A null routine to catch the SIGALRM signal which may come in as part
    of the 1 second timeout for a bullet.
#endif
int
bulalarm ()
{
    return;
}
#endif /* SYSSELECT */

static int bulsave = -1;    /* saved character from setbul */

#ifdef COMMENT
void
setbul (wt)
    Flag wt;
.
    Set a bullet at the current cursor postion.
    If 'wt' is non-0, set the 1-second alarm to hold the bullet in place.
#endif
void
setbul (wt)
Flag wt;
{
    if (!term.tt_bullets)
	return;
    if (curfile != NULLFILE) {
	Reg1 Ncols charpos;
	GetLine (cursorline + curwksp->wlin);
	bulsave = (charpos = cursorcol + curwksp->wcol) < ncline - 1
		  ? (cline[charpos] & CHARMASK)
		  : ' ';
    }
    else
	bulsave = ' ';

    putch (BULCHAR, YES);
    movecursor (LT, 1);
    d_put (0);
    if (wt) {
#ifdef  SYSSELECT
	Block {
	    static struct timeval onesec = { 1, 0 };

	    /*getkey (WAIT_PEEK_KEY, &onesec);*/
	    mGetkey (WAIT_PEEK_KEY, &onesec);
	    /* either this read will time out
	     * or a key will have come in, in which case we want to clear
	     * bullet right away and reset the alarm.
	     **/
#else /* SYSSELECT */
      /*if (getkey (PEEK_KEY) == NOCHAR) {*/
	if (mGetkey (PEEK_KEY) == NOCHAR) {
	    (void) signal (SIGALRM, bulalarm);
	    alarm (1);
	    if (getkey (WAIT_PEEK_KEY) != NOCHAR)
		alarm (0);
	    /* either this read will be interrupted by the alarm,
	     * or a key will have come in, in which case we want to clear
	     * bullet right away and reset the alarm.
	     **/
#endif /* SYSSELECT */
	}
	clrbul ();
    }
    return;
}

#ifdef COMMENT
void
clrbul ()
.
    Clear the bullet that was placed at the current cursor position.
#endif
void
clrbul ()
{
    if (bulsave == -1)
	return;
    putch (bulsave, NO);
    movecursor (LT, 1);
    bulsave = -1;
    return;
}

#ifdef COMMENT
    All three of the following functions: gotomvwin(), vertmvwin(), and
    horzmvwin() must not return until they have called movewin, because
    movewin looks to see if a putup needs to be done after one was aborted.
#endif

#ifdef COMMENT
void
gotomvwin (number)
    Nlines  number;
.
    Move the window so that line 'number' is the distance of one +LINE
    down from the top of the window.
#endif
void
gotomvwin (number)
Reg1 Nlines number;
{
    Reg3 Scols  cc;
    Reg4 Nlines cl;
    Reg2 Small  defpl;
    Reg5 Ncols wincol;

    number = max (0, number);
    wincol = curwksp->wcol;
    if (curwin->btext > 1)
	defpl = defplline;
    else
	defpl = Z;
    if (number == 0) {
	wincol = 0;
	cc = Z;
    }
    else
	cc = cursorcol;
    cl = min (number, defpl);
    movewin (number - defpl, wincol, cl, cc, 1);
    return;
}

#ifdef COMMENT
Small
vertmvwin (nl)
    Nlines nl;
.
    Moves window nl lines down.  If nl < 0 then move up.
#endif
Small
vertmvwin (nl)
Reg2 Nlines nl;
{
    Reg3 Nlines winlin;
    Reg1 Slines cl;

/**
dbgpr("\nvertmvwin:  nl=%ld curwksp->wlin=%d cursorline=%d, curwin->btext=%d curlas=%d\n",
 nl, curwksp->wlin, cursorline, curwin->btext, curlas);
**/

    /* We have seen the value of nl arrive as a very large
     * number (MAXLONG) as a result of an expression with
     * ints and longs.  We believe it is fixed but just in
     * case.  It is not fun to move to line 9223372036854775807L!
     */
    if (nl > MAXINT) {      /* 2147483647 */
	char buf[128];
	dbgpr("nl=%ld > filesize=%ld\n", nl, MAXINT);
	sprintf(buf, "data error, can't move to line %ld, max is %d", nl, MAXINT);
	mesg(ERRALL+1, buf);
	return -1;
    }

    winlin = curwksp->wlin;
    if (winlin == 0 && nl <= 0)
	nl = 0;
    if ((winlin += nl) < 0) {
	nl -= winlin;
	winlin = Z;
    }
    cl = (Slines) cursorline - nl;

    if (abs (nl) > curwin->btext)
	cl = cursorline;
    else if (cl < 0)
	cl = Z;
    else if (cl > curwin->btext)
	cl = curwin->btext;

    return movewin (winlin, curwksp->wcol, cl, cursorcol, 1);
}

#ifdef COMMENT
Small
horzmvwin (nc)
    Ncols nc;
.
    Moves window 'nc' columns right.  If nc < 0 then move left.
#endif
Small
horzmvwin (nc)
Reg2 Ncols nc;
{
    Reg1 Ncols  cc;

    cc = cursorcol;
    if ((curwksp->wcol + nc) < 0)
	nc = -curwksp->wcol;
    cc -= nc;
    cc = max (cc, curwin->ledit);
    cc = min (cc, curwin->redit);
    return movewin (curwksp->wlin, curwksp->wcol + nc, cursorline, (Scols) cc,
			YES);
}

#ifdef COMMENT
Small
movewin (winlin, wincol, curslin, curscol, puflg)
    Nlines  winlin;
    Ncols   wincol;
    Slines  curslin;
    Scols   curscol;
    Flag    puflg;
.
    Move the window so that winlin and wincol are the upper left corner,
    and curslin and curscol are the cursor position.
    If TRACKSET in curwin, move altwksp too, but do not disturb its
    cursor postion.
#endif
Small
movewin (winlin, wincol, curslin, curscol, puflg)
Reg4 Nlines  winlin;
     Ncols   wincol;
Reg5 Slines  curslin;
Reg6 Scols   curscol;
     Flag    puflg;
{
    Reg3 Small newdisplay;
    Reg4 Nlines vdist;      /* vertical distance to move */
    Reg4 Ncols  hdist;      /* horizontal distance to move */
    Reg2 S_wksp *cwksp = curwksp;

/** /
dbgpr("--movewin: winlin=%d, wincol=%d, curslin=%d, \
cursorcol=%d, curwksp->wlin=%d, puflg=%d\n",
  winlin, wincol, curslin, curscol, curwksp->wlin, puflg);
/ **/


/** /
dbgpr("movewin:  curwin=(%o) wholescreen=(%o) enterwin=(%o) infowin=(%o)\n",
    curwin, &wholescreen, &enterwin, &infowin);
dbgpr("movewin:  curwksp=(%o) wholescreen.wksp=(%o) enterwin.wksp=(%o) infowin.wksp=(%o)\n",
    curwksp, wholescreen.wksp, enterwin.wksp, infowin.wksp);
/ **/

if( 1 && curwin == &enterwin ) {
/** / dbgpr("movewin: curwin == &enterwin, chgwindow\n"); / **/
/*return 0;*/
  chgwindow(-1);
}


    winlin = max (0, winlin);
    wincol = max (0, wincol);
#ifdef LA_LONGFILES
    if (winlin + defplline < 0)
#else
/*  if ((long) winlin + defplline > LA_MAXNLINES) */
    if ((long) winlin + defplline > E_MAXNLINES)
#endif
    /*  winlin = LA_MAXNLINES - defplline; */
	winlin = E_MAXNLINES - defplline;
    curslin = max (0, curslin);
    curslin = min (curslin, curwin->btext);
    curscol = max (0, curscol);
    curscol = min (curscol, curwin->rtext);
    newdisplay = Z;

    Block {
	Reg1 S_wksp *awksp;
	awksp = curwin->altwksp;
	if ((vdist = winlin - cwksp->wlin)) {
	    if (curwin->winflgs & TRACKSET) {
		if ((awksp->wlin += winlin - cwksp->wlin) < 0)
		    awksp->wlin = 0;
#ifdef LA_LONGFILES
		else if (awksp->wlin + defplline < 0)
#else
	  /*    else if ((long) awksp->wlin + defplline > LA_MAXNLINES) */
		else if ((long) awksp->wlin + defplline > E_MAXNLINES)
#endif
	       /*   awksp->wlin = LA_MAXNLINES - defplline; */
		    awksp->wlin = E_MAXNLINES - defplline;
	    }
	    cwksp->wlin = winlin;
	    newdisplay |= WLINMOVED;
	}
	if ((hdist = wincol - cwksp->wcol)) {
	    if (   (curwin->winflgs & TRACKSET)
		&& (awksp->wcol += wincol - cwksp->wcol) < 0
	       )
		awksp->wcol = 0;
	    cwksp->wcol = wincol;
	    newdisplay |= WCOLMOVED;
	}
	if (   (curwin->winflgs & TRACKSET)
	    && (newdisplay & (WLINMOVED | WCOLMOVED))
	   ) {
	    awksp->clin = curslin;
	    awksp->ccol = curscol;
	}
    }

    if (newdisplay || needputup) {
	newcurline = curslin;
	newcurcol  = curscol;
	if (puflg) {
	    if (needputup)
		putupwin ();
	    else {
		if (   hdist == 0
		    && abs(vdist) <= curwin->btext
		    && curwksp->wlin < la_lsize (curlas)
		    && curwksp->wlin - vdist < la_lsize (curlas)
		   )
		    (void) vinsdel (0, -vdist, YES);
		else
		    putupwin ();
	    }
	}
    }
    if (cursorline != curslin)
	newdisplay |= LINMOVED;
    if (cursorcol != curscol)
	newdisplay |= COLMOVED;
#ifdef LMCMARG
    if (newdisplay & WCOLMOVED)
	drawborders (curwin, WIN_ACTIVE);
#endif /* LMCMARG */

    poscursor (curscol, curslin);
    return newdisplay;
}

struct svcs {
    struct svcs *sv_lastsv;
    AScols  sv_curscol;
    ASlines sv_cursline;
} *sv_curs;

#ifdef COMMENT
void
savecurs ()
.
    Save current cursor position.
    See restcurs().
#endif
void
savecurs ()
{
    Reg1 struct svcs *lastsv;

    lastsv = sv_curs;
    sv_curs = (struct svcs *) salloc (sizeof *sv_curs, YES);
    sv_curs->sv_lastsv = lastsv;
    sv_curs->sv_curscol = cursorcol;
    sv_curs->sv_cursline = cursorline;
    return;
}

#ifdef COMMENT
void
restcurs ()
.
    Save current cursor position.
    See savecurs().
#endif
void
restcurs ()
{
    /*Reg1 struct svcs *lastsv;*/

    poscursor (sv_curs->sv_curscol, sv_curs->sv_cursline);
    burncurs();
    return;
}

#ifdef COMMENT
void
burncurs ()
.
    burn top frame of cursor stack.
    See savecurs().
#endif
void
burncurs ()
{
    Reg1 struct svcs *lastsv;

    lastsv = sv_curs->sv_lastsv;
    sfree ((char *) sv_curs);
    sv_curs = lastsv;
    return;
}


#ifdef COMMENT
void
poscursor (col, lin)
    Scols  col;
    Slines lin;
.
    Position cursor.
    col is relative to curwin->ltext
    lin is relative to curwin->ttext
#endif
void
poscursor (col, lin)
Reg1 Scols  col;
Reg2 Slines lin;
{

/** /
if( DebugVal ) {
   dbgpr("poscursor, beg: col=%d lin=%d cursorline=%d cursorcol=%d, ltext=%d, ttext=%d curwin=(%o)\n",
     col,lin,cursorline,cursorcol,curwin->ltext, curwin->ttext,curwin);
}
/ **/

    if (cursorline == lin) {
	/* only need to change column?  */
	switch (col - cursorcol) {  /* fortran arithmetic if!!      */
	case -1:
	    d_put (VCCLEF);
	    --cursorcol;
	case 0:               /* already in the right place   */
	    return;
	case 1:
	    d_put (VCCRIT);
	    ++cursorcol;
	    return;
	}
    }
    else if (cursorcol == col) {   /* only need to change line?    */
	switch (lin - cursorline) {
	    /* fortran arithmetic if!!  zero taken care of above     */
	case -1:
	    d_put (VCCUP);
	    --cursorline;
	    return;
	case 1:
	    d_put (VCCDWN);
	    ++cursorline;
	    return;
	}
    }
    cursorcol = col;
    cursorline = lin;

    /* the 041 below is for the terminal simulator cursor addressing */
    putscbuf[0] = VCCAAD;
    putscbuf[1] = 041 + curwin->ltext + col;
    putscbuf[2] = 041 + curwin->ttext + lin;
    d_write (putscbuf, 3);

    return;
}

#ifdef COMMENT
void
movecursor (func, cnt)
    Small   func;
    Nlines cnt;
.
    Move cursor within boundaries of current window.
    Type of motion is specified by 'func'.
#endif
void
movecursor (func, cnt)
Small func;
Reg4 Nlines cnt;
{
    Reg3 Slines lin;
    Reg2 Ncols col;


    lin = cursorline;
    col = cursorcol;
    switch (func) {
    case 0:                     /* noop                             */
	break;
    case HO:                    /* upper left-hand corner of screen */
	col = Z;
	lin = Z;
	break;
    case UP:                    /* up one line                      */
	lin -= cnt;
	break;
    case RN:                    /* left column and down one */
	if (autofill)
	    col = autolmarg - curwksp->wcol;
	else col = curwin->ledit;
    case DN:                    /* down one line */
	lin += cnt;
	break;
    case RT:                    /* forward one space */
	col += cnt;
	break;
    case LT:                    /* back space (non-destructive) */
	col -= cnt;
	break;
    case TB:                    /* tab forward to next stop */
	Block {
	    Reg1 Short  i;
	    for (i = Z, col += curwksp->wcol; i < ntabs; i++)
		if (tabs[i] > col) {
		    if (tabs[i] <= curwksp->wcol + curwin->rtext)
			col = tabs[min (ntabs - 1, i + cnt - 1)];
		    break;
		}
	}
	col -= curwksp->wcol;
	break;
    case BT:                    /* tab back to previous stop        */
	Block {
	    Reg1 Short  i;
	    for (i = ntabs - 1, col += curwksp->wcol; i >= 0; i--)
		if (tabs[i] < col) {
		    if (tabs[i] >= curwksp->wcol)
			col = tabs[max (0, i - (cnt - 1))];
		    break;
		}
	}
	col -= curwksp->wcol;
	break;
    }

/** /
dbgpr("movecursor: func=%d [1=UP 2=DN 5=RT 6=LT] cnt=%d col=%d cursorcol=%d\n", func, cnt, col, cursorcol);
/ **/

    Block {
	Reg5 Nlines ldif;
	Reg6 Ncols  cdif;
	if ((cdif = col - curwin->ledit) < 0)
	    col = curwin->ledit;
	else if ((cdif = col - curwin->redit) > 0)
	    col = curwin->redit;
	else
	    cdif = NO;

	if ((ldif = lin - curwin->tedit) < 0)
	    lin = curwin->tedit;
	else if ((ldif = lin - curwin->bedit) > 0)
	    lin = curwin->bedit;
	else
	    ldif = NO;

	if ((ldif || cdif) && func)
	    movewin (curwksp->wlin + ldif,
		     curwksp->wcol + cdif,
		     lin, (Scols) col, YES);
	else
	    poscursor ((Scols) col, lin);
    }
    return;
}



#ifdef  SYSSELECT
#ifdef COMMENT
unsigned Short
getkey (peekflg, timeout)
    Flag peekflg;
    struct timeval *timeout;
.
    Read another character from the input stream.  If the last character
    wasn't used up (keyused == NO) don't read after all.
.
    peekflg is one of
      WAIT_KEY      wait for a character, ignore interrupted read calls.
      PEEK_KEY      peek for a character
      WAIT_PEEK_KEY wait for a character, then peek at it;
		    if read times out, return NOCHAR.
#endif /* COMMENT */
unsigned Short
getkey (peekflg, timeout)
Flag peekflg;
struct timeval *timeout;
#else /* SYSSELECT */
#ifdef COMMENT
unsigned Short
getkey (peekflg)
    Flag peekflg;
.
    Read another character from the input stream.  If the last character
    wasn't used up (keyused == NO) don't read after all.
.
    peekflg is one of
      WAIT_KEY      wait for a character, ignore interrupted read calls.
      PEEK_KEY      peek for a character
      WAIT_PEEK_KEY wait for a character, then peek at it;
		   if read is interrupted, return NOCHAR.
#endif
unsigned Short
getkey (peekflg)
Reg2 Flag peekflg;
#endif /* SYSSELECT */
{
    Reg1 unsigned Short rkey;
    static Flag knockdown  = NO;    /* set when entering CTRL chars into a file, eg, ^X^CI is a tab */
    /*extern*/ unsigned Short getkey1 ();

    if (peekflg == WAIT_KEY && keyused == NO) {
	return (Uint)key; /* then getkey is really a no-op */
    }

#ifdef  SYSSELECT
    rkey = getkey1 (peekflg, timeout);
#else /* SYSSELECT */
    rkey = getkey1 (peekflg);
#endif /* SYSSELECT */

    if (knockdown && rkey < 040)
	rkey |= 0100;
    if (peekflg != WAIT_KEY)
	return rkey;
    knockdown = rkey == CCCTRLQUOTE;    /* ^\ knockdown next char   */
    keyused = NO;
    key = (int)rkey;
    return rkey;
}

#ifdef  SYSSELECT
#ifdef COMMENT
static unsigned Short
getkey1 (peekflg, timeout)
    Small peekflg;
    struct timeval *timeout;
.
    Return the next key from the input stream.
    Write the key to the keystroke file unless it is CCINT, which will NOT
    be written to the keyfile by this routine.  The caller will write it
    there if and only if it actually interrupted something.
    See getkey() for the function of peekflg.
    if peekflg is WAIT_PEEK_CHAR, then wait for timeout microseconds.
#endif
/*static unsigned Short*/
unsigned Short
getkey1 (peekflg, timeout)
Small peekflg;
struct timeval *timeout;
#else /* SYSSELECT */
#ifdef COMMENT
static unsigned Short
getkey1 (peekflg)
    Small peekflg;
.
    Return the next key from the input stream.
    Write the key to the keystroke file unless it is CCINT, which will NOT
    be written to the keyfile by this routine.  The caller will write it
    there if and only if it actually interrupted something.
    See getkey() for the function of peekflg.
#endif
/*static unsigned Short*/
unsigned Short
getkey1 (peekflg)
Small peekflg;
#endif /* SYSSELECT */
{
    static Short lcnt;
    static int   lexrem;        /* portion of lcnt that is still raw */
    static Uchar chbuf[NREAD];
    static Uchar *lp;

/*
dbgpr("getkey1:  **OLD** routine called\n");
*/

    if (replaying) Block {
	static Small replaydone = 0;
	if (replaydone) {
 finishreplay:
	    close (inputfile);
	    inputfile = STDIN;
	    if (silent) {
		silent = NO;
		(*term.tt_ini1) (); /* not d_put(VCCICL) because fresh() */
				    /* follows */
		windowsup = YES;
		fresh ();
	    }
	    mesg (ERRALL + 1,
		  recovering
		  ? "Recovery completed."
		  : replaydone == 1
		    ? "Replay completed."
		    : "Replay aborted."
		 );
	    d_put (0);
	    replaying = NO;
	    recovering = NO;
	    replaydone = 0;
#ifdef  STARTUPFILE
	    /*
	     *  Don't read the the .e_profile file if recovering
	     *  from a crash.
	     */
	    if( dot_profile ) {
		fclose( fp_profile );
		dot_profile = NO;
	    }
#endif /* STARTUPFILE */
	    /*goto nonreplay; */
	    goto nonreplay;
	    return CCRETURN;
	}
	if (   !recovering
	    && !xempty (STDIN) /* any key stops replay */
	   ) {
	    lcnt = 0;
	    replaydone = 2;
	    while (xread (STDIN, chbuf, NREAD) > 0 && !xempty (STDIN))
		continue;
	    goto endreplay;
	}
	if (lcnt <= 0) Block {
	    static Uchar charsaved;
	    static Uchar svchar;
	    d_put (0);
	    for (;;) {
		Reg1 Uchar *cp;
		/* note that keystroke file is in canonical format and
		 * doesn't have to go through inlex */
		cp = lp = chbuf;
		if (charsaved)
		    *cp++ = svchar;
		if ((lcnt = xread (inputfile, (char *) cp, (size_t)(NREAD - charsaved)))
		    >= 0) {
		    if (lcnt > 0) {
			svchar = cp[lcnt - 1];
			if (!charsaved) {
			    charsaved = 1;
			    if (--lcnt == 0)
				continue;
			}
		    }
		    break;
		}
		if (errno != EINTR)
		    fatal (FATALIO, "Error reading input.");
	    }
	}
	if (   lcnt == 0
#ifdef UNSCHAR
	    || *lp == CCSTOP
#else
	    || (*lp & CHARMASK) == CCSTOP
#endif
	   ) {
	    replaydone = 1;
 endreplay:
	    if (!entering)
		goto finishreplay;
	    else {
		*chbuf = CCINT;
		lp = chbuf;
		lcnt = 1;
	    }
	}
    }

#ifdef RECORDING
    else if (playing) {
	    extern Flag play_silent;

	lp = (Uchar *)PlayChar (peekflg);
	if (!playing && play_silent) {
	    silent = 0;
	    fresh();
	}
	/*
	 * Any key stops playback
	 */
	if (!xempty(STDIN)) {
		char c;
	    /*
	     *  throw away keystroke[s]
	     */
	    do {
		read (inputfile, &c, 1);
	    } while (!xempty(STDIN));
	    if( play_silent )
		silent = 0;
	    play_count = 0;
	/*  playing = 0;  */
	    return (CCINT);
	}

	/*goto endGetkey1;*/
	return (unsigned Short)*lp;
    }
#endif /* RECORDING */

    else {
 nonreplay:

#ifdef  STARTUPFILE
	/*
	 *  Input from the .e_profile file?
	 */
	if( dot_profile && peekflg == WAIT_KEY )
	    return( (Uint)get_profilekey( NO ));
#endif /* STARTUPFILE */

	if (lcnt < 0)
	    fatal (FATALBUG, "lcnt < 0");
	if (   (lcnt - lexrem == 0 && peekflg != PEEK_KEY)
	    || (lcnt < NREAD && !xempty (inputfile))
	   ) {
	    /* read some more */
	    if (lcnt == 0)
		lp = chbuf;
	    else if (lp > chbuf) {
		if (lcnt == 1)
		    /* handle frequent case more efficiently */
		    *chbuf = *lp;
		else
		    my_move ((char *)lp, (char *) chbuf, (ulong) lcnt);
		lp = chbuf;
	    }
	    d_put (0);
#ifdef  SYSSELECT
#ifdef FDSET
	    Block {
		fd_set readmask;
		FD_ZERO(&readmask);
		FD_SET(inputfile, &readmask);
		/*dbgpr("inputfile=(%d)\n", inputfile);*/
		if (   peekflg == WAIT_PEEK_KEY
		    && select (inputfile + 1, &readmask, NULL, NULL, timeout) <= 0
		   )
		    return NOCHAR;
	    }
#else /* FDSET */
	    Block {
		int z = 0;
		int readmask = 1 << inputfile;

		if (   peekflg == WAIT_PEEK_KEY
		    && select (inputfile + 1, &readmask, &z, &z, timeout) <= 0
		   )
		    return NOCHAR;
	    }
#endif /* FDSET */
#endif /* SYSSELECT */
	    do Block {
		Reg3 int nread;
		nread = NREAD - lcnt;
#ifdef  SYSSELECT
#ifdef  FSYNCKEYS
		/*
		 *  If this 1 min. timer goes off, flush and fsync
		 *  the keystroke file.  (Needs to be done only once!).
		 */
#ifdef FDSET
		Block {
		    fd_set readmask;
		    FD_ZERO(&readmask);
		    FD_SET(inputfile, &readmask);
		    /*dbgpr("inputfile=(%d)\n", inputfile); **/
		    struct timeval tval;
		    int rc = 0;

		    tval.tv_sec = 60, tval.tv_usec = 0;
		    rc = select (inputfile + 1, &readmask, NULL, NULL, &tval);
		    if (rc == 0 && numtyp) {
			fsynckeys();    /* also does a fflush */
			numtyp = 0;
		    }
		}
#else
		Block {
		    int z = 0;
		    int readmask = 1 << inputfile;
		    struct timeval tval;
		    int rc = 0;

		    tval.tv_sec = 60, tval.tv_usec = 0;
		    rc = select (inputfile + 1, &readmask, &z, &z, &tval);
		    if (rc == 0 && numtyp) {
			fsynckeys();    /* also does a fflush */
			numtyp = 0;
		    }
		}
#endif /* FDSET */
#endif /* FSYNCKEYS */
#endif /* SYSSELECT */
		if ((nread = xread (inputfile, (char *) &chbuf[lcnt], (size_t)nread))
		    > 0) Block {
		    Reg4 Uchar *stcp;
		    stcp = &chbuf[lcnt -= lexrem];
		    lexrem += nread;
		    if ((nread = (*kbd.kb_inlex) (stcp, &lexrem)) > 0) Block {
			Reg1 Uchar *cp;
			Reg2 int nr;
			cp = &stcp[nread];
			/* look ahead for interrupts */
			nr = nread;
			do {
			    if (*--cp == CCINT)
				break;
			} while (--nr);
			if ((nr = cp - stcp) > 0) {
			    /* found CCINT character in chars just read */
			    /* and some characters in front of it are to be */
			    /* thrown away */
			    lp += lcnt + nr;
			    lcnt = nread - nr + lexrem;
			}
			else
			    lcnt += nread + lexrem;
		    }
		    else
			lcnt += lexrem;
		}
		else if (nread < 0) {
		    if (errno != EINTR)
			fatal (FATALIO, "Error reading input.");
		    if (peekflg == WAIT_PEEK_KEY)
			break;
		}
		else
		    fatal (FATALIO, "Unexpected EOF in key input.");
	    } while (lcnt - lexrem == 0);
	}
    }

    if (   lcnt - lexrem <= 0
	&& peekflg != WAIT_KEY
       )
	return NOCHAR;

    if (peekflg != WAIT_KEY)
#ifdef UNSCHAR
	return *lp;
#else
	return *lp & CHARMASK;
#endif

    Block {
	Reg1 unsigned Short rchar;

	lcnt--;

#ifdef UNSCHAR
	rchar = *lp++;
#else
	rchar = *lp++ & CHARMASK;
#endif

	if (keyfile != NULL && rchar != CCINT) {
	    putc ((int)rchar, keyfile);
	    if (numtyp > MAXTYP) {
		flushkeys ();
		numtyp = 0;
	    }
	}
#ifdef RECORDING
	if (recording)
	    RecordChar (rchar);
#endif /* RECORDING */
	return rchar;
    }
}


#ifdef RDNDELAY

/*  These routines adapt the late-model read() with O_NDELAY
 *  to look like an empty call and a blocking read call.
 *  The implementation assumes that at most two fd's will be used this way.
 **/

struct rdbuf {
    int  rd_nread;
    int  rd_ndelay;
    int  rd_fd;
    char rd_char;
 };
static struct rdbuf rd0, rdx;
extern struct rdbuf *getrdb ();

#ifdef COMMENT
int
xempty (fd)
    int fd;
.
    return YES if read on fd would block, else NO.
#endif
int
xempty (fd)
int fd;
{
    Reg1 struct rdbuf *rdb;

    rdb = getrdb (fd);

    if (rdb->rd_nread > 0)
	return NO;
    if (!rdb->rd_ndelay) {
	if (fcntl (fd, F_SETFL, O_NDELAY) == -1)
	    fatal(FATALBUG, "Can't set ndelay");
	rdb->rd_ndelay = YES;
    }
    return (rdb->rd_nread = read (fd, &rdb->rd_char, 1)) <= 0;
}

#ifdef COMMENT
int
xread (fd, buf, count)
    int fd;
    char *buf;
    int count;
.
    blocking read.
#endif
int
out_xread (fd, buf, count)
int fd;
char *buf;
int count;
{
/*dbgpr("xread: fd=%d count=$d\n", fd, count);*/
    int retval;
    Reg1 struct rdbuf *rdb;

    rdb = getrdb (fd);

    if (rdb->rd_nread > 0) {
	buf[0] = rdb->rd_char;
	count--;
    }
    else if (rdb->rd_ndelay) {
	if (fcntl (fd, F_SETFL, 0) == -1)
	    fatal (FATALBUG, "Can't clr ndelay");
	rdb->rd_ndelay = NO;
    }
    retval = rdb->rd_nread + read (fd, &buf[rdb->rd_nread], count);
    rdb->rd_nread = 0;
    return retval;
}

#ifdef COMMENT
struct rdbuf *
getrdb (fd)
    int fd;
.
    Return a pointer to the appropriate rdbuf structure for the fd.
#endif
struct rdbuf *
getrdb (fd)
int fd;
{
    Reg1 struct rdbuf *rdb;

    if (fd == STDIN) {
	rdb = &rd0;
	rd0.rd_fd = fd;
    }
    else {
	if (fd != rdx.rd_fd && rdx.rd_fd != 0)
	    fatal (FATALBUG, "empty bug in e.t.c");
	rdb = &rdx;
	rdx.rd_fd = fd;
    }
    return rdb;
}

#endif /* RDNDELAY */

#ifdef COMMENT
void
writekeys (code1, str, code2)
    Char    code1;
    char   *str;
    Char    code2;
.
    Write code1, then str, then code2 to the keys file.
#endif
void
writekeys (code1, str, code2)
Char    code1;
char   *str;
Char    code2;
{
    putc (code1, keyfile);
    fputs (str, keyfile);
    putc (code2, keyfile);
    return;
}

#ifdef COMMENT
Flag
dintrup ()
.
    Look ahead for certain keys which will interrupt a putup.
    Return YES if such a key is found, else NO.
#endif
Flag
dintrup ()
{
    Reg1 unsigned Short ichar;

/* todo, modify mGetkey to handle PEEK_KEY in replay mode */
if( replaying )
  return 0;

    intrupnum = 0;
#ifdef SYSSELECT
    static struct timeval k_timeval = {1, 0}; /* one-second timeout */

    ichar = getkey (PEEK_KEY, &k_timeval);
/*  ichar = mGetkey (PEEK_KEY, &k_timeval); */
#else /* SYSSELECT */
    ichar = getkey (PEEK_KEY);
/*  ichar = mGetkey (PEEK_KEY); */
#endif /* SYSSELECT */

    switch (ichar) {
	case CCOPEN:
	case CCCLOSE:
	    return atoldpos;

	/* NOTE: so far, all functions that appear here call horzmvwin(),
	 *       vertmvwin() when it is their turn to actually be performed,
	 *       and movewin does a putup if the last one was aborted.
	 **/
	case CCLWINDOW:
	case CCRWINDOW:
	case CCMIPAGE:
	case CCPLPAGE:
	case CCMILINE:
	case CCPLLINE:
	    return YES;

	case CCMOVEUP:
	    if (newcurline == curwin->tedit)
		return YES;
	    break;

	case CCMOVEDOWN:
	    if (newcurline == curwin->bedit)
		return YES;
	    break;

	case CCMOVELEFT:
	    if (newcurcol == curwin->ledit)
		return YES;
	    break;

	case CCMOVERIGHT:
	    if (newcurcol == curwin->redit)
		return YES;
	    break;

	case NOCHAR:
	case CCDEL:     /* should this be here? */
	case CCDELCH:
	case CCCMD:
	case CCINSMODE:
	case CCMISRCH:
	case CCPLSRCH:
	case CCSETFILE:
	case CCCHWINDOW:
	case CCPICK:
	case CCUNAS1:
	case CCREPLACE:
	case CCMARK:
	case CCINT:
	case CCTABS:
	case CCCTRLQUOTE:
	case CCBACKSPACE:
	default:
	    break;
    }
    return NO;
}

#ifdef COMMENT
Flag
la_int()
.
    If intok == 0, return 0, else return sintrup ().
#endif
Flag
la_int()
{
/*  if (intok)  */
    if (intok && !replaying)
	return sintrup ();
    else
	return 0;
}

#ifdef COMMENT
Flag
sintrup ()
.
    Look ahead for certain keys which will interrupt a search or any
    subprogram, e.g. a "run", "fill", etc.
    Return YES if such a key is found, else NO.
#endif
Flag
sintrup ()                        /* Whether to intrup search/exec      */
{
    intrupnum = 0;                /* reset the counter                  */

#ifdef SYSSELECT
    struct timeval k_timeval = {1, 0}; /* one-second timer */
    struct timeval k_t = {1, 0};

    if (getkey (PEEK_KEY, &k_timeval) == CCINT) {
/*  if (mGetkey (PEEK_KEY, &k_timeval) == CCINT) { */
	putc (CCINT, keyfile);
	keyused = YES;
	k_timeval = k_t; /* C standard best practice: reinitialize
			    it after use */
     /* mGetkey (WAIT_KEY, &k_timeval); */
	getkey (WAIT_KEY, &k_timeval);
	keyused = YES;
	return YES;
    }
#else
    if (getkey (PEEK_KEY) == CCINT) {
/*  if (mGetkey (PEEK_KEY) == CCINT) { */
	putc (CCINT, keyfile);
	keyused = YES;
    /*  mGetkey (WAIT_KEY); */
	getkey (WAIT_KEY);
	keyused = YES;
	return YES;
    }
#endif /* SYSSELECT */

    return NO;
}

#ifdef COMMENT
void
putch (chr, flg)
#ifdef UNSCHAR
    Uchar chr;
#else
    Uint chr;
#endif
    Flag    flg;
.
    Put a character up at current cursor position.
    The character has to be a space, a printing char, a bell or an 0177.
    Cannot be negative.
    If 'flg' is non-0, then 'chr' is being put into a display window,
    and its position may be before or after existing printing characters
    on the line.  This must be noted for putup ().
    'flg' is only YES in 3 places: 2 in printchar and 1 in setbul ().
#endif
void
putch (chr, flg)
#ifdef UNSCHAR
Uchar chr;
#else
Reg1 int chr;
#endif
Flag    flg;
{
#ifndef UNSCHAR
    chr &= CHARMASK;
#endif
    if (chr < 040) {
	d_put (chr);
	return;
    }
    if (flg && chr != ' ') {
	if (curwin->firstcol[cursorline] > cursorcol)
	    curwin->firstcol[cursorline] = cursorcol;
	if (curwin->lastcol[cursorline] <= cursorcol)
	    curwin->lastcol[cursorline] = cursorcol + 1;
    }

    /*  Adjust cursorcol, cursorline for edge effects of screen.
	Sets cursorcol, cursorline to correct values if they were
	positioned past right margin.
	If cursor was incremented from bottom right corner of screen do not
	put out a character since screen would scroll, but home cursor.
     **/
    if (curwin->ltext + ++cursorcol > term.tt_width) {
	cursorcol = -curwin->ltext;  /* left edge of screen */
	if (curwin->ttext + ++cursorline >= term.tt_height) {
	    cursorline = -curwin->ttext;
	    d_put (VCCHOM);
	}
	else
	    d_put (chr);
	poscursor (cursorcol < curwin->ledit
		   ? curwin->ledit
		   : cursorcol > curwin->redit
		     ? curwin->redit
		     : cursorcol,
		   cursorline < curwin->tedit
		   ? curwin->tedit
		   : cursorline > curwin->bedit
		     ? curwin->bedit
		     : cursorline);
    }
    else
	d_put (chr);
    return;
}

extern char *pkarg (S_wksp *wksp, Nlines line, Ncols col, int *len);

static S_window *msowin;
static Scols  msocol;
static Slines msolin;

static char *ccmdp;
static Short ccmdl;
static Short ccmdlen;
static char *lcmdp;
static Short lcmdl;
static Short lcmdlen;
#define  LPARAM 20       /* length of free increment */

#ifdef COMMENT
void
param ()
.
    Take input from the Command Line.
    For historical reasons, this is spoken of as 'getting a parameter'.
    There are three types of parameters:
	    paramtype = -1 -- cursor defined
	    paramtype = 0  -- just <arg> <function>
	    paramtype = 1  -- string, either integer or text:
    Returns pointer to the text string entered.
    The pointer is left in the global variable paramv, and
    its alloced length in bytes (not string length) in ccmdl.
#endif
void
param ()
{
    Reg4 Short ppos;
    Reg5 Flag cmdflg;
    Reg7 Flag uselast;

/* /dbgpr("param(), begin\n"); / */

    uselast = NO;
    entering = YES;     /* set this flag so that getkey1() knows you are in
			 * this routine. */
    savecurs ();
    setbul (NO);

    if (ccmdl == 0) {  /* first time only */
	ccmdp = salloc (ccmdl = LPARAM, YES);
	lcmdp = salloc (lcmdl = LPARAM, YES);
    }

    exchgcmds ();
 parmstrt:
    mesg ((TELSTRT|TELCLR) + 1, cmdmode ? "" : "CMD: ");
    if (uselast) Block {
	Reg1 char *c1;
	Reg2 char *c2;
	Reg3 Short  i;
	uselast = NO;
	if (lcmdlen >= ccmdl)
	    ccmdp = gsalloc (ccmdp, 0, ccmdl = lcmdlen + LPARAM, YES);
	for (c1 = ccmdp, c2 = lcmdp, i = lcmdlen; i--; )
	    putch ((Uchar)(*c1++ = *c2++), NO);
	*c1 = '\0';
	ppos = ccmdlen = lcmdlen;
#ifdef SYSSELECT
	struct timeval k_timeval = {1, 0};      /* one-second timer */
/*tmp*/
	/*getkey (WAIT_KEY, &k_timeval);*/
	mGetkey (WAIT_KEY, &k_timeval);
#else
	getkey (WAIT_KEY);
#endif /* SYSSELECT */
    }
    else {
	ccmdp[0] = '\0';
	ppos = Z;
	ccmdlen = Z;
#ifdef SYSSELECT
	struct timeval k_timeval = {1, 0}; /* one-second timer */
#endif /* SYSSELECT */
#ifdef  NOCMDCMD
rmcmd:
#endif /* NOCMDCMD */
#ifdef SYSSELECT
/*      getkey (WAIT_KEY, &k_timeval); */
/*tmp*/
	mGetkey (WAIT_KEY, &k_timeval);
#else
	getkey (WAIT_KEY);
#endif /* SYSSELECT */
	switch (key) {
	case CCDELCH:
	case CCMOVELEFT:
	case CCMOVERIGHT:
	case CCBACKSPACE:
	    goto done;
#ifdef  NOCMDCMD
	case CCCMD:
	    if (optnocmd == NO) break;
	    keyused = YES;
	    goto rmcmd;  /* remove multiple <cmd> keys */
#endif /* NOCMDCMD */
	}
    }

    cmdflg = NO;
#ifdef SYSSELECT
    struct timeval k_timeval = {1, 0}; /* one-second timer */

/*  for (; ; cmdflg = key == CCCMD, keyused = YES, getkey (WAIT_KEY, &k_timeval)) { */
    for (; ; cmdflg = key == CCCMD, keyused = YES, mGetkey (WAIT_KEY, &k_timeval)) {
#else
    for (; ; cmdflg = key == CCCMD, keyused = YES, getkey (WAIT_KEY)) {
#endif /* SYSSELECT */

/** /
if (CTRLCHAR) {
 dbgpr("param: got ctrlkey=(%o)(%s) curwin=(%o) enterwin=(%o) cmdflg=%d keyused=%d\n",
    key, getEkeyname(key), curwin, &enterwin, cmdflg, keyused);
}
else {
 dbgpr("param: got     key=(%o)(%c) curwin=(%o) enterwin=(%o) cmdflg=%d keyused=%d\n",
    key, (char)key, curwin, &enterwin, cmdflg, keyused);
}
/ **/

#ifdef MOUSE_BUTTONS
	/* check if a mouse button is pressed while in CMD mode */
	if( key == CCMOUSE ) {
/** /dbgpr("param, calling getMouseButtonEvent(), mouseFuncKey=(%d)\n",mouseFuncKey); / **/
	    getMouseButtonEvent();
/** /dbgpr("param, after getMouseButtonEvent(), mouseFuncKey=(%d)\n",mouseFuncKey);  / **/
	    if (mouseFuncKey != -1) {
		key = mouseFuncKey;
		mouseFuncKey = -1;
/* /dbgpr("param(), mouse button clicked, setting key to (%o)(%s)\n", key, getEkeyname(key)); / */
		/* force move cursor off button row, back to enterwin line */
		mvcur(-1,-1, curwin->ttext + cursorline, curwin->ltext + cursorcol);
		d_put(0);
	    }
	}
#endif /* MOUSE_BUTTONS */

	if (ccmdlen >= ccmdl)
	    ccmdp = gsalloc (ccmdp, ccmdlen, ccmdl += LPARAM, YES);
	if (CTRLCHAR)
	    /* make sure that all codes for which <CMD><key> is undefined
	     * are ignored */
	    switch ((unsigned) key) {
	    case CCBACKSPACE:
		if (ppos) {
		    if (cmdflg) {
			if (insmode) Block {
			    Reg1 char  *c1;
			    Reg2 char *c2;
			    Reg3 Short  i;
			    poscursor (cursorcol - ppos, cursorline);
			    if ((i = ccmdlen - ppos) > 0) {
				c2 = &(c1 = ccmdp)[ppos];
				do {
				    putch ((Uchar)(*c1++ = *c2++), NO);
				} while (--i);
			    }
			    i = ppos;
			    multchar (' ', i);
			    poscursor (cursorcol - ccmdlen, cursorline);
			    ccmdlen -= ppos;
			    ppos = 0;
			}
			else Block {
			    Reg2 Short  i;
			    Reg1 char  *c1;
			    i = ppos;
			    c1 = ccmdp;
			    poscursor (cursorcol - ppos, cursorline);
			    do {
				putch ((Uchar)(*c1++ = ' '), NO);
			    } while (--i);
			}
		    }
		    else Block {
			Reg1 Short  i;
			if ((i = ccmdlen - ppos--) > 0 && insmode) {
			    movecursor (LT, 1);
			    goto delchr;
			}
			else {
			    if (i == 0)
				ccmdlen--;
			    cursorcol--;
			    d_put (VCCBKS);
			    ccmdp[ppos] = ' ';
			}
		    }
		}
		break;

	    case CCDELCH:
		Block {
		    Reg1 Short  i;
		    if ((i = ccmdlen - ppos) > 0) {
			if (cmdflg) {
			    ccmdp[ppos] = '\0';
			    savecurs ();
			    multchar (' ', i);
			    restcurs ();
			    ccmdlen = ppos;
			}
			else {
 delchr:                    if (i > 0)
				my_move (&ccmdp[ppos + 1], &ccmdp[ppos],
				      (ulong) i);
			    ccmdlen--;
			    savecurs ();
			    for (i = ppos; i < ccmdlen; )
				putch ((Uchar)ccmdp[i++], NO);
			    putch (' ', NO);
			    restcurs ();
			}
		    }
		}
		break;

	    case CCINSMODE:
		/* do this whether or not preceded by <CMD> key */
		tglinsmode ();
		break;

	    case CCMOVELEFT:
		Block {
		    Reg1 Short  i;
		    if (ppos) {
			i = cmdflg ? ppos : 1;
			movecursor (LT, i);
			do {
			    if (ppos-- == ccmdlen && ccmdp[ppos] == ' ')
				ccmdp[--ccmdlen] = 0;
			} while (--i);
		    }
		}
		break;

	    case CCMOVERIGHT:
		for (;;) {
		    if (ppos < ccmdlen) {
			ppos++;
			movecursor (RT, 1);
			if (!cmdflg)
			    break;
		    }
		    else if (cmdflg)
			break;
		    else {
			ccmdp[ppos++] = ' ';
			ccmdlen++;
			movecursor (RT, 1);
			break;
		    }
		}
		break;

	    case CCPICK:
/** /dbgpr("param: key=CCPICK, cmdflg=%d\n", cmdflg);/ **/
		if (!cmdflg)
		    goto done;
		Block {
		    int wlen; /* must be int because of pkarg argument */
		    Reg1 char *wcp;
		    if ((wcp = pkarg (msowin->wksp,
				       msowin->wksp->wlin + msolin,
					msowin->wksp->wcol + msocol,
					 &wlen)) == NULL)
			break;
		    /* make sure alloced space is big enough */
		    if (ccmdlen + wlen >= ccmdl)
			ccmdp = gsalloc (ccmdp, ccmdlen,
					 ccmdl = ccmdlen + wlen + LPARAM, YES);
		    /* make room for insertion */
		    Block {
			Reg1 Short  i;
			if ((i = ccmdlen - ppos) > 0)
			    my_move (&ccmdp[ppos], &ccmdp[ppos + wlen],
				  (Uint) i);
			/* insert the word */
		    }
		    if (wlen > 0)
			my_move (wcp, &ccmdp[ppos], (Uint) wlen);
		    ccmdlen += wlen;
		    /* update screen */
		    Block {
			Reg1 char  *c1;
			Reg2 Short  i;
			Reg3 Slines lin;
			Reg6 Scols col;
			col = cursorcol + wlen;
			lin = cursorline;
			c1 = &ccmdp[ppos];
			for (i = ccmdlen - ppos; i-- > 0; )
			    putch ((Uchar)*c1++, NO);
			poscursor (col, lin);
		    }
		    ppos += wlen;
		}
		break;

	    case CCSETFILE:
		if (!cmdflg)
		    goto done;

		mesg (TELSTOP);
		uselast = YES;
		keyused = YES;
		goto parmstrt;

	    case CCCMD:
		break;

	    case CCCTRLQUOTE:
		/* do this whether or not preceded by <CMD> key */
		key = ESCCHAR;
		goto prchar;

	    case CCINT:
		/* do this whether or not preceded by <CMD> key */
		if (ccmdlen > 0)
		    /* we have generated something we may want to call back */
		    exchgcmds ();
		putc (CCINT, keyfile);
		keyused = YES;
		if (cmdmode) {
		    mesg (TELSTOP);
		    goto parmstrt;
		}
		ccmdlen = 0;
		/* fall through */

	    case CCNULL:                /* added for err ret from .estartup */
		mesg (TELSTOP);
		goto err;

	    case CCRETURN:
	    /*dbgpr("param, got CCRETURN\n");*/
		goto done;

	    default:
		if (!cmdflg)
		    goto done;
	    }
	else
 prchar:    if (insmode && ppos < ccmdlen) {
	    my_move (&ccmdp[ppos], &ccmdp[ppos + 1],
		  (Uint) (ccmdlen++ - ppos));
	    ccmdp[ppos] = (char) key;
	    Block {
		Reg1 Short i;
		Reg2 Slines lin;
		Reg3 Scols  col;
		col = cursorcol + 1;
		lin = cursorline;
		for (i = ppos++; i < ccmdlen; )
		    putch ((Uchar)ccmdp[i++], NO);
		poscursor (col, lin);
	    }
	}
	else {
	    if (ppos == ccmdlen)
		ccmdlen++;
	    ccmdp[ppos++] = (char) key;
	    putch (key, NO);
	}
    }
 done:
    Block {
	Reg1 Short i;
	if ((i = (key == CCINT ? lcmdlen : ccmdlen) - ppos) > 0)
	    movecursor (RT, i);
    }
    mesg (TELSTOP);
    ccmdp[ccmdlen] = '\0';

    if (ccmdlen == 0)
	paramtype = 0;
    else Block {
	/* in most cases, the interest is whether the arg string
	 *  0: was empty
	 *  1: contained only a number
	 *  2: contained only a rectangle spec (e.g. "12x16")
	 *  3: contained a single-word string
	 *  4: contained a multiple-word string
	 **/
	char *c2;
	c2 = ccmdp;
	paramtype = getpartype (&c2, YES, NO, curwksp->wlin + cursorline);

	switch (paramtype) {
	case 1:
	case 2:
	    Block {
		Reg1 char *cp;
		for (cp = c2; *cp && *cp == ' '; cp++)
		    continue;
		if (*cp)
		    paramtype = 4; /* more than one string */
	    }
	    break;

	case 3:
	    Block {
		Reg1 char *cp;
		for (cp = ccmdp; *cp && *cp == ' '; cp++)
		    continue;
		for (; *cp && *cp != ' '; cp++)
		    continue;
		for (; *cp && *cp == ' '; cp++)
		    continue;
		if (*cp)
		    paramtype = 4;
	    }
	}
    }
    paramv = ccmdp;
    if (ccmdlen == 0)
	/* exchange back so that we don't have empty string as last cmd */
	exchgcmds ();
err:    /* added for .estartup err return */
    restcurs ();
    clrbul ();
    entering = NO;
/* /
dbgpr("param() end\n");
/ */

    return;
}

#ifdef COMMENT
void
exchgcmds ()
.
    Exchange the current command and last command.
#endif
void
exchgcmds ()
{
    Reg1 int i;
    Reg2 char *cp;

    cp = lcmdp;
    lcmdp = ccmdp;
    ccmdp = cp;
    i = lcmdl;
    lcmdl = ccmdl;
    ccmdl = i;
    i = lcmdlen;
    lcmdlen = ccmdlen;
    ccmdlen = i;
    return;
}

#ifdef COMMENT
void
getarg ()
.
    Get the argument from the edit window - all characters up to space.
    Called from mainloop ().
#endif
void
getarg ()
{
    Reg1 char  *cp;
    int len;

    if (ccmdl == 0)
	ccmdp = salloc (ccmdl = LPARAM, YES);

    if ((cp = pkarg (curwksp, curwksp->wlin + cursorline,
		     curwksp->wcol + cursorcol, &len)) == NULL) {
	ccmdp[0] = '\0';
	return;
    }

    if (ccmdl < len + 1)
	ccmdp = gsalloc (ccmdp, 0, ccmdl = len + 1, YES);
    /* arg = rest of "word" */
    strncpy (ccmdp, cp, (size_t)len);
    ccmdp[len] = '\0';
    ccmdlen = len;
    paramv = ccmdp;
    return;
}

#ifdef COMMENT
char *
pkarg (wksp, line, col, len)
    S_wksp *wksp;
    Nlines line;
    Ncols col;
    int *len;
.
    Get the argument from wksp [line, col] i.e. all characters up to space.
    Return pointer to string within cline.
    Return length of string in len.
#endif
char *
pkarg (wksp, line, col, len)
Reg4 S_wksp *wksp;
Reg5 Nlines line;
Reg3 Ncols col;
int *len;
{
    Reg6 La_stream *olas;
    Reg2 int rlen;
    Reg1 char *cp;

    olas = curlas;
    curlas = &wksp->las;
    GetLine (line);
    curlas = olas;
    if (col >= ncline - 1 || cline[col] == ' ')
	return NULL;
    for (rlen = 0, cp = &cline[col]; *cp != ' ' && *cp != '\n'; rlen++, cp++)
	continue;
    *len = rlen;
    return &cline[col];
}

#ifdef COMMENT
void
limitcursor ()
.
    Limit cursor to be within current window.
    Used after a new window has been made to be sure that the cursor
    in the parent window stays within that window.
#endif
void
limitcursor ()
{
    curwksp->ccol = min (curwksp->ccol, curwin->rtext);
    curwksp->clin = min (curwksp->clin, curwin->btext);
    return;
}


#ifdef COMMENT
void
info (column, ncols, msg)
    Scols column;
    Scols ncols;
    char *msg;
.
    Put 'msg' up on the Info Line at 'column'.
    'ncols' is the length of the previous message that was there.
    If 'ncols' is longer than the length of 'msg', the remainder of
    the old message is blanked out.
#endif
void
info (column, ncols, msg)
Scols column;
Reg3 Scols ncols;
Reg2 char *msg;
{
    Reg1 Short chr;
    Reg4 S_window *oldwin;
    Reg5 Scols  oldcol;
    Reg6 Slines oldlin;

    oldwin = curwin;        /* save old window info   */
    oldcol = cursorcol;
    oldlin = cursorline;
/** /
if (DebugVal)
dbgpr("info(), col=(%d) ncols=(%d) COLS=(%d) msg=(%s) infowin.{tmarg ttext}=%d %d\n",
column, ncols, COLS, msg, infowin.tmarg, infowin.ttext);
/ **/
    switchwindow (&infowin);
    poscursor (column, 0);

    /* Truncate a long path/filename if it won't fit,
     * and display "..." in front of the shortened name
     */
    if (column == inf_file && (column + (int)strlen(msg) > COLS)) {
	msg += ((column + (int)strlen(msg)) - COLS) + 4;
	putch('.', NO); putch('.', NO); putch('.', NO);
    }

    for (; cursorcol < infowin.redit && (chr = *msg++); ncols--)
	if (040 <= chr && chr < 0177)
	    putch (chr, NO);
	else {
	    putch (ESCCHAR, NO);
	    if (chr != ESCCHAR)
		putch ((chr & 037) | 0100, NO);
	}
    multchar (' ', ncols);
    switchwindow (oldwin);
    poscursor (oldcol, oldlin);
    return;
}

#ifdef COMMENT
void
mesg (parm, msgs)
    Small parm;
    char *msgs;
.
    Put a message on the Command Line.
    The least significant 3 bits of 'parm' tell how many string arguments
    follow.  The rest of the bits are used to control the workings of
    'mesg'.
.
      TELSTRT 0010  Start a new message
      TELSTOP 0020  End of this message
      TELCLR  0040  Clear rest of Command Line after message
      TELDONE 0060  (TELSTOP | TELCLR)
      TELALL  0070  (TELSTRT | TELCLR)
      TELERR  0100  This is an error message.
    The following are analogous to TEL...
      ERRSTRT 0110
      ERRSTOP 0120            /* no more to write     */
      ERRCLR  0140            /* clear rest of line   */
      ERRDONE 0160
      ERRALL  0170
#endif

#ifdef OUT
/* VARARGS 1 */
#include <stdarg.h>
void
mesg (int parm, ...)
{
    Reg3 Scols lastcol;
    Reg6 Small nmsg;
    Reg7 char **msgp;
    Reg4 Flag to_image;         /* YES = to screen image, NO = putchar */
/* sun4 changes*/
    va_list ap;
    char *args[10];
    int argno = 0;
    int i;
    char **msgs;

    va_start(ap, parm);
    for(i = (parm&07); i>0; i-- ) {
      args[argno++] = va_arg(ap, char *);
    }

    /*
     *  Added so opt -silent can be used to execute the .e_profile
     *  stuff quietly.
     */
    if (silent)
       return;
    to_image = windowsup || replaying;
    if (to_image) {
	if (parm & TELSTRT) {
	    msowin = curwin;        /* save old window info   */
	    msocol = cursorcol;
	    msolin = cursorline;
	    switchwindow (&enterwin);
	    if (cmdmode)
		poscursor (sizeof "CMDS: " - 1, 0);
	    else
		poscursor (0, 0);
	}
	else
	    if (curwin != &enterwin)
		return;

	lastcol = cursorcol;
    }
    else {
	if (parm & TELSTRT) {
	    putchar ('\n');
	    fflush (stdout);
	}
    }
    if ((parm & ERRSTRT) == ERRSTRT) Block {
	Reg1 char *cp;
	cp = "\007 *** ";
	if (to_image)
	    for (; cursorcol < enterwin.redit && *cp; )
		putch (*cp++, NO);
	else
	    for (; cursorcol < enterwin.redit && *cp; )
		putchar (*cp++);
    }


/*  msgp = &msgs;   */
    i = 0;
    for (nmsg = parm & 7; nmsg-- > 0; ) Block {
	Reg2 Uchar *cp;
	Reg1 Short chr;
/*      cp = (Uchar *) *msgp++;   */
	cp = (Uchar *) args[i++];
	if (to_image) {
	    for (; cursorcol < enterwin.redit && (chr = *cp); cp++) {
		if ((040 <= chr && chr < 0177) || chr == 7)
		    putch (chr, NO);
		else {
		    putch (ESCCHAR, NO);
		    if (chr != ESCCHAR)
			putch ((chr & 037) | 0100, NO);
		}
	    }
	}
	else {
	    while (*cp)
		putchar (*cp++);
	}
    }

    if ((parm & ERRSTOP) == ERRSTOP) {
	if (to_image) {
	    putch ('\007', NO);
	    loopflags.hold = YES;
	}
	else
	    putchar ('\007');
    }

    if (to_image) Block {
	Reg1 Scols tmp;
	static Scols lparamdirt;       /* for edit part        */
	static Scols rparamdirt;       /* for info part        */
	lastcol = (cursorcol < lastcol) ? enterwin.redit : cursorcol;
	/* Did we wrap around? */
	tmp = (lastcol <= term.tt_width - 1) ? lparamdirt : rparamdirt;
	/* how much dirty from before? */
	if (lastcol > tmp || parm & TELCLR) {
	    if (lastcol <= term.tt_width - 1)
		lparamdirt = lastcol;
	    else
		rparamdirt = lastcol;
	}
	if ((parm & TELCLR) && (lastcol < enterwin.redit)) {
	    /* wipe out rest of what was there */
	    tmp = min (tmp, enterwin.redit);
	    multchar (' ', tmp - cursorcol);
	    if (!(parm & TELSTOP))
		poscursor (lastcol, cursorline);
	}
    }

    if (parm & TELSTOP) {         /* remember last position and.. */
	if (to_image) {
	    if (curwin != msowin) {
		switchwindow (msowin); /* go back to original pos      */
	    }
	    poscursor (msocol, msolin);
	}
	else
	    putchar (' ');
    }

    va_end(ap);
    return;
}
#endif

char MesgBuf[256];

void
mesg (int parm, ...)
{
   va_list ap;
   char *s1, *s2, *s3, *s4, *s5;

   va_start(ap, parm);
   int n = parm & 7;
   /*static char msg[200] = "";*/
   /*char *t = &msg[0];*/
   char *t = &MesgBuf[0];

   switch (n) {
     case 1:
       s1 = va_arg (ap, char *);
       strcpy(t, s1);
       break;
     case 2:
       s1 = va_arg (ap, char *);
       s2 = va_arg (ap, char *);
       sprintf(t, "%s%s", s1,s2);
       break;
     case 3:
       s1 = va_arg (ap, char *);
       s2 = va_arg (ap, char *);
       s3 = va_arg (ap, char *);
       sprintf(t, "%s%s%s", s1,s2,s3);
       break;
     case 4:
       s1 = va_arg (ap, char *);
       s2 = va_arg (ap, char *);
       s3 = va_arg (ap, char *);
       s4 = va_arg (ap, char *);
       sprintf(t, "%s%s%s%s", s1,s2,s3,s4);
       break;
     case 5:
       s1 = va_arg (ap, char *);
       s2 = va_arg (ap, char *);
       s3 = va_arg (ap, char *);
       s4 = va_arg (ap, char *);
       s5 = va_arg (ap, char *);
       sprintf(t, "%s%s%s%s%s", s1,s2,s3,s4,s5);
       break;
     break;
   }
   /*msg[199] = '\0';*/

   parm = parm&0170;
   /*mesg(parm+1, &msg[0]);*/
   MesgHack(parm+1, &MesgBuf[0]);
/* /
dbgpr("--after MesgHack (%s)\n", t);
/ */
    va_end (ap);
}

void
MesgHack (parm, msgs)
Reg5 Small parm;
char *msgs;
{
    Reg3 Scols lastcol;
    Reg6 Small nmsg;
    Reg7 char **msgp;
    Reg4 Flag to_image;         /* YES = to screen image, NO = putchar */

    /*
     *  Added so opt -silent can be used to execute the .e_profile
     *  stuff quietly.
     */
    if (silent)
       return;

    to_image = windowsup || replaying;

/* /
dbgpr("MsgHack:  parm=(%o) to_image=%d\n", parm, to_image);

dbgpr("MsgHack, start:  curwin=(%o) msowin=(%o) wholescreen=(%o) enterwin=(%o) infowin=(%o)\n",
    curwin, msowin, &wholescreen, &enterwin, &infowin);
/ */
    if (to_image) {
	if (parm & TELSTRT) {
	    msowin = curwin;        /* save old window info   */
	    msocol = cursorcol;
	    msolin = cursorline;
	    switchwindow (&enterwin);
	    if (cmdmode)
		poscursor (sizeof "CMDS: " - 1, 0);
	    else
		poscursor (0, 0);
	}
	else
	    if (curwin != &enterwin)
		return;

	lastcol = cursorcol;
    }
    else {
	if (parm & TELSTRT) {
	    putchar ('\n');
	    fflush (stdout);
	}
    }
    if ((parm & ERRSTRT) == ERRSTRT) Block {
	Reg1 char *cp;
	cp = "\007 *** ";
	if (to_image)
	    for (; cursorcol < enterwin.redit && *cp; )
		putch ((Uchar)*cp++, NO);
	else
	    for (; cursorcol < enterwin.redit && *cp; )
		putchar (*cp++);
    }

    msgp = &msgs;
    for (nmsg = parm & 7; nmsg-- > 0; ) Block {
	Reg2 Uchar *cp;
	Reg1 Short chr;
	cp = (Uchar *) *msgp++;
	if (to_image) {
	    for (; cursorcol < enterwin.redit && (chr = *cp); cp++) {
		if ((040 <= chr && chr < 0177) || chr == 7)
		    putch (chr, NO);
		else {
		    putch (ESCCHAR, NO);
		    if (chr != ESCCHAR)
			putch ((chr & 037) | 0100, NO);
		}
	    }
	}
	else {
	    while (*cp)
		putchar (*cp++);
	}
    }

    if ((parm & ERRSTOP) == ERRSTOP) {
	if (to_image) {
	    putch ('\007', NO);
	    loopflags.hold = YES;
	}
	else
	    putchar ('\007');
    }

    if (to_image) Block {
	Reg1 Scols tmp;
	static Scols lparamdirt;       /* for edit part        */
	static Scols rparamdirt;       /* for info part        */
	lastcol = (cursorcol < lastcol) ? enterwin.redit : cursorcol;
	/* Did we wrap around? */
	tmp = (lastcol <= term.tt_width - 1) ? lparamdirt : rparamdirt;
	/* how much dirty from before? */
	if (lastcol > tmp || parm & TELCLR) {
	    if (lastcol <= term.tt_width - 1)
		lparamdirt = lastcol;
	    else
		rparamdirt = lastcol;
	}
	if ((parm & TELCLR) && (lastcol < enterwin.redit)) {
	    /* wipe out rest of what was there */
	    tmp = min (tmp, enterwin.redit);
	    multchar (' ', tmp - cursorcol);
	    if (!(parm & TELSTOP))
		poscursor (lastcol, cursorline);
	}
    }

    if (parm & TELSTOP) {         /* remember last position and.. */
	if (to_image) {
/* /
dbgpr("MsgHack(), TELSTOP:  curwin=(%o) msowin=(%o) enterwin=(%o)\n",
    curwin, msowin, &enterwin);
/ */
	    if (curwin != msowin) {
		switchwindow (msowin); /* go back to original pos      */
	    }
	    poscursor (msocol, msolin);
	}
	else
	    putchar (' ');
    }

/* /
dbgpr("MsgHack, end:  curwin=(%o) wholescreen=(%o) enterwin=(%o) infowin=(%o)\n",
    curwin, &wholescreen, &enterwin, &infowin);
/ */

    return;
}



#ifdef COMMENT
void
credisplay (cwkspflg)
Flag cwkspflg;
.
    Redisplay the current line if it has changed since that last time
    credisplay was called.
    Only called from one place: mainloop ().
#endif
void
credisplay (cwkspflg)
Flag cwkspflg;
{
    /* this is a little tricky.  Everywhere else in the editor, fcline
     * is treated as if it is either 0 or non-0, thus it is given YES
     * or NO values.  Here, if it is YES, we set it to 2, so that we can
     * tell if it has been set to YES since the last time we were called.
     * Only if it is YES do we need to do a redisplay.
     **/
    if (fcline == YES) {
	redisplay (curfile, curwksp->wlin + cursorline, 1, 0, cwkspflg);
	fcline = 2;
    }
    return;
}

extern Nlines readjtop (Nlines win, Nlines from, Nlines num, Nlines delta);

#ifdef COMMENT
void
redisplay (fn, from, num, delta, cwkspflg)
    Fn      fn;
    Nlines  from;
    Nlines  num;
    Nlines  delta;
    Flag    cwkspflg;
.
    Redisplay is called after a change has been made in file 'fn',
    starting at line 'from',
    with a total change of 'delta' in the length of the file.
    If 'delta' is negative, 'num' lines must be redisplayed after
    deleting 'delta' lines.
    If 'delta' is positive, 'delta' + 'num' lines must be redisplayed.
    If 'delta' is 0, dislplay 'num' lines.
    We are supposed to:
    1. Redisplay any workspaces which are actually changed by this
	tampering, including curwksp if 'cwkspflg' is non-0
    2. Adjust the current line number of any workspace which may be pointing
	further down in the file than the change, and does not want
	to suffer any apparent motion.
#endif
void
redisplay (fn, from, num, delta, cwkspflg)
Fn      fn;
Reg5 Nlines  from;
Reg8 Nlines  num;
Reg9 Nlines  delta;
Flag    cwkspflg;
{
    Reg1 S_wksp *tw;
    Reg4 Small  win;
    Reg3 Slines winfirst;   /* from - wksp->wlin */
    Reg6 Slines first;      /* first line of area in window to be changed */
    Reg2 Slines endwin;     /* height of window -1 */

/** /
dbgpr("redisplay: fn=(%d) from=(%d) num=(%d), delta=(%d) cwkspflg=(%d)\n",
  fn,from,num,delta,cwkspflg);
/ **/
    for (win = Z; win < nwinlist; win++) {
	if ((tw = winlist[win]->altwksp)->wfile == fn)
	    /* tw->wlin += readjtop (tw->wlin, from, num, delta, winlist[win]->btext + 1); */
	    tw->wlin += readjtop (tw->wlin, from, num, delta);
	if ((tw = winlist[win]->wksp)->wfile == fn) {
	    Nlines wmove;
	    Reg7 S_window *oldwin;
	    /* wmove = readjtop (tw->wlin, from, num, delta, winlist[win]->btext + 1); */
	    wmove = readjtop (tw->wlin, from, num, delta);
	    winfirst = (Slines) (from - (tw->wlin += wmove));
	    if (tw == curwksp && !cwkspflg)
		continue;
	    first = winfirst > 0 ? winfirst : 0;
	    endwin = winlist[win]->btext;
	    /* are the changes below the bottom of the window? */
	    if (first > endwin)
		continue;
	    /* are the changes above the top of the window? */
	    if (delta == 0 && winfirst + num <= 0)
		continue;
	    if (   delta > 0
		&& winfirst < 0         /* for insert on first line */
		&& winfirst + num <= 0
	       )
		continue;
	    if (delta < 0 && winfirst - delta + num <= 0)
		continue;
	    /* something in the window will have to change */
	    oldwin = curwin;
	    savecurs ();
	    switchwindow (winlist[win]);
	    if (curwin != oldwin)
		chgborders = 0;
	    if (delta == 0) {
		/* wmove will also be 0 at this point */
		putup (first, winfirst + num - 1, 0, MAXWIDTH);
	    } else if (delta > 0) {
		if (winfirst + delta + num > endwin)
		    putup (first, endwin, 0, MAXWIDTH);
		else if (from >= la_lsize (curlas) - delta)
		    putup (first, winfirst + delta + num - 1, 0, MAXWIDTH);
		else if (  !vinsdel (first, delta, curwin == oldwin)
			 && num > 0
			)
		    putup (winfirst + delta, winfirst + delta + num - 1,
			   0, MAXWIDTH);
	    } else { /* delta < 0 */
		if (   first - (delta - wmove) > endwin
		    || (   num > 0
			&& winfirst - delta + num > endwin
		       )
		   )
		    putup (first, endwin, 0, MAXWIDTH);
		else if (from >= la_lsize (curlas))
		    putup (first, winfirst - delta - 1, 0, MAXWIDTH);
		else if (   !vinsdel (first, delta - wmove, curwin == oldwin)
			 && num > 0
			)
		    putup (first, winfirst + num - 1, 0, MAXWIDTH);
	    }
	    chgborders = 1;
	    switchwindow (oldwin);
	    restcurs ();
	}
    }
    return;
}

#ifdef COMMENT
Nlines
readjtop (wlin, from, num, delta)
    Nlines wlin;
    Nlines  from;
    Nlines num;
    Nlines delta;
.
    Called by redisplay () to determine how far to move the top of 'wksp'.
    Returns the distance to move the window.
.
    The logic in redisplay () and the algorithm
    implemented here are closely interdependent.
#endif
/*readjtop (wlin, from, num, delta, Slines height) */  /* height not used */
Nlines
readjtop (wlin, from, num, delta)
Reg1 Nlines wlin;
Reg3 Nlines from;
Reg4 Nlines num;
Reg2 Nlines delta;
{
    /* adjust line num of top of wksp, if necessary  */

    if (delta == 0)
	return 0;
    if (delta > 0) {
	if (from >= wlin)
	    return 0;
	if (from + num <= wlin)
	    return delta;
	return 0;
    }
    /* delta < 0 */
	if (from > wlin)
	    return 0;
	if (from - delta + num <= wlin)
	    /* no effect on screen */
	    return delta;
	if (num == 0)
	    return from - wlin;
	return 0;
}

#ifdef COMMENT
void
screenexit (scroll)
    Flag scroll;
.
    Reset terminal if required.
    Put cursor at lower left of screen
    Scroll the screen if scroll is non-0.
#endif
void
screenexit (scroll)
Flag scroll;
{
#ifdef LMCLDC
    if (line_draw)                      /* if in line-drawing mode, */
	(*term.tt_xlate) (' ');         /* send printable to exit mode. */
#endif /* LMCLDC */
#ifdef CLEARONEXIT
    /*  When a terminal simulator is used that keeps its own duplicate image
     *  of the screen, screenexit is only used upon exiting, so it doesn't
     *  need to reinitialize that image.
     **/
    d_put (VCCCLR);
    (*kbd.kb_end) ();
    d_put (VCCEND);
#else
    putscbuf[0] = VCCAAD;
    putscbuf[1] = 041 + 0;
    putscbuf[2] = (unsigned char) (041 + term.tt_height - 1);
    putscbuf[3] = 0;
    d_write (putscbuf, 4);
    (*kbd.kb_end) ();
    d_put (VCCEND);
    if (scroll) {
	putchar ('\n');
	fflush (stdout);
    }
#endif
    windowsup = NO;
    return;
}

#ifdef COMMENT
void
tglinsmode ()
.
    Toggle INSERT mode.
#endif
void
tglinsmode ()
{
/*  info (inf_insert, 6, (insmode = !insmode) ? "INSERT" : ""); */
    info (inf_insert, 4, (insmode = !insmode) ? "INS" : "");
    return;
}

#ifdef COMMENT
void
tglpatmode ()
.
    Toggle PATTERN mode.
#endif
void
tglpatmode ()                           /* MAB */
{
    info (inf_pat, 2, (patmode = !patmode) ? "RE" : "  ");
    return;
}


#ifdef COMMENT
Flag
vinsdel (start, delta, mainwin)
    Slines start;
    Slines delta;
    Flag mainwin;
.
    Insert 'delta' lines at line 'start' in the window.
    This means delete if 'delta' is negative, of course.
    Return YES if we had to do a putup for the whole thing,
    i.e. could not use ins/del terminal capability, else NO.
#endif /* COMMENT */
Flag
vinsdel (start, delta, mainwin)
Reg1 Slines start;
Reg3 Slines delta;
Flag mainwin;
{
    Reg5 S_wksp *cwksp = curwksp;

    if (mainwin) {
	clrbul ();
	offbullets ();
    }
    if (delta > 0) {
	Reg2 int num;
	/* insert lines */
	do {
	    Reg4 int stplnum;   /* start + num */
	    Reg5 Uint nmove;
	    num = d_vmove (curwin->ttext + start,
			   curwin->lmarg,
			   curwin->btext + 1 - (start + delta),
			   curwin->rmarg + 1 - curwin->lmarg,
			   delta,
			   YES);
	    if (num <= 0) {
 doputup:
		savecurs ();
		putup (start, curwin->btext, 0, MAXWIDTH);
		restcurs ();
		return YES;
	    }
	    nmove = (Uint)(curwin->btext + 1 - (stplnum = start + num));
	    my_move ((char *)&curwin->firstcol[start], (char *)&curwin->firstcol[stplnum],
		  (ulong) nmove * sizeof curwin->firstcol[0]);
	    fill (&curwin->firstcol[start],
		  (Uint)(num * (int)sizeof curwin->firstcol[0]), 0);
	    my_move ((char *)&curwin->lastcol[start], (char *)&curwin->lastcol[stplnum],
		  (ulong) nmove * sizeof curwin->lastcol[0]);
	    fill (&curwin->lastcol[start],
		  (Uint)(num * (int)sizeof curwin->lastcol[0]), 0);
	    my_move ((char *)&curwin->lmchars[start], (char *)&curwin->lmchars[stplnum], (ulong) nmove);
	    my_move ((char *)&curwin->rmchars[start], (char *)&curwin->rmchars[stplnum], (ulong) nmove);
	    savecurs ();
	    freshputup = YES;
	    nodintrup = YES;
	    cwksp->wlin += delta - num;
	    putup (start, start + num - 1, 0, MAXWIDTH);
	    cwksp->wlin -= delta - num;
	    nodintrup = NO;
	    freshputup = NO;
	    restcurs ();
	} while ((delta -= num) > 0);
    } else { /* delta < 0 */
	Reg2 int num;
	delta = -delta;
	/* delete lines */
	do {
	    Reg4 int stplnum;   /* start + num */
	    Reg5 Uint nmove;
	    num = d_vmove (curwin->ttext + start + delta,
			   curwin->lmarg,
			   curwin->btext + 1 - start - delta,
			   curwin->rmarg - curwin->lmarg + 1,
			   -delta,
			   YES);
	    if (num <= 0)
		goto doputup;
	    nmove = (Uint)(curwin->btext + 1 - (stplnum = start + num));
	    my_move ((char *)&curwin->firstcol[stplnum], (char *)&curwin->firstcol[start],
		  (ulong) nmove * sizeof curwin->firstcol[0]);
	    fill (&curwin->firstcol[curwin->btext + 1 - num],
		  (Uint)(num * (int)sizeof curwin->firstcol[0]), 0);
	    my_move ((char *)&curwin->lastcol[stplnum], (char *)&curwin->lastcol[start],
		  (ulong) nmove * sizeof curwin->lastcol[0]);
	    fill (&curwin->lastcol[curwin->btext + 1 - num],
		  (Uint)(num * (int)sizeof curwin->firstcol[0]), 0);
	    my_move ((char *)&curwin->lmchars[stplnum], (char *)&curwin->lmchars[start], (ulong) nmove);
	    my_move ((char *)&curwin->rmchars[stplnum], (char *)&curwin->rmchars[start], (ulong) nmove);
	    savecurs ();
	    freshputup = YES;
	    nodintrup = YES;
	    cwksp->wlin -= delta - num;
	    putup (curwin->btext + 1 - num, curwin->btext, 0, MAXWIDTH);
	    cwksp->wlin += delta - num;
	    nodintrup = YES;
	    freshputup = NO;
	    restcurs ();
	} while ((delta -= num) > 0);
    }
    return NO;
}

/*
 * What follows was added 2/8/83 at Purdue CS by MAB
 */
#ifdef COMMENT
void
tgltabmode ()
.
    Toggle TABS mode.
#endif
void
tgltabmode ()
{
    upblanks = litmode = NO;
    uptabs = YES;
    info (inf_tab, 3, "TAB");
    return;
}
void
tglblamode ()
{
    uptabs = litmode = NO;
    upblanks = YES;
    info (inf_tab, 2, "BL");
    return;
}
void
tgllitmode ()
{
    uptabs = upblanks = NO;
    litmode = YES;
    info (inf_tab, 3, "LIT");
    return;
}
void
tglstrmode ()
{
    upnostrip = YES;
    info (inf_str, 3, "NOS");
}


/* mGetkey and mGetkey1 are tmp copies, for debugging */

#ifdef SYSSELECT
unsigned Short
mGetkey (peekflg, timeout)
Flag peekflg;
struct timeval *timeout;
#else
unsigned Short
mGetkey (peekflg)
Flag peekflg;
#endif
{
    Reg1 unsigned Short rkey;
    static Flag knockdown  = NO;
    /*extern*/ unsigned Short mGetkey1 ();

/*dbgpr("mGetkey, top:  peekflg=%d\n", peekflg);*/

/** /
dbgpr("mGetkey: peekflg=%d haveChar=%d ('%c') peekCh=(%c)\n", peekflg, haveChar, (char) haveChar, (char) peekCh);
/ **/
    if (peekflg == WAIT_KEY && keyused == NO) {
	dbgpr("mGetkey: WAIT_KEY==yes and keyused==NO, returning key=%d (a no-op)\n", key);
	return (Uint)key; /* then getkey is really a no-op */
    }

#ifdef  SYSSELECT
    rkey = mGetkey1 (peekflg, timeout);
#else /* SYSSELECT */
    rkey = mGetkey1 (peekflg);
#endif /* SYSSELECT */

#ifdef NCURSES
/** /dbgpr("e.t.c, mGetkey() knockdown=(%d) return=(%d)(%04o)('%c') \n",
      knockdown, rkey,rkey,(char)rkey); / **/
#endif /* NCURSES */

    if (knockdown && rkey < 040)
	rkey |= 0100;
    if (peekflg != WAIT_KEY)
	return rkey;
    knockdown = rkey == CCCTRLQUOTE;    /* ^\ knockdown next char   */
    keyused = NO;

    if( !replaying && !dot_profile ) {
#ifdef RECORDING
	if (recording) {
	    if( rkey != NOCHAR ) {
		RecordChar (rkey);
	    }
	}
#endif /* RECORDING */
	if (keyfile != NULL && rkey != CCINT && rkey != CCMOUSE) {
	    putc ((int)rkey, keyfile);
	    if (numtyp > MAXTYP) {
		flushkeys ();
		numtyp = 0;
	    }
	}
    }
    key = (int)rkey;
/** /dbgpr("mGetkey returning (%o)\n", rkey); / **/
    return rkey;
}


#ifdef  SYSSELECT
unsigned Short
mGetkey1 (peekflg, timeout)
Small peekflg;
struct timeval *timeout;
#else /* SYSSELECT */
unsigned Short
mGetkey1 (peekflg)
Small peekflg;
#endif /* SYSSELECT */
{
    static Short lcnt;
    static int   lexrem = 0;         /* portion of lcnt that is still raw */
    static Uchar chbuf[NREAD];
    static Uchar *lp = &chbuf[0];

/** /
dbgpr("mGetkey1: peekflg=%d replaying=%d recovering=%d, silent=%d, entering=%d, winup=%d\n",
  peekflg, replaying, recovering, silent, entering, windowsup);
dbgpr("mGetkey1: curwin=(%o) enterwin=(%o)\n", curwin, &enterwin);
/ **/

    int c;
    static int replay_cnt;  /* number of chars replayed */
    if (replaying) Block {
	static Small replaydone = 0;

	/* better ways to do this, but in the interest
	 * of minimal recoding, for now ...
	 */

	if ((c = fgetc(replay_fp)) == EOF) {
	/*  dbgpr("mGetkey1, replaying, at EOF, entering=%d\n", entering); */
	    replaydone++;
	}

	if (replaydone) {
 finishreplay:
	    /*close (inputfile);*/
	    fclose (replay_fp);
	    inputfile = STDIN;  /* fd 0, not a FILE ptr */
	    if (silent) {
		silent = NO;
		(*term.tt_ini1) (); /* not d_put(VCCICL) because fresh() */
				    /* follows */
		windowsup = YES;
		fresh ();
	    }

	    /*
	    mesg (ERRALL + 1, recovering ? "Recovery completed." : replaydone == 1 ?
		"Replay completed." : "Replay aborted." );
	    */
	    char replaymsg[100];
	    sprintf(replaymsg, "%s Hit RETURN to continue:",
		recovering ? "Recovery completed." : replaydone == 1 ?
		    "Replay completed." : "Replay stopped." );

	    mesg ((TELSTRT|TELCLR|TELSTOP) + 1, replaymsg);
	    /*mesg (ERRALL+1, replaymsg);*/
	    d_put (0);
	    fflush(stdout);

	    /* Some replays result in this error condition, eg stopped while in <cmd> mode (5/2021) */
	    if (curwin == &enterwin) {
		chgwindow(-1);
	    }

	 /* int c1; */
	    nodelay(stdscr, FALSE);
	 /* c1 =*/ wgetch(stdscr);
	/*  dbgpr(" c1=(%o)(%c) after continue msg\n", c1, c1 ); */

	    /*
	    c1 = fgetc(stdin);
	    dbgpr(" c1=(%o)(%c) hit any key to continue\n", c1, c1 );
	    */

	    replaying = NO;
	    recovering = NO;
	    replaydone = 0;
	    /*
	    intok = YES;
	    */
	    loopflags.hold = YES;
	    finished_replay = YES;

#ifdef  STARTUPFILE
	    /*
	     *  Don't reread the the .e_profile file if recovering
	     *  from a crash.
	     */
	    if( dot_profile ) {
		fclose( fp_profile );
		dot_profile = NO;
	    }
#endif /* STARTUPFILE */

/*
dbgpr("---replaydone\n");
*/

	    /**
	    ungetch(CCRETURN);
	    return CCCMD;
	    **/

	    return CCNULL;

	    /*goto m_nonreplay; why? */
	}

	/* any key stops replay */
	replay_cnt++;

#ifdef NCURSES
	/*    Keep track of how many chars are left in keystroke file to
	 *    allow the replay to be cancelled before the last few keystrokes
	 */

	if (replay_stopcount) {   /* set by user to stop before last few keystrokes */
	    long pos = ftell (replay_fp);
	    if ((replay_filesize - pos) < replay_stopcount) {
		/** /
		dbgpr("STOP replay, pos=(%d) replay_size=(%d), stopcount=(%d)\n",
		    pos, replay_filesize, replay_stopcount);
		/ **/
		/*mesg(ERRALL+1, "hit any key to interrupt replay");*/
		nodelay(stdscr,TRUE);
		wgetch(stdscr);  /* keyboard input, not from keystroke file */
		nodelay(stdscr, FALSE);
		lcnt = 0;
		replaydone = 2;
		goto endreplay;
	    }
	}
		  /* don't think we need this anymore...*/
	if( !recovering ) {
	    nodelay(stdscr,TRUE);
	    int rc = wgetch(stdscr);  /* keyboard input, not keystroke file */
	    nodelay(stdscr, FALSE);
	    if (rc != ERR) {
		lcnt = 0;
		replaydone = 2;
		goto endreplay;
	    }
	}
#else
	if ( !recovering && !xempty (STDIN) ) {
	    lcnt = 0;
	    replaydone = 2;
	    while (xread (STDIN, chbuf, NREAD) > 0 && !xempty (STDIN))
		continue;
	    goto endreplay;
	}
#endif /* NCURSES */


/*
dbgpr("replaying, *cp=(%o)(%c) col=%d lin=%d, cnt=%d\n",
  c, c, cursorcol, cursorline, replay_cnt );
*/

	/* this is currently the only cmd that needs to peek ahead (need: y,x) */

	if( c == CCMOUSE ) {
	    int y, x;
	  /*int mouse_err = 0;*/
	    int rc = fscanf(replay_fp, "%3d%3d", &y, &x);
	    if( rc != 2 || feof(replay_fp)) {
	     /* mouse_err = 1; */
		/* skip mouse call */
	    /*  dbgpr("replay err, fscanf returned (%d) expected (%d)\n", rc, 2); */
		goto endreplay;
	    }
	    else {
		doMouseReplay(y, x);
		return NOCHAR;  /* or, CCNULL ? */
	    }
	}

	if ( c == CCSTOP ) {
	    replaydone = 1;
 endreplay:
	    if (!entering)
		goto finishreplay;
	    else {
		/*
		*chbuf = CCINT;
		lp = chbuf;
		lcnt = 1;
		*/
		c = CCINT;
	    }
	}

    /*  dbgpr("--replaying, returning (%o)(%c), entering=%d\n",
	    c, c, entering); */
	return (Uint)c;
    }

#ifdef RECORDING
    else if (playing) {
	    extern Flag play_silent;

	lp = (Uchar *)PlayChar (peekflg);
	if (!playing && play_silent) {
	    silent = 0;
	    fresh();
	}
	/*
	 * Any key stops playback
	 */
#ifdef NCURSES
	nodelay(stdscr,TRUE);
	int rc = getch();
	nodelay(stdscr, FALSE);
	if (rc != ERR) {
	    return (CCINT);
	}
	else {
	  return (unsigned Short)*lp;
	}
#else
	if (!xempty(STDIN)) {
	    char c;
	    /*
	     *  throw away keystroke[s]
	     */
	    do {
		read (inputfile, &c, 1);
	    } while (!xempty(STDIN));
	    if( play_silent )
		silent = 0;
	    play_count = 0;
	/*  playing = 0;  */
	    return (CCINT);
	}
	/*goto endGetkey1;*/
	return (unsigned Short)*lp;
#endif /* NCURSES */
    }
#endif /* RECORDING */

    else {
	static int firstkey = 1;

/* label not used
 m_nonreplay:
*/

/* START HACK */
	if (firstkey) {
		static Uchar redraw_buf[] = { CCCMD, 'r', 'e', 'd', '\015', '-' };
		static Uchar *redraw_ptr = redraw_buf;

		if (*redraw_ptr != '-') {  /* since CCCMD is 0, use '-' as stop point */
		   return (*redraw_ptr++);
		}

		firstkey = 0;
		/* return (CCCMD); */
	}
/* END HACK */
#ifdef  STARTUPFILE
	/*
	 *  Input from the .e_profile file?
	 */
	if( dot_profile && peekflg == WAIT_KEY )
	    return( (Uint)get_profilekey( NO ));
#endif /* STARTUPFILE */

/**
dbgpr("e.t.c, mGetkey1, nonreplay:  lcnt=(%d), lexrem=(%d), lp-chbuf=(%d), NREAD=(%d)\n",
   lcnt, lp-chbuf, lexrem, NREAD);
 **/

	if (lcnt < 0)
	    fatal (FATALBUG, "lcnt < 0");
	if (   (lcnt - lexrem == 0 && peekflg != PEEK_KEY)
	    || (lcnt < NREAD && !xempty (inputfile))
	   ) {
	    /* read some more */
	    if (lcnt == 0)
		lp = chbuf;
	    else if (lp > chbuf) {
		if (lcnt == 1) {
		    /* handle frequent case more efficiently */
		    *chbuf = *lp;
		}
		else
		    my_move ((char *)lp, (char *) chbuf, (ulong) lcnt);
		lp = chbuf;
	    }
	    d_put (0);
#ifdef  SYSSELECT
#ifdef FDSET
	    Block {
		fd_set readmask;
		FD_ZERO(&readmask);
		FD_SET(inputfile, &readmask);
		/*dbgpr("inputfile=(%d)\n", inputfile);*/
		if (   peekflg == WAIT_PEEK_KEY
		    && select (inputfile + 1, &readmask, NULL, NULL, timeout) <= 0
		   )
		    return NOCHAR;
	    }
#else /* FDSET */
	    Block {
		int z = 0;
		int readmask = 1 << inputfile;

		if (   peekflg == WAIT_PEEK_KEY
		    && select (inputfile + 1, &readmask, &z, &z, timeout) <= 0
		   )
		    return NOCHAR;
	    }
#endif /* FDSET */
#endif /* SYSSELECT */
	    do Block {
		Reg3 int nread;
		nread = NREAD - lcnt;
#ifdef  SYSSELECT
#ifdef  FSYNCKEYS
		/*
		 *  If this 1 min. timer goes off, flush and fsync
		 *  the keystroke file.  (Needs to be done only once!).
		 */
#ifdef FDSET
		Block {
		    fd_set readmask;
		    FD_ZERO(&readmask);
		    FD_SET(inputfile, &readmask);
		    /*dbgpr("inputfile=(%d)\n", inputfile); */
		    struct timeval tval;
		    int rc = 0;

		    tval.tv_sec = 60, tval.tv_usec = 0;
		    rc = select (inputfile + 1, &readmask, NULL, NULL, &tval);
		    if (rc == 0 && numtyp) {
			fsynckeys();    /* also does a fflush */
			numtyp = 0;
		    }
		}
#else
		Block {
		    int z = 0;
		    int readmask = 1 << inputfile;
		    struct timeval tval;
		    int rc = 0;

		    tval.tv_sec = 60, tval.tv_usec = 0;
		    rc = select (inputfile + 1, &readmask, &z, &z, &tval);
		    if (rc == 0 && numtyp) {
			fsynckeys();    /* also does a fflush */
			numtyp = 0;
		    }
		}
#endif /* FDSET */
#endif /* FSYNCKEYS */
#endif /* SYSSELECT */

/*
dbgpr("e.t.c, mGetkey1: before xread, fd=%d lcnt=(%d), lexrem=(%d), lp-chbuf=(%d), nread=(%d)\n",
inputfile, lcnt, lexrem, lp-chbuf, nread);
*/

#ifdef OUT
		/*
		 *  With the introduction of Curses initialization, there's a problem
		 *  displaying the initial edit screen.  These steps seem to force
		 *  the display.
		 */
		static Flag init_redraw = 1;
		if (init_redraw) {
		    init_redraw = 0;
		/*  dbgpr("init_redraw, doing fresh()\n"); */
		    fresh();
		    poscursor(cursorcol, cursorline);
		    /*ungetch(CCRETURN);*/
		    /*return CCCMD;*/
		    return (CCNULL);
		}
#endif

		/* MapCursesKey() sets FKey_p when a Fkey press maps
		 * to more than one E cmd/chars.  See MapCursesKey() below.
		 */
		if (FKey_p) {
		    unsigned short ch = *FKey_p++;
		    if (*FKey_p == '\0')
			FKey_p = NULL;
		/*  dbgpr("FKey_p, ch=(%o)\n", ch); */
		    return ch;
		}

		/* normal keyboard input begins here */
		int c, c1;


#ifdef USE_MOUSE_POSITION

#if 1 /*NCURSES_MOUSE_VERSION == 1*/
		/* In 5.7, the REPORT_MOUSE_POSITION event results whenever
		 * the mouse is moved; in 6.2 it is reported only when a key is
		 * held down and the mouse is moved
		 */

		/* skip mouse position reports w/o a prior press event */
		MEVENT evt;
		while((c = wgetch(stdscr)) == KEY_MOUSE) {
		    getmouse(&evt);
		    if (evt.bstate & REPORT_MOUSE_POSITION)
			continue;
		    ungetmouse(&evt);   /* push back the event */
		    c = wgetch(stdscr);
		    break;
		}
#else
		c = wgetch(stdscr);
#endif /* NCURSES_MOUSE_VERSION */

#else
		c = wgetch(stdscr);

#endif /* USE_MOUSE_POSITION */

/** / dbgpr("c=%o KEY_BACKSPACE=%o\n", c, KEY_BACKSPACE);  / **/

		if (c == KEY_BACKSPACE && bs_flag == 1) c = 010;  /* true ^H */

#ifdef OUT
/*debug*/
char hat = ' ';
c1 = c;
if(c1 < ' ') {
  hat = '^';
  c1 += 96;
}
if( c > 0400 ) {  /* curses KEY_xxx */
  dbgpr("\nwgetch(),keyboard input c=(%d)(%04o)(%s)\n", c, c, getCursesKeyname(c) );
}
else {
  dbgpr("\nwgetch(),keyboard input c=(%d)(%04o)('%c%c'), entering=%d\n",
    c, c, hat,c1, entering);
}
fflush(dbgfile);
/*end debug*/
#endif /* OUT */

		if( c >= KEY_MIN ) {
		    c1 = MapCursesKey(c);
		/** /
		   dbgpr("wGetkey1, MapCursesKey:  (%04o)->(%04o) (%s))\n",
			c, c1, getEkeyname(c1));
		/ **/
		    return ((Uint)c1);
		}


#ifdef NOTYET
/* Originally I thought a change was also needed to
 * handle the ^X^{key} and ^X^C{key} functions, but the orig
 * code seems to handle them ok.  Since the curses keypad is now enabled
 * each function key sends a single key (instead of eg, ESC followed by
 * a series of keys, and only need to look ahead for 1 more key. We already
 * have the ^X, so if the 2nd char is not C, at one more for the
 * case ^X^C{escchar} which is how a control character is inserted in the file.
 * If the 2nd char is not C, we have ^X{key} which is a command,
 * eg, ^X^L is <wright>, ^X^H is <wleft>
 */
		else if (c == CTRL('X') ) {   /* start of an two */
		    chbuf[0] = c;
		    chbuf[1] = wgetch(stdscr);
		    stcp = &chbuf[0];
		    lexrem = 1;
		      /* does kb_inlex place the CCcode in chbuf[0] */
		    nread = (*kbd.kb_inlex) (stcp, &lexrem))
		    c1 = chbuf[0];
		    return c1;
		}
#endif /* NOTYET */
		else {
		    chbuf[lcnt] = (Uchar) c;
		}
	     /* if ((nread = xread (inputfile, (char *) &chbuf[lcnt], nread)) > 0) Block { */
		nread = 1;  /* so we don't have to recode the following.... */
		if( nread > 0 ) Block {
		    Reg4 Uchar *stcp;
		    stcp = &chbuf[lcnt -= lexrem];
		    lexrem += nread;
		    if ((nread = (*kbd.kb_inlex) (stcp, &lexrem)) > 0) Block {
			Reg1 Uchar *cp;
			Reg2 int nr;
			cp = &stcp[nread];
			/* look ahead for interrupts */
			nr = nread;
			do {
			    if (*--cp == CCINT)
				break;
			} while (--nr);
			if ((nr = (int) (cp - stcp)) > 0) {
			    /* found CCINT character in chars just read */
			    /* and some characters in front of it are to be */
			    /* thrown away */
			    lp += lcnt + nr;
			    lcnt = nread - nr + lexrem;
			}
			else
			    lcnt += nread + lexrem;
		    }
		    else
			lcnt += lexrem;
		}
		else if (nread < 0) {
		    if (errno != EINTR)
			fatal (FATALIO, "Error reading input.");
		    if (peekflg == WAIT_PEEK_KEY)
			break;
		}
		else
		    fatal (FATALIO, "Unexpected EOF in key input.");
/*
dbgpr("e.t.c, mGetkey1: after xread, fd=%d lcnt=(%d), lexrem=(%d), lp-chbuf=(%d), nread=(%d)\n",
inputfile, lcnt, lexrem, lp-chbuf, nread);
*/
	    } while (lcnt - lexrem == 0);
	}
    }

/*
dbgpr("e.t.c, mGetkey1: AFTER xread loop, lcnt=(%d), lexrem=(%d), lp-chbuf=(%d)\n",
lcnt, lexrem, lp-chbuf);
*/

    if ( lcnt - lexrem <= 0 && peekflg != WAIT_KEY ) {
/*      dbgpr("mGetkey1: lcnt=%d lexrem=%d, return NOCHAR, peekflg=%d\n", lcnt, lexrem, peekflg); */
	return NOCHAR;
    }

    if (peekflg != WAIT_KEY)
#ifdef UNSCHAR
    {
/** /
dbgpr("e.t.c, peekflg != WAIT_KEY, returning (%o)(%c)\n", *lp, *lp);
/ **/
	return *lp;
    }
#else
	return *lp & CHARMASK;
#endif

    Block {
	Reg1 unsigned Short rchar;


#ifdef UNSCHAR
	lcnt--;
	rchar = *lp++;
#else
	lcnt--;
	rchar = *lp++ & CHARMASK;
#endif

#ifdef OUT
/* moved to end of mGetKey() */
	if (keyfile != NULL && rchar != CCINT) {
	    putc (rchar, keyfile);
	    if (numtyp > MAXTYP) {
		flushkeys ();
		numtyp = 0;
	    }
	}
#endif

/* moved to end of mGetKey() so Curses KEY_xxx keys are saved */
#ifdef xRECORDING
	if (recording)
	    RecordChar (rchar);
#endif /* RECORDING */
	return rchar;
    }
}



#ifdef NCURSES
#ifdef USER_FKEYS

/* see /usr/include/ncurses.h or e.keys.h for KEY_Fn keycode values */

struct Key2Ecode {
 int keycode;       /* ncurses KEY_ value */
 Uchar  ecmd1;      /* 1st E cmd, eg:  KEY_F6:<+page> */
 Uchar *ecmdstr;    /* 2nd -> end of e cmds, eg:  KEY_F6:<+word><redraw><-page> */
 int len;           /* num Uchars in ecmdstr */
 char keyname[16];  /* tmp for debugging */
} Key2Ecode[0633 - KEY_MIN];   /* defined in ncurses.h,  0633 - 0401 */



#ifdef OUT /* old method */

struct UserDefinedFuncKeys {
int keycode;    /* these codes are not really used, we index 0-35 */
int cmdval;
} UserFuncKeys[] = {

{265,   -1},     /* KEY_F1  0411-0411 = 0 */
{266,   -1},     /* KEY_F2  0412-0411 = 1 */
{267,   -1},     /* KEY_F3  0413 */
{268,   -1},     /* KEY_F4  0414 */
{269,   -1},     /* KEY_F5  0415 */
{270,   -1},     /* KEY_F6  0416 */
{271,   -1},     /* KEY_F7  0417 */
{272,   -1},     /* KEY_F8  0420 */
{273,   -1},     /* KEY_F9  0421 */
{274,   -1},     /* KEY_F10 0422 */
{275,   -1},     /* KEY_F11 0423 */
{276,   -1},     /* KEY_F12 0424-0411 = 11 */

{277,   -1},     /* SHIFT_F1 0425-0411 = 12 */
{278,	-1},     /* SHIFT_F2 */
{279,	-1},     /* SHIFT_F3 */
{280,	-1},     /* SHIFT_F4 */
{281,	-1},     /* SHIFT_F5 */
{282,	-1},     /* SHIFT_F6 */
{283,	-1},     /* SHIFT_F7 */
{284,	-1},     /* SHIFT_F8 */
{285,	-1},     /* SHIFT_F9 */
{286,	-1},     /* SHIFT_F10 */
{287,	-1},     /* SHIFT_F11 */
{288,   -1},     /* SHIFT_F12 0440-0411 = 23 */

{289,   -1},     /* CTRL_F1 0441-0411 = 24 */
{290,	-1},     /* CTRL_F2 */
{291,	-1},     /* CTRL_F3 */
{292,	-1},     /* CTRL_F4 */
{293,	-1},     /* CTRL_F5 */
{294,	-1},     /* CTRL_F6 */
{295,	-1},     /* CTRL_F7 */
{296,	-1},     /* CTRL_F8 */
{297,	-1},     /* CTRL_F9 */
{298,	-1},     /* CTRL_F10 */
{299,	-1},     /* CTRL_F11 */
{300,   -1},     /* CTRL_F12 0454-0411 = 35 */

};
#endif /* OUT */

#endif /* USER_FKEYS */


int
MapCursesKey (int c)
{

/** / dbgpr("MapCursesKey: c=%o\n", c);  / **/

    if( c < KEY_MIN || c > KEY_MAX ) {
	return ERR;  /* defined in curses as -1, or ?? NOCHAR ?? */
    }

#ifdef USER_FKEYS

/*  int i; */

    /* new mapping */
    int idx = c - KEY_MIN;
    FKey_p = (Uchar *) NULL;

    if ( Key2Ecode[idx].keycode == c ) {
	if ( Key2Ecode[idx].len == 1 ) {  /* eg, A Fkey maps to 1 E cmd */
	    dbgpr("MapCursesKey, NEW returning (%o)(%s) for Fkey=(%o)(%s)\n",
		Key2Ecode[idx].ecmd1,
	       getEkeyname(Key2Ecode[idx].ecmd1), c,
		Key2Ecode[idx].keyname);
	    return( (int) Key2Ecode[idx].ecmd1 );
	}
#ifdef OUT
	else {  /* more than one ecmd */
	    dbgpr("MapCursesKey, NEW returning (%o)(%s) for Fkey=(%o)(%s)\n",
		Key2Ecode[idx].ecmd1,
	       getEkeyname(Key2Ecode[idx].ecmd1), c,
		Key2Ecode[idx].keyname);

	    /* for some reason, using ungetch() didn't work in all cases */

	    /* first, ungetch() the remaining codes in reverse order */
	    for (i = Key2Ecode[idx].len - 2; i >= 0; i--) {
		ungetch( (int) Key2Ecode[idx].ecmdstr[i] );
		dbgpr("...ungetch (%o)(%c)\n",
		    Key2Ecode[idx].ecmdstr[i],
		    Key2Ecode[idx].ecmdstr[i] );
	    }
	}
#endif
       else {
	   /* A Fkey maps to more than one E cmd.  We return the
	    * 1st cmd and set a ptr to the rest so that mGetkey1()
	    * will input them after processing the 1st one.
	    */
	   FKey_p = Key2Ecode[idx].ecmdstr;
       }

       return( (int) Key2Ecode[idx].ecmd1 );
    }

/* old method */
#ifdef OUT

    /* User define KEY_Fn ? */

    /*
     * 0411 - 0424  (265-276):  KEY_F1 - KEY_F12
     * 0425 - 0440  (277-288):  SHIFT_F1 - SHIFT_F12
     * 0441 - 0454  (289-300):  CTRL_F1 - CTRL_F12
     */
    if (c >= 0411 && c <= 0454) {
	i = c - 0411;   /* i will be 0-11, 12-23, 24-35 */
    /*  if (UserFuncKeys[i].keycode >= 0) {  */
	if (UserFuncKeys[i].cmdval >= 0) {
	    return (UserFuncKeys[i].cmdval);
	}
    }
#endif  /* OUT */

#endif /* USER_FKEYS */

    switch( c ) {
	/* common keypad functions */

	case KEY_MOUSE:             /* mouse event */
	    return CCMOUSE;
	case KEY_ENTER:             /* enter/send key */
	    return CCRETURN;
	case KEY_BACKSPACE:         /* backspace key */
	    return CCBACKSPACE;
	case KEY_DC:                /* delete-character */
	    return CCDELCH;
	case KEY_BREAK:             /* Break key (unreliable) */
	    return CCCMD;
	case KEY_DOWN:              /* down-arrow key */
	    return CCMOVEDOWN;
	case KEY_UP:                /* up-arrow key */
	    return CCMOVEUP;
	case KEY_LEFT:              /* left arrow key */
	    return CCMOVELEFT;
	case KEY_RIGHT:             /* right arrow key */
	    return CCMOVERIGHT;
	case KEY_HOME:              /* home key */
	    return CCHOME;
	case KEY_NPAGE:             /* next-page key */
	    return CCPLPAGE;
	case KEY_PPAGE:             /* previous-page key */
	    return CCMIPAGE;
	case KEY_IC:                /* insert key */
	    return CCINSMODE;

#ifdef OUT
	case 0414:
       /*   dbgpr("MapCursesKey:  F4 pressed\n"); */
	    return NOCHAR;

	case 0415:
	/*  dbgpr("MapCursesKey:  F5 pressed\n"); */
	    return NOCHAR;

	case KEY_F(2):
	/*  dbgpr("MapCursesKey:  F2 pressed\n"); */
	    return KEY_F(2);
#endif /* OUT */

	case KEY_F(6):
	/*  dbgpr("MapCursesKey:  F6 pressed\n"); */
	/*  return KEY_F(6); */
	    return CCMOUSEONOFF;

	default:
	    return NOCHAR;  /* 0400, NOTE this conflicts with KEY_CODE_YES below */
			    /* but is unlikely to be any issue */
    }
    return NOCHAR;
}


/* debug routines, these require an #include of e.keys.h */
/*
 *   Lookup the name of an E command code
 *   eg, for 012 return "CCMOVEDOWN"
 */

char *
getEkeyname(int c) {

    unsigned int i;

    for(i=0; i < sizeof(E_Keys) / sizeof(E_Keys[0]); i++) {
       if( E_Keys[i].val == c )
	 return E_Keys[i].name;
    }
    return "NOT FOUND";
}

/*
 *   Lookup the name of a curses keycode
 *   eg, for 0402 return "KEY_DOWN"
 */

char *
getCursesKeyname (int c) {
    unsigned int i;

    if( c < KEY_MIN || c > KEY_MAX ) {
	return "NOT FOUND";
    }
    for(i=0; i < sizeof(Curses_Keys) / sizeof(Curses_Keys[0]); i++) {
       if( Curses_Keys[i].val == c )
	 return Curses_Keys[i].name;
    }

    return "NOT FOUND";
}


#ifdef OUT
void
dbg_winlist()
{
    int i;

    for (i = 0; i < nwinlist; i++) {
	dbgpr("winlist[%d]=(%o)\n", i, winlist[i]);
    }
    return;
}
#endif /* OUT */


#ifdef USER_FKEYS

#ifdef OUT
void addCursesFuncKey (char *);     /* old method */
#endif

void addCursesFkey (char *);
int CursesKeyValue(char *keyname);

#ifdef OUT
void
addCursesFuncKey(char *line)
{

    extern S_looktbl itsyms[];     /* see e.iit.c */

    int Fnum;           /* 1-12 for now */
    char ecmd[32];
    int i;
    int cmdval = -1;

    /* Offset into UserFuncKeys[]:
     *    0 for F1-F1, 12 for SHIFT_F1-F12, and 24 for CTRL_F1-F12
     */
    int key_offset;

    /* line eg, "KEY_F8:<+page>"  */

    int l_offset = 0;

    if (strstr(line, "KEY_")) {
	l_offset = 3;
	key_offset = 0;
    }
    else if (strstr(line, "SHIFT_")) {
	l_offset = 5;
	key_offset = 12;
    }
    else if (strstr(line, "CTRL_")) {
	l_offset = 4;
	key_offset = 24;
    }
    else {
	dbgpr("addCursesFKey: bad format: %s\n", line);
	return;
    }

    /* sscan the Fnumber, and ecmd:  omitting ending '>')  */
    if (sscanf (line + l_offset, "_F%d:<%[^>]", &Fnum, ecmd) != 2) {
	dbgpr("addCursesFKey: bad format: %s\n", line);
	return;
    }

    if (Fnum < 1 || Fnum > 12) {
	dbgpr("addCursesFuncKey:  FKey value (%d) not supported: 1-12\n", Fnum);
	return;
    }

    int idx = Fnum + key_offset - 1;

    for (i=0; itsyms[i].str != NULL; i++) {
    /*  dbgpr("cmp (%s) to (%s)\n", itsyms[i].str, ecmd); */
	if (strcmp(itsyms[i].str, ecmd) == 0) {
	    cmdval = (int) itsyms[i].val;
	    UserFuncKeys[idx].cmdval = cmdval;
	    break;
	}
    }


/** /
    dbgpr("line=(%s)\n", line);
    dbgpr("Fnum=%d ecmd=%s ECMD=%d idx=%d\n", Fnum, ecmd, cmdval, idx);
/ **/
    return;
}
#endif  /* OUT */

void
addCursesFkey (char *line ) {

    char keyname[64], *cp;
    int keycode, offset, idx, i;

    extern S_looktbl itsyms[];     /* see e.iit.c */

    if ((cp = index(line, ':')) == NULL) {
	getout (YES, "kbfile no colon found in %s\n", line);
    }
    offset = (int) (cp - line);
    strncpy (keyname, line, (size_t) offset);
    keyname[offset] = '\0';
/*  dbgpr("addCursesFkey:  keyname = (%s), value=(%s)\n", keyname, cp); */

    if ((keycode = CursesKeyValue(keyname)) == -1) {
	dbgpr("%s is not a recognized ncurses keyname.\n", keyname);
	getout (YES, "%s is not a valid ncurses keyname\n", keyname);
	return;
    }

    idx = keycode - KEY_MIN;
/*  dbgpr("keycode=%d %o idx=%d for key=(%s)\n", keycode, keycode, idx, keyname); */

    /*
     *  Punt if the the keycode is already defined
     */
    if (Key2Ecode[idx].keycode == keycode) {
	getout(YES, "kbfile error, key %s is already defined.", keyname);
    }

    Uchar ecmds[128];
    Uchar tmp[128], *tp;

    int cnt = 0;  /* number of ecmds[] entries */

    /*  Using a cnt to keep track of the number of ecmds because ecmds
     *  can contail entries of value=0 which is the E code for <cmd>.
     *  Had an interesting time figuring out why strlen() didn't work ...:)
     */

    /* parse line looking up <ecmd> values */
    for (cp++; *cp; cp++) {
	if (*cp == '<') {       /* eg, <+page> */
		/* The code in itparse allows "^B:<nnn>" where nnn is
		 * the octal code of an E function,  eg, <013> is <home>.
		 * This is not imlemented here.
		 */
	    tp = tmp;
	    cp++;
	    while (*cp && *cp != '>')
		*tp++ = (Uchar) *cp++;
	    *tp = '\0';
	    if (!*cp) {   /* '>' not found */
		dbgpr("bad kbfile entry, missing '>' in  (%s)\n", line);
		getout(YES, "kbfile entry missing '>' in line: %s", line);
		return;
	    }

	/*  dbgpr("addCursesFkey: looking up <%s>\n", tmp); */

	    int cmdval = -1;
	    for (i=0; itsyms[i].str != NULL; i++) {
		if (strcmp(itsyms[i].str, (char *)tmp) == 0) {
		/*
		    dbgpr(" -- itsyms: found (%s) matches (%s) at i=%d\n",
			tmp, itsyms[i].str, i);
		 */
		    cmdval = (int) itsyms[i].val;
		    break;
		}
	    }
	    if (cmdval == -1) {
		dbgpr("<%s> not found\n", tmp);
		getout(YES, "<%s> is not a valid e command in line: %s", tmp, line);
		return;
	    }
	/*  dbgpr("lookup, <%s> is ecmd %04o\n", tmp, cmdval); */

	    ecmds[cnt++] = (Uchar) cmdval;
	    ecmds[cnt] = '\0';
	/*  dbgpr("adding cmd=(%o) to ecmds[], len now=%d\n", cmdval, cnt); */
	}
	else if (*cp == '"') {  /* eg, KEY_F3:<cmd>"-er"<ret> */
	    tp = tmp;
	    cp++;
	    while (*cp && *cp != '"')
		*tp++ = (Uchar) *cp++;
	    *tp = '\0';

	    if (!*cp) {   /* " not found */
		dbgpr("bad kbfile entry, missing \" in  (%s)\n", line);
		getout(YES, "kbfile entry missing \" in line: %s", line);
		return;
	    }

	    /* copy the text to ecmds */
	    tp = tmp;
	    while (*tp) {
		ecmds[cnt++] = *tp++;
	    }
	    ecmds[cnt] = '\0';
	/*  dbgpr("adding text=(%s) to ecmds[], len now=%d\n", tmp, cnt); */
	}
	else if (*cp == '^') {  /* convert Ucase char to ctrl char, A -> ^A */
	    cp++;
	    if (*cp != '?' && (*cp < '@' || *cp > '_')) /* eg, A-Z */
		getout (YES, "kbfile: ^%c needs to be A-Z in %s\n", *cp, line);
	    /* ecmds[cnt++] = *cp^0100; */
	    ecmds[cnt++] = CCCTRLQUOTE;   /* 034 */
	    ecmds[cnt++] = (Uchar) *cp;
	    ecmds[cnt] = '\0';
	    dbgpr("added ^%c = (%o) to %s\n", *cp, ecmds[cnt-1], keyname);
	}
	else {  /* ?? */
	    dbgpr("bad kbfile entry, missing '<' or \" in  (%s)\n", line);
	    getout(YES, "kbfile entry does not start with '<' or \" in line: %s", line);
	    return;
	}
    }

#ifdef OUT
    /* dbg */
    dbgpr("len=%d, ecmds=", cnt);
    for (i=0; i <= cnt; i++)
	dbgpr("(%o)(%c),", ecmds[i], ecmds[i] );
    dbgpr("\n\n");
#endif

    strcpy(Key2Ecode[idx].keyname, keyname);    /* tmp for debugging */
    Key2Ecode[idx].keycode = keycode;  /* as a check if key already defined */
    Key2Ecode[idx].len = cnt;

    /* It is likely that the vast majority of F key mappings
     * will point to a single E function, so we might save
     * something by avoiding the calloc for a single byte.
     */
    Key2Ecode[idx].ecmd1 = ecmds[0];
    if (cnt > 1) {  /* copy remaining e codes to ecmdstr */
	Key2Ecode[idx].ecmdstr = calloc((size_t) cnt, sizeof(Uchar));
	for (i=0; i < cnt; i++)
	    Key2Ecode[idx].ecmdstr[i] = ecmds[i+1];
	Key2Ecode[idx].ecmdstr[i] = '\0';   /* !!! */

#ifdef OUT
	/* dbg */
	for (i=0; i < cnt - 1; i++)
	    dbgpr("ecmdstr[%d] = (%o)(%c)\n",i,
		Key2Ecode[idx].ecmdstr[i],
		isprint(Key2Ecode[idx].ecmdstr[i]) ? Key2Ecode[idx].ecmdstr[i] : ' ');
#endif
    }

    return;
}



int
CursesKeyValue(char *keyname) {
    int i;
    for (i=0; CursesKeyCodes[i].val != -1; i++) {
	if( strncmp(CursesKeyCodes[i].name, keyname, strlen(keyname)) == 0 ) {
	/*  dbgpr("CursesKeyValue:  (%s)=(%d)\n", keyname,  CursesKeyCodes[i].val); */
	    return CursesKeyCodes[i].val;
	}
    }
    dbgpr("CursesKeyValue:  (%s) not found\n", keyname);
    return -1;
}
#endif /* USER_FKEYS */
#endif /* NCURSES */

