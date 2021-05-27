/*
**      setwyse50 - a program which sets up the function key meanings
**      and messages as requested by the caller in the argument file,
**      which defaults to "~/.wy50[w]". This file consists of lines of
**      the form:
**
**      f#:message:sequence
**
**      where f# is a decimal number in the range 1-32, message is a
**      string of up to 8 characters to be displayed for that key (see
**      the Wyse50 manual (such as it is) for specs), and sequence is
**      the string of characters to be programmed into the function key.
**
**      lines 1-16 refer to the 16 function keys, lines 17-32 refer to
**      the same keys in shifted state. Wether this file uses .wy50 or
**      .wy50w depends on the value of the environmental variable TERM.
**
**      Defaults: Keys left undefined are not affected; the default message
**      is the previous contents (the field may be blanked by using \r
**      for the message); the default sequence is also the previous
**      contents. The escapes \r, \n, \b, \0, \E, \\ \: and \nnn are
**      honored in both strings.
**
**      Two special fk#'s are also defined: 100 and 101. Only the first
**      is used for each. These strings set the contents of, respectively,
**      the entire status line in unshifted and shifted mode.
*/

#include <stdio.h>
#include <ctype.h>

FILE *input;
extern char *getenv(), *index();

main (argc, argv)
	int argc;
	char **argv;
{
	int j, n;
	char *ip, *tp, *tp1, *arg[2], inbuf[BUFSIZ];
	static char ns[] = "";

	switch (argc) {
	case 1:
		if ((tp = getenv ("HOME")) == NULL) {
			fprintf (stderr, "setwyse50: no HOME env variable.\n");
			exit (1);
		}
		if ((tp1 = getenv ("TERM")) == NULL) {
			fprintf (stderr, "setwyse50: no TERM env variable.\n");
			exit (1);
		}
		if (strncmp (tp1, "wy50", 4) != 0) {
			fprintf (stderr, "setwyse50: $TERM not wy50\n");
			exit (1);
		}
		sprintf (inbuf, "%s/.%s", tp, tp1);
		if ((input = fopen (inbuf, "r")) == NULL) {
			/* silent exit if file doesn't exist. */
			exit (1);
		}
		break;
	case 2:
		if ((input = fopen (argv[1], "r")) == NULL) {
			fprintf ("setwyse50: Cannot open file %s.\n", argv[1]);
			exit (1);
		}
		break;
	default:
		fprintf (stderr, "Usage: setwyse50 [file]\n");
		exit (1);
	}
	while (fgets (inbuf, sizeof inbuf, input) != NULL) {
		inbuf [strlen (inbuf) - 1] = '\0';
		n = atoi (inbuf);
		if (n <= 32 && n >= 1) {
			if (n > 16) n += 16;
			arg[0] = arg[1] = ns;
			for (ip = inbuf, j = 0; *ip != 0 && j < 2; ip++) {
				if (*ip == ':' && *(ip-1) != '\\') {
					arg[j++] = ip + 1;
					*ip = 0;
				}
			}
			if (*arg[0] != 0)
				printf ("\033z%c%s\r", n+'0', arg[0]);
			if (*arg[1] != 0) {
				prep (arg[1]);
				printf ("\033z%c%s\177", n+'@', arg[1]);
			}
		} else if (n >= 100 && n <= 101) {
			if (tp = index(inbuf, ':')) tp++;
			else tp = ns;
			printf ("\033z%c%s\r", n - 100 + '(', tp);
		} else {
			fprintf ("setwyse50: unknown request number, line ignored:\n\t%s\n",
				inbuf);
		}
	}
	exit (0);
}

prep (line)
	char *line;
{
	char *i, *j;

	for (i = line, j = line; *i != 0; i++) {
		if (*i != '\\') {
			*j++ = *i;
		} else {
			switch (*(++i)) {
			case 'r':
				*j++ = '\r';
				break;
			case 'n':
				*j++ = '\n';
				break;
			case 'b':
				*j++ = '\b';
				break;
			case '\\':
				*j++ = '\\';
				break;
			case ':':
				*j++ = ':';
				break;
			case 'E':
				*j++ = '\033';
				break;
			default:
				if (isdigit (*i))
					*j++ = atoo (&i);
				else
					*j++ = *i;
			}
		}
	}
	*i = '\0';
}

atoo (ptr)
	char **ptr;
{
	int x;

	x = 0;
	while (**ptr >= '0' && **ptr <= '7') {
		x *= 8;
		x += (**ptr - '0');
		(*ptr)++;
	}
	return (char) x%127;
}
