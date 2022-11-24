#ifdef COMMENT
--------
file e.brace.c
    brace matching
#endif


#include "e.h"
#include "e.tt.h"

extern Uchar *image;

extern char *smso, *rmso, *bold_str, *setab, *setaf, *sgr0, *setab_p, *setaf_p, *hilite_str;
extern char *brace_p;   /* hilight mode for brace matching */
extern char *font_red;


void HiLightBracePairs(Flag);
int brace_marked;
extern void GetLine();
extern int braceRange;  /* limit match to +/- braceRange lines */
int win_has_eof();

extern int Pch(int ch);

/* brace matching/highlighting */
int bracematching;
extern Flag bracematchCoding;

/*
 *   visually mark matching pairs: {}, [], ()
 */
void
HiLightBracePairs(Flag set)      /* 0 when we're only clearing a mark */
{
    int w = term.tt_width;
    char buf[w+1];
    char *cp;

    int line;
    int from, to;
    int lcol, rcol;

    /* save these values to clear the last marked end brace */
    static int last_y = 0;
    static int last_x = 0;
    static int last_wlin = 0;
/*  static int last_ch = '\0'; */
    static char last_ch_type;      /* R=closing, L=opening */

    from = curwin->ttext;
    to = curwin->ttext + curwin->btext;
    lcol = curwin->ltext;
    rcol = curwin->rtext;

/** /
dbgpr("-----\n");
dbgpr("ttext=%d, btext=%d, lcol=%d, rcol=%d\n", from, to, lcol, rcol);

dbgpr("HiLiBrace: curwin->ttext=%d, curwin->btext=%d, curwin->ltext=%d, curwin->rtext=%d\n",
    curwin->ttext, curwin->btext, curwin->ltext, curwin->rtext);

dbgpr("curwin->tmarg=%d, curwin->bmarg=%d, curwin->lmarg=%d, curwin->rmarg=%d\n",
    curwin->tmarg, curwin->bmarg, curwin->lmarg, curwin->rmarg);

dbgpr("curwksp->wlin=%d, curwksp->wcol=%d cursorline=%d cursorcol=%d\n\n",
   curwksp->wlin, curwksp->wcol, cursorline, cursorcol);
/ **/

    /** /
    dbgpr("check unmark %c at (%d,%d) wlin=%d lwlin=%d\n",
	last_ch, last_y, last_x, curwksp->wlin, last_wlin);
    / **/

    int end_ch;

    int cursor_ch;    /* char pointed at by cursor */
    int cursor_screenline = cursorline + curwin->ttext;
    from = cursor_screenline;
    int cursor_screencol = cursorcol + lcol;

    // Try removing "simple" C-style  /* ... */ comments
    Flag inComment = 0;

/** /
dbgpr("cursorline=%d cursor_screenline=%d cursorcol=%d cursor_screencol=%d\n",
   cursorline, cursor_screenline, cursorcol, cursor_screencol);
/ **/

    /* clear previous highlight ch, if still on current screen */
    if (last_y > 0) {
	if (last_wlin >= curwksp->wlin && last_wlin <= (curwksp->wlin + curwin->btext)) {
	    if (last_wlin < curwksp->wlin) {
		last_y -= (int)((curwksp->wlin - last_wlin));
	    }
	    else if (last_wlin > curwksp->wlin) {
		last_y += (int)((last_wlin - curwksp->wlin));
	    }

	    /* for some reason, redrawing only the rt matching brace
	     * left an earlier portion of the line messed up, so we'll
	     * try redrawing the line upto and including the rt brace
	     */

	    cp = (char *)image + w*last_y;
	/*  int wid = curwin->rtext - curwin->ltext;  */ /* not whole line */
	    int wid = last_x - curwin->ltext + 2;   /* +1 for marg char, +1 for rt brace */
	    mvcur(-1, -1, last_y, curwin->ltext);
	    snprintf(buf, (size_t)wid, "%s", cp+curwin->ltext);
	    tputs(buf,1,Pch);
	    /** /
	    dbgpr("clear, buf='%s'\n", buf);
	    / **/
	    /* restoring the cursor doesn't work well when
	     * one is scrolling using the arrow keys (or LT, RT keys).
	     * Searching for an opening brace, or tabbing works ok
	     * w/o this...
	     */

	    int col = cursorcol;
	    int lin = cursorline;
	    cursorcol = cursorline = -1;

	    poscursor(0,0); /* probably don't need this now... */
	    d_put(0);
	    poscursor(col,lin);
	    d_put(0);

	    mvcur(-1, -1, cursor_screenline, cursor_screencol);
	    last_y = last_x = 0;
	    /** /
	    dbgpr("unmark %c at (%d,%d), then moving cur back to (%d,%d)\n",
		    last_ch, last_y, last_x, cursor_screenline, cursor_screencol );
	    / **/
	    fflush(stdout);
	    brace_marked = 0;
	}
    }

    if (set == NO)   /* we're only clearing prev mark */
	return;

/** /
dbgpr("after unmark, cursorline,cursorcol=(%d,%d) cursor_screenline,cursor_screencol=(%d,%d)\n",
   cursorline, cursorcol, cursor_screenline, cursor_screencol );
/ **/

    /* if cursor is not pointing at an open/close "brace", eg '{', return */
    int i;
    cp = (char *)image + cursor_screenline*w;
    i = cursorcol + lcol;
    cursor_ch = *(cp+i);

/** /dbgpr("cursor_ch=%c\n", cursor_ch); / **/

    /* omit if cursor_ch is a constant, eg '('  */
    if (cp[i+1] == '\'' && cp[i-1] == '\'') {
	//dbgpr("skipping %c in a constant\n", cursor_ch);
	return;
    }

    /* if cursor isn't pointing at a "brace"-char, return */
    switch (cursor_ch) {
	case '{':
	    end_ch = '}';
	    last_ch_type = 'R';
	    break;
	case '(':
	    end_ch = ')';
	    last_ch_type = 'R';
	    break;
	case '[':
	    end_ch = ']';
	    last_ch_type = 'R';
	    break;
	case '}':
	    end_ch = '{';
	    last_ch_type = 'L';
	    break;
	case ')':
	    end_ch = '(';
	    last_ch_type = 'L';
	    break;
	case ']':
	    end_ch = '[';
	    last_ch_type = 'L';
	    break;
	default:
	    return;
    }

    /*  Unless cursor has paused on the "brace" character
     *  avoid the lookup for the matching char, which
     *  could be a search to EOF.
     */
    int c;
    for (i=0; i<5; i++) {
	timeout(30);    /* ms */
	c = wgetch(stdscr);
	if (c == ERR) continue;
	break;
    }
    timeout(-1);    /* cancel timeout value */
    if (c != ERR) {
	ungetch(c);
	dbgpr("Brace srch skipped, have pending input\n");
	return;
    }

#ifdef OUT
/* if we want to do this, need add vars to keep
 * track of position in order to clear it next time
 */

    /* Cursor is resting on opening brace.
     * Do we want to hilight it in some other way?
     */
    mvcur(-1, -1, cursor_screenline, cursor_screencol);
    if( curs_set(0) == ERR)
	dbgpr("curs_set rc=ERR\n");
    tputs(hilite_str, 1, Pch);
    /*tputs(smso, 1, Pch);*/
    fputc(cursor_ch, stdout);
    tputs(sgr0, 1, Pch);
    fflush(stdout);
#endif


    /* First search the current screen */

    /*  Keep a tally of left braces found before an ending one.
     *  Have a match when we find an end brace and cnt returns to 0.
     */
    int cnt = 0;
    int found_match = 0;
    char msg[128];
    int ln_eof = (int) (la_lsize(curlas) - curwksp->wlin) + curwin->tmarg;

    int match_error = 0; /* set to show where the err is detected */

    if (last_ch_type == 'R') {  /* look forw for closing brace */
/** /
dbgpr("closing brace search, from=%d to=%d ln_eof=%d wlin=%ld, lcol=%d curs_col=%d\n",
from, to, ln_eof, curwksp->wlin, lcol, cursor_screencol);
/ **/
	for (line = from; line <= to; line++) {
	    if (line > ln_eof) {
		snprintf(msg, sizeof(msg),
		    " Matching %c not found thru end of file", end_ch);
		mesg(ERRALL+1, msg);
		return;     /* NOT FOUND */
	    }

	    cp = (char *)image + w*line;

	    for (i=lcol; i<=rcol; i++) {

		if (cp[i] == ' ') continue;

		/* begin comment filtering */

		/* skip comment style:  // */
		if (bracematchCoding && cp[i] == '/' && cp[i+1] == '/') {
		//  dbgpr("skip comment\n");
		    break;
		}

		// skip simple /*... */ comments
		if (bracematchCoding && cp[i] == '/' && cp[i+1] == '*') {
		    inComment++;
		    continue;
		}
		if (inComment) {
		    if (cp[i] == '*' && cp[i+1] == '/')
			inComment = 0;
		    continue;
		}
		/* end comment filtering */

		//if (*(cp+i) == cursor_ch) {
		if (*(cp+i) == cursor_ch && *(cp+i+1) != '\'') {
		    /* omit chars left of cursor or the one cursor is at */
		    if (line == cursor_screenline && (i <= cursor_screencol)) {
			//dbgpr("skip increment of cursor_ch, line=%d cur_lin=%d, i=%d cur_col=%d\n",
			//    line, cursor_screenline, i, cursor_screencol);
			continue;
		    }
		    cnt++;
		 // dbgpr("got %c at line=%d col=%d, cnt=%d\n", cursor_ch, line, i, cnt);
		    continue;
		}

		//if (*(cp+i) == end_ch) {
		if (*(cp+i) == end_ch && cp[i+1] != '\'') {

		    //dbgpr("got %c at line=%d col=%d, cnt=%d\n", end_ch, line, i, cnt);

		    /* on the cursor line, don't include matches left of the cursor */
		    if (line == cursor_screenline && (i <= cursor_screencol)) {
			//dbgpr("skip increment of cursor_ch, line=%d cur_lin=%d, i=%d cur_col=%d\n",
			//    line, cursor_screenline, i, cursor_screencol);
			continue;
		    }

		    /*  if bracematchCoding mode, and } appears in column 1
		     *  we only have a match if we started in column 1
		     *  and cnt is 0
		     */
		    if (i == lcol && end_ch == '}' && bracematchCoding) {
			    /* { and } both in column 1, ie have match  */
			if (cnt == 0 && cursor_screencol == lcol) {
			    found_match = 1;
			    break;
			}

			snprintf(msg, sizeof(msg),
"Matching %c not found in current function, ending at line %ld.",
end_ch, line + curwksp->wlin);
			mesg(ERRALL+1, msg);
		    /*  return; *//* NO MATCH */
			found_match = 1;
			match_error = 1;
			break;
		    }

		    if (cnt > 0) {
		 /** / dbgpr("got %c at col=%d, cnt=%d\n", end_ch, i, cnt);  / **/
		       cnt--;
		       continue;
		    }

		    /* cnt is 0 */

		    /* if bracematchCoding and cursor_screencol == lcol, no match */
		    if (cursor_screencol == lcol && bracematchCoding && i > lcol
			   && end_ch == '}' ) {
			snprintf(msg, sizeof(msg),
"The matching %c at line %ld, col %d is not at the end of a function.",
end_ch, line + curwksp->wlin, i);
			mesg(ERRALL+1, msg);
		     /*  return;*/   /* NO MATCH */
			found_match = 1;
			match_error = 1;
		    }

		    /** /
		    dbgpr("found %c at line %d col %d, cursor_screenline=%d, cursor_screencol=%d, wlin=%d\n\n",
			end_ch, line, i, cursor_screenline, cursor_screencol, curwksp->wlin);
		    / **/
		    found_match = 1;
		}
		if (found_match) break;
	    }
	    if (found_match) break;
	}
	    /* search forward, start at lines not in current window */
	if (!found_match) {
	    long l_num = -1;
	    int c_num = -1;

//dbgpr("!found cnt=%d before calling FindBraceForw\n", cnt);

	    int rc = FindBraceForw(cursor_ch, end_ch, cnt, &l_num, &c_num, inComment);
/** /
dbgpr("rc=%d from FindBraceForw, l_num=%d c_num=%d\n", rc, l_num, c_num);
/ **/
	    /* found match looking forward */
	    if (rc == 1) {
		snprintf(msg, sizeof(msg), " Matching %c found at line %ld col %d.",
		    end_ch, l_num+1, c_num+1);
	  /** / dbgpr("%s\n", msg);    / **/
		mesg(ERRALL + 1, msg);
		/* todo?? hilite match */
		return;  /* MATCH FOUND */
	    }

	    snprintf(msg, sizeof(msg), " Matching %c not found ", end_ch);
	    int len = (int)strlen(msg);


	    if (rc == 2) {  /* bracematchCoding mode, not found */
		snprintf (msg+len, sizeof(msg), "in current function() \
ending at line %ld col %d.", l_num+1, c_num+1);
	    }
	    else if (rc == 3 || win_has_eof()) {
		strcat (msg, "searching to the end of the file.");
	    }
	    else {  /* eof not in win  */
		if (braceRange == 0) {
		    strcat (msg, "searching to the end of file.");
		}
		else {
		    sprintf (msg+len, "in the next %d lines.", braceRange);
		}
	    }

	    mesg(ERRALL + 1, msg);
	    return;     /* NOT FOUND */
	}
    }
    else {  /* look back for opening match */
	from = curwin->ttext + cursorline;
	to = curwin->ttext;
	/** /
	dbgpr("open brace search, from=%d to=%d\n", from, to);
	/ **/
/** /
dbgpr("opening brace search, from=%d to=%d ln_eof=%d wlin=%ld, lcol=%d curs_col=%d\n",
from, to, ln_eof, curwksp->wlin, lcol, cursor_screencol);
/ **/

	inComment = 0;
	char __attribute__((unused)) buf[256];

	for (line = from; line >= to; line--) {
	    /** /
	    dbgpr("checking line (reverse) %d for %c, cnt=%d\n", line, end_ch, cnt);
	    / **/
	    cp = (char *)image + w*line;

	    int beg = rcol; /* backward */

	    /* skip simple comment style:  // */
	    if (bracematchCoding) {
		for (i=lcol; i<rcol; i++) {
		    if (cp[i] == '/' && cp[i+1] == '/') {
			beg = i - 1;
		    //  dbgpr("skip comment, back srch\n");
			break;
		    }
		}
	    }

	    //snprintf(buf, (size_t)w, "%s", cp+1);
	    //dbgpr("lcol=%d inComment=%d line=(%s)\n",lcol, inComment, buf);

	    for (i=beg; i>=lcol; i--) {

		if (cp[i] == ' ') continue;

		if (bracematchCoding) {

		    // begin comment filtering, type:  /* ... */

		    // an end "*/" => we're within the comment, moving rt to left
		    if (i>lcol && cp[i] == '/' && cp[i-1] == '*') {
			inComment = 1;
			//snprintf(buf, (size_t) w, "%s\n", cp);
			//dbgpr("found */ at i=%d (%s)\n", i, buf);
			continue;
		    }
		    if (inComment) {
			// a beg "/*" => we're no longer within a comment
			if (i>lcol && cp[i] == '*' && cp[i-1] == '/' ) {
			    inComment = 0;
			    //snprintf(buf, (size_t) w, "%s\n", cp);
			    //dbgpr("found /* i=%d (%s)\n", i, buf);
			}
			continue;
		    }
		}

		//if (*(cp+i) == cursor_ch) {
		if (*(cp+i) == cursor_ch && cp[i-1] != '\'') {
		    /* don't include the chars right of cursor, or the one
		     * cursor is pointing at
		     */

		    if (line == cursor_screenline && i >= (cursor_screencol))
			continue;
		    cnt++;
		    /*dbgpr("got %c, cnt=%d\n", cursor_ch, cnt);*/
		    continue;
		}

		//if (*(cp+i) == end_ch) {
		if (*(cp+i) == end_ch  && cp[i-1] != '\'') {
		    /* on the cursor line, don't include matches right of the cursor */
		    if (line == cursor_screenline && i >= (cursor_screencol))
			continue;
/** /
dbgpr("have %c at i=%d line=%d cnt=%d\n", end_ch, i, line, cnt);
dbgpr("  lcol=%d cursor_screencol=%d\n", lcol, cursor_screencol);
/ **/

		    if (cnt > 0) {
		       /*dbgpr("got %c at col=%d, cnt=%d\n", end_ch, i, cnt);*/
		       cnt--;
		       continue;
		    }

		    /*  if bracematchCoding mode, and { appears in column 1
		     *  we only have a match if we started in column 1
		     *  and cnt is 0
		     */
		    if (end_ch == '{' && bracematchCoding) {
			    /* { and } both in column 1, ie have match  */

/** /
dbgpr("  end_ch is } and braceCoding is on, i=%d cnt=%d\n", i, cnt);
dbgpr("  lcol=%d cursor_screencol=%d\n", lcol, cursor_screencol);
/ **/

			if (i == lcol && cnt == 0 && cursor_screencol == lcol) {
			/*  dbgpr("x1 found match\n"); */
			    found_match = 1;
			    break;
			}

			/* also a match if i != lcol && cursor_screencol != lcol */
			if (i != lcol && cursor_screencol != lcol) {
			/*  dbgpr("x2 found match\n"); */
			    found_match = 1;
			    break;
			}


			snprintf(msg, sizeof(msg),
"Matching %c not found in current function, starting at line %ld.",
end_ch, line + curwksp->wlin);
			mesg(ERRALL+1, msg);
		    /*  return;*/ /* NO MATCH */
			found_match = 1;
			match_error = 1;
			break;
		    }

/** /
dbgpr("found %c at line %d col %d, cursor_screenline=%d, cursor_screencol=%d, wlin=%d\n\n",
    end_ch, line, i, cursor_screenline, cursor_screencol, curwksp->wlin);
/ **/
		    found_match = 1;
		}
		if (found_match) break;
	    }
	    if (found_match) break;
	}
	    /* search backward, start at 1st line not in current window */
	if (!found_match) {

	    if (curwksp->wlin > 0) {  /* line 1 of file not in current window */
		long l_num = -1;
		int c_num = -1;
		int rc = FindBraceBack(cursor_ch, end_ch, cnt, &l_num, &c_num, inComment);

		/** /
		dbgpr("rc=%d from FindBraceBack, l_num=%d c_num=%d\n", rc, l_num, c_num);
		/ **/

		if (rc == 1) {
		    snprintf(msg, sizeof(msg), " Matching %c found at line %ld col %d.",
			end_ch, l_num+1, c_num+1);
	       /*   dbgpr("%s\n", msg); */
		    mesg(ERRALL + 1, msg);
		    return; /* FOUND MATCH */
		}

		snprintf(msg, sizeof(msg), " Matching %c not found ", end_ch);
		int len = (int)strlen(msg);

		if (braceRange == 0) {
		    strcat(msg, "searching to beginning of the file");
		}
		else {
		    sprintf (msg+len, "in the preceeding %d lines", braceRange);
		}
	    }
	    else {  /* no match, line 1 of file at top of window */
		snprintf(msg, sizeof(msg), " Matching %c not found %s ", end_ch,
		    "searching to beginning of the file");
	    }
	    mesg(ERRALL + 1, msg);
	    return;  /* NOT FOUND */

	}
    }

    /* have a match on the screen */

#ifdef OUT
    if( !found_match ) {
	snprintf(msg, 128, " Matching %c not found ", end_ch);
	int len = strlen(msg);
	if (last_ch_type == 'R') {  /* search forw for closing */
	    if (braceRange == 0) {
		sprintf (msg+len, " searching to the end of the file");
	    }
	    else {
		sprintf (msg+len, " in the following %d lines", braceRange);
	    }
	}
	else {
	    if (braceRange == 0) {
		sprintf (msg+len, " searching to beginning of the file");
	    }
	    else {
		sprintf (msg+len, " in the preceeding %d lines", braceRange);
	    }
	}
/** /   dbgpr(" matching %c not found on this screen", end_ch);  / **/
	mesg(ERRALL + 1, msg);
	return;
    }
#endif /* OUT */

    int match_col = i;

    /* Highlight matching brace */
    mvcur(-1,-1, line, match_col);
/** /
    dbgpr("moving to line %d col %i to highlight %c\n", line, match_col, end_ch);
/ **/

    /* save these in order to clear the mark, next time */
    last_y = line;
    last_x = match_col;
    last_wlin = (int)curwksp->wlin;
/*  last_ch = end_ch; */

    if (match_error)
/*      tputs("\033[41m", 1, Pch); */
	tputs(font_red, 1, Pch);
    else
	tputs(brace_p, 1, Pch);

    fputc(end_ch, stdout);
    tputs(sgr0, 1, Pch);

    fflush(stdout);

    brace_marked = 1;


/*DebugVal=1;*/

    /*  redraw first part of line, something
     *  is getting messed up when closing brace is found
     */
    if (last_ch_type == 'R') {
	cp = (char *)image + w*line;
	int wid = last_x - curwin->ltext + 1;   /* redraw upto the ending match char */
	mvcur(-1, -1, last_y, curwin->ltext);
	snprintf(buf, (size_t)wid, "%s", cp+curwin->ltext);
	tputs(buf,1,Pch);
    /** /
	dbgpr("buf='%s'\n", buf);
    / **/
    }

    /* seem to have trouble restoring cursor
     * to original position; this seems to help
     */
    int col = cursorcol;
    int lin = cursorline;

    cursorcol = cursorline = -1;


    poscursor(0,0);
    d_put(0);
    poscursor(col, lin);
    d_put(0);

    mvcur(-1, -1, cursor_screenline, cursor_screencol);
    fflush(stdout);

/** /
dbgpr(" calling poscursor(%d,%d)  mvcur(-1,-1, %d,%d)\n",
col, lin, cursor_screenline, cursor_screencol);
/ **/

    return;

}

