#ifdef COMMENT
--------
file e.la.c
    Stuff related to the LA Package.
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#include "e.h"

#include "e.m.h"

#include SIG_INCL

extern void excline (Ncols);
extern void chkcline ();
extern void shortenline();
extern void elasdump ();
extern int la_error ();
extern Flag putline ();

#ifdef COMMENT
void
GetLine (ln)
    Nlines  ln;
.
    Gets line ln of the current workspace into cline.
    If (ln < 0) then flush cline if modified
      and set clinelas to 0 so that the next call to GetLine is
      guaranteed to get a fresh copy of the line desired from the file.
#endif
void
GetLine (ln)
Nlines  ln;
{
    Reg5 char *clp;     /* cline pointer */
    Reg6 char *endcl;
    La_stream *rlas;

/*  dbgpr ("GetLine: *** old %d, new %d, clinelas %x, curfile %d\n",
	   clineno, ln, clinelas, curfile); */

/* 1/2003, try to locate SIGSEGV cause*/
/*  if (ln < 0) { */
    if (ln < 0 || curlas->la_file == NULL) {
	if (fcline)
	 /* putline (NO); */
	    putline ();
	clinelas = (La_stream *) 0;
	return;
    }

    if (ln >= la_lsize (curlas)) {
	if (fcline)
	 /* putline (NO); */
	    putline ();
	xcline = YES;         /* past end of file */
	/* make up a blank line */
	clinelas = curlas;
	clineno = ln;
	ecline = NO;
	cline8 = NO;
	fcline = NO;
	ncline = 1;
	cline[0] = '\n';
	return;
    }

    if ( clinelas &&  clinelas->la_file == curlas->la_file
	&& ln == clineno
       ) {
     /* dbgpr ("  Have it\n"); */
	return;  /* already have the line */
    }

    if (fcline)
	 /* putline (NO); */
	    putline ();

    clinefn = curfile;
    clinelas = rlas = curlas;

 /* dbgpr ("About to seek to line %d\n", ln); */
    (void) la_lseek (rlas, (La_linepos) ln, 0);

    ecline = NO;
    cline8 = NO;
    fcline = NO;
    ncline = 0;
    xcline = NO;

    clineno = ln;

    clp = cline;
    endcl = &cline[lcline - (TABCOL + 1)];
    for (;;) Block {
	Reg4 Uchar *cp;
	/*Reg3 Small nleft;*/
	Reg3 Ncols nleft;
	Uchar *bufp;
     /* elasdump (rlas, "before getlin");       */
	if ((nleft = la_lpnt (rlas, &bufp)) <= 0)
	  /*fatal (FATALIO, "GetLine read err #%d %d",*/
	    fatal (FATALIO, "GetLine read err #%d nleft=%ld",
		   la_error (), nleft);
    /*  dbgpr("GetLine: nleft=%ld\n", nleft);*/
	cp = bufp;
	while (--nleft >= 0) Block {
	    Reg2 Char ch;
	    /* dbg */
	    /*Ncols endcl_clp = (Ncols)(endcl-clp);*/
	    /**dbgpr("GetLine, --nleft bef excline, ncline=(%ld) nleft=(%ld) endcl=(%d)\n", ncline,nleft,endcl);**/
	    if (clp > endcl) Block {    /* check if room in cline */
		Reg1 Ncols i1;
		/*ncline = (i1 = clp - cline) + 1;*/
		ncline = (i1 = (Ncols)(clp - cline)) + 1;
		excline ((Ncols) 0);          /* 8 is max needed (for tab)        */
		clp = &cline[i1];
		endcl = &cline[lcline - 9];
		/*dbg*/
		/*endcl_clp = (Ncols)(endcl-clp);*/
		/**dbgpr("GetLine, clp > endcl--nleft bef excline, ncline=(%ld) nleft=(%ld) endcl=(%d)\n", ncline,nleft,endcl);**/

	    }
    /** dbgpr("GetLine, --nleft aft excline, ncline=(%ld) nleft=(%ld)\n", ncline,nleft); **/
	    //if(ncline>200000L) {
	    //  fatal (FATALIO, "GetLine while, ncline %ld", ncline);
	    //}

	    ch = *cp++ & 0177;
	    if ((ch >= 040 && ch < 0177))
		*clp++ = ch;
	    else if (ch == '\t'&& !litmode) Block {
    /*      else if (ch == '\t') Block {  */
		Reg1 Ncols ri;
		for (ri = 8 - (07 & (clp - cline)); ri--; )
		    *clp++ = ' ';
	    }
	    else if (ch == '\n') {
#ifdef out
		while (--clp >= cline && (*clp == ' ' || *clp == '\t'))
		    continue;
		*++clp = '\n';
#endif /* out */
		if (!upnostrip){
		    while (--clp >= cline && (*clp == ' ' || *clp == '\t'))
			continue;
		    clp++;
		}
		*clp = '\n';
		ncline = clp - cline + 1;
		return;
	    }
	    else {
		{
		    ecline = YES;   /* line has control char        */
		    *clp++ = ESCCHAR;
		    if (ch != ESCCHAR)
			ch |= 0100;
		}
		*clp++ = ch;
	    }
	}
    }
    /* returns from inside while loop */
    /* NOTREACHED */
}

