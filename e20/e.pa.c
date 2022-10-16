#ifdef COMMENT
--------
file e.pa.c
    command line parsing routines
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#ifdef UNIXV7
#include <ctype.h>
#endif
#include "e.h"
#include "e.m.h"
#include "e.cm.h"

Small getopteq (char **, S_looktbl *, int (*)());

#ifdef COMMENT
Small
getpartype (str, parflg, defparflg, parline)
    char  **str;
    Flag    parflg;
    Flag    defparflg;
    Nlines  parline;
.
    Parse for things like "3p", "3l", "45", "45x8" etc.
    When a number of paragraphs is specified, the "parline" argument
    is used as the first line of the paragraph parsing.
    If parflg != 0, then allow paragraph specs.
    If defparflg != 0, then default is paragraphs.
    str is the address of a pointer to the first character
    Returns:
      0  *str is empty (== "")
      1  one number found, and its value is stored in parmlines
	    parmlines will be < 0 if it means num of paragraphs.
      2  two numbers found - stored in paramlines and paramcols
      3  not recognizable as 0, 1, or 2
    str is set to next word if this word returns 1 or 2
#endif
Small
getpartype (str, parflg, defparflg, parline)
char **str;
Flag    parflg;
Flag    defparflg;
Nlines  parline;
{
    register char *cp,
		  *cp1;
    long tmp;    /* must be long for s2i */
    Nlines plines;

    cp = *str;
    if (*cp == 0)
	return 0;   /* no argument at all */
    for (; *cp && *cp == ' '; cp++)
	continue;
    cp1 = cp;
    cp = s2i (cp, &tmp);
    tmp = la_ltrunc (tmp);
    if (cp == cp1)
	return 3;   /* string */
    if (parflg && (*cp == 'p' || *cp == 'P')) {
	cp++;
	plines = lincnt (parline, tmp, 2);
    }
    else if (*cp == 'l' || *cp == 'L') {
	cp++;
	plines = tmp;
    }
    else if (defparflg)
	plines = lincnt (parline, tmp, 2);
    else
	plines = tmp;
    if (*cp == ' ' || *cp == 0) {
	for (; *cp && *cp == ' '; cp++)
	    continue;
	*str = cp;
	parmlines = plines;
	return 1;
    }
    if (*cp != 'x' && *cp != 'X')
	return 3;
    cp++;
    cp1 = cp;
    cp = s2i (cp, &tmp);
    if (cp == cp1)
	return 3;   /* string */
    if (*cp == ' ' || *cp == 0) {
	for (; *cp && *cp == ' '; cp++)
	    continue;
	*str = cp;
	parmlines = plines;
	parmcols = tmp;
	return 2;
    }
    return 3;
}

#define MAXLUPN 30      /* longer than longest possible name */
#ifdef COMMENT
int
lookup (name, table)     /* table entries must be sorted by name */
    char *name;
    S_looktbl *table;
.
    Lookup name in table.  Will take nonambiguous abbreviations.  If you
    want to insist that a certain table entry must be spelled out, enter it
    twice in the table.  Table entries must be sorted by name, and a name
    which is a substring of a longer name comes earlier in the table.
    Accepts upper or lower case if table entry is lower case.
    Returns:
     > 0 table entry index
      -1 not found
      -2 ambiguous
#endif
int
lookup (name, table)     /* table entries must be sorted by name */
char *name;
S_looktbl *table;
{
    register char  *namptr,
		   *tblptr;
    int ind;
    int value = 0;
    Small length;
    Small longest = 0;
    Flag ambig = NO;
    char lname[MAXLUPN];

    if (name == NULL)
	return -1;
    namptr = name;
    tblptr = lname;
    for (;;) {
	if ((*tblptr++ = (char)(isupper (*namptr) ? tolower (*namptr++) : *namptr++))
	    == '\0')
	    break;
	if (tblptr >= &lname[MAXLUPN])
	    return -1;
    }

    for (ind = 0; (tblptr = table->str) != 0; table++, ind++) {
	namptr = lname;
	for (; *tblptr == *namptr; tblptr++, namptr++) {
	    if (*tblptr == '\0')
		break;
	}
	if (*namptr == '\0') {  /* end of name or exact match */
	    length = (Small)(namptr - lname);
	    if (longest < length) {
		longest = length;
		ambig = NO;
		value = ind;
		if (*tblptr == '\0')
		    break;          /* exact match */
	    }
	    else /* longest == length */
		ambig = YES;
	}
	else if ( *namptr < *tblptr )
	    break;
    }
    if (ambig)
	return -2;

#ifdef OUT
if( DebugVal == 2 ) {
      char tmp[50];
    sprintf(tmp, "name = %s longest = %d, value = %d", name, longest, value);
    mesg (ERRALL+1, tmp);
}
#endif /* OUT */
    if (longest)
	return value;
    return -1;
}

#ifdef COMMENT
char *
getword (str)
    char **str;
.
    Finds the first non-blank in *str then finds the first blank or '\0'
    after that, then allocs enough space for the 'word' thus delimited,
    copies it into the alloced space, and null-terminates the new string.
    If the returned string is a null string (not a null pointer) then it
    was NOT alloced.
    Returns a pointer to the new string and makes *str to point to the
    next 'word'.

    2/2022:  modified to treat abc\ def or "abc def" as a single 'word'
    to allow editing of filenames with embedded spaces:
    eg, CMD: e my\ file, or CMD: e "my file".

