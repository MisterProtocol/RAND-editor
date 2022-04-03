#ifdef COMMENT
--------
file e.cm.c
    command dispatching routine and some actual command-executing routines.
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#ifdef UNIXV7
#include <ctype.h>
#endif
#include "e.h"
#include "e.e.h"
#include "e.m.h"
#include "e.ru.h"
#include "e.cm.h"
#include "e.wi.h"
#ifdef LMCHELP
#include "e.tt.h"
#endif /* LMCHELP */

extern Flag play_silent;
extern Cmdret
	edit (void),
	setoption (int);


extern int zaprpls ();
extern uid_t fgetowner ();

extern Cmdret gotocmd (void);
extern void fresh ();
extern void dostop (void);

#ifdef NCURSES
extern int toggle_mouse();
extern Flag highlight_mode;
extern Flag redrawflg;
extern char *smso, *rmso, *bold_str, *setab, *setaf, *sgr0, *setab_p, *setaf_p, *hilite_str;
extern char *fgbg_pair;
int doSetHighlight(char *arg);
char *highlight_info_str;
extern int n_colors;

/* brace matching/hilighting */
int braceRange = 100;           /* match to +/- braceRange lines */
Flag bracematchCoding = YES;    /* stop srch at beg/end of a function */
extern int bracematching;
#endif

char *exitmessage;  /* for new 'unsafe' option to eexit() */

#include SIG_INCL

S_looktbl cmdtable[] = {
#ifdef RECORDING
#define MACRO_CMDTBL_INDEX 0
   {"$"       , CMDMACRO    },
#endif
   {"-blot"   , CMD_BLOT    },
   {"-close"  , CMD_CLOSE   },
   {"-command", CMD_COMMAND },
#ifdef NOTYET
   {"-diff"   , CMD_DIFF    },
#endif /* NOTYET */
#ifdef LMCDWORD
   {"-dword"  , CMD_DWORD   },
#endif /* LMCDWORD */
   {"-erase"  , CMD_ERASE   },
   {"-join"   , CMDSPLIT    },
   {"-mark"   , CMD_MARK    },
   {"-pick"   , CMD_PICK    },
   {"-range"  , CMD_RANGE   },
   {"-re"     , CMD_PATTERN },  /* Added Purdue CS 10/18/82 MAB */
#ifdef RECORDING
   {"-record" , CMDRECORD   },
#endif
   {"-regexp" , CMD_PATTERN },  /* Added Purdue CS 10/18/82 MAB */
   {"-replace", CMD_REPLACE },
   {"-search" , CMD_SEARCH  },
   {"-split"  , CMDJOIN     },
   {"-tab"    , CMD_TAB     },
   {"-tabfile", CMD_TABFILE },
   {"-tabs"   , CMD_TABS    },
   {"-track"  , CMD_TRACK   },
   {"-update" , CMD_UPDATE  },
   {"-w"      , CMD_WINDOW  },
   {"-window" , CMD_WINDOW  },
#ifdef LMCAUTO
   {"-wp"     , CMD_AUTO    },
#endif /* LMCAUTO */
#ifdef RECORDING
   {"?macros" , CMDQMACROS  },
#endif
   {"?range"  , CMDQRANGE   },
   {"?set"    , CMDQSET     },
   {"b"       , CMDEXIT     },
   {"blot"    , CMDBLOT     },
   {"box"     , CMDBOX      },
   {"bye"     , CMDEXIT     },
#ifdef DOCALL
   {"call"    , CMDCALL     },
#endif /* DOCALL */
#ifdef LMCCASE
   {"caps"    , CMDCAPS     },
   {"ccase"   , CMDCCASE    },
#endif /* LMCCASE */
   {"center"  , CMDCENTER   },
   {"clear"   , CMDCLEAR    },
   {"close"   , CMDCLOSE    },
   {"command" , CMDCOMMAND  },
   {"cover"   , CMDCOVER    },
#ifdef DELETE
   {"delete"  , CMDDELETE   },
   {"delete"  , CMDDELETE   },
#endif
#ifdef NOTYET
   {"diff"    , CMDDIFF     },
#endif /* NOTYET */
   {"dword"   , CMDDWORD    },
   {"e"       , CMDEDIT     },
   {"edit"    , CMDEDIT     },
#ifdef NOTYET
   {"endm"    , CMDENDM     },
#endif /* NOTYET */
   {"erase"   , CMDERASE    },
   {"exit"    , CMDEXIT     },
   {"feed"    , CMDFEED     },
   {"fill"    , CMDFILL     },
   {"goto"    , CMDGOTO     },
   {"help"    , CMDHELP     },
   {"insert"  , CMDINSERT   },
   {"join"    , CMDJOIN     },
   {"justify" , CMDJUST     },
#ifdef NOTYET
   {"logoff"  , CMDLOGOUT   },
   {"logout"  , CMDLOGOUT   },
#endif /* NOTYET */
   {"mark"    , CMDMARK     },
   {"mouse"   , CMDMOUSEONOFF},
   {"name"    , CMDNAME     },
   {"open"    , CMDOPEN     },
   {"overlay" , CMDOVERLAY  },
   {"pick"    , CMDPICK     },
#ifdef RECORDING
   {"play"    , CMDPLAY     },
#endif
#ifdef NOTYET
   {"print"   , CMDPRINT    },
#endif /* NOTYET */
   {"range"   , CMDRANGE    },
   {"re"      , CMDPATTERN  },
#ifdef RECORDING
   {"record"  , CMDRECORD   },
#endif
   {"redraw"  , CMDREDRAW   },
   {"regexp"  , CMDPATTERN  },
   {"replace" , CMDREPLACE  },
   {"run"     , CMDRUN      },
   {"save"    , CMDSAVE     },
   {"search"  , CMDSEARCH   },
   {"set"     , CMDSET      },
#ifdef DOSHELL
   {"shell"   , CMDSHELL    },
#endif /* DOSHELL */
   {"split"   , CMDSPLIT    },
/*      DEBUG
   {"stats"   , CMDSTATS    },
 */
#ifdef SIGTSTP  /* 4bsd vmunix */
   {"stop"    , CMDSTOP     },
#endif
#ifdef RECORDING
   {"store"   , CMDSTORE    },
#endif
   {"tab"     , CMDTAB      },
   {"tabfile" , CMDTABFILE  },
   {"tabs"    , CMDTABS     },
   {"track"   , CMDTRACK    },
#ifdef RECORDING
   {"undefine", CMDUNDEFINE },
#endif
   {"underlay", CMDUNDERLAY },
   {"undo"    , CMDUNDO     },
   {"update"  , CMDUPDATE   },
   {"w"       , CMDWINDOW   },
   {"window"  , CMDWINDOW   },
#ifdef LMCAUTO
   {"wp"     ,  CMDAUTO     },
#endif /* LMCAUTO */
   {0, 0}

};


