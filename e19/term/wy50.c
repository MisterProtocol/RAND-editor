/*
**      Driver for the Wyse 50 terminal.
**
**      The Wyse 50 uses the termcap terminal driver for output.
**      The termcap entry for the wy50/wy50w used to test this routine is:
**
**   wyse50|wy50|wyse 50 80col:\
**           :li#24:co#80:am:bs:hs:\
**           :al=\EE:bl=^G:bt=\EI:ce=\ET:cm=\E=%+ %+ :\
**           :cl=\E*:cd=\EY:cr=^M:ct=\E0:do=^J:\
**           :is=\Eu\E`2\E"\E'\E(\EC\EDF\EO\EX\Er\E\072:\
**           :dc=\EW:dl=\ER:ei=\Er:ho=^^:im=\Eq:it#4:\
**           :kA=\EE:kb=^H:kC=\EY:kD=\EW:kd=^J:kE=\ET:kF=\Er:kR=\Eq:\
**           :kH=\E{:kh=^^:kI=\EQ:kL=\ER:kl=^H:kN=\EK:kP=\EJ:kS=\EY:\
**           :kr=^L:ku=^K:ug#1:sg#1:le=^H:mb=\E!2:md=\E!0:\
**           :mh=\E!p:mk=\E!1:mi:mp=\E):mr=\EA04:ms:nd=^L:nl=^J:\
**           :se=\EG0:so=\EG4:sr=\Ej:up=^K:ue=\EG0:us=\EG8:\
**           :ts=\EF:fs=\r:ws#44:ds=\EF\r:\
**           :k1=^A@\r:k2=^AA\r:k3=^AB\r:k4=^AC\r:k5=^AD\r:k6=^AE\r:\
**           :k7=^AF\r:k8=^AG\r:k9=^AH\r:10=^AI\r:11=^AJ\r:12=^AK\r:\
**           :13=^AL\r:14=^AM\r:15=^AN\r:16=^AO\r:\
**           :vb=\EA04\200\200\200\200\200\200\200\200\200\200\200\200\200\EA00:\
**           :vi=\E`1:ve=\E`2:vs=\E`1:\
**           :Gs=\EH^B:Ge=\EH^C:Tl=2:Tr=3:Tj=0:Bl=1:Br=5:Bj==:Lj=4:Rj=9:\
**           :Vl=6:Hl=\072:Cj=8:Xc=;:
**   wy50w|wyse50w|wyse 50 132 cols:\
**           :is=\Eu\E`2\E"\E'\E(\EC\EDF\EO\EX\Er\E;:\
**           :co#132:ws#98:tc=wy50:
*/

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1

