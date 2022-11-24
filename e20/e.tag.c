#ifdef COMMENT
--------
file e.tags.c
    support for ctags
#endif


#include <ctype.h>
#include "e.h"
#include "e.cm.h"
//#include "e.m.h"    /* for loopflags */
#include "e.tt.h"

#ifdef TAGS

/* Default std keyboard def for +srch is ^Y (25) */
int tag_plsrch_key = 25;

typedef struct sKey2Ecode { /* defined in e.keys.h */
 int keycode;       /* ncurses KEY_ value */
 Uchar  ecmd1;      /* 1st E cmd, eg:  KEY_F6:<+page> */
 Uchar *ecmdstr;    /* 2nd -> end of e cmds, eg:  KEY_F6:<+word><redraw><-page> */
 int len;           /* num Uchars in ecmdstr */
 char keyname[16];  /* tmp for debugging */
} sKey2Ecode;

/* in initTagIndex(), we check to see if ^Y has been redefined */
extern sKey2Ecode KeyCtrl[];    /* user defined ^X codes in kbfile */


Cmdret dotag (char *);
char *TagFromScreen (S_window *);

int FindTag1 (char *);    /* returns num of matches */

int initTagIndex(void);
int needEscape(int);
void info_tag(int, int);
int tagLineNo(int);

Flag editTagFile;
void PushTagCmds(void); /* does an ungetch() on "e" cmds TagMatches[] */
extern void chgwindow (Small);
extern Nlines srchline;

#define NTAGS 50
struct TagMatches {
//  char tag[32];
//  char fname[64];
//  char search_str[128];   /* use malloc instead */
    char *tag;
    char *fname;
    char *search_str;
    int lineno;
};
static struct TagMatches TagMatches[NTAGS];

int curtag = 0;  /* indexes TagMatches */
int numtags = 0; /* number of "active" TagMatches[] entries */

/* contains file offsets into ./tags from 0 to ('z' - 'A') */

static long Aindex['z' - 'A' + 1];
static int Asize;
int doInitTagIndex = 1; /* need to initialize index */
FILE *tagfp;

/* set when user has not used "tag + or -" */
Flag tagchwin_ok;   /* ok to change windows for a new tag */

Cmdret
dotag (char *str)
{
    char *tag = str;
    int n_mat = 0;

    /* temp, initalize alpha offsets */
    if (doInitTagIndex) {
	if ( !initTagIndex() )
	    return CROK;
	doInitTagIndex = 0;
    }


    if (!tag || *tag == '\0') {
	tag = TagFromScreen(curwin);
//      dbgpr("Screen tag=(%s)\n", tag);
    }

//  dbgpr("\n---\ntag=(%s)\n", tag);

    /* move up/down list of TagMatches[] */

    editTagFile = 1;  /* assume ok */

    if ((numtags == 0) && (*tag == '+' || *tag == '-')) {
	dbgpr("no tags matches\n");
	mesg(ERRALL + 1, "There are no tag matches to view.");
	editTagFile = 0;
	return CROK;
    }

    switch (*tag) {
	case '+':   /* view next tag */
	    curtag++;
	    if (curtag >= numtags) {
		curtag = numtags - 1;
	     // dbgpr("Already viewing the last tag match.\n");
		mesg(ERRALL + 1, "Already viewing the last tag match.");
		editTagFile = 0;
	    }
	    info_tag(curtag+1, numtags);
	    return CROK;

	case '-':   /* view prev tag */
	    if (curtag == 0) {
	    //  dbgpr("Already viewing the first tag match.\n");
		mesg(ERRALL + 1, "Already viewing the first tag.");
		editTagFile = 0;
		return CROK;
	    }
	    /* save current line no */
	    curtag--;
	    info_tag(curtag+1, numtags);
	    return CROK;

	default:    /* new tag srch */
	    if (*tag < 'A' || *tag > 'z') {
	    //  dbgpr("tag does not begin with an alpha character.\n");
		mesg (ERRALL + 1, "Tag does not begin with an alpha character, a '+', or '-'.");
		editTagFile = 0;
		return CROK;
	    }
	    break;
    }

    /* free up mem of previous matches */
    int i;
    for (i=0; i<numtags; i++) {
	free(TagMatches[i].search_str);
	free(TagMatches[i].fname);
	free(TagMatches[i].tag);
	TagMatches[i].search_str = NULL;
	TagMatches[i].fname = NULL;
	TagMatches[i].tag = NULL;
    }

    curtag = 0;
    numtags = 0;

    /*  FindTag1 populates the TagMatches[] struct and
     *  returns the number of matches
     */
    n_mat = FindTag1(tag);

    if (n_mat < 0) {
	editTagFile = 0;
	mesg(ERRALL + 1, "ERROR: no tabs found in \"tags\" file.");
	return CROK;
    }

    //dbgpr("n_mat=%d\n", n_mat);

    if (n_mat == 0) {
    //  dbgpr("FindTag1: tag=(%s) not found\n", tag);
	mesg(ERRALL + 3, "tag: '", tag, "' not found");
	editTagFile = 0;
	info_tag(-1, 0);
	return CROK;
    }

    editTagFile = 1;

    /* For the first "tag xxx", it ok to display tag file in a 2nd window
     * but subsequent "tag + or -" views should stay in 2nd window if present
     */
    tagchwin_ok = 1;
    info_tag(curtag+1, numtags);  /* ?? info on bottom rt margin ?? */
    return CROK;

}