#ifdef COMMENT
Cmdret
command (forcecmd, forceopt)
    int forcecmd;
    char *forceopt;
.
    Parses a command line and dispatches on the command.
    If a routine that was called to execute a command returns a negative
    value, then the error message is printed out here, and returns CROK.
    Else returns the completion status of the command.
#endif
Cmdret
command (forcecmd, forceopt)
    int forcecmd;
    char *forceopt;
{
    Short cmdtblind;
    Cmdret retval;
    Short cmdval;
    char *cmdstr;
    int clrcnt;                 /* Added 10/18/82 MAB */
#ifdef LMCHELP
    char helparg[128];
#endif /* LMCHELP */
#ifdef RECORDING
    char cmdName[128];  /* never exceed that, right!! */
#endif /* RECORDING */

    if (forcecmd != 0) {
	cmdval = forcecmd;
	for (cmdtblind=0; cmdval != cmdtable[cmdtblind].val; cmdtblind++)
		;
	nxtop = forceopt;
    } else {
	nxtop = paramv;
	cmdstr = getword (&nxtop);
	if (cmdstr[0] == '\0')
	    return CROK;
#ifdef RECORDING
	    /* Special case for macro calls so one can do "CMD: $name".
	     * All we care about here is that it starts with "$".
	     * The lookup() routine requires "name" to match.
	     */
	if (*cmdstr == '$') {
	    cmdtblind = MACRO_CMDTBL_INDEX;
	    strcpy (cmdName, cmdstr);
	}
	else
#endif /* RECORDING */
	cmdtblind = lookup (cmdstr, cmdtable);
	if (cmdtblind == -1) {
	    if (cmdstr[0] >= '1' && cmdstr[0] <= '9') {
		nxtop = paramv;
		cmdtblind = lookup ("g", cmdtable);
	    } else {
		mesg (ERRALL + 3, "Command \"", cmdstr, "\" not recognized");
		sfree (cmdstr);
		return CROK;
	    }
	}
	else if (cmdtblind == -2) {
	    mesg (ERRALL + 3, "Command \"", cmdstr, "\" ambiguous");
	    sfree (cmdstr);
	    return CROK;
	}
	sfree (cmdstr);
	cmdval = cmdtable [cmdtblind].val;
    }
    cmdopstr = nxtop;
    opstr = getword (&nxtop);
    cmdname = cmdtable[cmdtblind].str;
/*dbgpr("command: cmdname=%s cmdval=(%o)\n", cmdname, cmdval);*/
    switch (cmdval) {

    case CMDRANGE:
    case CMD_RANGE:
    case CMDQRANGE:
	retval = rangecmd (cmdval);
	break;

    case CMDTRACK:
	if ((curwin->winflgs & TRACKSET) == 0)
	    curwin->winflgs |= TRACKSET;
	else
	    curwin->winflgs &= ~TRACKSET;
	infotrack (TRACKSET & curwin->winflgs);
	retval = CROK;
	break;

    case CMD_TRACK:
	curwin->winflgs &= ~TRACKSET;
	infotrack (NO);
	retval = CROK;
	break;

#ifdef SIGTSTP  /* 4bsd vmunix */
    case CMDSTOP:
	dostop ();
	retval = CROK;
	break;
#endif

    case CMDUPDATE:
    case CMD_UPDATE:
	retval = my_doupdate (cmdval == CMDUPDATE);
	break;

    case CMDTAB:
	retval = dotab (YES);
	break;

    case CMD_TAB:
	retval = dotab (NO);
	break;

    case CMDTABS:
	retval = dotabs (YES);
	break;

    case CMD_TABS:
	retval = dotabs (NO);
	break;

    case CMDTABFILE:
	retval = tabfile (YES);
	break;

    case CMD_TABFILE:
	retval = tabfile (NO);
	break;

    case CMDREPLACE:
    case CMD_REPLACE:
	if (!okwrite ()) {
	    retval = NOWRITERR;
	    break;
	}
#ifdef  FILELOCKING
	if (!islocked (YES)) {
	    retval = NOLOCKERR;
	    break;
	}
#endif /* FILELOCKING */
	retval = replace (cmdval == CMDREPLACE? 1: -1);
	break;

    case CMDPATTERN:            /* Added at CS/Purdue 10/3/82 MAB */
    case CMD_PATTERN:
	if (*cmdopstr)
	    retval = CRTOOMANYARGS;
	    else if (patmode && cmdval == CMDPATTERN){
		mesg(ERRALL+1, "You are in RE mode");
		retval = CROK;
	    }
	    else if (!patmode && cmdval == CMD_PATTERN){
		mesg(ERRALL+1, "You are not in RE mode");
		retval = CROK;
	    }
	else{
		tglpatmode();
		clrcnt = 0;
		if (searchkey){
		    sfree(searchkey);
		    searchkey = (char *) 0;
		    clrcnt++;
		}
		clrcnt += zaprpls();
		retval = CROK;
	}
	break;

    case CMDNAME:
	retval = name ();
	break;

#ifdef DELETE
    case CMDDELETE:
	retval = delete ();
	break;
#endif

    case CMDCOMMAND:
	mesg (TELALL + 1, "CMDS: ");
    case CMD_COMMAND:
	cmdmode = cmdval == CMDCOMMAND ? YES : NO;
	retval = CROK;
	break;

    case CMDPICK:
    case CMDCLOSE:
    case CMDERASE:
    case CMDOPEN:
    case CMDBOX:
#ifdef LMCCASE
    case CMDCAPS:
    case CMDCCASE:
#endif /* LMCCASE */
	retval = areacmd (cmdval - CMDPICK);
	break;

    case CMDCOVER:
    case CMDOVERLAY:
    case CMDUNDERLAY:
    case CMDBLOT:
    case CMD_BLOT:
    case CMDINSERT:
	retval = insert (cmdval - CMDCOVER);
	break;

    case CMD_PICK:
    case CMD_CLOSE:
    case CMD_ERASE:
	retval = insbuf (cmdval - CMD_PICK + QPICK);
	break;

#ifdef DOCALL
    case CMDCALL:
	retval = call ();
	/* if the syntax of the shell command was correct and all of the
	 * saves went OK, forkshell will never return. */
	break;
#endif /* DOCALL */

#ifdef DOSHELL
    case CMDSHELL:
	retval = shell ();
	/* if the syntax of the shell command was correct and all of the
	 * saves went OK, forkshell will never return. */
	break;
#endif /* DOSHELL */

#ifdef NOTYET
    case CMDLOGOUT:
	if (!loginflg) {
	    mesg (ERRALL + 1, "This is not your login program.  Use \"exit\".");
	}
#endif /* NOTYET */

    case CMDEXIT:
	retval = eexit ();
	/* if the syntax of the exit command was correct and all of the
	 * saves went OK, eexit will never return. */
	break;

#ifdef RECORDING
    case CMDMACRO:
	return DoMacro(cmdName, opstr);

    case CMDRECORD:
	return SetRecording(CMDRECORD);

    case CMDPLAY:
	{
	    int i;
	    for (i = 0; isdigit (*opstr); opstr++) {
		i *= 10;
		i += *opstr - '0';
	    }
	    return PlayRecording(i);
	}

    case CMDSTORE:
	return StoreMacro(opstr);

    case CMDUNDEFINE:
	return UndefMacro(opstr);

    case CMDQMACROS:
	ShowMacros(opstr);
	fresh();
	return CROK;

#endif /* RECORDING */


#ifdef LMCHELP
    case CMDHELP:
	if (replaying)
	    return CROK;
	nokeyfile = YES;
	mesg (ERRALL + 1, "Calling help....");
	if (term.tt_help == NULL) {
	    mesg (ERRALL + 1, "No help routine available for this crt.");
	    return CROK;
	} else {
	    /*strcpy (helparg, ETC);*/
	    strcpy (helparg, etcdir);
	    strcat (helparg, "/kr.");
	    if (opstr [0] == 'l') {
		/* helparg [8] = 'l'; */
		/*strcpy (helparg, ETC);*/
		strcpy (helparg, etcdir);
		strcat (helparg, "/kl.");
	    }
	    strcat (helparg, kname);
	    retval = (*term.tt_help)(helparg);
	    if (retval != CROK)
		break;
	}
	nokeyfile = NO;
	/* intentional fallthrough !! */
#endif /* LMCHELP */

    case CMDREDRAW:
	fresh ();
	retval = CROK;
	redrawflg = YES;
	break;

    case CMDSPLIT:
    case CMDJOIN:
	if (*cmdopstr)
	    retval = CRTOOMANYARGS;
	else
	    retval = cmdval == CMDJOIN ? join () : split ();
	break;

    case CMDRUN:
    case CMDFEED:
	if (!okwrite ()) {
	    retval = NOWRITERR;
	    break;
	}

#ifdef NORUNCMD_IN_PROFILE
	/* Unsafe:  don't allow run cmd in a .e_profile */
	if (dot_profile && cmdval == CMDRUN) {
	    exitmessage = "The run cmd is UNSAFE in the .e_profile startup file!";
	    opstr = "unsafe";
	    eexit();
	}
#endif

#ifdef  FILELOCKING
	if (!islocked (YES)) {
	    retval = NOLOCKERR;
	    break;
	}
#endif /* FILELOCKING */
	retval = run (cmdopstr, cmdval);
	break;

    case CMDFILL:
	retval = my_filter (FILLNAMEINDEX, YES);
	break;

    case CMDJUST:
	retval = my_filter (JUSTNAMEINDEX, YES);
	break;

    case CMDCENTER:
	retval = my_filter (CENTERNAMEINDEX, YES);
	break;

    case CMDSAVE:
	if (curmark) {
	    retval = NOMARKERR;
	    break;
	}
	retval = save ();
	break;

    case CMDEDIT:
	if (curmark) {
	    retval = NOMARKERR;
	    break;
	}
	retval = edit ();
/** dbgpr("command, edit() returned %d\n", retval); **/
	break;

    case CMDWINDOW:
	if (curmark) {
	    retval = NOMARKERR;
	    break;
	}
	if (*opstr == '\0')
	    makewindow ((char *)0);
	else {
	    if (*nxtop) {
		retval = CRTOOMANYARGS;
		break;
	    }
	    makewindow (opstr);
	}
	loopflags.bullet = YES;
	retval = CROK;
	break;

    case CMD_WINDOW:
	if (curmark) {
	    retval = NOMARKERR;
	    break;
	}
	if (*opstr) {
	    retval = CRTOOMANYARGS;
	    break;
	}
	removewindow ();
	retval = CROK;
	break;

    case CMDGOTO:
	retval = gotocmd ();
	break;

#ifdef LMCAUTO
    case CMDAUTO:
	retval = parsauto (NO);
	break;

    case CMD_AUTO:
	retval = parsauto (YES);
	break;
#endif /* LMCAUTO */

#ifdef LMCDWORD
    case CMDDWORD:
	retval = dodword (YES);
	break;

    case CMD_DWORD:
	retval = dodword (NO);
	break;
#endif /* LMCDWORD */

/* DEBUG
    case CMDSTATS:
	Block {  char temp[50];
		 extern Ff_stats ff_stats;
	    sprintf(temp, "sys r:%d w:%d s:%d; ff r:%d w:%d s:%d",
		ff_stats.fs_read, ff_stats.fs_write, ff_stats.fs_seek,
		ff_stats.fs_ffread, ff_stats.fs_ffwrite, ff_stats.fs_ffseek);
	    mesg (ERRALL + 1, temp);
	}
	retval = CROK;
	break;
 */
    case CMDSET:
	retval = setoption(NO);
	break;

    case CMDQSET:
	retval = setoption(YES);
	break;

#ifdef NOTYET
    case CMD_DIFF:
	retval = diff (-1);
	break;

    case CMDDIFF:
	retval = diff (1);
	break;
#endif /* NOTYET */

    case CMDMOUSEONOFF:
	retval = toggle_mouse(opstr);
	break;

    default:
	mesg (ERRALL + 3, "Command \"", cmdtable[cmdtblind].str,
		"\" not implemented yet");
	retval = CROK;
	break;
    }
    if (opstr[0] != '\0')
	sfree (opstr);

    if (retval >= CROK) {
/** dbgpr("command() returning %d\n", retval); **/
	return retval;
    }
    switch (retval) {
    case CRUNRECARG:
	mesg (1, " unrecognized argument to ");
	break;

    case CRAMBIGARG:
	mesg (1, " ambiguous argument to ");
	break;

    case CRTOOMANYARGS:
	mesg (ERRSTRT + 1, "Too many arguments to ");
	break;

    case CRNEEDARG:
	mesg (ERRSTRT + 1, "Need an argument to ");
	break;

    case CRNOVALUE:
	mesg (ERRSTRT + 1, "No value for option to ");
	break;

    case CRMULTARG:
	mesg (ERRSTRT + 1, "Duplicate arguments to ");
	break;

    case CRMARKCNFL:
	return NOMARKERR;

    case CRBADARG:
    default:
	mesg (ERRSTRT + 1, "Bad argument(s) to ");
    }
    mesg (ERRDONE + 3, "\"", cmdname, "\"");


    return CROK;
}