/* returns 1 if matching end brace is found
 * updates lnum and cnum ptrs
 */
int
FindBraceBack(int beg_ch, int end_ch, int cnt,
    long *lnum, int *cnum, Flag inComment)
{

    Nlines wlin = curwksp->wlin;
    Nlines nextline;
    Nlines lastline = 0;      /* first line in file is the lastline we examine */

    nextline = wlin - 1;

	/* limit search range? */
    if (braceRange && (wlin - braceRange > 0)) {
	lastline = wlin - braceRange;
    }

/** /
dbgpr("----------------\n");
dbgpr("FindBraceBack:  beg_ch=%c end_ch=%c, cnt=%d, wlin=%d \
btext=%d ttext=%d\n   nextline=%d lastline=%d braceRange=%d\n",
    beg_ch, end_ch, cnt, wlin, curwin->btext,
curwin->ttext, nextline, lastline, braceRange);
/ **/

    Ncols i;
    Nlines ln;

#ifdef DBUG
    char __attribute__((unused)) buf[1024];
#endif

    /* search backward in file */
    for (ln = nextline; ln >= lastline; ln--) {
	GetLine(ln);

#ifdef DBUG
	dbgpr("ln=%d lcline=%d, ncline=%d inCom=%d cnt=%d\n",
	    ln, lcline, ncline, inComment, cnt);
	strncpy(buf, cline, (size_t) ncline-1);
	buf[ncline] = '\0';
	dbgpr("(%s)\n", buf);
#endif

	int beg = (int) (ncline+1);

	/* skip simple comment style:  // */

	if (bracematchCoding) {
	    for (i=0; i<ncline; i++) {
		if (cline[i] == '/' && cline[i+1] == '/') {
		    beg = (int) (i - 1);
		//  dbgpr("FindBraceBack,skip comment, set beg=%d\n", beg);
		}
	    }
	}

	for (i=beg; i >= 0; i--) {

	    /* begin comment checking */

	    // an end "*/" => we're within the comment, moving rt to left
	    if (i>1 && cline[i] == '/' && cline[i-1] == '*') {
		inComment = 1;
	     // dbgpr("FindBraceBack, start comment (%c) i=%d ln=%d\n", cline[i], i,ln);
		continue;
	    }
	    if (inComment) {
		// a beg "/*" => we're no longer within a comment
		if (i>1l && cline[i] == '*' && cline[i-1] == '/' ) {
		    inComment = 0;
		//   dbgpr("FindBraceBack, end comment at i=%d ln=%d\n", i, ln);
		}
		continue;
	    }
	    /* end comment checking */

	    //if (cline[i] == end_ch) {  // ie, not '}'
	    if (cline[i] == end_ch && cline[i-1] != '\'') {  // ie, not '}'
		if (cnt <= 0) {
		    *lnum = ln; *cnum = (int)i;
		  //dbgpr("--found %c at ln %d c %d\n", end_ch, ln, i);
		    return 1;
		}
		cnt--;
	    //  dbgpr("--got end_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch);
	    }
	    //else if (cline[i] == beg_ch) {
	    else if (cline[i] == beg_ch && cline[i-1] != '\'') {  // ie, not '{'
		cnt++;
	    // dbgpr("--got beg_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch);
	    }
	}
    }


    return 0;
}

