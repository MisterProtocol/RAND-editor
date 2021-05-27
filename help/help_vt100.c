# include "e.h"
# include "e.m.h"
# include "e.cm.h"
# include "e.tt.h"

Cmdret
help_vt100(filename)

	char *filename;
{
	FILE *helpin, *fopen();
	int strcmp(), strlen(), c, vtq;
	char *fgets(), *getenv(), *x;
	Short morehelp(), qq;

	/* filename contains "/etc/e/k?.name". We will use v?.name, instead.

	filename[7] = 'v';
	helpin = fopen (filename, "r");

	/* since the vt100 has this nifty 132 column mode, we'll use
	   it (if we're not in it allready) so that a nice picture of
	   the keyboard will fit.  */

	x = getenv ("TERM");
	if (vtq = (x[strlen(x)-1] != 'w')) {
		fwrite ("\033[?3h", 5, 1, stdout);
		delay (150);
	}
	(*term.tt_clear) ();
	(*term.tt_home) ();
	if (helpin == NULL)
		printf ("Cannot open the help file: %s.\n\r", filename);
	else {
		while ((c = fgetc (helpin)) != EOF) {
			fputc ((char)c, stdout);
			if(c == '\n')
				fputc ('\r', stdout);
		}
		fclose (helpin);
	}
	printf ("\n\rFor info on a particular key, press that combination;");
	printf ("\n\rto continue edit, press RETURN. ");
	fflush (stdout);

	/* Now get back into raw mode so that getkey will work properly,
	   and return a lexed token (from the list "CC*" in e.h).  */

wait:   keyused = YES;
	qq = getkey (WAIT_KEY);
	if (morehelp(qq) == YES) goto wait;
	if (vtq) {
		fwrite("\033[?3l", 5, 1, stdout);
		delay (500);
	}
	return CROK ;
}
