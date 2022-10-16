#ifdef COMMENT
--------
file e.iit.c
    initialize input (keyboard) table parsing and lookup
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.it.h"

extern void addCursesFkey();
extern void addCtrl2Ecmds();
extern void addDefineKey();

S_looktbl itsyms[] = {
    {"+line",   CCPLLINE},
    {"+page",   CCPLPAGE},
    {"+sch",    CCPLSRCH},
    {"+tab",    CCTAB},
    {"+word",   CCRWORD},
    {"-close",  CCMICLOSE},
    {"-erase",  CCMIERASE},
    {"-line",   CCMILINE},
    {"-mark",   CCUNMARK},
    {"-page",   CCMIPAGE},
    {"-pick",   CCPUT},
    {"-sch",    CCMISRCH},
    {"-tab",    CCBACKTAB},
    {"-word",   CCLWORD},
    {"abort",   CCABORT},
    {"bksp",    CCBACKSPACE},
    {"blot",    CCBLOT},
    {"box",     CCBOX},
    {"brace",   CCBRACE},
#ifdef LMCCASE
    {"caps",    CCCAPS},
    {"ccase",   CCCCASE},
#endif /* LMCCASE */
    {"cchar",   CCCTRLQUOTE},
    {"center",  CCCENTER},
    {"chwin",   CCCHWINDOW},
    {"clear",   CCCLEAR},
    {"close",   CCCLOSE},
    {"cltabs",  CCCLRTABS},
    {"cmd",     CCCMD},
    {"cover",   CCCOVER},
    {"dchar",   CCDELCH},
    {"del",     0177},
    {"down",    CCMOVEDOWN},
#ifdef LMCDWORD
    {"dword",   CCDWORD},
#endif /* LMCDWORD */
    {"edit",    CCSETFILE},
    {"erase",   CCERASE},
    {"esc",     033},
    {"exit",    CCEXIT},
    {"fill",    CCFILL},
#ifdef LMCHELP
    {"help",    CCHELP},
#endif /* LMCHELP */
    {"home",    CCHOME},
    {"insmd",   CCINSMODE},
    {"int",     CCINT},
    {"join",    CCJOIN},
    {"justify", CCJUSTIFY},
    {"kbend",   KBEND},
    {"kbinit",  KBINIT},
    {"left",    CCMOVELEFT},
    {"mark",    CCMARK},
    {"mouse",   CCMOUSEONOFF},
    {"null",    CCNULL},
    {"open",    CCOPEN},
    {"overlay", CCOVERLAY},
    {"pick",    CCPICK},
#ifdef RECORDING
    {"play",    CCPLAY},
#endif
    {"put",     CCPUT},
    {"quit",    CCQUIT},
    {"quote",   042},
    {"range",   CCRANGE},
#ifdef RECORDING
    {"record",  CCRECORD},
#endif
    {"redraw",  CCREDRAW},
    {"regex",   CCREGEX},
    {"replace", CCREPLACE},
    {"ret",     CCRETURN},
    {"right",   CCMOVERIGHT},
    {"space",   040},
    {"split",   CCSPLIT},
    {"srtab",   CCTABS},
    {"stopx",   CCSTOPX},
    {"tab",     CCTAB},
    {"track",   CCTRACK},
    {"undef",   CCUNAS1},
    {"up",      CCMOVEUP},
    {"wleft",   CCLWINDOW},
    {"wp",      CCAUTOFILL},
    {"wright",  CCRWINDOW},
    {0,         0}
 };

char *kbinistr;
char *kbendstr;
int  kbinilen;
int  kbendlen;

void
getkbfile (filename)
char *filename;
{
    char line[TMPSTRLEN];
    FILE *f;
    int lineno = 0;

    if ((f = fopen (filename, "r")) == NULL) {
	/* todo, try filename in user's home directory */
	/* dbgpr("mypath=%s\n", mypath); */
	getout (YES, "Can't open keyboard file \"%s\"", filename);
    }

    while (fgets (line, sizeof line, f) != NULL) {
	line[strlen (line)-1] = '\0';   /* Stomp on the newline */
	lineno++;

/** / dbgpr("getkbfile: line=%s\n", line); / **/

	if(line[0] == '#' || line[0] == '\0')   /* gww 21 Feb 82 */
	    continue;

	/* eg, KEY_F4,  SHIFT_F8,  CTRL_F2 */
	if (strstr(line, "KEY_") || strstr(line, "SHIFT_") ||
		strstr(line, "CTRL_")) {
	    addCursesFkey(line);
	    continue;
	}

	/* map single ctrl-c, eg:  ^o:<open> */
	if (line[0] == '^' && line[2] == ':') {
	/*  dbgpr("calling addCtrl2Ecmds\n"); */
	    addCtrl2Ecmds(line);
	    continue;
	}

	/* map entries starting with '<' (eg, <esc>) or ^X^C */
	if ( line[0] == '<' ||
	       (line[0] == '^' && line[1] == 'X' && line[2] == '^') ) {
	/*  dbgpr("calling addDefineKey: %s\n", line); */
	    addDefineKey(line);
	    continue;
	}
	dbgpr("bad keyboard entry at line %d:  %s\n", line);
	continue;
    }
    fclose(f);
    return;
}

