#ifdef COMMENT
--------
file e.cm.c
    command dispatching routine and some actual command-executing routines.
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.e.h"
#include "e.m.h"
#include "e.ru.h"
#include "e.cm.h"
#include "e.wi.h"
#ifdef LMCHELP
#include "e.tt.h"
#endif LMCHELP

#include SIG_INCL

S_looktbl cmdtable[] = {
#ifdef CMDVERALLOC
    "#veralloc", CMDVERALLOC,
#endif
#ifdef LMCAUTO
    "-auto"   , CMD_AUTO    ,
#endif
    "-blot"   , CMD_BLOT    ,
    "-close"  , CMD_CLOSE   ,
    "-command", CMD_COMMAND ,
    "-diff"   , CMD_DIFF    ,
#ifdef LMCDWORD
    "-dword"  , CMD_DWORD   ,
#endif LMCDWORD
    "-erase"  , CMD_ERASE   ,
    "-join"   , CMDSPLIT    ,
    "-macro"  , CMD_MACRO   ,
    "-mark"   , CMD_MARK    ,
    "-pick"   , CMD_PICK    ,
    "-range"  , CMD_RANGE   ,
    "-re"     , CMD_PATTERN ,   /* Added Purdue CS 10/18/82 MAB */
    "-regexp" , CMD_PATTERN ,   /* Added Purdue CS 10/18/82 MAB */
    "-replace", CMD_REPLACE ,
    "-search" , CMD_SEARCH  ,
    "-split"  , CMDJOIN     ,
    "-tab"    , CMD_TAB     ,
    "-tabfile", CMD_TABFILE ,
    "-tabs"   , CMD_TABS    ,
    "-track"  , CMD_TRACK   ,
    "-update" , CMD_UPDATE  ,
    "-window" , CMD_WINDOW  ,
    "?range"  , CMDQRANGE   ,
    "?set"    , CMDQSET     ,
#ifdef LMCAUTO
    "auto"    , CMDAUTO     ,
#endif LMCAUTO
    "b"       , CMDEXIT     ,
    "blot"    , CMDBLOT     ,
    "box"     , CMDBOX      ,
    "bye"     , CMDEXIT     ,
    "call"    , CMDCALL     ,
#ifdef LMCCASE
    "caps"    , CMDCAPS     ,
    "ccase"   , CMDCCASE    ,
#endif LMCCASE
    "center"  , CMDCENTER   ,
    "clear"   , CMDCLEAR    ,
    "close"   , CMDCLOSE    ,
    "command" , CMDCOMMAND  ,
    "cover"   , CMDCOVER    ,
    "delete"  , CMDDELETE   ,
    "delete"  , CMDDELETE   ,
    "diff"    , CMDDIFF     ,
    "dword"   , CMDDWORD    ,
    "e"       , CMDEDIT     ,
    "edit"    , CMDEDIT     ,
    "endm"    , CMDENDM     ,
    "erase"   , CMDERASE    ,
    "exit"    , CMDEXIT     ,
    "feed"    , CMDFEED     ,
    "fill"    , CMDFILL     ,
    "goto"    , CMDGOTO     ,
    "help"    , CMDHELP     ,
    "help"    , CMDHELP     ,
    "insert"  , CMDINSERT   ,
    "join"    , CMDJOIN     ,
    "justify" , CMDJUST     ,
    "logoff"  , CMDLOGOUT   ,
    "logout"  , CMDLOGOUT   ,
    "macro"   , CMDMACRO    ,
    "mark"    , CMDMARK     ,
    "name"    , CMDNAME     ,
    "open"    , CMDOPEN     ,
    "overlay" , CMDOVERLAY  ,
    "pick"    , CMDPICK     ,
    "print"   , CMDPRINT    ,
    "range"   , CMDRANGE    ,
    "re"      , CMDPATTERN  ,
    "redraw"  , CMDREDRAW   ,
    "regexp"  , CMDPATTERN  ,
    "replace" , CMDREPLACE  ,
    "run"     , CMDRUN      ,
    "save"    , CMDSAVE     ,
    "search"  , CMDSEARCH   ,
    "set"     , CMDSET      ,
    "shell"   , CMDSHELL    ,
    "split"   , CMDSPLIT    ,
/*      DEBUG
    "stats"   , CMDSTATS    ,
 */
#ifdef SIGTSTP  /* 4bsd vmunix */
    "stop"    , CMDSTOP     ,
#endif
    "tab"     , CMDTAB      ,
    "tabfile" , CMDTABFILE  ,
    "tabs"    , CMDTABS     ,
    "track"   , CMDTRACK    ,
    "underlay", CMDUNDERLAY ,
    "undo"    , CMDUNDO     ,
    "update"  , CMDUPDATE   ,
    "window"  , CMDWINDOW   ,
    0, 0

};