#ifdef COMMENT
Cmdret
gotocmd ()
.
    Do the "goto" command.
#endif
Cmdret
gotocmd ()
{
    if (opstr[0] == '\0') {
	gotomvwin (0);
/** / dbgpr ("gotocmd: Returned from first gotomvwin()\n"); / **/
	return CROK;
    }
    if (*nxtop)
	return CRTOOMANYARGS;

    Block {
	Reg2 Short tmp;
	Reg1 char *cp;
	char *cp1;

	cp1 = opstr;
	tmp = getpartype (&cp1, 0, 0, 0);
	if (tmp == 1) {
	    for (cp = cp1; *cp && *cp == ' '; cp++)
		continue;
	    if (*cp == 0) {
		gotomvwin (parmlines - 1);
/** / dbgpr ("gotocmd: Returned from second gotomvwin()\n"); / **/
		return CROK;
	    }
	}
	else if (tmp == 2)
	    return CRBADARG;
    }

    Block {
	static S_looktbl gttbl[] = {
	   {"b",            0},  /* guranteed abbreviation */
	   {"beginning",    0},
	   {"e",            1},  /* guranteed abbreviation */
	   {"end",          1},
	   {"rb",           2},  /* guranteed abbreviation */
	   {"rbeginning",   2},
	   {"re",           3},  /* guranteed abbreviation */
	   {"rend",         3},
	   {0,              0}
	};
	Reg1 Small ind;
	Reg2 Small val;
	if ((ind = lookup (opstr, gttbl)) < 0) {
	    mesg (ERRSTRT + 1, opstr);
	    return ind == -2 ? CRAMBIGARG : CRUNRECARG;
	}
	switch (val = gttbl[ind].val) {
	case 0:
	    gotomvwin ((Nlines) 0);
	    break;

	case 1:
	    gotomvwin (la_lsize (curlas));
	    break;

	case 2:
	case 3:
	    if (curwksp->brnglas)
		gotomvwin (la_lseek (val == 2
				     ? curwksp->brnglas : curwksp->ernglas,
				     0, 1));
	    else
		return NORANGERR;
	    break;
	}
    }
    return CROK;
}

