#ifdef COMMENT
--------
file e.keys.c
    ncurses keyboard routines
#endif
#include "e.h"

#include "e.keys.h"
#include "e.it.h"       /* for kbinistr, kbendstr defs */
#include <ctype.h>

/* If !NULL, FKey_p points to remaining codes to process from a F_KEY mapping/press
 * which are defined in the user's EKBFILE
 */
Uchar *FKey_p, *FKey_end;
int pendFKeys = 0;
int getFkeychar(void);

int parseEcmds(char *lp, Uchar *ecmds, int *cmdlen, char *line);

Char MapCtrlKey(int c);

Char MapCursesKey(Char);
char *getEkeyname(int);
char *getCursesKeyname(int);
char *dbg_KeyName(int);

void addCtrl2Ecmds(char *line);
void addCursesFkey (char *);
int CursesKeyValue(char *keyname);
void addDefineKey(char *line);
int okCtrlChar(int c);

/* preset these */
int defINTchar = 034;       /* via ^key, eg: ^\, default std keyboard ctrl */
int altINTchar = 0;         /* via Fkey, see addCursesFkey() */

/* see /usr/include/ncurses.h or e.keys.h for KEY_Fn keycode values */

/*
 * MAX_KEY is defined in e.keys.h as 0633 (411)
 * KEY_MIN is 0401 (257)
 */

/* Max number of user defined keys via the
 * define_key(str, value) routine.  This is separate
 * from the total number of Fkeys, and/or ctrl-c keys.
 * They're are created by Alt-key definitions (eg, <esc>"m":<open>)
 * and by multiple ctrl-char sequences (eg, ^X^E:<erase>)
 * See addDefineKey() below.
 */
#define N_DEFINED_KEYS 128
#define KEY2ECODE_SIZE ((MAX_KEY-KEY_MIN)+1+N_DEFINED_KEYS)

sKey2Ecode Key2Ecode[KEY2ECODE_SIZE];   /* approx 283 */

extern sKey2Ecode KeyEsc[];     /* user defined <esc> sequences in kbfile */
extern sKey2Ecode KeyCtrl[];    /* user defined ^c codes in kbfile */
extern S_looktbl itsyms[];      /* see e.iit.c */