extern void dostop ();

#ifdef LMCCMDS
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
    char helparg[30];
#endif LMCHELP

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
	cmdtblind = lookup (cmdstr, cmdtable);
	if (cmdtblind == -1) {
#ifdef LMCGO
	    if (cmdstr[0] >= '1' && cmdstr[0] <= '9') {
		nxtop = paramv;
		cmdtblind = lookup ("g", cmdtable);
	    } else {
#endif
		mesg (ERRALL + 3, "Command \"", cmdstr, "\" not recognized");
		sfree (cmdstr);
		return CROK;
#ifdef LMCGO
	    }
#endif
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
    switch (cmdval) {
#else  LMCCMDS
#ifdef COMMENT
Cmdret
command ()
.
    Parses a command line and dispatches on the command.
    If a routine that was called to execute a command returns a negative
    value, then the error message is printed out here, and returns CROK.
    Else returns the completion status of the command.
#endif
Cmdret
command ()
{
    Short cmdtblind;
    Cmdret retval;
    Short cmdval;
    char *cmdstr;
    int clrcnt;                 /* Added 10/18/82 MAB */
#ifdef LMCHELP
    char helparg[30];
#endif LMCHELP

    nxtop = paramv;
    cmdstr = getword (&nxtop);
    if (cmdstr[0] == 0)
	return CROK;
    cmdtblind = lookup (cmdstr, cmdtable);
    if (cmdtblind == -1) {
	mesg (ERRALL + 3, "Command \"", cmdstr, "\" not recognized");
	sfree (cmdstr);
	return CROK;
    }
    else if (cmdtblind == -2) {
	mesg (ERRALL + 3, "Command \"", cmdstr, "\" ambiguous");
	sfree (cmdstr);
	return CROK;
    }
    sfree (cmdstr);

    cmdopstr = nxtop;
    opstr = getword (&nxtop);

    cmdname = cmdtable[cmdtblind].str;
    switch (cmdval = cmdtable[cmdtblind].val) {
#endif LMCCMDS

#ifdef CMDVERALLOC
    case CMDVERALLOC:
	veralloc ();
	retval = CROK;
#endif
	break;

    case CMDRANGE:
    case CMD_RANGE:
    case CMDQRANGE:
	retval = rangecmd (cmdval);
	break;

    case CMDTRACK:
#ifdef LMCTRAK
	if ((curwin->winflgs & TRACKSET) == 0)
	    curwin->winflgs |= TRACKSET;
	else
	    curwin->winflgs &= ~TRACKSET;
	infotrack (TRACKSET & curwin->winflgs);
#else  LMCTRAK
	curwin->winflgs |= TRACKSET;
	infotrack (YES);
#endif LMCTRAK
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
	retval = doupdate (cmdval == CMDUPDATE);
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

    case CMDDELETE:
	retval = delete ();
	break;

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
#endif LMCCASE
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

    case CMDCALL:
	retval = call ();
	/* if the syntax of the shell command was correct and all of the
	/* saves went OK, forkshell will never return. */
	break;

    case CMDSHELL:
	retval = shell ();
	/* if the syntax of the shell command was correct and all of the
	/* saves went OK, forkshell will never return. */
	break;

    case CMDLOGOUT:
	if (!loginflg) {
	    mesg (ERRALL + 1, "This is not your login program.  Use \"exit\".");
	}
    case CMDEXIT:
	retval = eexit ();
	/* if the syntax of the exit command was correct and all of the
	/* saves went OK, eexit will never return. */
	break;

#ifdef LMCHELP
    case CMDHELP:
	mesg (ERRALL + 1, "Calling help....");
	if (*term.tt_help == NULL) {
	    mesg (ERRALL + 1, "No help routine available for this crt.");
	    return CROK;
	} else {
	    strcpy (helparg, "/etc/e/kr.");
	    if (opstr [0] == 'l')
		helparg [8] = 'l';
	    strcat (helparg, kname);
	    retval = (*term.tt_help)(helparg);
	    if (retval != CROK)
		break;
	}
		/* intentional fallthrough !! */
#endif LMCHELP

    case CMDREDRAW:
	fresh ();
	retval = CROK;
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
	retval = run (cmdopstr, cmdval);
	break;

    case CMDFILL:
	retval = filter (FILLNAMEINDEX, YES);
	break;

    case CMDJUST:
	retval = filter (JUSTNAMEINDEX, YES);
	break;

    case CMDCENTER:
	retval = filter (CENTERNAMEINDEX, YES);
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
#endif LMCAUTO

#ifdef LMCDWORD
    case CMDDWORD:
	retval = dodword (YES);
	break;

    case CMD_DWORD:
	retval = dodword (NO);
	break;
#endif LMCDWORD

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

    case CMD_DIFF:
	retval = diff (-1);
	break;

    case CMDDIFF:
	retval = diff (1);
	break;

    default:
	mesg (ERRALL + 3, "Command \"", cmdtable[cmdtblind].str,
		"\" not implemented yet");
	retval = CROK;
	break;
    }
    if (opstr[0] != '\0')
	sfree (opstr);

    if (retval >= CROK)
	return retval;
    switch (retval) {
    case CRUNRECARG:
	mesg (1, " unrecognized argument to ");
	break;

    case CRAMBIGARG:
	mesg (1, " ambiguous argument to ");
	break;

    case CRTOOMANYARGS:
	mesg (ERRSTRT + 1, "Too many of arguments to ");
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
		return CROK;
	    }
	}
	else if (tmp == 2)
	    return CRBADARG;
    }

    Block {
	static S_looktbl gttbl[] = {
	    "b",            0,  /* guranteed abbreviation */
	    "beginning",    0,
	    "e",            1,  /* guranteed abbreviation */
	    "end",          1,
	    "rb",           2,  /* guranteed abbreviation */
	    "rbeginning",   2,
	    "re",           3,  /* guranteed abbreviation */
	    "rend",         3,
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
doupdate (on)
    Flag on;
.
    Do the "update" command.
    The 'on' argument is non-0 for "update" and 0 for "-update"
#endif
Cmdret
doupdate (on)
Flag on;
{
    Small ind;
    static S_looktbl updatetable[] = {
	"-inplace", 0,
	"inplace",  0,
	0        ,  0
    };

    if (*nxtop || !on && opstr[0] != '\0')
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
	/* at this point, ind can be 0 = -inplace or 1 = inplace */
	if (ind) { /* inplace */
	    if (fileflags[curfile] & NEW) {
		mesg (ERRALL + 1, "\"inplace\" is useless;  file is new");
		return CROK;
	    }
	    if (!(fileflags[curfile] & FWRITEABLE)) {
		mesg (ERRALL + 1, "Don't have file write permission");
		return CROK;
	    }
	    fileflags[curfile] |= INPLACE | CANMODIFY;
	}
	else /* -inplace */
	    fileflags[curfile] &= ~INPLACE;
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
#endif LMCAUTO
#ifdef LMCVBELL
#define SET_VBELL       17
#endif LMCVBELL



#ifdef COMMENT
Cmdret
setoption( showflag )
.
	Set/show various options
#endif COMMENT

Cmdret
setoption( showflag )
  int showflag;
{
	Reg1 char *arg;
	Reg2 Small ind;
	Reg3 Small value;
	Cmdret retval;
	static S_looktbl setopttable[] = {
	    "+line",        SET_PLLINE,  /* defplline */
	    "+page",        SET_PLPAGE,  /* defplpage */
	    "-line",        SET_MILINE,  /* defmiline */
	    "-page",        SET_MIPAGE,  /* defmipage */
	    "?",            SET_SHOW,    /* show options */
	    "bell",         SET_BELL,    /* echo \07 */
	    "debug",        SET_DEBUG,
	    "left",         SET_WINLEFT, /* deflwin */
	    "line",         SET_LINE,    /* defplline and defmiline */
#ifdef LMCAUTO
	    "lmargin",      SET_LMARG,   /* left margin */
#endif LMCAUTO
	    "nobell",       SET_NOBELL,  /* do not echo \07 */
	    "page",         SET_PAGE,    /* defmipage and defplpage */
	    "right",        SET_WINRIGHT,/* defrwin */
#ifdef LMCAUTO
	    "rmargin",      SET_WIDTH,   /* same as width */
#endif LMCAUTO
#ifdef LMCVBELL
	    "vbell",        SET_VBELL,   /* visible bell */
#endif LMCVBELL
	    "width",        SET_WIDTH,   /* linewidth */
	    "window",       SET_WIN,     /* deflwin and defrwin */
	    "worddelimiter",SET_WORDDELIM,/* set word delimiter */
	    0        ,  0
	};

	if (showflag) {
	    ind = SET_SHOW - 1;         /* tricky, tricky */
	    arg = (char *)NULL;
	}
	else {
	    if (!opstr[0])
		return CRNEEDARG;

	    ind = lookup (opstr, setopttable);
	    if (ind == -1 || ind == -2) {
		mesg (ERRSTRT + 1, opstr);
		return ind;
	    }

	    arg = getword(&nxtop);
	    if (arg == NULL) {
		cmdname = cmdopstr;
		return CRNEEDARG;
	    }
	}

	switch( setopttable[ind].val ) {

	    case SET_SHOW:
		{       char buf[80];
#ifndef LMCVBELL
sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, bell %s",
    defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
    linewidth, NoBell ? "off" : "on" );
#else LMCVBELL
sprintf(buf, "+li %d, -li %d, +pg %d, -pg %d, wr %d, wl %d, wid %d, bell %s, vb %s",
    defplline, defmiline, defplpage, defmipage, defrwin, deflwin,
    linewidth, NoBell ? "off" : "on", VBell ? "on" : "off" );
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
		linewidth = value;
		retval = CROK;
		break;
#ifdef LMCAUTO
	    case SET_LMARG:
		if ((value = abs( atoi( arg ))) == 0)
		    goto BadVal;
		autolmarg = value;
		retval = CROK;
		break;
#endif LMCAUTO
	    case SET_BELL:
		NoBell = NO;
#ifdef LMCVBELL
		VBell = NO;
#endif LMCVBELL
		retval = CROK;
		break;

	    case SET_NOBELL:
		NoBell = YES;
#ifdef LMCVBELL
		VBell = NO;
#endif LMCVBELL
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
#endif LMCVBELL

	    case SET_WORDDELIM:
		retval = setwordmode (arg);
		break;

	    case SET_DEBUG:
		DebugVal =  atoi (arg);
		retval = CROK;
		break;

	    default:
BadVal:         retval = CRBADARG;
		break;
	}

	if (arg)
	    sfree(arg);

	return (retval);
}
