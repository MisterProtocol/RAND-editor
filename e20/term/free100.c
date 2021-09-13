/*
**      Driver for the Liberty Electronics Freedom 100 terminal.
**
**      The Freedom 100 uses the termcap terminal driver for output.
**      The termcap entry for the free100 used to test this routine is:
**
**  Mdl|free100|f100l|Liberty Freedom 100:\
**          :vb=\Eb\200\200\200\200\200\200\200\200\200\200\200\200\200\200\200\Ed:\
**          :tc=fr100:
**  Mdm|fr100|Liberty Freedom 100, no vis-bell:\
**          :am:bs:bw:mi:ms:pt:co#80:kn#20:li#24:ip=6:\
**          :ct=\E3:st=\E1:kr=^L:\
**          :bl=^G:cl=^Z:cr=^M:do=^J:ho=^^:kb=^H:kl=^H:\:kd=^V:\
**          :ko=dc,al,dl,cl,bt,ce,cd:ku=^K:le=^H:nd=^L:nl=^J:\
**          :ch=\E]%+ :cm=\E=%+ %+ :cv=\E[%+ :sr=\Ej:ta=^I:up=^K:\
**          :al=8.5*\EE:bt=\EI:cd=\EY:ce=\ET:dc=\EW:dl=11.5*\ER:ei=\Er:im=\Eq:\
**          :se=\EG0:so=\EG4:ue=\EG0:us=\EG8:as=\E$:ae=\E%:\
**          :k1=^A@\r:k2=^AA\r:k3=^AB\r:k4=^AC\r:k5=^AD\r:\
**          :k6=^AE\r:k7=^AF\r:k8=^AG\r:k9=^AH\r:k0=^AI\r:\
**          :hs:ts=\E^O\Eg\Ef:fs=\r:ds=\Eg\Ef\r:\
**          :Gs=\E$:Ge=\E%:Tl=F:Tr=G:Tj=N:Bl=E:Br=H:Bj=O:Lj=M:Rj=L:Cj=I:Hl=K:Vl=J:\
**          :ma=^Kk^Vj^Hh^Ll^^H:mh=\EG@:mr=\EG4:me=\EG0:\
**          :is=\EA\E"\EC\E\^\EX\E'\EG0\EO\El\Er^O^T\EDF\Ea\Ee \E%\Eg\Ef\r\Ed:
**
**      Notice the primitives for line drawing in the 3rd to last line;
**      see comments in term/tcap.c about that.
*/

#define CTRL(x) ((x) & 31)
#define _BAD_ CCUNAS1

extern Cmdret help_std();

in_free100 (lexp, count)
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
	    if ('@' <= chr && chr <= 'I') {
		static char xlt[] = {
		    CCMIPAGE,           /* fk1 */
		    CCPLPAGE,           /* fk2 */
		    CCMILINE,           /* fk3 */
		    CCPLLINE,           /* fk4 */
		    CCLWINDOW,          /* fk5 */
		    CCRWINDOW,          /* fk6 */
		    CCOPEN,             /* fk7 */
		    CCCLOSE,            /* fk8 */
		    CCMARK,             /* fk9 */
		    CCPICK              /* fk10 */
		};
		*ocp++ = xlt [chr - '@'];
		nr--; icp++;    /* burn the cr */
	    } else if ('`' <= chr && chr <= 'i') {
		static char xlt[] =
		{
		    CCCHWINDOW,         /* fk1 shifted */
		    CCSETFILE,          /* fk2 shifted */
		    CCREDRAW,           /* fk3 shifted */
		    CCCAPS,             /* fk4 shifted */
		    CCCCASE,            /* fk5 shifted */
		    CCAUTOFILL,         /* fk6 shifted */
		    CCCLRTABS,          /* fk7 shifted */
		    CCTABS,             /* fk8 shifted */
		    CCTRACK,            /* fk9 shifted */
		    CCRANGE             /* fk10 shifted */
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
	    case 'I':
		*ocp++ = CCBACKTAB;
		break;
	    case 'E':
		*ocp++ = CCINSMODE;
		break;
	    case 'L':
		*ocp++ = CCPLSRCH;
		break;
	    case 'P':
		*ocp++ = CCHELP;
		break;
	    case 'Q':
		*ocp++ = CCDELCH;
		break;
	    case 'R':
		*ocp++ = CCMISRCH;
		break;
	    case 'T':
		*ocp++ = CCSPLIT;
		break;
	    case 'W':
		*ocp++ = CCREPLACE;
		break;
	    case 'Y':
		*ocp++ = CCUNAS1;
		break;
	    case 'a':                    /* characters following esc */
		*ocp++ = CCABORT;
		break;
	    case 'b':
		*ocp++ = CCBOX;
		break;
	    case 'f':
		*ocp++ = CCFILL;
		break;
	    case 'j':
		*ocp++ = CCJOIN;
		break;
	    case 'k':
		*ocp++ = CCJUSTIFY;
		break;
	    case 'q':
		*ocp++ = CCQUIT;
		break;
	    case 's':
		*ocp++ = CCSPLIT;
		break;
	    case 'x':
		*ocp++ = CCEXIT;
		break;
	    case 'z':
		*ocp++ = CCSTOPX;
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
		    CCRETURN,       /* ^J */
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
		    CCMOVEDOWN,     /* ^V */
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

S_kbd kb_free100 = {
/* kb_inlex */  in_free100,
/* kb_init  */  nop,
/* kb_end   */  nop
};