/* returns 1 if matching end brace is found
 * updates lnum and cnum ptrs
 */
int
FindBraceForw(int beg_ch, int end_ch, int cnt,
    long *lnum, int *cnum, Flag inComment)
{

    Nlines wlin;    /* curwin topline */
    Nlines ln;
    Nlines nextline;
    Nlines lastline = la_lsize(curlas);

    wlin = curwksp->wlin;
    nextline = wlin + curwin->btext + 1; /* nxt line not displayed in curwin */

    if (braceRange) {  /* look only this far ahead */
	if (nextline + braceRange < lastline)
	    lastline = nextline + braceRange;
    }

/** /
dbgpr("----------------\n");
dbgpr("FindBraceForw:  beg_ch=%c end_ch=%c, cnt=%d, wlin=%d \
btext=%d ttext=%d\n   nextline=%d lastline=%d braceRange=%d\n",
    beg_ch, end_ch, cnt, wlin, curwin->btext,
curwin->ttext, nextline, lastline, braceRange);
/ **/

// lastline = nextline + 5;    / * debugging */

#ifdef DBG
    char __attribute__((unused)) buf[1024];
#endif

    Ncols i;


    /* search forward in file */
    for (ln = nextline; ln <= lastline; ln++) {
	GetLine(ln);

	int nSkip = 0;

#ifdef DBG
	dbgpr("\n-------------\n");
	dbgpr("ln=%d lcline=%d, ncline=%d inCom=%d cnt=%d\n",
	    ln, lcline, ncline, inComment, cnt);
	strncpy(buf, cline, (size_t) ncline-1);
	buf[ncline-1] = '\0';
	dbgpr("cline=(%s)\n", buf);
#endif

	for (i=0; i <= ncline; i++) {

/** /
dbgpr("FindBraceForward: chk (%c) mode=%d\n", cline[i], bracematchCoding);
/ **/

	    /* begin comment checking */

	    /* skip simple comment style:  //  */
	    if (bracematchCoding && cline[i] == '/' && cline[i+1] == '/') {
		//dbgpr("FindBraceForw, skip // comment\n");
		break;
	    }
	    // skip simple /*... */ comments

	    if (inComment) {
		if (cline[i] == '*' && cline[i+1] == '/') {
		    inComment = 0;
		    //dbgpr("FindBraceForw(cnt=%d), end of a comment at ln=%d (%c)(%c)\n",
		    //    cnt, ln, cline[i], cline[i+1]);
		    continue;
		}
		nSkip++;
		//dbgpr("FindBraceForw, skipping (%c) still in a comment\n", cline[i]);
		continue;
	    }
	    if (bracematchCoding && cline[i] == '/' && cline[i+1] == '*') {
		inComment++;
		//dbgpr("FindBraceForw(cnt=%d), start of a comment at ln=%d (%c)(%c)\n",
		//    cnt, ln, cline[i],cline[i+1]);
		continue;
	    }
	    /* end comment checking */

	    //if (cline[i] == end_ch) {
	    if (cline[i] == end_ch && cline[i+1] != '\'') { //eg, '}'
/**/
dbgpr("FindBraceForw:  have end_ch %c at ln %d c %d braceCoding=%d cnt=%d\n",
 end_ch, ln, i, bracematchCoding, cnt);
/**/
		if (cnt <= 0) {
		    *lnum = ln; *cnum = (int)i;
		    //dbgpr("--found %c at ln %d c %d\n", end_ch, ln, i);
		    return 1;
		}

		/* stop at end of a function, which we assume
		 * to be a } in column 0
		 */
		if (i == 0 && end_ch == '}' && bracematchCoding) {
		    *lnum = ln; *cnum = (int)i;
		    return 2;
		}

		cnt--;
		//dbgpr("--got end_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch);
	    }
	    //else if (cline[i] == beg_ch) {   // ie, not '{'
	    else if (cline[i] == beg_ch && cline[i+1] != '\'' ) {   // ie, not '{'
		cnt++;
		//dbgpr("--got beg_ch at %d, cnt=%d looking for %c\n", i, cnt, end_ch);
	    }
	}
	//dbgpr("--skipped %d chars while inComment\n", nSkip);
    }
    if (ln >= lastline)
	return 3;

    return 0;
}




int
win_has_eof()
{

/** /
dbgpr("win_has:  wlin=%d btext=%d ll=%d",
    curwksp->wlin, curwin->btext, la_lsize(curlas));
/ **/

    if ((curwksp->wlin + curwin->btext) >= la_lsize(curlas))
	return 1;

    return 0;
}

