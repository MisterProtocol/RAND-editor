#ifdef COMMENT
	The sun2 keyboard sends 3-byte escape sequences for the arrow
	keys, and 6-byte sequences for all other function keys.
	Note that (especially over a network) the full escape sequence
	for a particular function key is not always present,
	and action must be delayed.

	1st cut -- not optimized.  Should download the keys with
	more reasonable codes (ie the standard keyboard).
	    /Terry West, Rand - 10/2/84

	Started using this again with the Sun3s because (apparently)
	certain beta 3.0 software wouldn't work if the keyboard
	was downloaded at boot time.    1/8/85

	NOTE:  Currently, sun2.c must also be loaded in e.tt.c!
	       (hopefully, this will go away and we can use the
		downloaded keyboard again).
#endif COMMENT



#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1


in_sun3 (lexp, count)
char *lexp;
int *count;
{
    Reg1 int chr;
    Reg2 int nr;
    Reg3 char *icp;
    Reg4 char *ocp;
/*extern Flag DebugVal;*/

    icp = ocp = lexp;
    nr = *count;

/***
if( DebugVal ) {
    char buf[60];
  sprintf(buf, "in_sun3:, nr=%d  icp=%o icp1=%o, icp2=%o, icp3=%o",
      nr, icp[0], icp[1], icp[2], icp[3]);
  dbgpr(buf);
}
***/

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
	    if( nr == 1 ) {
		icp--;
		goto nomore;
	    }
	    else if ((*icp & 0177) != '[' ) {
		*ocp++ = CCUNAS1;
		continue;
	    }
	    else if (nr == 2) {    /* need at least 3 */
		icp--;
		goto nomore;
	    }

	    nr--;
	    chr = *icp++ & 0177;    /* a known '[' */

	    nr--;
	    switch (chr = *icp++ & 0177) {

	    /*
	     *  the following 4 are for a reset sun2.
	     */
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

	    case '1':   /* ESC[19{3-9}z */
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
		case '3':                       /* L2 */
		    *ocp++ = CCINT;
		    break;
		case '4':                       /* L3 */
		    *ocp++ = CCUNAS1;
		    break;
		case '5':                       /* L4 */
		    *ocp++ = CCTABS;
		    break;
		case '6':                       /* L5 */
		    *ocp++ = CCJOIN;
		    break;
		case '7':                       /* L6 */
		    *ocp++ = CCCHWINDOW;
		    break;
		case '8':                       /* L7 */
		    *ocp++ = CCSPLIT;
		    break;
		case '9':                       /* L8 */
		    *ocp++ = CCMARK;
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
		case '0':      /* ESC[20{0189}z */
		    chr = *icp++ & 0177;
		    icp++;
		    switch( chr ) {
		    case '0':                   /* L9 */
			*ocp++ = CCBACKTAB;
			break;
		    case '1':                   /* L10 */
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
		    case '5':                   /* R8 on sun3 */
			*ocp++ = CCMOVEUP;
			break;
		    case '6':                   /* R9 */
			*ocp++ = CCPLSRCH;
			break;
		    case '7':                   /* R10 on sun3 */
			*ocp++ = CCMOVELEFT;
			break;
		    case '8':                   /* R11 */
			*ocp++ = CCHOME;
			break;
		    case '9':                   /* R11 */
			*ocp++ = CCMOVERIGHT;   /* R12 on sun3 */
			break;
		    default:
			*ocp++ = CCUNAS1;
			break;
		    }
		    break;

		case '2':       /* ESC[22{02456789}z */
		    chr = *icp++ & 0177;
		    icp++;            /* skip 'z' */
		    switch( chr ) {
		    case '0':                   /* R13*/
			*ocp++ = CCINSMODE;
			break;
		    case '1':                   /* R14 on sun3 */
			*ocp++ = CCMOVEDOWN;
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
		    case '6':                   /* F3 */
			*ocp++ = CCSETFILE;
			break;
		    case '7':                   /* F4 */
			*ocp++ = CCPICK;
			break;
		    case '8':                   /* F5 */
			*ocp++ = CCERASE;
			break;
		    case '9':                   /* F6 */
			*ocp++ = CCCLOSE;
			break;
		    default:
			*ocp++ = CCUNAS1;
			break;
		    }
		    break;

		case '3':               /* ESC[23{012}z */
		    chr = *icp++ & 0177;
		    icp++;
		    switch( chr ) {
		    case '0':                   /* F7 */
			*ocp++ = CCOPEN;
			break;
		    case '1':                   /* F8 */
			*ocp++ = CCLWORD;
			break;
		    case '2':                   /* F9 */
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
	else if( chr == CTRL('X')) {
	    if( nr == 1 ) {
		icp--;
		goto nomore;
	    }
	    else {
		nr--;
		*ocp++ = sun_Xlex[*icp++ & 0177];
	    }
	}
	else
	    *ocp++ = sun_lex[chr];
    }
 nomore:
    Block {
	int conv;
	*count = nr;     /* number left over - still raw */
	conv = ocp - lexp;
/***
dbgpr("nomore: nr=%d, conv=%d\n", nr, conv);
***/
	while (nr-- > 0)
	    *ocp++ = *icp++;
	return conv;
    }
}


/*
 *  insert line
 */
il_sun3 (num) { printf ("\033[%dL", num); return num; }

/*
 *  delete line
 */
dl_sun3 (num) { printf ("\033[%dM", num); return num; }

kini_sun3 ()
{
#ifdef OUT
	extern Flag singlescroll;

	/*
	 *  Unset singlescroll, and point to our own versions of
	 *  insert/delete line for fast operation.
	 */
	singlescroll = NO;
	term.tt_insline = il_sun3;
	term.tt_delline = dl_sun3;
	term.tt_bullets = NO;
#endif OUT
	kini_sun2();
}

kend_sun3 ()
{
}


S_kbd kb_sun3 = {
/* kb_inlex */  in_sun3,
/* kb_init  */  kini_sun3,
/* kb_end   */  kend_sun3,
};