#ifdef COMMENT
Cmdret
my_doupdate (on)
    Flag on;
.
    Do the "update" command.
    The 'on' argument is non-0 for "update" and 0 for "-update"
#endif
Cmdret
my_doupdate (on)
Flag on;
{
    Small ind;
/*  int owner;  */
    static S_looktbl updatetable[] = {
	{"-inplace",     0},
	    /*
	     *  --------------------  e19 change -------------------------
	     *  "update -readonly" is used: (1) when you've invoked e with
	     *  the -readonly option and you've now changed your mind; or
	     *  (2) you'd like to now make changes to a file in which you
	     *  own, but lack write permission on.
	     */
	{"-readonly",    0},
	{"inplace",      0},
#ifdef RECORDING
	{"macros",       0},
#endif
	{0        ,  0}
    };

    if (*nxtop || (!on && opstr[0] != '\0'))
	return CRTOOMANYARGS;

    if (on && !(fileflags[curfile] & DWRITEABLE)) {
	mesg (ERRALL + 1, "Don't have directory write permission");
	return CROK;
    }
    if (opstr[0] != '\0') {
	ind = lookup (opstr, updatetable);
	if (ind == -1  || ind == -2) {
	    mesg (ERRSTRT + 1, opstr);
	    return ind;
	}
	switch (ind) {
	    case 0:     /* -inplace */
		fileflags[curfile] &= ~INPLACE;
		break;

	    case 1:     /* -readonly */
		if (fileflags[curfile] & CANMODIFY)
		    break;
		if (fileflags[curfile] & FWRITEABLE ||
			userid == fgetowner (curfile))
		    fileflags[curfile] |= CANMODIFY;
		break;

	    case 2:     /* inplace */
		if (fileflags[curfile] & NEW) {
		    mesg (ERRALL + 1, "\"inplace\" is useless;  file is new");
		    return CROK;
		}
		if (!(fileflags[curfile] & FWRITEABLE)) {
		    mesg (ERRALL + 1, "Don't have file write permission");
		    return CROK;
		}
		fileflags[curfile] |= INPLACE | CANMODIFY;
		break;

#ifdef RECORDING
	    case 3:
		SaveMacros();
		break;
#endif /* RECORDING */
	}
    }
    if (on)
	fileflags[curfile] |= UPDATE;
    else
	fileflags[curfile] &= ~UPDATE;
    return CROK;
}


