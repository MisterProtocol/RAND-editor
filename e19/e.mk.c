#ifdef COMMENT
--------
file e.p.c
    cursor marking
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.inf.h"
#include "e.m.h"

extern void markprev ();

#ifdef COMMENT
void
mark ()
.
    Mark this spot.
#endif
void
mark ()
{
    if (prevmark == 0)
	info (inf_mark, 4, "MARK");
    if (curmark)
	loopflags.flash = YES;
    markprev ();
    exchmark (YES);
    return;
}

#ifdef COMMENT
void
unmark ()
.
    Remove marks, if any.
#endif
void
unmark ()
{
    if (curmark) {
	info (inf_mark, 4, "");
	info (inf_area, infoarealen, "");
    }
    curmark = prevmark = 0;
    infoarealen = 0;
    marklines = 0;
    markcols = 0;
    mklinstr[0] = 0;
    mkcolstr[0] = 0;
    return;
}

#ifdef COMMENT
Nlines
topmark ()
.
    Return the top line of a marked area.
#endif
Nlines
topmark ()
{
    return min (curwksp->wlin + cursorline,
		curmark->mrkwinlin + curmark->mrklin);
}

#ifdef COMMENT
Ncols
leftmark ()
.
    Return the leftmost column of a marked area.
#endif
Ncols
leftmark ()
{
    return min (curwksp->wcol + cursorcol,
		curmark->mrkwincol + curmark->mrkcol);
}

#ifdef COMMENT
Flag
gtumark (leftflg)
    Flag leftflg;
.
    Go to upper mark.
    if leftflg is YES, go to upper left corner
    else if lines must be exchanged, then exchange columns also
    else do not exchange columns
#endif
Flag
gtumark (leftflg)
Flag leftflg;
{
    register Short tmp;
    Flag exchlines = NO;
    Flag exchcols  = NO;

    /* curmark is the OTHER corner */
    markprev ();

    if (    curmark->mrkwinlin + curmark ->mrklin
	 < prevmark->mrkwinlin + prevmark->mrklin
       )
	exchlines = YES;
    if (   (   leftflg
	    &&    curmark->mrkwincol + curmark ->mrkcol
	       < prevmark->mrkwincol + prevmark->mrkcol
	   )
	|| (!leftflg && exchlines)
       )
	exchcols = YES;
    if (exchlines || exchcols) {
	if (!exchcols) {
	    /* exchange the cols so mark will set them back */
	    tmp = curmark->mrkwincol;
	    curmark->mrkwincol = prevmark->mrkwincol;
	    prevmark->mrkwincol = tmp;
	    tmp = curmark->mrkcol;
	    curmark->mrkcol = prevmark->mrkcol;
	    prevmark->mrkcol = tmp;
	}
	else if (!exchlines) {
	    /* exchange the lines so mark will set them back */
	    tmp = curmark->mrkwinlin;
	    curmark->mrkwinlin = prevmark->mrkwinlin;
	    prevmark->mrkwinlin = tmp;
	    tmp = curmark->mrklin;
	    curmark->mrklin = prevmark->mrklin;
	    prevmark->mrklin = tmp;
	}
	return exchmark (NO) & WINMOVED;
    }
    return 0;
}

#ifdef COMMENT
Small
exchmark (puflg)
    Flag puflg;
.
    Exchange the two marked positions.
#endif
Small
exchmark (puflg)
Flag puflg;
{
    register struct markenv *tmp;
    Short retval = 0;

    if (curmark)
	retval = movewin (curmark->mrkwinlin,
			   curmark->mrkwincol,
			    curmark->mrklin,
			     curmark->mrkcol, puflg);
    tmp = curmark;
    curmark = prevmark;
    prevmark = tmp;
    return retval;
}

#ifdef COMMENT
void
markprev ()
.
    Copy the current position into the prevmark structure.
#endif
void
markprev ()
{
    static struct markenv mk1, mk2;

    if (prevmark == 0)
	prevmark = curmark == &mk1 ? &mk2 : &mk1;
    prevmark->mrkwincol = curwksp->wcol;
    prevmark->mrkwinlin = curwksp->wlin;
    prevmark->mrkcol = cursorcol;
    prevmark->mrklin = cursorline;
    return;
}
