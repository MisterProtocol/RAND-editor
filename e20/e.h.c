#ifdef COMMENT
--------
file e.h.c
    support  standard help functions.
#endif

# include "e.h"
#ifdef LMCHELP
# include "e.cm.h"
# include "e.m.h"
# include "e.tt.h"
# include "e.h.h"

Short morehelp(unsigned Short);
Cmdret help_std(char *);

#ifdef COMMENT
Cmdret
help_std(filename)
	char *filename;
.
	Standard help display driver.
#endif
Cmdret
help_std(filename)

	char *filename;
{
	FILE *helpin, *fopen();
	int c;
	char *fgets();
	unsigned Short qq;
	static unsigned char boxtab[11] = {
		BLCMCH, TLCMCH, TRCMCH, BRCMCH, BULCHAR,
		LMCH, TMCH, MLMCH, MRMCH, BTMCH, TTMCH };
	/* Notice the assumptions I'm making w.r.t. the window characters
	   defined in e.t.h. I assume that LMCH = RMCH, TMCH = BMCH,
	   and that MLMCH, MRMCH, BTMCH, and TTMCH are, respectively,
	   the right join, left join, top join, and bottom join.
	*/

	if (*nxtop)
		return CRTOOMANYARGS;
	savecurs();
	MCLEAR;
	MHOME;

	/* Keep track of how many lines are read from the keyboard layout
	 * and move the cursor past the last text.
	 */

	int nl = 0;

	if ((helpin = fopen (filename, "r")) != NULL) {
		while ((c = fgetc (helpin)) != EOF) {
			if (c == '\n') nl++;  /* trw */
			if (c == HELP_GS) {
				while ((c = fgetc (helpin)) != HELP_GE) {
					if (c == '\n') nl++;  /* trw */
					if (c != EOF) {
						if (c >= HELP_CH && c <= HELP_CX)
							MXLATE (boxtab [c - HELP_CH]);
						else
							MXLATE (c);
						if(c == '\n')
							MXLATE ('\r');
					} else {
						MXLATE (' ');
						break;
					}
				}
			} else if (c == HELP_SS && term.tt_so)
				MSSO;
			else if (c == HELP_SE && term.tt_soe)
				MSSE;
			else
				MXLATE (c);
			if(c == '\n')
				MXLATE ('\r');
		}
		fclose (helpin);
	}
	printf ("\n\rFor info on a particular key, press that combination;");
	printf ("\n\rto continue edit, press RETURN. ");

	nl += 3;
	poscursor(0, nl);
	fflush(stdout);

/*dbgpr("--end of show keyboard, nl=%d\n", nl);*/

wait:   keyused = YES;
#ifdef NCURSES
	qq = mGetkey (WAIT_KEY, NULL);
#else
	qq = getkey (WAIT_KEY, NULL);
#endif
	if (morehelp(qq) == YES) goto wait;
	restcurs();
	mesg(TELALL+1, " ");
	fflush(stdout);
	poscursor(cursorcol, cursorline+1);

	return CROK ;
}

#ifdef COMMENT
Short
morehelp (key)
	unsigned Short key;
.
    Displays a message describing the effects of the key whose lexed
    value is passed in "key".
#endif
Short
morehelp (key)
	unsigned Short key;
{
	FILE *helpin, *fopen();
	int go, atoi(), n;
	char buf [200], helpfile[200];

	fputs ("\r\n", stdout);
	sprintf (helpfile, "%s/helpkey", etcdir);
	if ((helpin = fopen (helpfile, "r")) == NULL) {
		printf ("Can't open %s\n\r", helpfile);
#ifdef NCURSES
		mGetkey (WAIT_KEY, NULL);
#else
		getkey (WAIT_KEY);
#endif
		return (NO);
	}
	else {
		go = NO;
		if (key != CCRETURN) {
			while (fgets (buf, sizeof buf, helpin) != NULL) {
				if (go == YES) {
					if (buf [0] == '~')
						break;
					fputs (buf, stdout);
					fputc ('\r', stdout);
				}
				else
					if (buf [0] == '~')
						if ((n = atoi (buf + 1)) == (int) key)
							go = YES;
			}
			fclose (helpin);
			fflush (stdout);
		}
		return (go);
	}
}

#endif /* LMCHELP */