#endif
char *
getword (str)
char **str;
{
    register char *cp1, /* first non-blank in str */
		  *cp2, /* first blank or '\0' after cp1 */
		  *cp3; /* first non-blank or '\0' after cp2 */
    char *newstr;
    static char nullstr[] = "";

/**
dbgpr("getword: input str=(%s)\n", *str);
 **/
    char word[128], *wp;
    wp = word;

    int wordQuoted = 0;  /* word begins and ends with a " */

    if (*str == 0)
	return nullstr;

    /* remove leading spaces */
    for (cp1 = *str; *cp1 == ' '; cp1++)
	continue;

    /* remove trailing spaces */
    cp2 = *str + strlen(*str) - 1;
    while (*cp2 == ' ') cp2--;
    if (strlen(cp2) > 1)
	cp2[1] = '\0';

    if (*str[0] == '"' && *cp2 == '"') {
	wordQuoted = 1;
   /** /dbgpr("  word quoted, str=(%s)\n", *str); / **/
    }

/*  for (cp2 = cp1; *cp2 && *cp2 != ' '; cp2++) */
    for (cp2 = cp1; *cp2; cp2++) {
	if (*cp2 == '\\' && cp2[1] == ' ') {
	    *wp++ = ' ';
	    cp2++;
	    continue;
	}
	if (*cp2 == ' ' && !wordQuoted)
	    break;

	/* omit beg/end " chars */
	if (*cp2 == '"' && wordQuoted)
	    continue;

	*wp++ = *cp2;
	continue;
    }
    *wp = '\0';

    for (cp3 = cp2; *cp3 == ' '; cp3++)
	continue;
    *str = cp3;
    if (cp2 == cp1)
	return nullstr;

#ifdef OUT
    newstr = salloc (cp2 - cp1 + 1, YES);
    for (cp3 = newstr; cp1 < cp2; )
	*cp3++ = *cp1++;
    *cp3 = '\0';
#endif /* OUT */

    newstr = salloc ((Ncols) strlen(word) + 1, YES);
    strncpy(newstr, word, strlen(word));

/** /
dbgpr("getword: word=(%s) newstr=(%s) str=(%s) \n", word, newstr, *str);
/ **/

    return newstr;
}

#ifdef COMMENT
Cmdret
scanopts (cpp, pardefault, table, tblfunc)
    char **cpp;
    Flag pardefault;
    S_looktbl *table;
    int (*tblfunc) ();
.
    Scans the command line for options from a table of options,
    and will take an areaspec along the way.
    You might not be at end of string if this routine returns >= 0
    returns:
	 3 marked area        \
	 2 rectangle           \
	 1 number of lines      > may have stopped at an unknown option
	 0 no area spec        /
	-2 ambiguous option     ) see parmlines, parmcols for type of area
     <= -3 other error
#endif
Cmdret
scanopts (cpp, pardefault, table, tblfunc)
char **cpp;
Flag pardefault;
S_looktbl *table;
int (*tblfunc) ();
{
    int areatype;
    int tmp;

    if (curmark)
	areatype = 3;
    else
	areatype = 0;
    parmlines = 0;
    parmcols  = 0;
    for (;;) {
	tmp = getpartype (cpp, YES, pardefault, curwksp->wlin + cursorline);
	switch (tmp) {
	case 0: /* end of string */
	    return (Cmdret) areatype;

	case 1:
	case 2:
	    if (areatype) {
		if (areatype == 3)
		    return CRMARKCNFL;
		else
		    return CRMULTARG;
	    }
	    areatype = tmp;
	    break;

	default:
	    tmp = getopteq (cpp, table, tblfunc);
	    if (tmp <= -2)
		return (Cmdret) tmp;
	    if (tmp != 1 )
		return (Cmdret) areatype;
	}
    }
    /* only return is from inside for loop */
    /* NOTREACHED */
}

#ifdef COMMENT
Small
getopteq (str, table, tblfunc)
    char **str;
    S_looktbl *table;
    int (*tblfunc) ();
.
    Look for things like width=XXX
    Returns:
       1  valid option dealt with by calling tblfunc
       0  end of string
      -1  option not found
      -2  ambiguous option
     < 0  other error
#endif
Small
getopteq (str, table, tblfunc)
char **str;
S_looktbl *table;
int (*tblfunc) ();
{
    Small tmp;
    register char *cp;
    register char *cp1;
    Char svchr;
    Flag equals;

    /* skip over blanks */
    for (cp = *str; *cp && *cp == ' '; cp++)
	continue;
    cp1 = cp;
    equals = NO;
    /* delimit a word */
    for (; *cp && *cp != ' '; cp++)
	if (*cp == '=') {
	    equals = YES;
	    break;
	}
    if (cp == cp1) {
	if (equals)
	    return -1;
	return 0;
    }

    svchr = *cp;
    *cp = '\0';
    tmp = lookup (cp1, table);
    *cp = (char)svchr;
    if (tmp < 0)
	return tmp;
    if (tblfunc)
	return (*tblfunc) (cp, str, tmp, equals);
    return CRBADARG;
}

#ifdef COMMENT
Small
doeq (cpp, value)
    char **cpp;
    int *value;
.
    Parse stuff after '=' -- called only by fillopts().
    Returns:
       0 good value, and it was stuffed in *value
     < 0 error type
#endif
Small
doeq (cpp, value)
char **cpp;
int *value;
{
    char *cp;
    char *cp1;
    /*int tmp;*/
    long tmp;

    cp = *cpp;
    if (*cp++ != '=')
	return CRNOVALUE;
    cp1 = cp;
    cp = s2i (cp1, &tmp);
    if (cp == cp1)
	return CRNOVALUE;
    if (*cp == ' ' || *cp == '\0') {
	for (; *cp && *cp == ' '; cp++)
	    continue;
	*value = (int) tmp;
	*cpp = cp;
	return 0;
    }
    return CRBADARG;
}
