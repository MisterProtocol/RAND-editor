#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

Date:     Sun, 4 Jan 81 20:50-EST
From:     Szurko at UDel
Subject:  e.vt100.c
To:       day@rand-unix

/*
 *     VT100 terminal driver
 */




# include  "e.h"
# include  "e.tt.h"




extern int  v100_ilex(), v100_init(), v100_end(), v100_left(), v100_right(),
	    v100_down(), v100_up(), v100_cr(), v100_nl(), v100_clear(),
            v100_home(), v100_bksp(), v100_addr(), v100_xlate();

extern int  bad();

extern char  stdxlate[];








S_term  v100_term =
    {
    v100_ilex,            /*  tt_inlex  */
    v100_init,            /*  tt_init  */
    v100_end,             /*  tt_end  */
    v100_left,            /*  tt_left  */
    v100_right,           /*  tt_right  */
    v100_down,            /*  tt_down  */
    v100_up,              /*  tt_up  */
    v100_cr,              /*  tt_cret  */
    v100_nl,              /*  tt_nl  */
    v100_clear,           /*  tt_clear  */
    v100_home,            /*  tt_home  */
    v100_bksp,            /*  tt_bsp  */
    v100_addr,            /*  tt_addr  */
    bad,                  /*  tt_lad  */
    bad,                  /*  tt_cad  */
    v100_xlate,           /*  tt_xlate  */
    2,                    /*  tt_nleft  */
    2,                    /*  tt_nright  */
    2,                    /*  tt_ndn  */
    2,                    /*  tt_nup  */
    2,                    /*  tt_nnl  */
    3,                    /*  tt_bksp  */
    4,                    /*  tt_naddr  */
    0,                    /*  tt_nlad  */
    0,                    /*  tt_ncad  */
    3,                    /*  tt_wl  */
    3,                    /*  tt_cwr  */
    3,                    /*  tt_pwr  */
    0,                    /*  tt_axis  */
    NO,                   /*  tt_bullets  */
    YES,                  /*  tt_prtok  */
    80,                   /*  tt_width  */
    24                    /*  tt_height  */
    };












/*  terminal input analyzer  */

v100_ilex(lexp, count)
    char  *lexp;
    int  *count;
	{
	register char  *ip, *op;
	register int  nleft;
	char  c, *sp;
	int  nconv;

	ip = op = lexp;
	nleft = *count;

	while (nleft > 0)
	    {
	    sp = ip;
	    c = *ip++ & 0177;

	    switch (c)
		{
		case  '\3':                                /*  CTRL C  */

		    *op++ = CCINT;
		    break;

		case  '\5':                                /*  CTRL E  */

		    *op++ = CCLPORT;
		    break;

		case  '\7':                                /*  CTRL G  */

		    *op++ = CCSETFILE;
		    break;

		case  '\10':                               /*  BACKSPACE  */
		case  '\27':                               /*  CTRL W  */
		case  '\177':                              /*  DELETE  */

		    *op++ = CCBACKSPACE;
		    break;

		case  '\11':                               /*  TAB  */

		    *op++ = CCTAB;
		    break;

		case  '\15':                               /*  CR  */

		    *op++ = CCRETURN;
		    break;

		case  '\21':                               /*  CTRL  Q  */

		    *op++ = CCCTRLQUOTE;
		    break;

		case  '\23':                               /*  CTRL  S  */

		    *op++ = CCRPORT;
		    break;

		case  '\24':                               /*  CTRL T  */

		    *op++ = CCBACKTAB;
		    break;

		    break;

		case  '\33':

		    if (nleft < 2)
			goto  nomore;

		    c = *ip++ & 0177;

		    switch (c)
			{
			case  'A':                         /*  UP ARROW  */

			    *op++ = CCMOVEUP;
			    break;

			case  'B':                         /*  DOWN ARROW  */

			    *op++ = CCMOVEDOWN;
			    break;

			case  'C':                         /*  RIGHT ARROW  */

			    *op++ = CCMOVERIGHT;
			    break;

			case  'D':                         /*  LEFT ARROW  */

			    *op++ = CCMOVELEFT;
			    break;

			case  'P':                         /*  PF1  */

			    *op++ = CCINSMODE;
			    break;

			case  'Q':                         /*  PF2  */

			    *op++ = CCMARK;
			    break;

			case  'R':                         /*  PF3  */

			    *op++ = CCSETFILE;
			    break;

			case  'S':                         /*  PF4  */

			    *op++ = CCDELCH;
			    break;

			case  '?':

			    if (nleft < 3)
				goto  nomore;

			    c = *ip++ & 0177;

			    switch (c)
				{
				case  'p':                 /*  Keypad 0  */

				    *op++ = CCINT;
				    break;

				case  'q':                 /*  Keypad 1  */

				    *op++ = CCPLSRCH;
				    break;

				case  'r':                 /*  Keypad 2  */

				    *op++ = CCMISRCH;
				    break;

				case  's':                 /*  Keypad 3  */

				    *op++ = CCMILINE;
				    break;

				case  't':                 /*  Keypad 4  */

				    *op++ = CCPLLINE;
				    break;

				case  'u':                 /*  Keypad 5  */

				    *op++ = CCMILINE;
				    break;

				case  'v':                 /*  Keypad 6  */

				    *op++ = CCUNAS1;
				    break;

				case  'w':                 /*  Keypad 7  */

				    *op++ = CCPLPAGE;
				    break;

				case  'x':                 /*  Keypad 8  */

				    *op++ = CCMIPAGE;
				    break;

				case  'y':                 /*  Keypad 9  */

				    *op++ = CCUNAS1;
				    break;

				case  'n':                 /*  Keypad .  */

				    *op++ = CCPICK;
				    break;

				case  'm':                 /*  Keypad -  */

				    *op++ = CCOPEN;
				    break;

				case  'l':                 /*  Keypad ,  */

				    *op++ = CCCLOSE;
				    break;

				case  'M':                 /*  Keypad ENTER  */

				    *op++ = CCCMD;
				    break;

				default:

				    *op++ = CCUNAS1;
				}

			    nleft -= 3;
			    continue;

			default:

			    *op++ = CCUNAS1;
			}

		    nleft -= 2;
		    continue;

		default:

		    if (c >= ' ')
			*op++ = c;
		    else
			*op++ = CCUNAS1;

		}

	    nleft--;
	    continue;
	    }

/*  normal finish -- all input characters converted  */

	*count = 0;
	nconv = op - lexp;
	return(nconv);

/*  we come here when there are insufficent characters to parse  */

    nomore:

	*count = nleft;
	nconv = op - lexp;
	ip = sp;

	while (nleft-- > 0)
	    *op++ = *ip++;

	return(nconv);
	}












/*  set initial terminal mode, in this case alternate keypad  */

v100_init()
	{

	fputs("\33=", stdout);
	}












/*  clear any modes set in init routine -- disable alternate keypad  */

v100_end()
	{

	fputs("\33>", stdout);
	}












/*  move cursor one space to the left  */

v100_left()
	{

	fputs("\33D", stdout);
	}












/*  move cursor one space to the right  */

v100_right()
	{

	fputs("\33C", stdout);
	}












/*  move cursor down one line  */

v100_down()
	{

	fputs("\33B", stdout);
	}












/*  move cursor up one line  */

v100_up()
	{

	fputs("\33A", stdout);
	}












/*  move cursor to column 0 of current line  (carriage return)  */

v100_cr()
	{

	putchar('\15');
	}












/*  move cursor to column 0 of next line  *