#define SET_PLLINE      1
#define SET_PLPAGE      2
#define SET_MILINE      3
#define SET_MIPAGE      4
#define SET_SHOW        5       /* very tacky usage below! */
#define SET_BELL        6
#define SET_WINLEFT     7
#define SET_LINE        8
#define SET_NOBELL      9
#define SET_PAGE        10
#define SET_WINRIGHT    11
#define SET_WIDTH       12
#define SET_WIN         13
#define SET_WORDDELIM   14
#define SET_DEBUG       15              /* internal debugging */

#ifdef LMCAUTO
#define SET_LMARG       16
#endif /* LMCAUTO */

#ifdef LMCVBELL
#define SET_VBELL       17
#endif /* LMCVBELL */

#define SET_RMSTICK     18
#define SET_RMNOSTICK   19

#define SET_HY          20
#define SET_NOHY        21

#define SET_FILLDOT     22
#define SET_NOFILLDOT   23

#ifdef RECORDING
#define SET_PLAY        24
#endif

#define SET_HIGHLIGHT   25
#define SET_NOHIGHLIGHT 26
#define SET_MOUSE       27

#define SET_BRACEMATCH  28

#ifdef COMMENT
Cmdret
setoption( showflag )
.
	Set/show various options
#endif /* COMMENT */

