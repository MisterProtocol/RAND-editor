#include "lalocal.h"
#include <errno.h>
#include <unistd.h>
extern int errno;
unsigned alarm ();

#include <signal.h>

#include "la_prototypes.h"

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
#ifdef GCC
    void (*pipesig) ();
    void (*alarmsig) ();
    extern void la_lflalarm ();
#else
    int (*pipesig) ();
    int (*alarmsig) ();
    extern int la_lflalarm ();
#endif /* GCC */

    if (nlines <= 0)
	return (0);

    if ((tlas = la_clone (plas, &llas)) == (La_stream *) 0)
	return (0);

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
	    nwr = write (chan, wcp, (size_t)cnt);
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
    return (totlines);
}

/* #ifdef GCC */
void
/* #endif / * GCC */
la_lflalarm (const int i __attribute__((unused)))
{
    /* i++;    / * keep compiler happy; we don't care about the mandatory arg */
    signal (SIGALRM, la_lflalarm);
    return;
}
