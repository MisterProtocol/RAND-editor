#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#ifndef CTRL
#define CTRL(x) ((x) & 31)
#endif
#define _BAD_ CCUNAS1


int
in_mac (lexp, count)
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
	else if (chr == 033) {
	    if (nr < 2) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    if (chr == '[') {
		if (nr < 2) {
		    icp -= 2;
		    nr++;
		    goto nomore;
		}
		nr--;
		switch (chr = *icp++ & 0177) {
		case 'A':
		    *ocp++ = CCMOVEUP; /*CCHOME;*/
		    break;

		case 'B':
		    *ocp++ = CCMOVEDOWN; /*CCREPLACE; */
		    break;

		case 'C':
		    *ocp++ = CCMOVERIGHT; /*CCRWINDOW;*/
		    break;

		case 'D':
		    *ocp++ = CCMOVELEFT; /*CCLWINDOW;*/
		    break;

		default:
		    *ocp++ = CCUNAS1;
		    break;
		}
	    }
	    else if (chr == 'O') {
		if (nr < 2) {
		    icp -= 2;
		    nr++;
		    goto nomore;
		}
		nr--;
		chr = *icp++ & 0177;
		if ('l' <= chr && chr <= 'y') Block {
		    static Uchar xlt[] = {
			CCMOVEDOWN     , /* l  , */
			CCMOVEUP       , /* m  - */
			CCDELCH        , /* n  . */
			CCUNAS1        , /* o    */
			CCINSMODE      , /* p  0 */
			CCMISRCH       , /* q  1 */
			CCPLSRCH       , /* r  2 */
			CCMARK         , /* s  3 */
			CCMIPAGE       , /* t  4 */
			CCPLPAGE       , /* u  5 */
			CCPICK         , /* v  6 */
			CCMILINE       , /* w  7 */
			CCPLLINE       , /* x  8 */
			CCSETFILE      , /* y  9 */
		    };
		    *ocp++ = xlt[chr - 'l'];
		}
		else if ('M' <= chr && chr <= 'S') Block {
		    static Uchar xlt[] = {
			CCCMD          , /* M  ENTER */
			CCUNAS1        , /* N        */
			CCUNAS1        , /* O        */
			CCCLOSE        , /* P  PF1   */
			CCOPEN         , /* Q  PF2   */
			CCMOVELEFT     , /* R  PF3   */
			CCMOVERIGHT    , /* S  PF4   */
		    };
		    *ocp++ = xlt[chr - 'M'];
		}
		else
		    *ocp++ = CCUNAS1;
	    }
	    else
		*ocp++ = CCUNAS1;
	}
	else if (chr == ('X' & 31)) {
	    if (nr < 2) {
		icp--;
		goto nomore;
	    }
	    nr--;
	    chr = *icp++ & 0177;
	    switch (chr) {
	    case CTRL ('a'):
		*ocp++ = CCSETFILE;
		break;
	    case CTRL ('b'):
		*ocp++ = CCSPLIT;
		break;
	    case CTRL ('c'):
		*ocp++ = CCCTRLQUOTE;
		break;
	    case CTRL ('e'):
		*ocp++ = CCERASE;
		break;
	    case CTRL ('h'):
		*ocp++ = CCLWINDOW;
		break;
	    case CTRL ('j'):
		*ocp++ = CCJOIN;
		break;
	    case CTRL ('l'):
		*ocp++ = CCRWINDOW;
		break;
	    case CTRL ('n'):
		*ocp++ = CCDWORD;
		break;
#ifdef RECORDING
	    case CTRL ('p'):
		*ocp++ = CCPLAY;
		break;
	    case CTRL ('r'):
		*ocp++ = CCRECORD;
		break;
#else
	    case CTRL ('r'):
		*ocp++ = CCREPLACE;
		break;
#endif
	    case CTRL ('t'):
		*ocp++ = CCTABS;
		break;
	    case CTRL ('u'):
		*ocp++ = CCBACKTAB;
		break;
	    case CTRL ('w'):
		*ocp++ = CCCHWINDOW;
		break;
	    default:
		*ocp++ = CCUNAS1;
		break;
	    }
	}
	else {
	    if( chr == CTRL('h') )
		*ocp++ = CCBACKSPACE;
	    else if( chr == CTRL('c') )
		*ocp++ = CCMOVELEFT;
	    else
		*ocp++ = lexstd[chr];
	}
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

int
kini_mac ()
{
    static char ini[] = {033,'='};
    fwrite (ini, sizeof ini, 1, stdout);
    return (0);
}

int
kend_mac ()
{
    /* leave the keypad enabled */
/*  static char end[] = {033,'>'};
    fwrite (end, sizeof end, 1, stdout);
 */
	return (0);
}


S_kbd kb_mac = {
/* kb_inlex */  in_mac,
/* kb_init  */  kini_mac,
/* kb_end   */  kend_mac,
};