int
MapCursesKey (int c)
{

/** / dbgpr("MapCursesKey: c=(%04o)(%d)\n", c, c);  / **/

    if (c < KEY_MIN || c > (MAX_KEY + N_DEFINED_KEYS)) {
	dbgpr("MapCursesKey: c=(%d) out of range.\n", c);
	dbgpr("---MAX_KEY+N_DEFINED_KEYS=%d\n", MAX_KEY + N_DEFINED_KEYS);
	return ERR;  /* defined in curses as -1, or ?? NOCHAR ?? */
    }

/** /
dbgpr("MAX_KEY=%d KEY_MIN=%d  (MAX_KEY-KEY_MIN)=%d, N_DEF=%d sizeof(Key2Ecode[])= %d\n",
MAX_KEY, KEY_MIN, MAX_KEY-KEY_MIN, N_DEFINED_KEYS,
(sizeof(Key2Ecode) / sizeof(Key2Ecode[0])) );
/ **/

    int idx = c - KEY_MIN;
    FKey_p = FKey_end = (Uchar *) NULL;

    pendFKeys = 0;  /* set when a Fkey maps to more than one cmd */

/** /
dbgpr("MapCursesKey: c=%04o, idx=%d, .keycode=%04o\n",
  c, idx, Key2Ecode[idx].keycode);
/ **/

    if ( Key2Ecode[idx].keycode == c ) {  /* is the key defined */
	if ( Key2Ecode[idx].len == 1 ) {  /* eg, a Fkey maps to one E cmd */
	    /** /
	    dbgpr("MapCursesKey, returning (%03o)(%s) for Fkey=(%o)(%s)\n",
		Key2Ecode[idx].ecmd1,
	       getEkeyname(Key2Ecode[idx].ecmd1), c,
		Key2Ecode[idx].keyname);
	    / **/
	    return( (int) Key2Ecode[idx].ecmd1 );
	}
	else {
	    /*  A Fkey maps to more than one E cmd.  We return the
	     *  1st cmd and set a ptr to the rest so that mGetkey1()
	     *  will input the remainder after processing the 1st one.
	     *  Note that .len is one greater than length of .ecmdstr.
	     */
	    FKey_p = Key2Ecode[idx].ecmdstr;
	    FKey_end = FKey_p + Key2Ecode[idx].len - 1;
	/*  dbgpr("setting FKey_end to FKey_p + %d\n", Key2Ecode[idx].len-1); */
	    pendFKeys = 1;
	    return( (int) Key2Ecode[idx].ecmd1 );
	}
    }

    /* An Fkey code was not found in Key2Ecode[] (presumably
     * not defined in the kbfile).  Check if the key is one of
     * the common keypad keys we've added default support for.
     */
    switch( c ) {
	case KEY_MOUSE:             /* mouse event, but won't see this here  */
	    return CCMOUSE;
	case KEY_ENTER:             /* enter/send key */
	    return CCRETURN;
	case KEY_BACKSPACE:         /* backspace key */
	    return CCBACKSPACE;
	case KEY_DC:                /* delete-character */
	    return CCDELCH;
	case KEY_BREAK:             /* Break key (unreliable) */
	    return CCCMD;
	case KEY_DOWN:              /* down-arrow key */
	    return CCMOVEDOWN;
	case KEY_UP:                /* up-arrow key */
	    return CCMOVEUP;
	case KEY_LEFT:              /* left arrow key */
	    return CCMOVELEFT;
	case KEY_RIGHT:             /* right arrow key */
	    return CCMOVERIGHT;
	case KEY_HOME:              /* home key */
	    return CCHOME;
	case KEY_NPAGE:             /* next-page key */
	    return CCPLPAGE;
	case KEY_PPAGE:             /* previous-page key */
	    return CCMIPAGE;
	case KEY_IC:                /* insert key */
	    return CCINSMODE;
	case KEY_RESIZE:            /* ncurses SIGWINCH event */
	    return CCNULL;          /* sent by older versions */

#ifdef OUT
	/*  We could provide defaults for various
	 *  F1-F12 keys, but, instead let the user
	 *  define them in a keyboard file
	 */
	case 0414:
       /*   dbgpr("MapCursesKey:  F4 pressed\n"); */
	    return NOCHAR;

	case 0415:
	/*  dbgpr("MapCursesKey:  F5 pressed\n"); */
	    return NOCHAR;

	case KEY_F(2):
	/*  dbgpr("MapCursesKey:  F2 pressed\n"); */
	    return KEY_F(2);
#endif /* OUT */

	case KEY_F(6):
	/*  dbgpr("MapCursesKey:  F6 pressed\n"); */
	/*  return KEY_F(6); */
	    return CCMOUSEONOFF;

	default:
	    return NOCHAR;  /* 0400, NOTE this conflicts with KEY_CODE_YES below */
			    /* but is unlikely to be any issue */
    }
    return NOCHAR;
}


/* debug routines, these require an #include of e.keys.h */
/*
 *   Lookup the name of an E command code
 *   eg, for 012 return "CCMOVEDOWN"
 */

char *
getEkeyname(int c) {

    unsigned int i;

    for(i=0; i < sizeof(E_Keys) / sizeof(E_Keys[0]); i++) {
       if( E_Keys[i].val == c )
	 return E_Keys[i].name;
    }
    return "NOT FOUND";
}

/*
 *   Lookup the name of a curses keycode
 *   eg, for 0402 return "KEY_DOWN"
 */

char *
getCursesKeyname (int c) {
    unsigned int i;

    if( c < KEY_MIN || c > MAX_KEY ) {
	return "NOT FOUND";
    }
    for(i=0; i < sizeof(Curses_Keys) / sizeof(Curses_Keys[0]); i++) {
       if( Curses_Keys[i].val == c )
	 return Curses_Keys[i].name;
    }

    return "NOT FOUND";
}



