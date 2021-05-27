#include <stdio.h>
#define TCAPBUF 1024

main (argc, argv)
	int argc;
	char **argv;
{
	char *cp, buf[TCAPBUF], *malloc();
	char *GS, *GE, *SO, *SE, *gc[12], *tgetstr();
	static char star[] = "*";
	static char plus[] = "+";
	static char minus[] = "-";
	static char vbar[] = "|";
	static char null[] = "";
	int c, n;
	int gmode = 0;

	if (argc > 1) {
		if (freopen (*(++argv), "r", stdin) <= 0) {
			fprintf (stderr, "Cannot open %s\n", *argv);
			exit (1);
		}
	}
	if (tgetent (buf, getenv ("TERM")) < 1) {
		fprintf (stderr, "bad return from tgetent.\r\n");
		exit(1);
	}
	cp = malloc (TCAPBUF);
	GS = tgetstr ("Gs", &cp);
	GE = tgetstr ("Ge", &cp);
	SO = tgetstr ("so", &cp);
	SE = tgetstr ("se", &cp);
	if (tgetnum ("sg") != 0)
		SO = SE = null;
	if ((gc[0] = tgetstr ("Bl", &cp)) == 0) gc[0] = plus;
	if ((gc[1] = tgetstr ("Tl", &cp)) == 0) gc[1] = plus;
	if ((gc[2] = tgetstr ("Tr", &cp)) == 0) gc[2] = plus;
	if ((gc[3] = tgetstr ("Br", &cp)) == 0) gc[3] = plus;
	if ((gc[4] = tgetstr ("Cj", &cp)) == 0) gc[4] = plus;
	if ((gc[5] = tgetstr ("Vl", &cp)) == 0) gc[5] = vbar;
	if ((gc[6] = tgetstr ("Hl", &cp)) == 0) gc[6] = minus;
	if ((gc[7] = tgetstr ("Rj", &cp)) == 0) gc[7] = plus;
	if ((gc[8] = tgetstr ("Lj", &cp)) == 0) gc[8] = plus;
	if ((gc[9] = tgetstr ("Tj", &cp)) == 0) gc[9] = plus;
	if ((gc[10] = tgetstr ("Bj", &cp)) == 0) gc[10] = plus;
	if ((gc[11] = tgetstr ("Xc", &cp)) == 0) gc[11] = star;
	fputs ("\n", stdout);
	while ((c = getchar()) != EOF)
		if (c == 016) {
			fputs (GS, stdout); gmode = 1;
		} else if (c == 017) {
			fputs (GE, stdout); gmode = 0;
		} else if (c == 020) {
			fputs (SO, stdout);
		} else if (c == 021) {
			fputs (SE, stdout);
		} else if (gmode) {
			if ((n = c - 'E') <= 10 && n >= 0)
				fputs (gc [n], stdout);
			else
				fputs (gc [11], stdout);
		} else {
			putchar ((char) c);
		}
	exit (0);
}
