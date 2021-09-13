#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

Date:     Sun, 4 Jan 81 20:50-EST
From:     Szurko at UDel
Subject:  e.z19.c
To:       day@rand-unix

# include  "e.h"
# include  "e.tt.h"











extern int  z19_ilex(), z19_init(), z19_end(), z19_left(), z19_right(),
	    z19_down(), z19_up(), z19_cr(), z19_nl(), z19_clear(),
            z19_home(), z19_bksp(), z19_addr(), z19_xlate();

extern int  bad();

extern char  stdxlate[];








S_term  z19_term =
    {
    z19_ilex,             /*  tt_inlex  */
    z19_init,             /*  tt_init  */
    z19_end,              /*  tt_end  */
    z19_left,             /*  tt_left  */
    z19_right,            /*  tt_right  */
    z19_down,             /*  tt_down  */
    z19_up,               /*  tt_up  */
    z19_cr,               /*  tt_cret  */
    z19_nl,               /*  tt_nl  */
    z19_clear,            /*  tt_clear  */
    z19_home,             /*  tt_home  */
    z19_bksp,             /*  tt_bsp  */
    z19_addr,             /*  tt_addr  */
    bad,                  /*  tt_lad  */
    bad,                  /*  tt_cad  */
    z19_xlate,            /*  tt_xlate  */
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
    1,                    /*  tt_pwr  */
    0,                    /*  tt_axis  */
    NO,                   /*  tt_bullets  */
    YES,                  /*  tt_prtok  */
    80,                   /*  tt_width  */
    24                    /*  tt_height  */
    };












/*  terminal input analyzer  */

z19_ilex(lexp, count)
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
			case  'S':                         /*  F1  */

			    *op++ = CCINSMODE;
			    break;

			case  'T':                         /*  F2  */

			    *op++ = CCSETFILE;
			    break;

			case  'U':                         /*  F3  */

			    *op++ = CCOPEN;
			    break;

			case  'V':                         /*  F4  */

			    *op++ = CCCLOSE;
			    break;

			case  'W':                         /*  F5  */

			    *op++ = CCMARK;
			    break;

			case  'J':                         /*  ERASE  */

			    *op++ = CCINT;
			    break;

			case  'P':                         /*  BLUE  */

			    *op++ = CCPICK;
			    break;

			case  'Q':                         /*  RED  */

			    *op++ = CCREPLACE;
			    break;

			case  'R':                         /*  WHITE  */

			    *op++ = CCDELCH;
			    break;

			case  '?':

			    if (nleft < 3)
				goto  nomore;

			    c = *ip++ & 0177;

			    switch (c)
				{
				case  'p':                 /*  Keypad 0  */

				    *op++ = CCPLSRCH;
				    break;

				case  'q':                 /*  Keypad 1  */

				    *op++ = CCPLLINE;
				    break;

				case  'r':                 /*  Keypad 2  */

				    *op++ = CCMOVEDOWN;
				    break;

				case  's':                 /*  Keypad 3  */

				    *op++ = CCMILINE;
				    break;

				case  't':                 /*  Keypad 4  */

				    *op++ = CCMOVELEFT;
				    break;

				case  'u':                 /*  Keypad 5  */

				    *op++ = CCHOME;
				    break;

				case  'v':                 /*  Keypad 6  */

				    *op++ = CCMOVERIGHT;
				    break;

				case  'w':                 /*  Keypad 7  */

				    *op++ = CCPLPAGE;
				    break;

				case  'x':                 /*  Keypad 8  */

				    *op++ = CCMOVEUP;
				    break;

				case  'y':                 /*  Keypad 9  */

				    *op++ = CCMIPAGE;
				    break;

				case  'n':                 /*  Keypad .  */

				    *op++ = CCMISRCH;
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

/*  we come here when there are insufficent input characters to parse  */

    nomore:

	*count = nleft;
	nconv = op - lexp;
	ip = sp;

	while (nleft-- > 0)
	    *op++ = *ip++;

	return(nconv);
	}












z19_init()
	{

	fputs("\33=", stdout);
	}












z19_end()
	{

	fputs("\33>", stdout);
	}












/*  move cursor one space to the left  */

z19_left()
	{

	fputs("\33D", stdout);
	}












/*  move cursor one space to the right  */

z19_right()
	{

	fputs("\33C", stdout);
	}












/*  move cursor down one line  */

z19_down()
	{

	fputs("\33B", stdout);
	}












/*  move cursor up one line  */

z19_up()
	{

	fputs("\33A", stdout);
	}












/*  move cursor to column 0 of current line  (carriage return)  */

z19_cr()
	{

	putchar('\15');
	}












/*  move cursor to column 0 of next line  */

z19_nl()
	{

	fputs("\15\12", stdout);
	}












/*  clear screen  */

z19_clear()
	{

	fputs("\33E", stdout);
	}












/*  move cursor home  */

z19_home()
	{

	fputs("\33H", stdout);
	}












/*  move cursor to the left and erase character in that spot  */

z19_bksp()
	{


	fputs("\10 \10", stdout);
	}












/*  move cursor to random screen address  */

z19_addr(lin, col)
    register int  lin, col;
	{

	fputs("\33Y", stdout);
	putchar(lin + 32);
	putchar(col + 32);
	}












/*  translate funny characters  */

z19_xlate(c)
    unsigned int  c;
	{

	c = stdxlate[(c & 0377) - FIRSTSPCL];
	putchar(c);
	}

