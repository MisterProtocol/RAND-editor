#ifdef COMMENT
--------
file e.wk.c
    workspace manipulation code.
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.inf.h"

extern void switchfile ();
extern void exchgwksp ();
extern void savewksp ();
extern void releasewk ();
extern void inforange ();
extern void infoprange ();
extern void GetLine ();

#ifdef COMMENT
void
switchfile ()
.
    Switch to alternate wksp if there is one, else do error mesg.
#endif
void
switchfile ()
{
    if (curwin->altwksp->wfile == OLDLFILE)
	GetLine (-1);   /* so that the last line modified will show up */
    if (!swfile ())
	mesg (ERRALL + 1, "No alternate file");
    return;
}

#ifdef COMMENT
Flag
swfile ()
.
    Return NO if no alt wksp else switch to alt wksp and return YES.
#endif
Flag
swfile ()
{
    if (curwin->altwksp->wfile == NULLFILE)
	return NO;
    exchgwksp (NO);
    putupwin ();
    if (curwin->winflgs & TRACKSET)
	poscursor (curwin->altwksp->ccol, curwin->altwksp->clin);
    else
	poscursor (curwksp->ccol, curwksp->clin);
    return YES;
}

#ifdef COMMENT
void
exchgwksp ()
.
    Exchange wksp and altwksp.  If silent is set, do not turn on lights.
#endif
void
exchgwksp (silent)
Flag silent;
{
    register S_wksp *cwksp;

    savewksp (cwksp = curwksp);

    cwksp->ccol = cursorcol;
    cwksp->clin = cursorline;

    curwin->wksp    = curwin->altwksp;
    curwin->altwksp = cwksp;
    curwksp = curwin->wksp;
    if (!silent) inforange (curwksp->wkflags & RANGESET);

    curfile = curwksp->wfile;
    curlas = &curwksp->las;
    limitcursor ();

    return;
}

#ifdef COMMENT
void
savewksp (pwk)
    register S_wksp *pwk
.
    Save workspace position in in lastlook[pwk->wfile].
#endif
void
savewksp (pwk)
register S_wksp *pwk;
{
    register S_wksp *lwksp;

    if (curwksp == pwk) {
	pwk->ccol = cursorcol;
	pwk->clin = cursorline;
    }
    if (pwk->wfile == NULLFILE)
	return;
    lwksp = &lastlook[pwk->wfile];

    /* save where we are in current worksp */
    (void) la_align (&pwk->las, &lwksp->las);
    lwksp->wcol = pwk->wcol;
    lwksp->wlin = pwk->wlin;
    lwksp->ccol = pwk->ccol;
    lwksp->clin = pwk->clin;

    return;
}

#ifdef COMMENT
void
releasewk (wk)
    register S_wksp *wk;
.
    Release the file in workspace.
    La_close the range streams, if any, for workspace 'wk'.
#endif
void
releasewk (wk)
register S_wksp *wk;
{
    if (wk != NULL) {
	if (wk->wfile != NULLFILE) {
	    (void) la_close (&wk->las);
	    wk->wfile = NULLFILE;
	}
	wk->wkflags &= ~RANGESET;
	if (wk->brnglas) {
	    (void) la_close (wk->brnglas);
	    wk->brnglas = (La_stream *) 0;
	    (void) la_close (wk->ernglas);
	}
    }
    return;
}

static char *lastwhere = " ";

#ifdef COMMENT
void
inforange (onoff)
    Flag onoff;
.
    Update the RANGE display on the info line
#endif
void
inforange (onoff)
Reg1 Flag onoff;
{
    static Flag wason;

/*  if ((onoff = (onoff ? YES : NO) ^ wason)) { */
    if ((onoff = onoff ? YES : NO) ^ wason) {
	if ((wason = onoff))
	    info (inf_range + 1, 5, "RANGE");
	else {
	    info (inf_range, 6, "");
	    lastwhere = " ";
	}
    }
    return;
}

#ifdef COMMENT
void
infoprange (line)
    Nlines line;
.
    Update the position portion of the RANGE display on the info line
#endif
void
infoprange (line)
Reg2 Nlines line;
{
    Reg1 char *where;

    if (line < la_lseek (curwksp->brnglas, 0, 1))
	where = "<";
/*  else if (line > la_lseek (curwksp->ernglas, 0, 1)) */
    else if (line >= la_lseek (curwksp->ernglas, 0, 1))
	where = ">";
    else
	where = "=";
    if (   *lastwhere == ' '
	|| *where != *lastwhere
       ) {
	info (inf_range, 1, where);
	lastwhere = where;
    }
    return;
}