Cmdret
setoption( showflag )
  int showflag;
{
	Reg1 char *arg;
	Reg2 Small ind;
	Reg3 Small value;
	Cmdret retval = 0;
	extern Flag fill_hyphenate;
	extern Flag fill_troffmode;
	static S_looktbl setopttable[] = {    /* Must be in sort order!!! */
	   {"+line",        SET_PLLINE},   /* defplline */
	   {"+page",        SET_PLPAGE},   /* defplpage */
	   {"-line",        SET_MILINE},   /* defmiline */
	   {"-page",        SET_MIPAGE},   /* defmipage */
	   {"?",            SET_SHOW},     /* show options */
	   {"[]",           SET_BRACEMATCH},   /* set brace matching mode, shortcut */
	   {"bell",         SET_BELL},     /* echo \07 */
	   {"brace",        SET_BRACEMATCH}, /* set brace options */
	   {"brace ?",      SET_BRACEMATCH}, /* show brace range */
	   {"bracematch",   SET_BRACEMATCH},   /* set brace matching mode, options */
	   {"debug",        SET_DEBUG},
	   {"filldot",      SET_FILLDOT},  /* fill doesn't stops at ^. */
	   {"highlight",    SET_HIGHLIGHT},/* marked areas are highlighted */
	   {"hy",           SET_HY},       /* fill: split hyphenated words */
	   {"left",         SET_WINLEFT},  /* deflwin */
	   {"line",         SET_LINE},     /* defplline and defmiline */
#ifdef LMCAUTO
	   {"lmargin",      SET_LMARG},    /* left margin */
#endif /* LMCAUTO */
	   {"mouse",        SET_MOUSE},    /* enable/disable mouse */
	   {"nobell",       SET_NOBELL},   /* do not echo \07 */
	   {"nofilldot",    SET_NOFILLDOT},/* fill stops at ^. */
	   {"nohighlight",  SET_NOHIGHLIGHT},/* marked areas are highlighted */
	   {"nohy",         SET_NOHY},     /* fill: don't split hy-words */
	   {"nostick",      SET_RMNOSTICK},/* auto scroll past rt edge */
	   {"page",         SET_PAGE},     /* defmipage and defplpage */

#ifdef RECORDING
	   {"play",         SET_PLAY},     /* play options */
#endif
	   {"right",        SET_WINRIGHT}, /* defrwin */
#ifdef LMCAUTO
	   {"rmargin",      SET_WIDTH},    /* same as width */
#endif /* LMCAUTO */
	   {"stick",        SET_RMSTICK},  /* no auto scroll past rt edge */
#ifdef LMCVBELL
	   {"vbell",        SET_VBELL},    /* visible bell */
#endif /* LMCVBELL */
	   {"width",        SET_WIDTH},    /* linewidth */
	   {"window",       SET_WIN},      /* deflwin and defrwin */
	   {"worddelimiter",SET_WORDDELIM},/* set word delimiter */
	   {"{}",           SET_BRACEMATCH},   /* set brace matching mode, shortcut */
	   {0, 0}
	};

	if (showflag) {
	    ind = SET_SHOW - 1;         /* tricky, tricky */
	    arg = (char *)NULL;
	}
	else {
	    if (!opstr[0])
		return CRNEEDARG;

	    ind = lookup (opstr, setopttable);
/*dbgpr("ind=%d from lookup of (%s) in setopttable\n", ind, opstr);*/
	    if (ind == -1 || ind == -2) {
		mesg (ERRSTRT + 1, opstr);
		return ind;
	    }

	    arg = getword(&nxtop);
	}

	switch( setopttable[ind].val ) {

	    case SET_SHOW:
		{       char buf[term.tt_width-1];
#ifndef LMCVBELL
snprintf(buf, sizeof(buf), "+li %ld, -li %ld, +pg %ld, -pg %ld, wr %ld, wl %ld, wid %d, \
bell %s, hy %s, highlight: %s",
    defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
    linewidth, NoBell ? "off" : "on", fill_hyphenate ? "on" : "off", highlight_info_str);
#else /* LMCVBELL */
snprintf(buf, sizeof(buf), "+li %ld, -li %ld, +pg %ld, -pg %ld, wr %ld, wl %ld, wid %d, \
bell %s, vb %s, hy %s",
    defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
    linewidth, NoBell ? "off" : "on", VBell ? "on" : "off",
    fill_hyphenate ? "on" : "off" );
#endif
		    mesg (TELALL + 1, buf);
		}
		loopflags.hold = YES;
		retval = CROK;
		break;

	    case SET_PLLINE:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defplline = value;
		curwin->plline = defplline;
		curwin->winflgs |= PLINESET;
		retval = CROK;
		break;

	    case SET_MILINE:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defmiline = value;
		curwin->miline = defmiline;
		curwin->winflgs |= MLINESET;
		retval = CROK;
		break;

	    case SET_LINE:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defmiline = value;
		defplline = defmiline;
		curwin->plline = defmiline;
		curwin->miline = defmiline;
		curwin->winflgs |= PLINESET;
		curwin->winflgs |= MLINESET;
		retval = CROK;
		break;

	    case SET_MIPAGE:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defmipage = value;
		curwin->mipage = defmipage;
		retval = CROK;
		break;

	    case SET_PLPAGE:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defplpage = value;
		curwin->plpage = defplpage;
		retval = CROK;
		break;

	    case SET_PAGE:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defplpage = value;
		defmipage = defplpage;
		curwin->plpage = defplpage;
		curwin->mipage = defplpage;
		retval = CROK;
		break;

	    case SET_WINLEFT:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		deflwin = value;
		curwin->lwin = deflwin;
		retval = CROK;
		break;

	    case SET_WIN:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defrwin = value;
		deflwin = defrwin;
		curwin->rwin = defrwin;
		curwin->lwin = deflwin;
		retval = CROK;
		break;

	    case SET_WINRIGHT:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		defrwin = value;
		curwin->rwin = defrwin;
		retval = CROK;
		break;

	    case SET_WIDTH:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
#ifdef LMCAUTO
		if (value <= autolmarg)
		    mesg (ERRALL+1, "rmar must be greater than lmar.");
		else
#endif /* LMCAUTO */
/*trw, 8.28.2010*/
#ifdef LMCMARG
		setmarg ((Ncols *)&linewidth, value);
#endif
		retval = CROK;
		break;
#ifdef LMCAUTO
	    case SET_LMARG:
		if ((value = abs( atoi( arg ))) < 0)
		    goto BadVal;
		if (value >= linewidth)
		    mesg (ERRALL+1, "lmar must be less than rmar.");
		else
		    setmarg (&autolmarg, value);
		retval = CROK;
		break;
#endif /* LMCAUTO */
	    case SET_BELL:
		NoBell = NO;
#ifdef LMCVBELL
		VBell = NO;
#endif /* LMCVBELL */
		retval = CROK;
		break;

	    case SET_NOBELL:
		NoBell = YES;
#ifdef LMCVBELL
		VBell = NO;
#endif /* LMCVBELL */
		retval = CROK;
		break;

#ifdef LMCVBELL
	    case SET_VBELL:
		if (*term.tt_vbell == NULL)
			mesg (ERRALL+1, "No vis-bell on this terminal.");
		else {
			NoBell = NO;
			VBell = YES;
		}
		retval = CROK;
		break;
#endif /* LMCVBELL */

	    case SET_WORDDELIM:
		if (!*arg) {
		    cmdname = cmdopstr;
		    return CRNEEDARG;
		}
		retval = setwordmode (arg);
		break;

	    case SET_DEBUG:
		DebugVal =  atoi (arg);
		retval = CROK;
		break;

	    case SET_RMSTICK:
		optstick = YES;
		break;

	    case SET_RMNOSTICK:
		optstick = NO;
		break;

	    case SET_HY:
		fill_hyphenate = YES;
		break;

	    case SET_NOHY:
		fill_hyphenate = NO;
		break;

	    case SET_NOHIGHLIGHT:
		/*highlight_mode = NO;*/
		doSetHighlight("off");
		break;

	    case SET_HIGHLIGHT:
		retval = doSetHighlight(arg);
		break;

	    case SET_BRACEMATCH:
		retval = doSetBraceMode(arg);
		break;

	    case SET_FILLDOT:
		fill_troffmode = NO;
		break;

	    case SET_MOUSE:
		toggle_mouse(arg);
		break;

	    case SET_NOFILLDOT:
		fill_troffmode = YES;
		break;

#ifdef RECORDING
	    case SET_PLAY:
		if (*arg == '-' || !strncmp (arg, "no", 2))
		    play_silent++;
		else
		    play_silent = 0;
		break;
#endif
	    default:
BadVal:         retval = CRBADARG;
		break;
	}

	if (arg && *arg)
	    sfree(arg);

	return (retval);
}