void
info_tag(int tagno, int ntags)
{
    char msg[16];
    int ncol = 6;

    if (tagno == -1) {
	info(term.tt_width - ncol, ncol, "");
    }
    else{
	sprintf(msg, "%d/%d", tagno, ntags);
	info(term.tt_width - ncol, ncol, msg);
    }

    return;
}


/*   Populate TagMatches, return number of matches
 */

int
FindTag1(char *tag)
{
    char buf[512];

    int i  = (int) (tag[0] - 'A');

    long offset = Aindex[i];
    int n_matches = 0;
    int n, rc;

    char pattern[64], filename[64], srch_str[256];
    int line;
    char *t;

    fseek(tagfp, offset, SEEK_SET);

    char *sub;
    while (fgets(buf, (int) sizeof(buf), tagfp)) {
	if (feof(tagfp))
	    break;

	if (*buf == '!')    // comment
	    continue;


	//dbgpr("got buf=(%s)\n", buf);

	if (*buf < 'A' || *buf > 'z' ) {
	    dbgpr("error: tag line begins with a non-alpha character: (%c)\n", *buf);
	    continue;
	}

	    /* we're past our alpha section */
	if (*buf > *tag) {
	//  dbgpr("STOP:  buf[0]=(%c) > tag[0]=(%c)\n", buf[0], tag[0]);
	    break;
	}

	    /* save a strcmp below, 2nd char must match */
	if (buf[1] != tag[1])
	    continue;

	buf[strlen(buf)-1] = '\0';

	/*  if the 1st char after the 2nd tab is a digit, the field is
	 *  a line no; otherwise a search pattern
	 */
	t = index(buf+1, '\t');

	if (t == NULL) {
	    dbgpr("Error:  no tabs found in tag file.\n");
	    n_matches = -1;
	    break;
	}

	t = index(t+1, '\t');
	if (isdigit(t[1])) {
	    n = sscanf(buf, "%s\t%s\t%d", pattern, filename, &line);
	    srch_str[0] = '\0';
	}
	else {
	    if ((sub = strstr(buf, "/;\""))) {  /* discard ;" -> eol */
		sub[1] = '\n';
	    }
	    n = sscanf(buf, "%s\t%s\t%[^\n]", pattern, filename, srch_str);
	    line = -1;
	}

	if (n < 3) {
	    dbgpr("n=%d, sscanf() failed on:(%s)\n", n, buf);
	    continue;
	}

	rc = strcmp(tag, pattern);
	//dbgpr("rc=%d cmp tag (%s) to (%s) (n=%d, srch=%s)\n",
	//    rc, tag, pattern, n, srch_str);
	if (rc > 0) continue;   // tag is alpabetically 'higher'
	if (rc < 0) break;      // tag is 'lower', no further matches exist

	// have a match
	//dbgpr("MATCH:  pattern=(%s), file=(%s), line=(%d), srch_str=(%s)\n",
	//    pattern, filename, line, srch_str);


	if (srch_str[0]) {
	    TagMatches[numtags].search_str = malloc(strlen(srch_str));
	    strcpy(TagMatches[numtags].search_str, srch_str);
	}
	TagMatches[numtags].tag = malloc(strlen(tag));
	TagMatches[numtags].fname = malloc(strlen(filename));

	strcpy(TagMatches[numtags].tag, tag);
	strcpy(TagMatches[numtags].fname, filename);

	TagMatches[numtags].lineno = line;

	numtags++;
	n_matches++;

	if (n_matches >= NTAGS - 1)
	    break;
    }

    return n_matches;
}



/*   return ptr to the word pointed at by the cursor
 */

