/*
	Copyright (c) 1985, 1986 David Yost     All Rights Reserved
	Copyright (c) 1986 Grand Software, Inc. All Rights Reserved
	THIS FILE CONTAINS UNPUBLISHED PROPRIETARY SOURCE CODE
	which is property of Grand Software, Inc.
	Los Angeles, CA 90046 U.S.A 213-650-1089
	The copyright notice above does not evidence any
	actual or intended publication of such source code.
	This file is not to be copied by anyone except as
	covered by written agreement with Grand Software, Inc.,
	and this notice is not to be removed.
 */


#include "lalocal.h"
#include <errno.h>
extern int errno;

#include <signal.h>

/* VARARGS 5 */
La_linepos
la_lflush (plas, start, nlines, chan, oktoint, timeout)
La_stream *plas;
La_linepos start;
La_linepos nlines;
int chan;
int oktoint;
Reg6 unsigned int timeout;
{
    Reg5 La_stream *tlas;
    char buf[BUFSIZ];
    La_linepos totlines;
    La_stream llas;     /* can't be in a block */
    int (*pipesig) ();
    int (*alarmsig) ();
    extern int la_lflalarm ();

    if (nlines <= 0)
	return 0;

    if ((tlas = la_clone (plas, &llas)) == (La_stream *) 0)
	return 0;

    pipesig = signal (SIGPIPE, SIG_IGN);
    totlines = 0;
    (void) la_lseek (tlas, start, 0);
    if (!oktoint)
	timeout = 0;
    else if (timeout) {
	alarm (0);
	alarmsig = signal (SIGALRM, la_lflalarm);
    }
    while (nlines > 0) {
	Reg2 int lcnt;
	Reg3 int cnt;
	Reg4 char *wcp;

	cnt = 0;
	for (lcnt = 0; lcnt < nlines; ) {
	    Reg1 int nget;
	    int ngot;

	    if ((nget = la_lrsize (tlas)) <= BUFSIZ - cnt)
		lcnt++;
	    else
		nget = BUFSIZ - cnt;
	    if ((ngot = la_lget (tlas, &buf[cnt], nget)) != nget) {
		if (ngot != -1)
		    la_errno = LA_GETERR;
		totlines = -1;
		goto ret;
	    }
	    if ((cnt += nget) >= BUFSIZ)
		break;
	}
	wcp = buf;
	for (;;) {
	    Reg1 int nwr;

	    if (oktoint && la_int ()) {
		la_errno = LA_INT;
		goto ret;
	    }
	    if (timeout)
		alarm (timeout);
	    nwr = write (chan, wcp, cnt);
	    if (timeout)
		alarm (0);
	    if (nwr == cnt)
		break;
	    else {
		if (!timeout || errno != EINTR) {
		    la_errno = LA_WRTERR;
		    goto ret;
		}
	    }
	    if (nwr > 0) {
		cnt -= nwr;
		wcp += nwr;
	    }
	}
	nlines -= lcnt;
	totlines += lcnt;
    }
 ret:
    if (timeout)
	signal (SIGALRM, alarmsig);
    signal (SIGPIPE, pipesig);
    (void) la_close (tlas);
    return totlines;
}

la_lflalarm ()
{
    signal (SIGALRM, la_lflalarm);
    return;
}