int
doSetHighlight(char *arg)
{

	extern char *highlight_info_str;


	if (strncmp(arg, "on",2) == 0) {
	    highlight_mode = YES;
	    highlight_info_str = "on";
	/*  dbgpr("hilite mode on: %d\n", highlight_mode); */
	}
	else if (strncmp(arg, "off",3) == 0) {
	    highlight_mode = NO;
	    highlight_info_str = "off";
	/*  dbgpr("hilite mode off: %d\n", highlight_mode); */
	}
	else if (strncmp(arg, "bold",4) == 0) {
	    highlight_mode = YES;
	    hilite_str = bold_str;
	    highlight_info_str = "bold";
	}
	else if (strncmp(arg, "rev",3) == 0) {
	    highlight_mode = YES;
	    hilite_str = smso;
	    highlight_info_str = "rev";
	}
	else if (strncmp(arg, "color",5) == 0) {
	    if (n_colors == 0) {
		highlight_mode = NO;
		highlight_info_str = "bold";
		hilite_str = bold_str;
		loopflags.hold = YES;
		mesg (TELALL + 1, "Highlighting requires a terminal that supports colors");
		return (CROK);
	    }
	    highlight_mode = YES;
	    hilite_str = fgbg_pair;
	    highlight_info_str = "color";
	}
	else {
	    loopflags.hold = YES;
	    mesg (TELALL + 1, "Usage:  set highlight on|off|color|bold|rev");
	}

	return (CROK);
}


