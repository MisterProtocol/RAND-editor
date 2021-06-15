#ifdef COMMENT
--------
file e.iit.c
    initialize input (keyboard) table parsing and lookup
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#include "e.h"
#ifdef  KBFILE
#include "e.it.h"

extern char *salloc();

void itparse (), itadd(), itprint();

/* struct itable *ithead = NULLIT; */
extern struct itable *ithead;

S_looktbl itsyms[] = {
    {"+line",   CCPLLINE},
    {"+page",   CCPLPAGE},
    {"+sch",    CCPLSRCH},
    {"+tab",    CCTAB},
    {"+word",   CCRWORD},
    {"-line",   CCMILINE},
    {"-page",   CCMIPAGE},
    {"-sch",    CCMISRCH},
    {"-tab",    CCBACKTAB},
    {"-word",   CCLWORD},
    {"abort",   CCABORT},
    {"bksp",    CCBACKSPACE},
    {"blot",    CCBLOT},
    {"box",     CCBOX},
#ifdef LMCCASE
    {"caps",    CCCAPS},
    {"ccase",   CCCCASE},
#endif /* LMCCASE */
    {"cchar",   CCCTRLQUOTE},
    {"center",  CCCENTER},
    {"chwin",   CCCHWINDOW},
    {"clear",   CCCLEAR},
    {"close",   CCCLOSE},
    {"cltabs",  CCCLRTABS},
    {"cmd",     CCCMD},
    {"cover",   CCCOVER},
    {"dchar",   CCDELCH},
    {"del",     0177},
    {"down",    CCMOVEDOWN},
#ifdef LMCDWORD
    {"dword",   CCDWORD},
#endif /* LMCDWORD */
    {"edit",    CCSETFILE},
    {"erase",   CCERASE},
    {"esc",     033},
    {"exit",    CCEXIT},
    {"fill",    CCFILL},
#ifdef LMCHELP
    {"help",    CCHELP},
#endif /* LMCHELP */
    {"home",    CCHOME},
    {"insmd",   CCINSMODE},
    {"int",     CCINT},
    {"join",    CCJOIN},
    {"justify", CCJUSTIFY},
    {"kbend",   KBEND},
    {"kbinit",  KBINIT},
    {"left",    CCMOVELEFT},
    {"mark",    CCMARK},
    {"mouse",   CCMOUSEONOFF},
    {"null",    CCNULL},
    {"open",    CCOPEN},
    {"overlay", CCOVERLAY},
    {"pick",    CCPICK},
#ifdef RECORDING
    {"play",    CCPLAY},
#endif
    {"quit",    CCQUIT},
    {"quote",   042},
    {"range",   CCRANGE},
#ifdef RECORDING
    {"record",  CCRECORD},
#endif
    {"redraw",  CCREDRAW},
    {"replace", CCREPLACE},
    {"ret",     CCRETURN},
    {"right",   CCMOVERIGHT},
    {"space",   040},
    {"split",   CCSPLIT},
    {"srtab",   CCTABS},
    {"stopx",   CCSTOPX},
    {"tab",     CCTAB},
    {"track",   CCTRACK},
    {"undef",   CCUNAS1},
    {"up",      CCMOVEUP},
    {"wleft",   CCLWINDOW},
    {"wp",      CCAUTOFILL},
    {"wright",  CCRWINDOW},
    {0,         0}
 };

char *kbinistr;
char *kbendstr;
int  kbinilen;
int  kbendlen;

void
getkbfile (filename)
char *filename;
{
    char line[TMPSTRLEN], string[TMPSTRLEN], value[TMPSTRLEN];
    FILE *f;
    int str_len, val_len;

    if ((f = fopen (filename, "r")) == NULL)
	getout (YES, "Can't open keyboard file \"%s\"", filename);
    while (fgets (line, sizeof line, f) != NULL) {
	line[strlen (line)-1] = '\0';   /* Stomp on the newline */
	if(line[0] == '#'               /* gww 21 Feb 82 */
	|| line[0] == '\0')             /* gww 21 Feb 82 */
		continue;               /* gww 21 Feb 82 */
	itparse (line, string, &str_len, value, &val_len);
	switch (string[0]) {
	case KBINIT:
	    kbinilen = val_len;
	    kbinistr = salloc (kbinilen);
	    my_move (value, kbinistr, kbinilen);
	    break;
	case KBEND:
	    kbendlen = val_len;
	    kbendstr = salloc (kbendlen);
	    my_move (value, kbendstr, kbendlen);
	    break;
	default:
	    itadd (string, str_len, &ithead, value, val_len, line);
	}
    }
    fclose (f);
#ifdef  DEBUG_KBFILE
    itprint (ithead, 0);
#endif
    return;
}