char *
TagFromScreen (S_window *win)
{
    Uchar *cp;
    int w = term.tt_width;
    static char tagbuf[64];
    extern Uchar *image;

    Uchar *beg, *end, *lm, *rm;
    cp = (Uchar *)image + (int)(w * (cursorline + win->tmarg + 1));
    rm = cp + win->rmarg;
    lm = cp + win->lmarg;
    cp += (int) (win->lmarg + cursorcol + 1);
    beg = end = cp;

    /*  cp now points somewhere on the tag;
     *  middle, beg, or end
     */

    /* if cursor is on a space, find beginning of tag, to the right */
    /* or is this a user error?? */
    while (*cp == ' ') {
	cp++;
	if (cp >= rm) {
	    dbgpr("cp >= rm, returning NULL\n");
	    return NULL;
	}
    }

    /*  find the start of the tag, moving left,
     *  eg, cursor is at the end or middle of a tag
     */

    if (*cp != ' ') {
	end = cp;
	    /* any other chars valid in a tag? */
	while (isalpha(*cp) || isdigit(*cp) || *cp == '_') {
	    cp--;
	    if (cp <= lm) break;
	}
	beg = ++cp;
    }

    /* find the end of the tag, moving right */
    while (end++) {
	if (end >= rm) break;
	if (isalpha(*end) || isdigit(*end) || *end == '_')
	    continue;
	break;
	//if (isspace (*end) || ispunct(*end))
	//    break;
    }
    *end = '\0';
    if (beg == end) {   /* valid tag not found */
	// dbgpr("beg==end\n");
	return NULL;
    }

    strncpy(tagbuf, (char *)beg, sizeof tagbuf);
//  dbgpr("tagbuf=(%s)\n", tagbuf);

    return tagbuf;
}

/*
 *   Add the "e" commands to the input queue
 */
void
PushTagCmds()
{

/** /
dbgpr("PushTagCmds: curtag=%d, tag=%s file=%s lineno=%d\n",
 curtag, TagMatches[curtag].tag,
 TagMatches[curtag].fname,
 TagMatches[curtag].lineno);
dbgpr("srch_str=%s\n",
 TagMatches[curtag].search_str);
/ **/

//editTagFile = 0;
//return;

    Uchar ecmds[128];
    Uchar *s = ecmds;

    /* this "easy" srch saves a lot of trouble with E's
     * handling of tabs, and reduces the overhead of
     * an E search.
     */
    if (TagMatches[curtag].lineno == -1) {
	tagLineNo(curtag);
    }

    Flag doingSrch = TagMatches[curtag].lineno > 0 ? NO : YES;

    /*  populate ecmds[] with "e" keystrokes to
     *  to switch to the tagfile as "e"'s alternate file
     */

    /* Add:  <cmd>e file<ret> */
    *s++ = CCCMD;
    sprintf((char *)s, "e %s\r", TagMatches[curtag].fname);
    s += 3 + strlen(TagMatches[curtag].fname);

    /* in case we have any -1 lineno's from a failed search */
    int line = max(1, TagMatches[curtag].lineno);

    if (!doingSrch) {   /* have line numbers */
	/* Add:  <cmd>goto lineno<ret> */
	*s++ = CCCMD;
	sprintf((char *)s, "goto %d\r", line);
    }

    /* ungetch() chars in reverse order so they're read
     * by wgetch() in the correct order
     */
    int len = (int) strlen((char *)ecmds) - 1;
    int i;

//  dbgpr("PushTag, len=%d:", len);
    for (i=len; i >= 0; i--) {
	ungetch ((int)ecmds[i]);
#ifdef OUT
	if (isprint(ecmds[i]))
	  dbgpr("%c",ecmds[i]);
	else
	  dbgpr("(%o)", ecmds[i]);
#endif /* OUT */
    }
//  dbgpr("\n");

    editTagFile = 0;

    /*  don't switch windows if viewing a subsequent match of
     *  the same tag, specified by + or -
     */
    if ((nwinlist > 1) && (curtag == 0) && tagchwin_ok) {
	chgwindow(-1);
	tagchwin_ok = 0;
    }

    return;
}


/* save offsets to the beginning of each alphabetic
 * section in the tag file
 */