int
doSetBraceMode(char *arg)
{

/** /
dbgpr("doSetBraceMode: opstr=(%s) arg=(%s) cmdopstr=(%s)\n",
opstr, arg, cmdopstr);
/ **/
	static int mode = 0; /*0=off, 1=function(), 2=range limit, lines */
	char buf[128];

	if (strncmp(arg, "toggle", 6) == 0) { /* from {} button */
	    switch (mode) {
		case 0: /* mode is off */
		    bracematching = 1;
		    bracematchCoding = 1;
		    mode = 1;
		    snprintf(buf, sizeof(buf), " brace mode: function()");
		    mesg(TELALL + 1, buf);
		    loopflags.hold = YES;
		    break;

		case 1: /* mode is bracematchCoding mode */
		    bracematchCoding = 0;
		    bracematching = 1;
		    mode = 2;
		    char sbuf[64];
		    if (braceRange)
			snprintf(sbuf, sizeof(sbuf), "%d lines", braceRange);
		    else
			strcpy(sbuf, "EOF");

		    snprintf(buf, sizeof(buf), " brace mode: range +/- %s", sbuf);
		    mesg(TELALL + 1, buf);
		    loopflags.hold = YES;
		    break;

		case 2: /* mode is range lines */
		    bracematching = 0;
		    mode = 0;
		    break;
	    }
	}
	else if (arg == NULL || *arg == '\0') {
	    bracematching = bracematching == 1 ? 0 : 1;
	}
	else if (strncmp(arg, "on",2) == 0) {
	    bracematching = YES;
	}
	else if (strncmp(arg, "off",3) == 0) {
	    bracematching = 0;
	}
	else if (strncmp(arg, "coding",6) == 0) {
	    /* eg, arg=(bracematch) cmdopstr=(coding) */
	    char *s = cmdopstr + strlen(opstr) + strlen(arg) + 1;
	    if (strstr(s, " on")) {
		bracematchCoding = YES;
	    }
	    else if (strstr(s, " off")) {
		bracematchCoding = NO;
	    }
	    else {
		bracematchCoding = bracematchCoding ? NO : YES;
	    }
	    mode = bracematchCoding ? 1 : 0;
/** /
dbgpr("bracematchCoding=%d, arg=(%s) cmdopstr=(%s) s=(%s) mode=%d\n",
bracematchCoding, arg, cmdopstr, s, mode);
/ **/

	}
	else if (arg && *arg == '?') {
	    snprintf(buf, sizeof(buf), " {} search range is ");
	    if (braceRange == 0) {
		strcat(buf, "off, +/- EOF");
	    }
	    else {
		snprintf(buf + strlen(buf), sizeof(buf), " +/- %d lines.",
		    braceRange);
	    }

	    mesg(TELALL + 1, buf);
	    loopflags.hold = YES;
	    *cmdopstr = '\0';   /* clear for next time */
	    return (CROK);
	}
	else if (cmdopstr && strstr(cmdopstr, "range")) {
	    mode = 2;
	    /* allow:  off or 0 or N  */
	    if (strstr(cmdopstr, " off")) {
		braceRange = 0;
		info(7, 3, "   ");
		return CROK;
	    }

	    char *s = cmdopstr;

	    while (*s && !isdigit(*s)) {
		s++;
		if ((s - cmdopstr) > 30) {
		    mesg(ERRALL+1, "Can't locate value for brace range");
		    return CROK;
		}
	    }
	    braceRange = atoi (s);
	    if (braceRange < 0 || braceRange > 2000) {
		dbgpr("braceRange=%d negative or too large\n", braceRange);
	    }

	    /*dbgpr("braceRange=%d, s=(%s)\n", braceRange, s);*/
	    bracematching = 1;
	}
	else {
	    snprintf(buf, sizeof(buf), "Set brace option \"%s\" not recognized.", arg);
	    mesg(ERRALL+1, buf);
	    return CROK;
	}



/*#ifdef NOTYET*/
#if 1
	if (bracematching) {
	    snprintf(buf, sizeof(buf), "%c{}", mode==1 ? 'f' : ' ');
	}
	else {
	    strcpy(buf, "   ");
	}

	info(7, 3, buf);
#endif

#ifdef OUT
	char *msg = bracematching ? "{}" : "  ";
	info(7, 2, msg);
#endif

	return (CROK);
}