in_wy50 (lexp, count)
char *lexp;
int *count;
{
    Reg1 int chr;
    Reg2 int nr;
    Reg3 char *icp;
    Reg4 char *ocp;

    icp = ocp = lexp;
    nr = *count;
    for (; nr > 0; nr--)
    {
	/* RAW MODE on V7 inputs all 8 bits so we and with 0177 */
	if ((chr = *icp++ & 0177) >= 040)
	    *ocp++ = (chr == 0177) ? CCBACKSPACE : chr;
	else if (chr == 1) {
	    if (nr < 3) {         /* need at least soh-x-cr */
		icp--;            /* backout to before esc */
		goto nomore;
	    }
	    nr--; chr = *icp++ & 0177;   /* count off the esc */
	    if ('@' <= chr && chr <= 'O') {
		static char xlt[] = {
		    CCMIPAGE,           /* fk1 */
		    CCPLPAGE,           /* fk2 */
		    CCMILINE,           /* fk3 */
		    CCPLLINE,           /* fk4 */
		    CCLWORD,            /* fk5 */
		    CCRWORD,            /* fk6 */
		    CCDWORD,            /* fk7 */
		    CCREDRAW,           /* fk8 */
		    CCSETFILE,          /* fk9 */
		    CCCHWINDOW,         /* fk10 */
		    CCMARK,             /* fk11 */
		    CCPICK,             /* fk12 */
		    CCCMD,              /* fk13 */
		    CCMISRCH,           /* fk14 */
		    CCREPLACE,          /* fk15 */
		    CCPLSRCH,           /* fk16 */
		};
		*ocp++ = xlt [chr - '@'];
		nr--; icp++;    /* burn the cr */
	    } else if ('`' <= chr && chr <= 'o') {
		static char xlt[] =
		{
		    CCHELP,             /* fk1 shifted */
		    CCCOVER,            /* fk2 shifted */
		    CCLWINDOW,          /* fk3 shifted */
		    CCRWINDOW,          /* fk4 shifted */
		    CCCCASE,            /* fk5 shifted */
		    CCCAPS,             /* fk6 shifted */
		    CCRANGE,            /* fk7 shifted */
		    CCTRACK,            /* fk8 shifted */
		    CCCENTER,           /* fk9 shifted */
		    CCFILL,             /* fk10 shifted */
		    CCJUSTIFY,          /* fk11 shifted */
		    CCERASE,            /* fk12 shifted */
		    CCCMD,              /* fk13 shifted */
		    CCUNAS1,            /* fk14 shifted */
		    CCCLRTABS,          /* fk15 shifted */
		    CCTABS,             /* fk16 shifted */
		};
		*ocp++ = xlt [chr - '`'];
		nr--; icp++;    /* burn the cr */
	    } else
		*ocp++ = CCUNAS1;
	} else if (chr == '\033') {       /* escape seq */
	    if (nr < 2) {
		icp--;            /* backout to before esc */
		goto nomore;
	    }
	    nr--; chr = *icp++ & 0177;   /* count off the esc */
	    switch (chr) {
	    case 'B':
		*ocp++ = CCBOX;
		break;
	    case 'E':
	    case 'r':
		*ocp++ = CCCLOSE;
		break;
	    case 'I':
		*ocp++ = CCBACKTAB;
		break;
	    case 'j':
		*ocp++ = CCJOIN;
		break;
	    case 'J':
	    case 'K':
		*ocp++ = CCINT;
		break;
	    case 'P':
		*ocp++ = CCAUTOFILL;
		break;
	    case '7':
		*ocp++ = CCREDRAW;
		break;
	    case 'Q':
	    case 'q':
		*ocp++ = CCINSMODE;
		break;
	    case 'R':
	    case 'W':
		*ocp++ = CCDELCH;
		break;
	    case 's':
		*ocp++ = CCSPLIT;
		break;
	    case 'T':
	    case 'Y':
		*ocp++ = CCOPEN;
		break;
	    case 'b':
		*ocp++ = CCBOX;
		break;
	    case 'x':
		*ocp++ = CCEXIT;
		break;
	    case 'z':
		*ocp++ = CCSTOPX;
		break;
	    case '{':
		*ocp++ = CCCMD;
		break;
	    case 033:
		*ocp++ = CCCTRLQUOTE;
		break;
	    default:
		*ocp++ = CCUNAS1;
		break;
	    }
	} else if ('\0' <= chr && chr <= '\037') Block {
		static char xlt[] = {
		    CCCMD,          /* ^@ */
		    CCNULL,         /* ^A  cannot use - function keys */
		    CCLWORD,        /* ^B */
		    CCCLEAR,        /* ^C */
		    CCCENTER,       /* ^D */
		    CCERASE,        /* ^E */
		    CCCAPS,         /* ^F */
		    CCHOME,         /* ^G */
		    CCMOVELEFT,     /* ^H */
		    CCTAB,          /* ^I */
		    CCMOVEDOWN,     /* ^J */
		    CCMOVEUP,       /* ^K */
		    CCMOVERIGHT,    /* ^L */
		    CCRETURN,       /* ^M */
		    CCRWORD,        /* ^N */
		    CCOVERLAY,      /* ^O */
		    CCCOVER,        /* ^P */
		    CCNULL,         /* ^Q    must be left unassigned */
		    CCREDRAW,       /* ^R */
		    CCNULL,         /* ^S    must be left unassigned */
		    CCTABS,         /* ^T */
		    CCHELP,         /* ^U */
		    CCCLOSE,        /* ^V */
		    CCCHWINDOW,     /* ^W */
		    CCEXIT,         /* ^X */
		    CCCLRTABS,      /* ^Y */
		    CCCMD,          /* ^Z */
		    CCUNAS1,        /* ^[    must be left unassigned */
		    CCINT,          /* ^\ */
		    CCUNAS1,        /* ^] */
		    CCCMD,          /* ^^ */
		    CCRANGE         /* ^_ */
		};
		*ocp++ = xlt [chr - '\0'];  /* Yeh, I know... */
	    }
	    else
		*ocp++ = CCUNAS1;
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

char labtab[] =
  "-PAG +PAG -LIN +LIN  -wrd +wrd dwrd auto ALT CGW MARK PICK  cmd -src repl +src";
char slabtab[] =
  "HELP COVR WINL WINR ccas caps rang trak CNTR FILL JUST ERAS cmd      clrt sctb";


ki_wy50 ()
{
	int i;

	for (i = 0; i < 16; i++)
		printf ("\033z%c\001%c\r\177\033z%c\001%c\r\177",
			i+'@', i+'@', i+'`', i+'`');
	printf ("\033z(%s\r\033z)%s\r", labtab, slabtab);
}

ke_wy50 ()
{
	int pid;

	printf ("\033z(\r\033z)\r");
	fflush (stdout);
	if ( ! (pid = fork())) {
		(void) execl ("/etc/e/setwyse50", "setwyse50", 0);
		printf ("\ne: Cannot exec setwyse50.\n");
		exit (0);
	} else
		(void) wait (0);
}

S_kbd kb_wy50 = {
/* kb_inlex */  in_wy50,
/* kb_init  */  ki_wy50,
/* kb_end   */  ke_wy50,
};
