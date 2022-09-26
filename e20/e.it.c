#ifdef COMMENT
--------
file e.it.c
    input (keyboard) table parsing and lookup
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#include "e.h"
#ifdef  KBFILE
#include "e.it.h"

struct itable *ithead;

extern int in_file (Uchar *, int *);
int itget (Uchar **, int *, struct itable *, Uchar *);

/****************************************/
/* file-driven input (no associated terminal number) */

int
in_file (lexp, count)
Uchar *lexp;
int *count;
{
    int code;
    Uchar *inp, *outp;
    int i;

/** / dbgpr ("in_file called, count=%d\n", *count); / **/
    for (inp = lexp, i = *count; i-- > 0;)
	*inp++ &= 0177;           /* Mask off high bit of all chars */
/* outp should be different so a string can be replaced by a longer one */
    inp = outp = lexp;
    while (*count > 0) {
	if (*inp >= ' ' && *inp <= '~') {
	    *outp++ = *inp++;
	    --*count;
	    continue;
	}

	code = itget (&inp, count, ithead, outp);
/** /	dbgpr("itget returns code=(%d)(%03o) count=%d\n", code, code, *count); / **/
	if (code >= 0) {    /* Number of characters resulting */
	    outp += code;
	    continue;
	}
	if (code == IT_MORE)    /* Stop here if in the middle of a seq */
	    break;
				/* Otherwise not in table */
	mesg (ERRALL + 1, "Bad control key");
	inp++;       /* Skip over this key */
	--*count;
    }
    my_move ((char *) inp, (char *) outp, (ulong) *count);
/** / dbgpr ("in_file returning %d\n", outp-lexp); / **/
    return outp - lexp;
}

#ifdef COMMENT
itget (..) matches input (at *cpp) against input table
If some prefix of the input matches the table, returns the number of
   characters in the value corresponding to the matched input, stores
   the address of the value in valp, points cpp past the matching input,
   and decrements *countp by the number of characters matched.

If no match, returns IT_NOPE.

If the input matches some proper prefix of an entry in the input table,
   returns IT_MORE.

cpp and countp are not changed in the last two cases.
#endif

int
itget (cpp, countp, head, valp)
Uchar **cpp;
int *countp;
struct itable *head;
Uchar *valp;
{
    register struct itable *it;
/*  register char *cp; */
    register Uchar *cp;
    int count;
    int len;

    cp = *cpp;
    count = *countp;
next:
    for (it = head; it != NULLIT; it = it->it_next) {
	if (count <= 0)
	    return IT_MORE;             /* Need more input */
	if (it->it_c == *cp) {          /* Does character match? */
	    cp++;
	    --count;
	    if (it->it_leaf) {
		    *cpp = cp;
		    len = it->it_len;
		    my_move (it->it_val, (char *)valp, (ulong) len);
		    *countp = count;
		    return len;
	    }
	    else {
		head = it->it_link;
		goto next;
	    }
	}
    }
    return IT_NOPE;
}

#endif /* KBFILE */
