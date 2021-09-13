#ifdef COMMENT
	Assumes function keys have been downloaded with
	the standard keyboard.  (The pgm "loadsunkeys.c" is included
	in this directory).
#endif COMMENT



#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1



/*
 *  sun_lex[0-31] contains commands that map directly from
 *      <^char>,
 *
 *  sun_lex[32-177] (initialized in kini_sun2) is a direct
 *      translation of the character set, i.e.,
 *      sun_lex['a'] = 'a', etc.
 */

char sun_lex[128] = {
    CCCMD       , /* <BREAK > */
    CCCMD       , /* <cntr A> */
    CCLWORD     , /* <cntr B> */
    CCMOVELEFT  , /* <cntr C> */
    CCMILINE    , /* <cntr D> */
    CCMIPAGE    , /* <cntr E> */
    CCPLLINE    , /* <cntr F> */
    CCHOME      , /* <cntr G> */
    CCBACKSPACE , /* <cntr H> */
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

/*
 *  extended functions - <^x><^char>
 */

static char sun_Xlex[128] = {
    CCUNAS1     , /* <BREAK > */
    CCSETFILE   , /* <cntr A> */
    CCSPLIT     , /* <cntr B> */
    CCCTRLQUOTE , /* <cntr C> */
    CCUNAS1     , /* <cntr D> */
    CCERASE     , /* <cntr E> */
    CCUNAS1     , /* <cntr F> */
    CCUNAS1     , /* <cntr G> */
    CCLWINDOW   , /* <cntr H> */
    CCUNAS1     , /* <cntr I> */
    CCJOIN      , /* <cntr J> */
    CCUNAS1     , /* <cntr K> */
    CCRWINDOW   , /* <cntr L> */
    CCUNAS1     , /* <cntr M> */
    CCDWORD     , /* <cntr N> */
    CCUNAS1     , /* <cntr O> */
    CCUNAS1     , /* <cntr P> */
    CCUNAS1     , /* <cntr Q> */
    CCREPLACE   , /* <cntr R> */
    CCUNAS1     , /* <cntr S> */
    CCTABS      , /* <cntr T> */
    CCBACKTAB   , /* <cntr U> */
    CCUNAS1     , /* <cntr V> */
    CCCHWINDOW  , /* <cntr W> */
    CCUNAS1     , /* <cntr X> */
    CCUNAS1     , /* <cntr Y> */
    CCUNAS1     , /* <cntr Z> */
    CCUNAS1     , /* <escape> */
    CCUNAS1     , /* <cntr \> */
    CCUNAS1     , /* <cntr ]> */
    CCUNAS1     , /* <cntr ^> */
    CCUNAS1     , /* <cntr _> */
};


/*
 *      Very similar to in_std().  Changes:
 *          1.  \b -> BACKSPACE (destructive), was MOVELEFT
 *          2.  ^c -> MOVELEFT, was destructive BACKSPACE
 */

in_sun2(lexp, count)
char *lexp;
int *count;
{
	register int nr;
	register Uint chr;
	Reg3 char *icp;
	Reg4 char *ocp;
	static int state = 0;

	icp = ocp = lexp;
	nr = *count;
	for (; nr > 0; nr--) {
	    chr = *icp++ & 0177;
	    if (state == 0) {
		if (chr == CTRL ('X'))
		    state = 1;
		else
		    *ocp++ = sun_lex[chr];
	    }
	    else {
		/*
		 *  last char was ^X
		 */
		*ocp++ = sun_Xlex[chr];
		 state = 0;
	    }
	}

	*count = 0;
	return (ocp - lexp);
}


/*
 *  insert line
 */
il_sun2 (num) { printf ("\033[%dL", num); return num; }

/*
 *  delete line
 */
dl_sun2 (num) { printf ("\033[%dM", num); return num; }

kini_sun2 ()
{
	extern Flag singlescroll;
	register int n;

	/*
	 *  Unset singlescroll, and point to our own versions of
	 *  insert/delete line for fast operation.
	 */
	singlescroll = NO;
	term.tt_insline = il_sun2;
	term.tt_delline = dl_sun2;
	term.tt_so      = so_tcap;
	term.tt_soe     = soe_tcap;
	term.tt_bullets = NO;

	/*
	 * Initialize the input lex tables
	 */
	for(n=32; n<127; n++)
	    sun_lex[n] = n;             /* state 0 - std. char set */
	sun_lex[127] = CCBACKSPACE;

	for(n=32; n<128; n++)           /* state 1 - <^x><?> */
	    sun_Xlex[n] = CCUNAS1;

	return;
}

kend_sun2 ()
{
}


S_kbd kb_sun2 = {
/* kb_inlex */  in_sun2,
/* kb_init  */  kini_sun2,
/* kb_end   */  kend_sun2,
};