#ifdef COMMENT
Flag
putline (allflg)
    Flag allflg;
.
    If fcline != 0, inserts the line in cline in place of the current one.
    Returns YES is successful, NO if not.
    In future, several clines may be cached, and if allflg == YES, putline
    will flush them all out.
#endif
Flag
/*putline (Flag allflg)  */
putline ()
{
    La_stream tlas;
    int nr;

/*  fatal (FATALBUG, "putline called"); */
 /* dbgpr ("Doing putline of line %d\n", clineno);  */

#ifdef lint
    if (allflg)
	allflg = allflg;
#endif

    if (!fcline)
	return YES;
 /* dbgpr ("  fcline is YES\n"); */

    if (!la_clone (clinelas, &tlas))
	return NO;
 /* dbgpr ("  clone ok\n"); */

    (void) la_lseek (&tlas, clineno, 0);
    cline[ncline - 1] = '\n';
    chkcline ();
    nr = dechars (cline);
    fcline = NO;
    clinelas = (La_stream *) 0; /* force a GetLine next time */
    Block {
	La_linepos ln;
	La_linepos nins;
	ln = 1;
	nins = la_lreplace (&tlas, cline, nr, &ln,
			    la_lwsize (&tlas) > 1
			    ? &fnlas[OLDLFILE]
			    : (La_stream *) 0);
	(void) la_close (&tlas);
     /* dbgpr ("  Done with putline\n"); */
	if (la_lsize (&tlas) < clineno)
	    fatal (FATALBUG, "lsize (%d) < clineno (%d)",
		   la_lsize (&tlas), clineno);
	if (nins < 1)
	    return NO;
	return YES;
    }
}

#ifdef COMMENT
void
chkcline ()
.
    Check cline for a ^J before the end of the line.  If there is one,
    truncate the line and redisplay it.
#endif
void
chkcline ()
{
    Reg2 char  *fm;

    if (!ecline)
	return;
    fm = cline;
    for (;;) Block {
	Reg1 Char cc;   /* current character */
	if ((cc = *fm++) == ESCCHAR && *fm != '\n') {
	    if ((cc = (*fm == ESCCHAR) ? *fm++ : (*fm++ & 037)) == '\n') {
		fm -=2;
		*fm++ = '\n';
		break;
	    }
	}
	if (cc == '\n')
	    break;
    }
    if (fm - cline < ncline) {
	ncline = fm - cline;
	redisplay (clinefn, clineno, 1, 0, YES);
    }
    return;
}

#ifdef COMMENT
Ncols
dechars (line)
    char   *line;
