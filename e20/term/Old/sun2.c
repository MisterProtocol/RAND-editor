#ifdef COMMENT
	The sun2 keyboard sends 3-byte escape sequences for the arrow
	keys, and 6-byte sequences for all other function keys.
	Note that (especially over a network) the full escape sequence
	for a particular function key is not always present,
	and action must be delayed.

	1st cut -- not optimized.  Should download the keys with
	more reasonable codes (ie the standard keyboard).
	    /Terry West, Rand - 10/2/84
#endif COMMENT



#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1


in_sun2 (lexp, count)
char *lexp;
int *count;
{
    Reg1 int chr;
    Reg2 int nr;
    Reg3 char *icp;
    Reg4 char *ocp;

    icp = ocp = lexp;
    nr = *count;

    for (; nr > 0; nr--) {
	if ((chr = *icp++ & 0177) >= 040) {
	    if (chr == 0177)         /* DEL KEY KLUDGE FOR NOW */
		*ocp++ = CCBACKSPACE;
	    else
		*ocp++ = chr;
	}
	else if (chr == '\b')
	    *ocp++ = CCBACKSPACE;
	else if (chr == 033) {
	    if (nr < 3) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    if (chr != '[') {
		nr++;
		icp -= 2;
		goto nomore;
	    }
	    nr--;
	    switch (chr = *icp++ & 0177) {
	    case 'A':                           /* R8 */
		*ocp++ = CCMOVEUP;
		break;

	    case 'B':                           /* R14 */
		*ocp++ = CCMOVEDOWN;
		break;

	    case 'C':                           /* R12 */
		*ocp++ = CCMOVERIGHT;
		break;

	    case 'D':                           /* R10 */
		*ocp++ = CCMOVELEFT;
		break;

	    case '1':   /* ESC[19{4,5,6,8}z */
		if( nr < 4 ) {
		    nr += 2;        /* set count and ptr back to ESC */
		    icp -= 3;
		    goto nomore;
		}
		nr -= 3;        /* for loop also subtracts 1 */
		icp++;          /* skip '9' */
		chr = *icp++ & 0177;
		icp++;          /* skip 'z' */
		switch (chr){
		case '4':                       /* L2 */
		    *ocp++ = CCINT;
		    break;
		case '5':                       /* L3 */
		    *ocp++ = CCUNAS1;
		    break;
		case '6':                       /* L4 */
		    *ocp++ = CCTABS;
		    break;
		case '8':                       /* L5 */
		    *ocp++ = CCJOIN;
		    break;
		default:
		    *ocp++ = CCUNAS1;
		    break;
		}
		break;

	    case '2':
		if( nr < 4 ) {
		    nr += 2;
		    icp -= 3;
		    goto nomore;
		}
		nr -= 3;
		switch( chr = *icp++ & 0177 ) {
		case '0':      /* ESC[20{0124689}z */
		    chr = *icp++ & 0177;
		    icp++;
		    switch( chr ) {
		    case '0':                   /* L6 */
			*ocp++ = CCCHWINDOW;
			break;
		    case '1':                   /* L7 */
			*ocp++ = CCSPLIT;
			break;
		    case '2':                   /* L8 */
			*ocp++ = CCMARK;
			break;
		    case '4':                   /* L9 */
			*ocp++ = CCBACKTAB;
			break;
		    case '6':                   /* L10 */
			*ocp++ = CCCMD;
			break;
		    case '8':                   /* R1 */
			*ocp++ = CCMIPAGE;
			break;
		    case '9':                   /* R2 */
			*ocp++ = CCREPLACE;
			break;
		    default:
			*ocp++ = CCUNAS1;
			break;
		    }
		    break;

		case '1':      /* ESC[21{0123468}z */
		    chr = *icp++ & 0177;
		    icp++;            /* skip 'z' */
		    switch( chr ) {
		    case '0':                   /* R3 */
			*ocp++ = CCPLPAGE;
			break;
		    case '1':                   /* R4 */
			*ocp++ = CCMILINE;
			break;
		    case '2':                   /* R5 */
			*ocp++ = CCCTRLQUOTE;
			break;
		    case '3':                   /* R6 */
			*ocp++ = CCPLLINE;
			break;
		    case '4':                   /* R7 */
			*ocp++ = CCMISRCH;
			break;
		    case '6':                   /* R9 */
			*ocp++ = CCPLSRCH;
			break;
		    case '8':                   /* R11 */
			*ocp++ = CCHOME;
			break;
		    default:
			*ocp++ = CCUNAS1;
			break;
		    }
		    break;

		case '2':       /* ESC[22{024579}z */
		    chr = *icp++ & 0177;
		    icp++;            /* skip 'z' */
		    switch( chr ) {
		    case '0':                   /* R13*/
			*ocp++ = CCINSMODE;
			break;
		    case '2':                   /* R15*/
			*ocp++ = CCDELCH;
			break;
		    case '4':                   /* F1 */
			*ocp++ = CCLWINDOW;
			break;
		    case '5':                   /* F2 */
			*ocp++ = CCRWINDOW;
			break;
		    case '7':                   /* F3 */
			*ocp++ = CCSETFILE;
			break;
		    case '9':                   /* F4 */
			*ocp++ = CCPICK;
			break;
		    default:
			*ocp++ = CCUNAS1;
			break;
		    }
		    break;

		case '3':               /* ESC[23{13567}z */
		    chr = *icp++ & 0177;
		    icp++;
		    switch( chr ) {
		    case '1':                   /* F5 */
			*ocp++ = CCERASE;
			break;
		    case '3':                   /* F6 */
			*ocp++ = CCCLOSE;
			break;
		    case '5':                   /* F7 */
			*ocp++ = CCOPEN;
			break;
		    case '6':                   /* F8 */
			*ocp++ = CCLWORD;
			break;
		    case '7':                   /* F9 */
			*ocp++ = CCRWORD;
			break;
		    default:
			*ocp++ = CCUNAS1;
			break;
		    }
		   break;

		default:
		    *ocp++ = CCUNAS1;
		    break;
		}
		break;

	    default:
		*ocp++ = CCUNAS1;
		break;
	    }

	}
	else
	    *ocp++ = lexstd[chr];
    }
 nomore:
    Block {
	int conv;
	*count = nr;     /* number left over - still raw */
	conv = ocp - lexp;
	while (nr-- > 0)
	    *ocp++ = *icp++;
	return conv;
    }
}

il_sun2 (num) { printf ("\033[%dL", num); return num; }
dl_sun2 (num) { printf ("\033[%dM", num); return num; }

kini_sun2 ()
{
	extern Flag singlescroll;

	/*
	 *  Unset singlescroll, and point to our own versions of
	 *  insert/delete line for fast operation.
	 */
	singlescroll = NO;
	term.tt_insline = il_sun2;
	term.tt_delline = dl_sun2;
	term.tt_bullets = NO;
}

kend_sun2 ()
{
}


S_kbd kb_sun2 = {
/* kb_inlex */  in_sun2,
/* kb_init  */  kini_sun2,
/* kb_end   */  kend_sun2,
};