void
addCursesFkey (char *line ) {

    char keyname[64], *cp;
    int keycode, offset, idx, i;


    if ((cp = index(line, ':')) == NULL) {
	getout (YES, "kbfile no colon found in %s\n", line);
    }
    offset = (int) (cp - line);
    strncpy (keyname, line, (size_t) offset);
    keyname[offset] = '\0';
/** /dbgpr("addCursesFkey:  keyname = (%s), value=(%s)\n", keyname, cp); / **/

    /* If we have KEY_nnn where nnn is the decimal keycode,
     * there's no need to look it up.
     */
    keycode = 0;
    if (!strncmp(keyname, "KEY_", 4)) {
	char *s = keyname + 4;
	if (isdigit(*s) && isdigit(*(s+1)) && isdigit(*(s+2))) {
	    sscanf(s, "%d", &keycode);
	/*  dbgpr("%s keycode is %d\n", keyname, keycode); */
	}
    }

    /* do not have KEY_nnn */
    if (keycode == 0) {
	if ((keycode = CursesKeyValue(keyname)) == -1) {
	    dbgpr("%s is not a recognized ncurses keyname.\n", keyname);
	    getout (YES, "%s is not a valid ncurses keyname\n", keyname);
	    return;
	}
    }

    if (keycode > MAX_KEY)
	getout(YES, "kbfile error:  max key value of %d in %s is decimal %d (%04o).\n",
	    keycode, keyname, MAX_KEY, MAX_KEY);

    idx = keycode - KEY_MIN;
/** /
    dbgpr("keycode=%d %o idx=%d for key=(%s) MAX_KEY=%d \n",
	keycode, keycode, idx, keyname, MAX_KEY);
/ **/
    /*
     *  Punt if the the keycode is already defined
     */
    if (Key2Ecode[idx].keycode == keycode) {
	getout(YES, "kbfile error: key %s is already defined.", keyname);
    }

    Uchar ecmds[128];
    int cnt = 0;  /* number of ecmds[] entries */

/*  Uchar tmp[128], *tp; */

    cp++;   /* 1st char after ":" */
    parseEcmds(cp, ecmds, &cnt, line);

#ifdef OUT
    /* dbg */
    dbgpr("len=%d, ecmds=", cnt);
    for (i=0; i <= cnt; i++)
	dbgpr("(%o)(%c),", ecmds[i], ecmds[i] );
    dbgpr("\n\n");
#endif

    strcpy(Key2Ecode[idx].keyname, keyname);    /* tmp for debugging */
    Key2Ecode[idx].keycode = keycode;  /* as a check if key already defined */
    Key2Ecode[idx].len = cnt;

    /* It is likely that the vast majority of F-key mappings
     * will map to a single E function, so we will save 'something'
     * by avoiding the calloc for a single byte.
     */
    Key2Ecode[idx].ecmd1 = ecmds[0];
    if (cnt > 1) {  /* copy remaining e codes to ecmdstr */
	Key2Ecode[idx].ecmdstr = calloc((size_t) cnt, sizeof(Uchar));
	for (i=0; i < cnt; i++)
	    Key2Ecode[idx].ecmdstr[i] = ecmds[i+1];
	Key2Ecode[idx].ecmdstr[i] = '\0';   /* !!! */

	/* Note that Key2Ecode[idx].len is one greater than
	 * the length of Key2Ecode[idx].ecmdstr
	 */

#ifdef OUT
	/* dbg */
	for (i=0; i < cnt - 1; i++)
	    dbgpr("ecmdstr[%d] = (%o)(%c)\n",i,
		Key2Ecode[idx].ecmdstr[i],
		isprint(Key2Ecode[idx].ecmdstr[i]) ? Key2Ecode[idx].ecmdstr[i] : ' ');
#endif
    }

    /* save the alternate interrupt char, via a Fkey */
    if (ecmds[0] == CCINT)
	altINTchar = keycode;

    return;
}



int
CursesKeyValue(char *keyname) {
    int i;
    for (i=0; CursesKeyCodes[i].val != -1; i++) {
	if( strncmp(CursesKeyCodes[i].name, keyname, strlen(keyname)) == 0 ) {
	/*  dbgpr("CursesKeyValue:  (%s)=(%d)\n", keyname,  CursesKeyCodes[i].val); */
	    return CursesKeyCodes[i].val;
	}
    }
    dbgpr("CursesKeyValue:  (%s) not found\n", keyname);
    return -1;
}


int
getFkeychar() {
    if (FKey_p) {
	int c = (int) *FKey_p++;
	/*if (*FKey_p == '\0') { */  /* no, string can contain nulls */
	if (FKey_p >= FKey_end) {
	    FKey_p = FKey_end = NULL;
	    pendFKeys = 0;
	}
    /** /dbgpr("FKey_p, c=(%03o)\n", c); / **/
	return c;
    }
    return NOCHAR;
}

/*  Lookup ^C in KeyCtrl[] and
 *
 *  1.  return the standard keyboard definition if not defined in
 *      users keyboard file
 *  2.  return a single E cmd if the mapping is one-to-one
 *  3.  for a one-to-many mapping, return 1st E cmd, and set a ptr
 *      to the remaining E cmds to be picked up in mGetkey()
 */


