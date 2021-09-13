#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#define CTRL(x) ((x) & 31)

#ifdef COMMENT
--------
file term/standard.c
    Standard stuff in support of
    terminal-dependent code and data declarations
#endif

char lexstd[32] = {
    CCCMD       , /* <BREAK > */
    CCCMD       , /* <cntr A> */
    CCLWORD     , /* <cntr B> */
    CCBACKSPACE , /* <cntr C> */
    CCMILINE    , /* <cntr D> */
    CCMIPAGE    , /* <cntr E> */
    CCPLLINE    , /* <cntr F> */
    CCHOME      , /* <cntr G> */
    CCMOVELEFT  , /* <cntr H> */
    CCTAB       , /* <cntr I> */
    CCMOVEDOWN  , /* <cntr J> */
    CCMOVEUP    , /* <cntr K> */
    CCMOVERIGHT , /* <cntr L> */
    CCRETURN    , /* <cntr M> */
    CCRWORD     , /* <cntr N> */
    CCOPEN      , /* <cntr O> */
    CCPICK      , /* <cntr P> */
    CCUNAS1     , /* <cntr Q> */
    CCPLPAGE    , /* <cntr R> */
    CCUNAS1     , /* <cntr S> */
    CCMISRCH    , /* <cntr T> */
    CCMARK      , /* <cntr U> */
    CCCLOSE     , /* <cntr V> */
    CCDELCH     , /* <cntr W> */
    CCUNAS1     , /* <cntr X> */
    CCPLSRCH    , /* <cntr Y> */
    CCINSMODE   , /* <cntr Z> */
    CCINSMODE   , /* <escape> */
    CCINT       , /* <cntr \> */
    CCREPLACE   , /* <cntr ]> */
    CCERASE     , /* <cntr ^> */
    CCSETFILE   , /* <cntr _> */
};

/* use this for column or row cursor addressing if the terminal doesn't
/* support separate row and column cursor addressing
/**/
bad ()
{
    fatal (FATALBUG, "Impossible cursor address");
}

nop () {}

/* Special characters */
char stdxlate[] = {
    0177,    /* ESCCHAR escape character */
    '*',     /* BULCHAR bullet character */
    '|',     /* LMCH    left border */
    '|',     /* RMCH    right border */
    '<',     /* MLMCH   more left border */
    '>',     /* MRMCH   more right border */
    '-',     /* TMCH    top border */
    '-',     /* BMCH    bottom border */
    '+',     /* TLCMCH  top left corner border */
    '+',     /* TRCMCH  top right corner border */
    '+',     /* BLCMCH  bottom left corner border */
    '+',     /* BRCMCH  bottom right corner border */
    '+',     /* BTMCH   top tab border */
    '+',     /* TTMCH   bottom tab border */
    ';',     /* ELMCH   empty left border */
    '.'      /* INMCH   inactive border */
};
#define P putchar

/* standard character translate, using stdxlate[] array */
xlate0 (chr)
#ifdef UNSCHAR
Uchar chr;
#else
int chr;
#endif
{
#ifndef UNSCHAR
    chr &= 0377;
#endif
    if (chr >= FIRSTSPCL)
	P (stdxlate[chr - FIRSTSPCL]);
    else
	if (chr) P (chr);
}

/* standard character translate, using stdxlate[] array */
/* except using '^' for ESCCHAR */
xlate1 (chr)
#ifdef UNSCHAR
Uchar chr;
#else
int chr;
#endif
{
#ifndef UNSCHAR
    chr &= 0377;
#endif
    if (chr == ESCCHAR)
	P ('^');
    else if (chr >= FIRSTSPCL)
	P (stdxlate[chr - FIRSTSPCL]);
    else
	if (chr) P (chr);
}

kini_nocbreak()
{
#ifdef  CBREAK
    cbreakflg = NO;
#endif  CBREAK
}


Flag in_std_p ();

in_std (lexp, count)
char *lexp;
int *count;
{
    int nr;
    Uint chr;
    char *icp;
    char *ocp;

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--) {
		if (in_std_p (&icp, &ocp, &nr)) break;
    }
    Block {
	Reg1 int conv;
	*count = nr;     /* number left over - still raw */
	conv = ocp - lexp;
	while (nr-- > 0)
	    *ocp++ = *icp++;
	return conv;
    }
}

Flag in_std_p (ic, oc, n)
	char **ic, **oc;
	int (*n);
{
	char chr;

	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *(*ic)++ & 0177) >= 32) {
	    if (chr == 0177)            /* DEL KEY KLUDGE FOR NOW */
		*(*oc)++ = CCBACKSPACE;
	    else
		*(*oc)++ = chr;
	}
	else if (chr == CTRL ('X')) {
	    if (*n < 2) {
		(*ic)--;
		return (YES);
	    }
	    (*n)--;
	    chr = *(*ic)++ & 0177;
	    switch (chr) {
	    case CTRL ('a'):
		*(*oc)++ = CCSETFILE;
		break;
	    case CTRL ('b'):
		*(*oc)++ = CCSPLIT;
		break;
	    case CTRL ('c'):
		*(*oc)++ = CCCTRLQUOTE;
		break;
	    case CTRL ('e'):
		*(*oc)++ = CCERASE;
		break;
	    case CTRL ('h'):
		*(*oc)++ = CCLWINDOW;
		break;
	    case CTRL ('j'):
		*(*oc)++ = CCJOIN;
		break;
	    case CTRL ('l'):
		*(*oc)++ = CCRWINDOW;
		break;
	    case CTRL ('r'):
		*(*oc)++ = CCREPLACE;
		break;
	    case CTRL ('t'):
		*(*oc)++ = CCTABS;
		break;
	    case CTRL ('u'):
		*(*oc)++ = CCBACKTAB;
		break;
	    case CTRL ('w'):
		*(*oc)++ = CCCHWINDOW;
		break;
	    default:
		*(*oc)++ = CCUNAS1;
		break;
	    }
	}
	else
	    *(*oc)++ = lexstd[chr];
	return (NO);
}

S_kbd kb_std = {
/* kb_inlex */  in_std,
/* kb_init  */  nop,
/* kb_end   */  nop,
};