int
initTagIndex()
{
    long offset = 0;
    int i;
    int i_last = -1;
    char buf[512];

    if ((tagfp = fopen("tags", "r")) == NULL) {
	dbgpr("can't open tags\n");
	mesg(ERRALL + 1, "Can't open \"./tags\"");
	return 0;
    }

    Asize = sizeof(Aindex) / sizeof(Aindex[0]);
//  dbgpr("Aindex size=%d\n", Asize);

    while (fgets(buf, sizeof(buf), tagfp)) {
	if (buf[0] == '!') continue;
	i = (buf[0] - 'A');   /* 1st entry is 0:  'A' - 'A' */

	if (i > i_last) {
	    offset = (long) (ftell(tagfp) - (long)strlen(buf));
	    Aindex[i] = offset;
	    i_last = i;
	}
    }
    // leave file open
    //fclose(tagfp);

    /*  if user has redefined ^Y not to be CCPLSRCH
     *  find which ctrl-key is CCPLSRCH
     *
     *  note: 25 is the index position of CCPLSRCH in KeyCtrl[]
     */
//  dbgpr("initTag: before key=%o %d\n", tag_plsrch_key, tag_plsrch_key);
//  dbgpr("initTag, KeyCtrl[25].ecmd1=%d CCPLSRCH=%d\n", KeyCtrl[25].ecmd1, CCPLSRCH);

    if (KeyCtrl[25].ecmd1 != CCPLSRCH) {
	for (i=0; i<=31; i++) {    /* see e.keys.h */
	    if (KeyCtrl[i].ecmd1 == CCPLSRCH) {
		tag_plsrch_key = i;
		//dbgpr("initTagIndex, plsrch_key now = %d\n", i);
		break;
	    }
	}
    }

    return 1;
}


int
needEscape(int c)
{
    switch (c) {
	case '.':
	case '*':
	case '?':
	case '$':
	case '[': case ']':
	//case '(': case ')':   /* not in E's v6 regex */
	//case '{': case '}':
	    return 1;
	default:
	    break;
    }

    return 0;
}

/*  Find the line number matching the tag.
 *  This routine is way less "overhead" compared
 *  to GetLine()--which munges tabs, and E's
 *  other search routines.
 */

int
tagLineNo(int tagnum)
{
    FILE *tagfp;
    char buf[BUFSIZ];
    char *tagfile = TagMatches[tagnum].fname;
    char srch[128], *s;
    int ln, i, end;
    int inCom = 0;
    int n = tagnum;

    if ((tagfp = fopen(tagfile, "r")) == NULL) {
	dbgpr("can't open %s\n", tagfile);
	mesg(ERRALL + 1, "Can't open \"%s\"", tagfile);
	return -1;
    }

    // make a copy in srch[], removing backslashes from search_str:
    // eg:  "\/* comment *\/$/"
    //
    s = TagMatches[n].search_str;
    if (*s == '/' && *(s+1) == '^')   // skip initial "/^"
	s += 2;

    for(i=0; *s && *s != '$'; s++) {
	if (*s == '\\' && *(s+1) == '/')
	    continue;
	srch[i++] = *s;
    }
    srch[i] = '\0';

    s = srch;
    end = (int) (strlen(s) - 1);

    ln = 0;
    while (fgets(buf, (int) sizeof(buf), tagfp)) {
	ln++;
	if (feof(tagfp)) {
	    ln = -1;
	    break;
	}
	if (buf[0] == '\n') continue;

	  // beg skip E's COMMENTS
	if (strstr(buf, "#ifdef COMMENT")) {
	    inCom = 1;
	    continue;
	}
	if (inCom && strstr(buf, "#endif")) {
	    inCom = 0;
	    continue;
	}
	if (inCom) continue;
	// end COMMENT checking

	for (i=0; i<end; i++) {
	    if (s[i] != buf[i]) {
		break;
	    }
	}

	//dbgpr(" i=%d, end=%d, ln=%d\n", i, end, ln);
	//dbgpr("   s:%s\n", s);
	//dbgpr(" buf:%s\n", buf);

	if (i == end) {    /* a match */
	      //dbgpr("found string in %s at line %d\n", tagfile, ln);
	      //dbgpr("string: %s\n", s);
	      //dbgpr("  line: %s\n", buf);

	    /* There is a case where two identical strings in the same file
	     * occur at different line numbers, so the search for the 2nd one
	     * finds the 1st one.  We can peek at the previous entry and skip
	     * this match if we find such is the case.
	     */

	    //Example:
	    //c1      e.t.c   /^ ...  Reg1 char  *c1;$/;" l       line:1907
	    //c1      e.t.c   /^ ...  Reg1 char  *c1;$/;" l       line:1925

	    /* You won't see this problem using the default ctags options
	     * because the sort removes identical lines.  But it will show up
	     * if you add extra fields (as in the above example).
	     * Another reason why including line numbers is best.
	     */

	    if (n > 0 && !strcmp(TagMatches[n].fname, TagMatches[n-1].fname)
		      && !strcmp(TagMatches[n].search_str, TagMatches[n-1].search_str)
		      && (ln == TagMatches[n-1].lineno)) {
		dbgpr("skipping duplicate match at ln=%d\n", ln);
		continue;
	    }

	    break;
	}
    }
    fclose(tagfp);
    TagMatches[tagnum].lineno = ln;

    return ln;
}
#endif /* TAGS */