.
    Performs in-place character conversion from internal
    to external format of the characters starting at line up to a newline or
    a ^J.  May alter contents of line.  Line MUST have a newline in it
    note: replaces initial spaces with tabs; deletes trailing spaces
    returns number of characters in the converted, external representation
#endif
Ncols
dechars (line)
char   *line;
{
    Reg5 Ncols   cn;                   /* col number                 */
    Reg4 char  *fm;
    Reg3 char  *to;           /* pointers for move          */
    Reg2 Ncols  lnb;          /* 1 + last non-blank col     */

    fm = to = line;
    cn = 0;
    lnb = 0;
/*****                  Added Purdue CS 2/8/83  MAB             */
    if (upblanks)
	for (;;) Block {
	    Reg1 Char    cc;    /* current character          */
	    if ((cc = *fm++) == ESCCHAR && *fm != '\n')
		cc = (*fm == ESCCHAR) ? *fm++ : (*fm++ & 037);
	    if (cc == '\n')
		break;
	    *to++ = cc;
	    ++cn;
	}
    else if (uptabs){
#define MAXLINE                 10000           /* some large number */
#define nxttab(x)               (((x%8)==0)?((x)+8):(((x)&~07)+8))
	lnb = MAXLINE;
	for (;;) Block {
	    Reg1 Char    cc;    /* current character          */
	    if ((cc = *fm++) == ESCCHAR && *fm != '\n')
		cc = (*fm == ESCCHAR) ? *fm++ : (*fm++ & 037);
	    if (cc == ' ' && lnb == MAXLINE)
		lnb = cn;
	    if (cc != ' ') {
		while (nxttab(lnb) <= cn){
		    *to++ = '\t';
		    lnb = nxttab(lnb);
		}
		while (lnb < cn){
		    *to++ = ' ';
		    lnb++;
		}
		if (cc == '\n')
		    break;
		*to++ = cc;
		lnb = MAXLINE;
	    }
	    ++cn;
	}
    }
    else if (litmode)
	for (;;) Block {
	    Reg1 Char    cc;    /* current character          */
	    if ((cc = *fm++) == ESCCHAR && *fm != '\n')
		cc = (*fm == ESCCHAR) ? *fm++ : (*fm++ & 037);
	    if (cc == '\n')
		break;
	    *to++ = cc;
	    ++cn;
	}
    else
/*****                  End of Purdue Addition                  */
	for (;;) Block {
	    Reg1 Char    cc;    /* current character          */
	    if ((cc = *fm++) == ESCCHAR && *fm != '\n')
		cc = (*fm == ESCCHAR) ? *fm++ : (*fm++ & 037);
	    if (cc != ' ') {
		if (lnb == 0)
		    while (8 + (lnb & ~7) <= cn) {
			*to++ = (lnb & 7) == 7 ? ' ' : '\t';
			lnb &= ~7;
			lnb += 8;
		    }
		while (++lnb <= cn)
		    *to++ = ' ';
		if (cc == '\n')
		    break;
		*to++ = cc;
	    }
	    ++cn;
	}
/*****                  Added Purdue CS 2/8/83  MAB             */
    if (!upnostrip){
	while(--to >= line && (*to == ' ' || *to == '\t'));
	to++;
    }
/*****                  End of Purdue Addition                  */
    *to++ = '\n';
    return to - line;
}

#ifdef OUT
#ifdef COMMENT
Ncols
dechars (line)
    char   *line;
.
    Performs in-place character conversion from internal
    to external format of the characters starting at line up to a newline or
    a ^J.  May alter contents of line.  Line MUST have a newline in it
    note: replaces initial spaces with tabs; deletes trailing spaces
    returns number of characters in the converted, external representation
