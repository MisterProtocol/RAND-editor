#ifdef COMMENT
--------
file e.mac.c
    define and use macros

    The only part of the macro code not here is buried in the depths of
    getkey1 (e.t.c), and a little weensy bit in mesg, and the save/restore
    stuff in e.c and e.q.c.
#endif

#include "e.h"
#include "e.cm.h"
#include "e.m.h"

#ifdef LMCMACROS

#ifdef COMMENT
Cmdret
domacrodef()
.
    Very simple macro implementation; no parameters, no recursion, no
    multiple macros.  Simple things for simple people.
#endif

Cmdret
domacrodef()
{
    int key, svcnt;
    Uchar *cp;

    if (macroing)
	return NORECURSION;
    macroing = nokeyfile = YES;
    if (macrodefined)
	sfree (macroline);
    macroline = (Uchar *) salloc (100, NO);
    macrosize = 100;
    cp = macroline;
    keyused = YES;
    savecurs();
    svcnt = 1;
    mesg (TELSTRT|TELCLR + 1, "MACRO: ");
    while ((key = getkey (WAIT_KEY)) != CCINT) {
	if (key != CCNULL && key != CCUNAS1) {
	    if (key == CCDEL) {
		if (cp > macroline) {
		    cp--;
		    restcurs();
		    svcnt--;
		    mesg (TELCLR);
		}
	    } else {
		if (cp - macroline >= macrosize) {
		    int i;

		    macroptr = (Uchar *) salloc (macrosize + 50, NO);
		    for (i = 0; i < macrosize; i++)
			macroptr[i] = macroline[i];
		    sfree (macroline);
		    i = cp - macroline;
		    macroline = macroptr;
		    cp = macroline + i;
		    macrosize += 50;
		}
		*cp++ = key;
		savecurs();
		svcnt++;
		if (key >= 040 && key < 0177) {
		    char sbuf[3];
		    sprintf (sbuf, "%c", key);
		    mesg (TELCLR + 1, sbuf);
		} else
		    mesg (TELCLR + 3, "<",
			keycaps[key < 040 ? key : key - 0177 + 040], ">");
	    }
	}
	keyused = YES;
    }
    for (; svcnt; svcnt--)
	burncurs();
    *cp = CCNULL;
    macrolen = cp - macroline + 1;
    mesg (TELDONE);
    loopflags.hold = YES;
    macrodefined = YES;
    macroing = nokeyfile = NO;
    return (CROK);
}

Cmdret
undomacrodef()
{
	macrodefined = NO;
	sfree (macroline);
	return CROK;
}

Cmdret
usemacro (n)
	int n;
{
	if (!macrodefined) {
		mesg (ERRALL + 1, "No macro defined");
		return CROK;
	}
	if (n <= 0) n = 1;
	macroiter = macrocount = n;
	using = YES;
	breakout = NO;
	macroptr = macroline;
	return CROK;
}

#endif /* LMCMACROS */
