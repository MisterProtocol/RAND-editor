/*
	Copyright (c) 1985, 1986 David Yost     All Rights Reserved
	Copyright (c) 1986, 1987 Grand Software, Inc. All Rights Reserved
	THIS FILE CONTAINS UNPUBLISHED PROPRIETARY SOURCE CODE
	which is property of Grand Software, Inc.
	Los Angeles, CA 90046 U.S.A 213-650-1089
	The copyright notice above does not evidence any
	actual or intended publication of such source code.
	This file is not to be copied by anyone except as
	covered by written agreement with Grand Software, Inc.,
	and this notice is not to be removed.
 */

#include "lalocal.hxx"

char *la_cfile = "changes.tmp";

#define OUT_FILE "test.out"

#define MAXLINE 512

#define signal Csignal
#include <signal.h>
#undef  signal
typedef int (*sigfunc_t) (int signal);
extern sigfunc_t signal (int signum, sigfunc_t catch);
extern int sig (int signum);

char *ttyname ();
extern int getdtablesize ();
extern int creat (char* filename, int mode);

extern int deletetest (int argc, char** argv);

int showdebug = 0;

int
main (
    int argc,
    char** argv
) {
    char test;
    int ind;

    if (argc-- <= 1) {
	    printf ("Usage: %s <test> <args> ...\n", argv[0]);
	    exit (1);
    }

    for (ind = 1; ind <= NSIG; ++ind) {
	if (ind == ABORT_SIG)
	    {}  /* do nothing */
	else
	    signal (ind, sig);
    }

    test = argv[1][0];
    for (ind = 1; ind < argc; ind++)
	argv[ind] = argv[ind + 1];

#ifdef	SYSGETDTABLESIZE
    la_maxchans = getdtablesize ();
#else /*SYSGETDTABLESIZE */
    la_maxchans = _NFILE;
#endif/*SYSGETDTABLESIZE */
    switch (test) {
    case 'd':
	return deletetest (argc, argv);

#ifdef  NOTYET
    case 'u':
	return collecttest (argc, argv);

    case 'i':
	return inserttest (argc, argv);

    case 'g':
	return gettest (argc, argv);

    case 'c':
	return clonetest (argc, argv);

    case 'p':
	return picktest (argc, argv);

    case 'P':
	return pointtest (argc, argv);

    case 's':
	return seektest (argc, argv);

    case 'b':
	return brktest (argc, argv);
#endif/*NOTYET */
    }
    return 0;
}

#ifdef  NOTYET