Char
MapCtrlKey(int c)
{

/** / dbgpr("MapCtrlChar: c=(%03o)(%d)\n", c, c);  / **/

    if (c >= 32) {
	dbgpr("MapCtrlKey: c=%d not a ctrl-c, too large  >= 32\n", c);
	return NOCHAR;
    }

    int idx = c;
    FKey_p = FKey_end = (Uchar *) NULL;

    pendFKeys = 0;  /* set when a key maps to more than one E cmd */

/** /
dbgpr("MapCtrlKey, idx=%d, returning (%03o)(%s) for key=(%03o)\n",
   idx, KeyCtrl[idx].ecmd1, getEkeyname(KeyCtrl[idx].ecmd1), c);
/ **/

    if ( KeyCtrl[idx].len > 1 ) {
	/*  A ctrl-key maps to more than one E cmd.  We return the
	 *  1st cmd and set a ptr to the rest so that mGetkey1()
	 *  will input the remainder after processing the 1st one.
	 *  Note that .len is one greater than length of .ecmdstr.
	 */
	FKey_p = KeyCtrl[idx].ecmdstr;
	FKey_end = FKey_p + KeyCtrl[idx].len - 1;
    /*  dbgpr("setting FKey_end to FKey_p + %d\n", KeyCtrl[idx].len-1); */
	pendFKeys = 1;
    }

    return( (int) KeyCtrl[idx].ecmd1 );
}




/*  Parse right hand side of a keyboard mapping.
 *
 *  Examples:
 *      ^U:<mark>
 *      <esc>"[4~":<exit><ret>
 *
 *  returns 0 on success, exits with a getout(...) on ERR *
 *
 *  cp points to the first char after the ':'
 *  ecmds[] is populated with the Ecode(s) that "<xxx>" maps to
 *  cmdlen = number of elements in ecmds[]
 *  line = original line from keyboard file
 */


int
parseEcmds(char *cp, Uchar *ecmds, int *cmdlen, char *line)
{
    Uchar *tp;
    Uchar tmp[128];
    int i;
    int cnt = 0;  /* number of ecmds[] entries */

/** /
dbgpr("parseEcmds: cp[0]=(%o) line=(%s)\n", *cp, line);
/ **/

    /* parse origline looking up <ecmd> values */
    for (; *cp; cp++) {
	if (*cp == '<') {       /* eg, <+page> */
		/* The code in itparse allows "^B:<nnn>" where nnn is
		 * the octal code of an E function,  eg, <013> is <home>.
		 * This is not imlemented here.
		 */
	    tp = tmp;
	    cp++;
	    while (*cp && *cp != '>')
		*tp++ = (Uchar) *cp++;
	    *tp = '\0';
	    if (!*cp) {   /* '>' not found */
		dbgpr("bad kbfile entry, missing '>' in  (%s)\n", line);
		getout(YES, "kbfile entry missing '>' in line: %s", line);
		return -1;
	    }

    /** /   dbgpr("parseEcmds: looking up <%s>\n", tmp); / **/

	    int cmdval = -1;
	    for (i=0; itsyms[i].str != NULL; i++) {
		if (strcmp(itsyms[i].str, (char *)tmp) == 0) {
		    cmdval = (int) itsyms[i].val;
		/** /
		    dbgpr(" -- itsyms: found (%s) matches (%s) at i=%d cmdval=(%03o)\n",
			tmp, itsyms[i].str, i, cmdval);
		/ **/
		    break;
		}
	    }
	    if (cmdval == -1) {
		dbgpr("<%s> not found\n", tmp);
		getout(YES, "<%s> is not a valid e command in line: %s", tmp, line);
		return -1;
	    }
	/*  dbgpr("lookup, <%s> is ecmd %04o\n", tmp, cmdval); */

	    ecmds[cnt++] = (Uchar) cmdval;
	    ecmds[cnt] = '\0';
    /** /   dbgpr("adding cmd=(%o) to ecmds[], len now=%d\n", cmdval, cnt); / **/
	}
	else if (*cp == '"') {  /* eg, KEY_F3:<cmd>"-er"<ret> */
	    tp = tmp;
	    cp++;
	    while (*cp && *cp != '"')
		*tp++ = (Uchar) *cp++;
	    *tp = '\0';

	    if (!*cp) {   /* " not found */
		dbgpr("bad kbfile entry, missing \" in  (%s)\n", line);
		getout(YES, "kbfile entry missing \" in line: %s", line);
		return -1;
	    }

	    /* copy the text to ecmds */
	    tp = tmp;
	    while (*tp) {
		ecmds[cnt++] = *tp++;
	    }
	    ecmds[cnt] = '\0';
	/*  dbgpr("adding text=(%s) to ecmds[], len now=%d\n", tmp, cnt); */
	}
	else if (*cp == '^') {  /* convert Ucase char to ctrl char, A -> ^A */
	    cp++;
	    if (!isupper(*cp)) {
		getout (YES, "kbfile error: ^%c needs to be uppercase A-Z in %s\n", *cp, line);
	    }
	    /* ecmds[cnt++] = *cp^0100; */
	    ecmds[cnt++] = CCCTRLQUOTE;   /* 034 */
	    ecmds[cnt++] = (Uchar) *cp;
	    ecmds[cnt] = '\0';
	/*  dbgpr("added ^%c = (%o) to %s\n", *cp, ecmds[cnt-1], keyname); */
	}
	else {  /* ?? */
	    dbgpr("bad kbfile entry, missing '<' or \" in  (%s)\n", line);
	    getout(YES, "kbfile entry does not start with '<' or \" in line: %s", line);
	    return -1;
	}
    }

    *cmdlen = cnt;

    return 0;
}