void
itadd (str, str_len, headp, val, val_len, line)
char *str;              /* Character string */
struct itable **headp;  /* Pointer to head (where to start) */
char *val;              /* Value */
int str_len, val_len;
char *line;             /* For debugging */
{
    register struct itable *it;         /* Current input table entry */
    register struct itable *pt;         /* Previous table entry */

    if (str_len == 0)
	getout (YES, "kbfile invalid prefix in %s\n", line);
    for (it = *headp; it != NULLIT; pt = it, it = it->it_next) {
	if (it->it_c == *str) {         /* Character match? */
	    if (it->it_leaf)            /* Can't add this */
		getout (YES, "kbfile duplicate string in %s\n", line);
	    else        /* Go down the tree */
		itadd (str+1, str_len-1, &it->it_link, val, val_len, line);
	    return;
	}
    }
    it = (struct itable *) salloc (sizeof *it);           /* Get new node */
    if (*headp == 0)                    /* Change head if tree was empty */
	*headp = it;
    else
	pt->it_next = it;               /* Otherwise update prev node */
    it->it_c = *str++;                  /* Save current character */
    it->it_next = 0;
    if (--str_len > 0) {                /* Is this a leaf? */
	it->it_leaf = 0;                /* No */
	it->it_link = 0;
	itadd (str, str_len, &it->it_link, val, val_len, line);
    } else {
	it->it_leaf = 1;
	it->it_val = salloc (val_len);
	it->it_len = val_len;
	my_move (val, it->it_val, val_len);
    }
    return;
}

void
itparse (inp, strp, str_lenp, valp, val_lenp)
register char *inp;
char *strp, *valp;      /* Pointers to string to match and value to return */
int *str_lenp, *val_lenp; /* Where to put the respective lengths */
{
    register char c;
    unsigned int n;
    int i;
    int gotval = 0;
    register char *outp = strp;
    char tmpstr[50], *tp;
    char *line = inp;            /* Save for error messages */

    while ((c = *inp++) != '\0') {
	switch (c) {
	    case '"':       /* String "foo bar" (with no quotes) */
		while ((c = *inp++) != '"') {
		    if (c == '\0')
			getout (YES, "kbfile mismatched in %s\n", line);
		    *outp++ = c;
		}
		break;
	    case '<':
		if ((c = *inp) >= '0' && c <= '7') {
		    for (n = 0; (c = *inp++) != '>'; ) {
			if (c < '0' || c > '7')
			    getout (YES, "kbfile bad digit in %s\n", line);
			n = n*8 + (c-'0');
		    }
		    if (n > 0377)
			getout (YES, "Number %d too big in kbfile\n", n);
		    *outp++ = (char) n;
		} else {
		    for (tp = tmpstr; (c = *inp++) != '>'; ) {
			if (c == '\0')
			    getout (YES, "kbfile mismatched < in %s\n", line);
			*tp++ = c;
		    }
		    *tp = '\0';
		    i = lookup (tmpstr, itsyms);
		    if (i < 0)
			getout (YES, "Bad symbol %s in kbfile\n", tmpstr);
		    *outp++ = (char) itsyms[i].val;
		}
		break;
	    case '^':
		c = *inp++;
		if (c != '?' && (c < '@' || c > '_'))
		    getout (YES, "kbfile bad char ^<%o> in %s\n", c, line);
		*outp++ = c^0100;
		break;
	    case ':':
		*str_lenp = outp - strp;
		if (gotval++)
		    getout (YES, "kbfile too many colons in %s\n", line);
		if (*str_lenp > TMPSTRLEN)
		    goto toolong;
		outp = valp;
		break;
	    default:
		getout (YES, "kbfile bad char <%o> in %s\n", c, line);
	}
    }
    *val_lenp = outp - valp;
    if (*str_lenp > TMPSTRLEN)
toolong:
	getout (YES, "kbfile line too long %s\n", line);
    return;
}

#ifdef DEBUG_KBFILE
#include <ctype.h>

void
itprint (head, n)        /* n should be 0 the first time */
struct itable *head;
int n;
{
    register struct itable *it;
    int i;
    char c;
    char *cp;

    for (it = head; it != NULLIT; it = it->it_next) {
	for (i = 0; i < n; i++)
	    putchar (' ');
	c = it->it_c;
	if (isalnum (c))
	    printf ("%c  ", c);
	else
	    printf ("<%3o>", c);
	if (it->it_leaf) {
	    printf ("=");
	    for (cp = it->it_val, i = it->it_len; i-- > 0; )
		printf ("<%o>", *cp++);
	    printf (" (len %d)\n", it->it_len);
	}
	else {
	    printf ("\n");
	    itprint (it->it_link, n+2);
	}
    }
    return;
}
#endif /* DEBUG_KBFILE */

#endif /* KBFILE */