#endif
Ncols
dechars (line)
char   *line;
{
    Reg5 Ncols   cn;                   /* col number                 */
    Reg4 char  *fm;
    Reg3 char  *to;           /* pointers for move          */
    Reg2 Ncols  lnb;          /* 1 + last non-blank col     */

    fm = to = line;
    cn = 0;
    lnb = 0;
    for (;;) Block {
	Reg1 Char    cc;    /* current character          */
	if ((cc = *fm++) == ESCCHAR && *fm != '\n')
	    cc = (*fm == ESCCHAR) ? *fm++ : (*fm++ & 037);
	if (cc == '\n')
	    break;
	if (cc != ' ') {
	    if (lnb == 0)
		while (8 + (lnb & ~7) <= cn) {
		    *to++ = (lnb & 7) == 7 ? ' ' : '\t';
		    lnb &= ~7;
		    lnb += 8;
		}
	    while (++lnb <= cn)
		*to++ = ' ';
	    *to++ = cc;
	}
	++cn;
    }
    *to++ = '\n';
    return to - line;
}
#endif /* OUT */

#ifdef COMMENT
void
shortencline ()
.
    Shorten cline if necessary so that there are no trailing blanks or tabs,
    and make sure cline[ncline -1] = '\n';
#endif
void
shortencline ()
{
    Reg1 char *cp;
    Reg2 char *rcline = cline;

    if (ncline <= 1)
	return;

    cp = &rcline[ncline - 2];
    if (!upnostrip) {
	for ( ; cp >= rcline && (*cp == ' ' || *cp == '\t') ; cp-- )
	    continue;
    }
    *++cp = '\n';
    ncline = cp - rcline + 1;
    return;
}

#ifdef COMMENT
void
excline (length)
    Ncols length;
.
    Expand cline to max (length, lcline + icline)
#endif
void
excline (length)
Ncols length;
{
    Reg1 Ncols  j;
    Reg2 char  *tmp;

    if ((j = lcline + icline + 2) < length)
	j = length;
    if (j < ncline) {
    /**/ dbgpr("excline FIX: j=%ld, length=%ld, lcline=%ld, icline=%ld, ncline=%ld\n", j, length, lcline, icline, ncline); /**/
	/* j = ncline + 1; */
    }

    /*tmp = salloc ((int) (j + 1), YES);*/
/**   dbgpr("excline: calling salloc, j=(%ld) ncline=(%ld) lcline=(%ld) icline=(%ld)\n",
      j, ncline, lcline, icline);  **/
    tmp = salloc ((Ncols) (j + 1), YES);
    if (tmp == NULL) {
	dbgpr("excline: null return!\n");
	fatal (LAFATALBUG, "NULL return from salloc()\n");
    }
    if (ncline > 0) {
	/*move (cline, tmp, (Uint) ncline);*/
/**	dbgpr("excline: my_move(%x, %x, %d);\n", cline, tmp, ncline); **/
	my_move (cline, tmp, (ulong) ncline);
    }
    icline += icline / 2; /* should we have a max here? */
    lcline = j;
    sfree (cline);
    cline = tmp;
    return;
}

#ifdef COMMENT
Flag
extend (ext)
    Nlines ext;
.
    Extend the file with ext blank lines so that
	la_lsize (curlas) = line
#endif
Flag
extend (ext)
Reg1 Nlines ext;
{
    if (ext > 0) {
	(void) la_lseek (curlas, 0, 2);
	if (la_blank (curlas, ext) != ext)
	    return NO;
    }
    return YES;
}

#ifdef COMMENT
Nlines
lincnt (stline, num, type)
    Nlines  stline;
    Nlines  num;
    Flag    type;
.
    Type: -2 and 2 - paragraphs backward and forward
    Type: -1 and 1 - lines backward and forward
    Return the actual number of lines.  End of file can cause
    the number to be less than num.
#endif
Nlines
lincnt (stline, num, type)
Nlines  stline;
Nlines  num;
Flag    type;
{
  /*putline (YES);*/
    putline ();
    return la_lcount (curlas, stline, num, type);
}