/* returns 1 if ok */
int
okCtrlChar(int c)
{
    if (c != '?' && (c < '@' || c > '_'))
	return 0;

    return 1;
}



/* Update KeyCtrl[] with user's keyboard mapping, eg:
 *   ^o:<open> -> {CCOPEN}
 *   ^r:<cmd><right>  -> {CCCMD}{CCMOVERIGHT}
 */

void
addCtrl2Ecmds(char *line)   /* line:   ^c:cmds */
{
    char *cp;
    int i, ch;
    int idx;

    if ((cp = index(line, ':')) == NULL) {
	getout (YES, "kbfile no colon found in %s\n", line);
    }
    cp++;

    ch = line[1];
    idx = ch^0100;  /* eg, 'A' -> ctrl-A */

    Uchar ecmds[128];
    int cmdlen; /* number of ecmds[] entries */

    parseEcmds(cp, ecmds, &cmdlen, line);

/** /
dbgpr("addCtrl: got (%03o)('%c') idx=%d ecmds[0]=(%03o) ECMD=(%s) cmdlen=%d line=(%s)\n",
    ch, ch, idx, ecmds[0], getEkeyname(ecmds[0]), cmdlen, line);
/ **/

    KeyCtrl[idx].ecmd1 = ecmds[0];
    KeyCtrl[idx].len = cmdlen;

    /* save the interrupt char */
    if (ecmds[0] == CCINT)
	defINTchar = idx;   /* eg, ^\ */

    if (cmdlen > 1) {  /* copy remaining e codes to ecmdstr */
	KeyCtrl[idx].ecmdstr = calloc((size_t) cmdlen, sizeof(Uchar));
	for (i=0; i < cmdlen; i++)
	    KeyCtrl[idx].ecmdstr[i] = ecmds[i+1];
	KeyCtrl[idx].ecmdstr[i] = '\0';   /* !!! */
    }

    return;
}


/*    Handles kbfile entries beginning with '<', eg: <esc> or ^X^c,
 *    via the ncurses define_key(string, value) routine.
 *    The advantage is that ncurses will collect a string like
 *    \033OP eg, when one hits a Fkey and return a single
 *    integer value for the key.
 */