seektest (argc, argv)
int argc;
char *argv[];
{
    La_stream *tlas1;
    La_stream slas1;
    register int j;

    if (argc < 4){
	    printf ("Usage: %s s file [line# seektype]...\n", argv[0]);
	    exit (1);
    }
    if ((tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    la_fsddump (tlas1->la_file->la_ffsd, tlas1->la_file->la_lfsd, YES, "");
    for (j = 2; j < argc; j += 2) {
	(void) tlas1->la_lseek ((La_linepos) atoi (argv[j]), atoi (argv[j + 1]));
	tlas1->la_sdump ("");
    }
    la_fdump (tlas1->la_file, "");
 /* i = creat (OUT_FILE, 0644);
    tlas1->la_lflush ((La_linepos) 0, tlas1->la_lsize (), i, NO);
 */ printf ("%d\n", (int) tlas1->la_close ());
    return 0;
}

brktest (argc, argv)
int argc;
char *argv[];
{
    La_stream *tlas1;
    La_stream slas1;
    La_fsd *ffsd, *lfsd, *n1fsd, *n2fsd;
    La_linepos line;
    La_linepos nlines;
    La_bytepos nbytes;

    if (argc != 4){
	printf ("Usage: %s b file line# nlines\n", argv[0]);
	exit (1);
    }
    line = atoi (argv[2]);
    nlines = atoi (argv[3]);

    if ((tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    (void) la_lseek (tlas1, line, 0);
    ffsd = tlas1->la_cfsd;
    (void) la_lseek (tlas1, line + nlines, 0);
    lfsd = tlas1->la_cfsd;
    (void) la_lseek (tlas1, line, 0);
    la_fsddump (ffsd, lfsd, YES, "");
    la_sdump (tlas1, "");

#ifdef LA_BP
    la_break (tlas1, BRK_REAL, &n1fsd, &n2fsd, &nlines, &nbytes);
#else
    la_break (tlas1, BRK_REAL, &n1fsd, &n2fsd, &nlines);
    nbytes = 0;
#endif
    printf ("Break results: lines=%ld, bytes=%ld, n1fsd=0x%04x, n2fsd=0x%04x\n",
	    nlines, nbytes, n1fsd, n2fsd);
    if ((ffsd = tlas1->la_cfsd)->fsdback)
	ffsd = ffsd->fsdback;
    (void) la_lseek (tlas1, line + nlines, 0);
    la_fsddump (ffsd, n2fsd, YES, "");
    (void) la_lseek (tlas1, (long) 0, 0);
    (void) la_lseek (tlas1, line, 0);
    la_sdump (tlas1, "");

 /* la_fdump (tlas1->la_file, "");
    i = creat (OUT_FILE, 0644);
    la_lflush (tlas1, (La_linepos) 0, la_lsize (tlas1), i, NO);
 */ printf ("%d\n", (int) la_close (tlas1));
    return 0;
}

clonetest (argc, argv)
int argc;
char *argv[];
{
    La_stream *tlas1, *tlas2;
    La_stream slas1, slas2;
    register int j;

    if (argc < 4){
	    printf ("Usage: %s c file [line# seektype]...\n", argv[0]);
	    exit (1);
    }
    if ((tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    la_fsddump (tlas1->la_file->la_ffsd, tlas1->la_file->la_lfsd, YES, "");
    for (j = 2; j < argc; j += 2) {
	(void) la_lseek (tlas1, (La_linepos) atoi (argv[j]), atoi (argv[j + 1]));
	la_sdump (tlas1, "");
	tlas2 = la_clone (tlas1, &slas2);
	la_sdump (tlas2, "");
	la_close (tlas2);
    }
 /* la_fdump (tlas1->la_file, "");
    i = creat (OUT_FILE, 0644);
    la_lflush (tlas1, (La_linepos) 0, la_lsize (tlas1), i, NO);
 */ printf ("%d\n", (int) la_close (tlas1));
    return 0;
}

inserttest (argc, argv)
int argc;
char *argv[];
{
    register char *cp;
    La_stream *tlas1;
    La_stream slas1;
    La_linepos line;
    char buf[28];

    if (argc != 4 && argc != 5){
	printf ("Usage: %s i file line# string\n", argv[0]);
	exit (1);
    }
    line = atoi (argv[2]);

    if ((tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    (void) la_lseek (tlas1, line, 0);
    la_fsddump (tlas1->la_file->la_ffsd, tlas1->la_file->la_lfsd, YES, "");
    la_sdump (tlas1, "");
    la_fdump (tlas1->la_file, "");

    move (argv[3], buf, (unsigned int) sizeof buf);
    for (cp = buf; *cp; cp++)
	continue;
    *cp++ = '\n';

    if (la_linsert (tlas1, buf, cp - buf) == -1) {
	printf ("Insert result: %ld\n", la_errno);
	exit (0);
    }

    (void) la_lseek (tlas1, line, 0);
    la_fsddump (tlas1->la_file->la_ffsd, tlas1->la_file->la_lfsd, YES, "");
    la_sdump (tlas1, "");
    la_fdump (tlas1->la_file, "");

    la_lflush (tlas1, (La_linepos) 0, la_lsize (tlas1), 1, NO);
    printf ("%d\n", (int) la_close (tlas1));
    return 0;
}

collecttest (argc, argv)
int argc;
char *argv[];
{
    La_stream *tlas1, *tlas2;
    La_stream slas1, slas2;
    La_linepos line;
    La_linepos ndel;
    int nbytes;
    La_linepos insret;
    int collect;

    if (argc != 4){
	printf ("Usage: %s u file line# insfile\n", argv[0]);
	exit (1);
    }
    line = atoi (argv[2]);

    if ((tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    if ((tlas2 = la_open (argv[3], "", &slas2, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    (void) la_lseek (tlas1, line, 0);
    la_fsddump (tlas1->la_file->la_ffsd, tlas1->la_file->la_lfsd, YES, "");
    la_sdump (tlas1, "");
    la_fdump (tlas1->la_file, "");

    collect = 1;
    for (;;) {
	char buf[MAXLINE];
	if ((nbytes = la_lget (tlas2, buf, MAXLINE)) <= 0)
	    break;
	if ((insret = la_lcollect (collect, buf, nbytes)) == -1) {
	    printf ("Insert result: %ld\n", la_errno);
	    exit (0);
	}
	collect = 2;
    }
    ndel = 0;
    if ((insret = la_lrcollect (tlas1, &ndel, tlas2)) == -1) {
	printf ("Insert result: %ld\n", la_errno);
	exit (0);
    }
    printf ("Inserted: %ld\n", insret);

    (void) la_lseek (tlas1, line, 0);
    la_fsddump (tlas1->la_file->la_ffsd, tlas1->la_file->la_lfsd, YES, "");
    la_sdump (tlas1, "");
    la_fdump (tlas1->la_file, "");

    la_lflush (tlas1, (La_linepos) 0, la_lsize (tlas1), 1, NO);
    printf ("%d\n", (int) la_close (tlas1));
    return 0;
}
#endif/*NOTYET */

int
deletetest (int argc, char** argv)
{
    La_stream *tlas1, *tlas2;
    La_stream slas1, slas2;
    La_fsd *ffsd, *lfsd;
    La_linepos line;
    La_linepos nlines;

    if (argc != 4 && argc != 5){
	fprintf (stderr, "Usage: %s d file line# nlines [save]\n", argv[0]);
	exit (1);
    }
    line = atoi (argv[2]);
    nlines = atoi (argv[3]);

    if ((tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL) {
	fprintf (stderr, "Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    if ((tlas2 = la_open ("", "n", &slas2, (La_bytepos) 0)) == NULL) {
	fprintf (stderr, "Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    (void) tlas1->la_lseek (line, 0);
    ffsd = tlas1->la_cfsd;
    (void) tlas1->la_lseek (line + nlines, 0);
    lfsd = tlas1->la_cfsd;
    (void) tlas1->la_lseek (line, 0);
    if (showdebug) {
	la_fsddump (ffsd, lfsd, YES, "");
	tlas1->la_sdump ("tlas1");
    }

    nlines = tlas1->la_ldelete (nlines, argc == 5? tlas2 : 0);
    fprintf (stderr, "Delete results: lines=%ld\n", nlines);
    if ((ffsd = lfsd = tlas1->la_cfsd)->fsdback)
	ffsd = ffsd->fsdback;
    if (showdebug) {
	la_fsddump (ffsd, lfsd, YES, "");
	tlas1->la_sdump ("tlas1");
    }

    if (argc == 5) {
	la_fsddump (tlas2->la_file->la_ffsd, tlas2->la_file->la_lfsd, YES,
		    "tlas2");
	tlas2->la_sdump ("tlas2");
    }
    if (showdebug)
	la_fdump (tlas1->la_file, "tlas1");
    if (argc == 5)
	la_fdump (tlas2->la_file, "tlas2");

    fprintf (stderr, "Flushed: %ld\n",
	     tlas1->la_lflush ((La_linepos) 0, tlas1->la_lsize (), 1, NO));

    fprintf (stderr, "Close returned: %d\n", (int) tlas1->la_close ());
    return 0;
}

#ifdef NOTYET

picktest (argc, argv)
int argc;
char *argv[];
{
    La_stream *tlas1, *tlas2, *tlas3;
    La_stream slas1, slas2, slas3;
    La_fsd *ffsd, *lfsd;
    La_linepos line;
    La_linepos nlines;

    if (argc != 4 && argc != 5){
	printf ("Usage: %s p file line# nlines [stay]\n", argv[0]);
	exit (1);
    }
    line = atoi (argv[2]);
    nlines = atoi (argv[3]);

    if (   (tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL
	|| (tlas3 = la_open ("", "n", &slas3, (La_bytepos) 0)) == NULL
	|| (tlas2 = la_clone (&slas3, &slas2)) == NULL
       ) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    if (argc == 5)
	la_stayset (tlas2);
    (void) la_lseek (tlas1, line, 0);
    ffsd = tlas1->la_cfsd;
    (void) la_lseek (tlas1, line + nlines, 0);
    lfsd = tlas1->la_cfsd;
    (void) la_lseek (tlas1, line, 0);
    la_fsddump (ffsd, lfsd, YES, "");
    la_sdump (tlas1, "tlas1");
    la_fdump (tlas1->la_file, "tlas1");

    nlines = la_lcopy (tlas1, tlas3, nlines);
    printf ("Copy results: lines=%ld\n", nlines);

    la_fsddump (tlas2->la_file->la_ffsd, tlas2->la_file->la_lfsd, YES, "");
    la_sdump (tlas2, "tlas2");
    la_sdump (tlas3, "tlas3");

    la_fdump (tlas1->la_file, "tlas1");
    la_fdump (tlas2->la_file, "tlas2");

    {
    int i;

    i = creat (OUT_FILE, 0644);
    printf ("Flushed: %ld\n",
	     la_lflush (tlas2, (La_linepos) 0, la_lsize (tlas2), i, NO));
    }
    return 0;
}

pointtest (argc, argv)
int argc;
char *argv[];
{
    La_stream *tlas;
    La_stream slas;
    La_linepos line;
    La_linepos nlines;

    if (argc != 3 && argc != 4) {
	printf ("Usage: %s P file line# [nlines]\n", argv[0]);
	exit (1);
    }
    line = atoi (argv[2]);
    if (argc == 4)
	nlines = atoi (argv[3]);
    else
	nlines = 1;

    if ((tlas = la_open (argv[1], "", &slas, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }

    (void) la_lseek (tlas, line, 0);

    {
	int count;
	char *cp;

	count = la_lpnt(tlas, &cp);
	write(1, cp, count);
    }
    return 0;
}

int
gettest (
    int argc,
    char *argv[]
)
{
    La_stream *tlas1;
    La_stream slas1;
    register int j;
    int lbyte;
    int nbytes;
    char buf[MAXLINE];

    if (argc < 4){
	    printf ("Usage: %s g file [line# lbyte #bytes]...\n", argv[0]);
	    exit (1);
    }
    if ((tlas1 = la_open (argv[1], "", &slas1, (La_bytepos) 0)) == NULL) {
	printf ("Open failed: la_errno = %d\n", la_errno);
	exit (0);
    }
    la_fsddump (tlas1->la_file->la_ffsd, tlas1->la_file->la_lfsd, YES, "");
    for (j = 2; j < argc; j += 3) {
	(void) la_lseek (tlas1, (La_linepos) atoi (argv[j]), 0);
	tlas1->la_lbyte = lbyte = atoi (argv[j + 1]);
	tlas1->la_ffpos += lbyte;
	tlas1->la_bpos += lbyte;
	if ((nbytes = atoi (argv[j + 2])) > MAXLINE) {
	    printf ("%d > %d.  Abort.\n", nbytes, MAXLINE);
	    break;
	}
	la_sdump (tlas1, "");
	nbytes = la_lget (tlas1, buf, nbytes);
	printf ("%d: %*.*s\n", nbytes, nbytes, nbytes, buf);
	la_sdump (tlas1, "");
    }
 /* la_fdump (tlas1->la_file, "");
    i = creat (OUT_FILE, 0644);
    la_lflush (tlas1, (La_linepos) 0, la_lsize (tlas1), i, NO);
 */ printf ("%d\n", (int) la_close (tlas1));
    return 0;
}

#endif/*NOTYET */

int
sig (int signum)
{
    abort (0);
    /* NOTREACHED */
    return 0;
}

extern void dodbgpr (
    char** fmt
);
extern void dopr (
    FILE *iop,
    char **fmt
);

#ifdef COMMENT
void
dbgpr (
    char *fmt
)
.
#endif
/* VARARGS1 */
void
dbgpr (
    char *fmt
)
{
    dodbgpr (&fmt);
    return;
}

#ifdef COMMENT
void
dodbgpr (
    char** fmt
)
.
    Invoked by dbgpr to do the work.
#endif
/* VARARGS1 */
void
dodbgpr (
    char** fmt
)
{
    dopr (stderr, fmt);
    fflush (stderr);
    return;
}

#ifdef COMMENT
void
dopr (iop, fmt)
    FILE *iop;
    char **fmt;
.
    Do limited printf from a pointer to an arg list.
    Assumes that char pointers and ints are the same size.
#endif
void
dopr (
    FILE *iop,
    char **fmt
)
{
    char *fmtptr;
    Reg2 char chr;
    Reg3 int *arg;
    static char prfmt[] = "%lxxxxxxxx";

    fmtptr = *fmt;
    arg = (int *) &fmt[1];

    for (;;) {
	Block {
	    Reg1 char *start;
	    start = fmtptr;
	    while ((chr = *fmtptr++) && chr != '%')
		continue;
	    fwrite (start, fmtptr - start - 1, 1, iop);
	}
	if (chr == '\0')
	    break;
	prfmt[1] = chr = *fmtptr++;
	prfmt[2] = '\0';
	switch (chr) {
	case 'l':
	    prfmt[2] = *fmtptr++;
	    prfmt[3] = '\0';
#ifdef  INT2
	    Block {
		long *lptr;
		lptr = (long *) arg;
		fprintf (iop, prfmt, *lptr++);
		arg = (int *) lptr;
	    }
	    break;
#endif/*INT2 */
	case 'c':
	case 'd':
	case 'o':
	case 's':
	case 'x':
	    fprintf (iop, prfmt, *arg++);
	    break;

	default:
	    fprintf (iop, prfmt);
	    break;
	}
    }
    return;
}
