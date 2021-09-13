#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

Date:     Sun, 4 Jan 81 20:49-EST
From:     Szurko at UDel
Subject:  e.info200.c
To:       day@rand-unix

# include  "e.h"
# include  "e.tt.h"











extern int  i200_ilex(), i200_init(), i200_end(), i200_left(), i200_right(),
	    i200_down(), i200_up(), i200_cr(), i200_nl(), i200_clear(),
            i200_home(), i200_bksp(), i200_addr(), i200_xlate();

extern int  bad();

extern char  stdxlate[];








S_term  i200_term =
    {
    i200_ilex,            /*  tt_inlex  */
    i200_init,            /*  tt_init  */
    i200_end,             /*  tt_end  */
    i200_left,            /*  tt_left  */
    i200_right,           /*  tt_right  */
    i200_down,            /*  tt_down  */
    i200_up,              /*  tt_up  */
    i200_cr,              /*  tt_cret  */
    i200_nl,              /*  tt_nl  */
    i200_clear,           /*  tt_clear  */
    i200_home,            /*  tt_home  */
    i200_bksp,            /*  tt_bsp  */
    i200_addr,            /*  tt_addr  */
    bad,                  /*  tt_lad  */
    bad,                  /*  tt_cad  */
    i200_xlate,           /*  tt_xlate  */
    1,                    /*  tt_nleft  */
    1,                    /*  tt_nright  */
    1,                    /*  tt_ndn  */
    1,                    /*  tt_nup  */
    2,                    /*  tt_nnl  */
    3,                    /*  tt_bksp  */
    3,                    /*  tt_naddr  */
    0,                    /*  tt_nlad  */
    0,                    /*  tt_ncad  */
    3,                    /*  tt_wl  */
    1,                    /*  tt_cwr  */
    1,                    /*  tt_pwr  */
    0,                    /*  tt_axis  */
    NO,                   /*  tt_bullets  */
    YES,                  /*  tt_prtok  */
    80,                   /*  tt_width  */
    24                    /*  tt_height  */
    };












/*  terminal input analyzer  */

i200_ilex(lexp, count)
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
		case  '\0':                                /*  F1  */

		    *op++ = CCINSMODE;
		    break;

		case  '\1':                                /*  F2  */

		    *op++ = CCPLLINE;
		    break;

		case  '\2':                                /*  F3  */

		    *op++ = CCMILINE;
		    break;

		case  '\3':                                /*  F4  */

		    *op++ = CCINT;
		    break;

		case  '\4':                                /*  F5  */

		    *op++ = CCPLSRCH;
		    break;

		case  '\5':                                /*  CTRL E  */

		    *op++ = CCLPORT;
		    break;

		case  '\6':                                /*  F6  */

		    *op++ = CCMISRCH;
		    break;

		case  '\7':                                /*  CTRL G  */
		case  '\20':                               /*  F7  */

		    *op++ = CCSETFILE;
		    break;

		case  '\10':                               /*  LEFT ARROW  */

		    *op++ = CCMOVELEFT;
		    break;

		case  '\11':                               /*  TAB  */

		    *op++ = CCTAB;
		    break;

		case '\12':                                /*  LINE FEED  */

		    *op++ = CCCMD;
		    break;

		case  '\13':                               /*  F13  */

		    *op++ = CCMIPAGE;
		    break;

		case  '\14':                               /*  CTRL L  */

		    *op++ = CCCHPORT;
		    break;

		case  '\15':                               /*  CR  */

		    *op++ = CCRETURN;
		    break;

		case  '\16':                               /*  F14  */

		    *op++ = CCMARK;
		    break;

		case  '\17':                               /*  F15  */

		    *op++ = CCDELCH;
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

		case  '\25':                               /*  F8  */

		    *op++ = CCREPLACE;
		    break;

		case  '\26':                               /*  F9  */

		    *op++ = CCPICK;
		    break;

		case  '\27':                               /*  CTRL W  */
		case  '\177':                              /*  DELETE  */

		    *op++ = CCBACKSPACE;
		    break;

		case  '\30':                               /*  F10  */

		    *op++ = CCOPEN;
		    break;

		case  '\31':                               /*  RIGHT ARROW  */

		    *op++ = CCMOVERIGHT;
		    break;

		case  '\32':                               /*  HOME  */

		    *op++ = CCHOME;
		    break;

		case  '\33':                               /*  ESCAPE  */

		    *op++ = CCTABS;
		    break;

		case  '\34':                               /*  UP ARROW  */

		    *op++ = CCMOVEUP;
		    break;

		case  '\35':                               /*  DOWN ARROW  */

		    *op++ = CCMOVEDOWN;
		    break;

		case  '\36':                               /*  F11  */

		    *op++ = CCCLOSE;
		    break;

		case  '\37':                               /*  F12  */

		    *op++ = CCPLPAGE;
		    break;

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
	}












/*  set initial terminal mode. not needed here.  */

i200_init()
	{
	}












/*  clear any modes set in init routine.  also unnecessary  */

i200_end()
	{
	}












/*  move cursor one space to the left  */

i200_left()
	{

	putchar('\10');
	}












/*  move cursor one space to the right  */

i200_right()
	{

	putchar('\31');
	}












/*  move cursor down one line  */

i200_down()
	{

	putchar('\35');
	}












/*  move cursor up one line  */

i200_up()
	{

	putchar('\34');
	}












/*  move cursor to column 0 of current line  (carriage return)  */

i200_cr()
	{

	putchar('\15');
	}












/*  move cursor to column 0 of next line  */

i200_nl()
	{

	fputs("\15\12", stdout);
	}












/*  clear screen  */

i200_clear()
	{

	putchar('\14');
	}












/*  move cursor home  */

i200_home()
	{

	putchar('\32');
	}












/*  move cursor to the left and erase character in that spot  */

i200_bksp()
	{


	fputs("\10 \10", stdout);
	}












/*  move cursor to random screen address  */

i200_addr(lin, col)
    register int  lin, col;
	{

	putchar('\27');
	putchar(col + 040);
	putchar(lin + 040);
	}












/*  translate funny ned characters  */

i200_xlate(c)
    unsigned int  c;
	{

	c = stdxlate[(c  & 0377) - FIRSTSPCL];
	putchar(c);
	}