void
addDefineKey(char *line)   /* have <esc>c:cmds , or ^X^A:ecmds */
{
    char *cp;
    int i;
    char lhs[16];   /* line:  lhs:ecmds */

    /* sets the 1st index into Key2Ecode[idx] */
    static int define_idx = KEY2ECODE_SIZE - N_DEFINED_KEYS;

    if ((cp = index(line, ':')) == NULL) {
	getout (YES, "kbfile no colon found in %s\n", line);
    }

    if ((cp - line) > 15)
	getout (YES, "kbfile, number of chars before colon >= 15: %s\n", line);

    strncpy(lhs, line, (size_t)(cp-line));
    lhs[cp-line] = '\0';
    cp++;

    /* two special cases */
    if (!strncmp(lhs, "<kbinit>", 8)) {
	kbinilen = (int) strlen(cp);
	kbinistr = salloc(kbinilen, NO);
	my_move(cp, kbinistr, (unsigned long)kbinilen);
	return;
    }
    if (!strncmp(lhs, "<kbend>", 8)) {
	kbendlen = (int) strlen(cp);
	kbendstr = salloc(kbendlen, NO);
	my_move(cp, kbendstr, (unsigned long)kbendlen);
	return;
    }

/** /
dbgpr("\naddDefineKey:  define_idx=%d lhs=%s KEY2ECODE_SIZE=%d\n",
define_idx, lhs, KEY2ECODE_SIZE);
/ **/

    int idx = define_idx;   /* next define_key() entry */

    Uchar ecmds[128];
    int cmdlen;

    /* Copy rhs into ecmds[] converting <xxx> tags
     * into E cmd codes
     */
    parseEcmds(cp, ecmds, &cmdlen, line);

/** /
dbgpr("addDefineKey: ecmds[0]=(%03o) cmdlen=%d line=%s\n",
    ecmds[0], cmdlen, line);
/ **/

    char tmp[16], *s;

    /*  Now convert lhs[] into a string suitable for
     *  the define_key() function
     */

    s = lhs;
    if (!strncmp(lhs, "<esc>", 5) || !strncmp(lhs, "<alt>", 5)) {
	tmp[0] = '\033';
	for (i=1, s += 5; *s; s++) {
	    if (*s == '"')        /* eg, <esc>"o":<open> */
		continue;
	    tmp[i++] = *s;
	}
	tmp[i] = '\0';
    }
    else if (lhs[0] == '^' && lhs[2] == '^') { /* ^X^C */
	if (!okCtrlChar(lhs[1]) || !okCtrlChar(lhs[3])) {
	    getout (YES, "kbfile error: ^%c^%c allowed chars: \"A-Z, ?, \\, ], ^, _\" in %s\n",
	      lhs[1], lhs[3], line);
	}
	tmp[0] = lhs[1]^0100;  /*CTRL(lhs[1]);*/
	tmp[1] = lhs[3]^0100;  /*CTRL(lhs[3]);*/
	tmp[2] = '\0';
    }
    else if (!strncmp(lhs, "<del>", 6)) {
	tmp[0] = 0177;
	tmp[1] = '\0';
    }
	/* eg, <0nnn>  octal ch */
    else if (lhs[0] == '<' && lhs[1] >= '0' && lhs[1] <= '7') {
	int n;
	if (sscanf(lhs, "<%o>", &n) != 1) {
	    dbgpr("sscanf for <octal code> failed on line: %s\n", line);
	    return;
	}
	tmp[0] = (char) n;
	tmp[1] = '\0';
    /** /dbgpr("addDefineKey: have octal ch=(%03o)(%d) in line=(%s)\n",
	    n, n, line); / **/
    }
    else {
	dbgpr("addDefineKey: entry not supported: lhs=(%s) line=(%s)\n", lhs, line);
	getout (YES, "kbfile error: \"%s\" not supported in line %s\n", lhs, line);
	return;
    }

    int keycode = idx + KEY_MIN;
    int rc = define_key(tmp, keycode);
    if (rc != 0) {
	dbgpr("define_key failed on line %s\n", line);
	return;
    }

/** /
dbgpr("  tmp[0][1]=(%03o)(%03o) idx=%d ecmd[0]=(%03o) ECMD=(%s) cmdlen=%d line=(%s)\n",
    tmp[0], tmp[1], keycode, ecmds[0], getEkeyname(ecmds[0]), cmdlen, line);
/ **/

/** /
dbgpr("rc=%d from define_key(%s,%d), idx=%d\n", rc, tmp, keycode, idx);
/ **/
    Key2Ecode[idx].keycode = keycode; /* for debugging */
    Key2Ecode[idx].ecmd1 = ecmds[0];
    Key2Ecode[idx].len = cmdlen;

    if (cmdlen > 1) {  /* copy remaining e codes to ecmdstr */
	Key2Ecode[idx].ecmdstr = calloc((size_t) cmdlen, sizeof(Uchar));
	for (i=0; i < cmdlen; i++)
	    Key2Ecode[idx].ecmdstr[i] = ecmds[i+1];
	Key2Ecode[idx].ecmdstr[i] = '\0';   /* !!! */
    }

    define_idx++;

    return;
}
