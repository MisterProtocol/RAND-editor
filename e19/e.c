#ifdef COMMENT
--------
file e.c
    main program and startup code.
    All code in this file is used only prior to calling mainloop.
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The RAND Corporation
#endif

#include "e.h"
#include "e.e.h"
#ifdef  KBFILE
#include "e.it.h"
#endif /* KBFILE */
#include "e.tt.h"
#include "e.wi.h"
#include "e.fn.h"
#include "e.sg.h"
#include "e.inf.h"
#include <sys/stat.h>
#include <stdarg.h>
#if SYSIII || SOLARIS
#include <fcntl.h>
#endif

#ifdef SYSIII
#include <termio.h>
#endif

#ifdef FILELOCKING
#ifdef BSD
#include <sys/dir.h>
#endif /* BSD */
#include <sys/file.h>
#if SYSV || SOLARIS
#include <unistd.h>
#include <dirent.h>
#endif
#endif /* FILELOCKING */

#ifdef ENVIRON
extern char *getenv ();
#endif

#include SIG_INCL
#ifdef TTYNAME
extern char *ttyname ();
#endif

/*  After the two-byte revision number in the keystroke file
 *  comes a character telling what to use from the state file.
 *  As of this writing, only ALL_WINDOWS and NO_WINDOWS are usable.
 **/
#define NO_WINDOWS  ' ' /* don't use any files from state file */
#define ONE_FILE    '-' /* use only one file from current window */
#define ONE_WINDOW  '+' /* use current and alt file from current window */
#define ALL_WINDOWS '!' /* use all windows and files */

#define OPTREPLAY       0
#define OPTRECOVER      1
#define OPTDEBUG        2
#define OPTSILENT       3
#define OPTHELP         4
#define OPTNOTRACKS     5
#define OPTINPLACE      6
#define OPTNORECOVER    7
#define OPTTERMINAL     8
#define OPTKEYBOARD     9
#ifdef  KBFILE
#define OPTKBFILE      10
#endif /* KBFILE */
#define OPTBULLETS     11
#define OPTNOBULLETS   12
#ifdef TERMCAP
#define OPTDTERMCAP    13
#endif /* TERMCAP */
#define OPTPATTERN     15
#define OPTSTATE       16
#define OPTSTDKBD      17
#define OPTSTICK       18
#define OPTNOSTICK     19   /* no pun intended */
#ifdef NOCMDCMD
#define OPTNOCMDCMD    20
#endif /* NOCMDCMD */
#define OPTREADONLY    21   /* edit file in 'readonly' mode */
#define OPTNOREADONLY  22   /* disable 'readonly' mode for mode "444" files*/
#ifdef  STARTUPFILE
#define OPTNOPROFILE   23   /* don't execute the .e_profile */
#define OPTPROFILE     24   /* specify the .e_profile */
#endif /* STARTUPFILE */
#define OPTCREATE      25   /* don't ask when creating new files */
#ifdef  FILELOCKING
#define OPTNOLOCKS     26   /* allow changes, even if file is locked */
#endif                      /* (this option doesn't really make sense */
			    /* since one could copy a locked file, change */
			    /* it, and copy it back in an emergency. */
#define OPTSEARCH      27
#define OPTLITERAL     28
#define OPTBLANKS      29
#define OPTNOSTRIP     30
#define OPTTABS        31
#ifdef RECORDING
#define OPTNOMACROS    32
#define OPTMACROFILE   33
#endif

#define OPTNEWFILEMODE 34

#ifdef NCURSES_MOUSE
#define OPTSKIPMOUSE   35   /* don't enable mouse support */
#endif

#ifdef NCURSES
#define OPTBGRGB       36   /* highlight background rgb values, -bg=r,g,b */
#define OPTFGRGB       37   /* highlight foreground rgb values, -fg=r,g,b */
#define OPTSETAB       38   /* alt method to set bg color,      -setab=N */
#define OPTSETAF       39   /* alt method to set fg color,      -setaf=N */
#endif



/* Entries must be alphabetized. */
/* Entries of which there are two in this table must be spelled out. */
S_looktbl opttable[] = {
   {"bg"       , OPTBGRGB    },  /* -bg=r,g,b */
   {"bgcolor"  , OPTBGRGB    },  /* -bg=r,g,b */
   {"blanks"   , OPTBLANKS   },  /* expand tabs to blanks */
   {"bullets"  , OPTBULLETS  },
   {"create"   , OPTCREATE   },
   {"debug"    , OPTDEBUG    },
   {"dkeyboard", OPTSTDKBD   },
#ifdef TERMCAP
   {"dtermcap" , OPTDTERMCAP },
#endif /* TERMCAP */
   {"fg"       , OPTFGRGB    },  /* -fg=r,g,b */
   {"fgcolor"  , OPTFGRGB    },  /* -fg=r,g,b */
   {"help"     , OPTHELP     },
   {"inplace"  , OPTINPLACE  },
#ifdef  KBFILE
   {"kbfile"   , OPTKBFILE   },
#endif /* KBFILE */
   {"keyboard" , OPTKEYBOARD },
   {"literal"  , OPTLITERAL  },
#ifdef  RECORDING
   {"macrofile", OPTMACROFILE},  /* which .e_macro to read */
#endif /* RECORDING */
   {"newfilemode", OPTNEWFILEMODE},  /* eg, 0644 */
   {"nobullets", OPTNOBULLETS},
#ifdef NOCMDCMD
   {"nocmdcmd" , OPTNOCMDCMD },
#endif /* NOCMDCMD */
#ifdef FILELOCKING
   {"nolocks"  , OPTNOLOCKS  },
#endif /* FILELOCKING */
#ifdef  RECORDING
   {"nomacros",  OPTNOMACROS },  /* don't read the .e_macro */
#endif /* RECORDING */
#ifdef  STARTUPFILE
   {"noprofile", OPTNOPROFILE},  /* don't read the .e_profile */
#endif /* STARTUPFILE */
   {"noreadonly",OPTNOREADONLY}, /* no auto 'readonly' mode for '444' files*/
   {"norecover", OPTNORECOVER},
   {"nostick"  , OPTNOSTICK  },
   {"nostrip"  , OPTNOSTRIP  },  /* don't strip blanks at end of line */
   {"notracks" , OPTNOTRACKS },
#ifdef  STARTUPFILE
   {"profile"  , OPTPROFILE  },  /* specify the .e_profile */
#endif /* STARTUPFILE */
   {"readonly" , OPTREADONLY },  /* edit file in 'readonly' mode */
   {"regexp"   , OPTPATTERN  },  /* Added Purdue CS 2/8/83 */
   {"replay"   , OPTREPLAY   },
   {"search"   , OPTSEARCH   },
   {"setab"    , OPTSETAB    },
   {"setaf"    , OPTSETAF    },
   {"silent"   , OPTSILENT   },
#ifdef NCURSES_MOUSE
   {"skipmouse", OPTSKIPMOUSE},  /* 3/19/21: added to skip ncurses mouse initialization */
#endif
   {"state"    , OPTSTATE    },
   {"stick"    , OPTSTICK    },
   {"tabs"     , OPTTABS     },  /* don't convert tabs to spaces */
   {"terminal" , OPTTERMINAL },
   {0          , 0}
};

#ifdef NCURSES
Flag optskipmouse = NO;  /* YES 3/19/21: added to skip ncurses mouse initialization */
extern void initCurses();
/* variables for user to set custom rgb values for highlighting */
extern int opt_bg_r;
extern int opt_bg_g;
extern int opt_bg_b;
extern Flag bg_rgb_options;
extern int opt_fg_r;
extern int opt_fg_g;
extern int opt_fg_b;
extern Flag fg_rgb_options;
/* alternate method for setting fg/bg color, using termio capabilities */
extern int opt_setaf;
extern int opt_setab;
/* testing */
void resize_handler (int sig);
#endif

#ifdef  NOCMDCMD
Flag optnocmd;          /* YES = -nocmdcmd; <cmd><cmd> functions disabled */
#endif /* NOCMDCMD */
#ifdef BSD
/*extern char _sobuf[];*/
extern unsigned char _sobuf[];
#endif /* BSD */
Small numargs;
Small curarg;
char *curargchar;
char **argarray;
char *dbgname;      /* argument to -debug=xxx option */
char *replfname;    /* argument to -replay=xxx option */
Flag state=NO;
char *statefile;    /* argument to -state=xxx option */
char stdk[] = "standard";
Flag helpflg,
     dbgflg;
Flag crashed;       /* prev session crashed */
Flag chosereplay;   /* user chose replay or recovery */
char *opttname;
char *optkname;     /* Keyboard name */
#ifdef  KBFILE
char *kbfile;           /* name of input (keyboard) table file */
char *optkbfile;        /* Keyboard translation file name */
#endif /* KBFILE */
Flag optbullets = -1;   /* YES = -bullets, NO = -nobullets */
#ifdef TERMCAP
Flag optdtermcap;       /* YES = force use of termcap */
#endif /* TERMCAP */
Flag    readonly;       /* YES = -readonly; edit file in 'readonly' mode */
Flag    noreadonly;     /* YES = no auto 'readonly' mode for '444' files */
#ifdef  STARTUPFILE
Flag    noprofile;      /* YES = don't execute the .e_profile */
char    *optprofile;    /* -profile=filename option specified */
#endif /* STARTUPFILE */
Flag    autocreate;     /* YES = don't ask when creating new files */

#ifdef  FILELOCKING
Flag    nolocks;        /* allow file changes, even if file is locked */
#endif /* FILELOCKING */

#ifdef SAVETMPFILES
extern Flag HaveBkeytmp;
#endif

#ifdef  RECORDING
Flag    optnomacros;    /* YES = don't read .e_macros */
char    *optmacrofile;  /* -macrofile=filename option specified */
void    ReadMacroFile (void);
#endif /* RECORDING */

extern void main1 ();
extern void my_getprogname ();
extern void checkargs ();
extern void startup ();
extern void showhelp ();
extern void dorecov ();
extern void gettermtype ();
extern void setitty ();
extern void setotty ();
extern void makescrfile ();
extern void getstate ();
extern void getskip ();
extern void makestate ();
extern void infoinit ();
extern Small getcap ();
#ifdef LMCAUTO
extern void infoint0 ();
#endif /* LMCAUTO */
#ifdef  KBFILE
extern void getkbfile ();
#endif /* KBFILE */
#ifdef  STARTUPFILE
extern void chk_profile();
#endif /* STARTUPFILE */
extern void initwindows();
_Noreturn void getout (Flag, char *, ...);
extern int fileno();
off_t replay_filesize;
int replay_stopcount = 0; /* set by user when picking replay option */

#ifdef COMMENT
void
main1 (argc, argv)
    int     argc;
    char   *argv[];
.
    All of the code prior to entering the main loop of the editor is
    either in or called by main1.
#endif
void
main1 (argc, argv)
Reg3 int argc;
Reg2 char *argv[];
{
    char    ichar;      /* must be a char and can't be a register */
    char    *cp;

/* works! here */
#ifdef xNCURSES
    dbgpr("e.c, near the start of main1(), calling initCurses()\n");
    initCurses();
#endif

#ifdef BSD
#ifndef VBSTDIO
    setbuf (stdout, (char *)_sobuf);
#endif
#endif
    fclose (stderr);

#ifdef  UID_IS_CHAR
    userid  = getuid () & 0377;
    groupid = getgid () & 0377;
#else
    userid = getuid ();
    groupid = getgid ();
#endif

    my_getprogname (argv[0]);

#ifdef  STARTUPFILE
    chk_profile(NULL);      /* look at .e_profile info */
#endif /* STARTUPFILE */

    numargs = argc;
    argarray = argv;

    /*
     *  Tailor location of Es etc dir, so people can use E
     *  at sites that don't have a full installation.
     */
    if ((cp = getenv ("E_ETCDIR"))) {
	etcdir = salloc (strlen(cp)+1, YES);
	strcpy(etcdir, cp);
    }
    else
	etcdir = ETC;

    checkargs ();

#ifdef  STARTUPFILE
    if( noprofile && dot_profile ) {
	fclose( fp_profile );
	dot_profile = NO;
	if( silent && !replaying && !recovering )
	    silent = NO;
    }
    if( optprofile )
	chk_profile( optprofile );
#endif /* STARTUPFILE */

#ifdef  RECORDING
    if( !optnomacros )
	ReadMacroFile();        /* TODO: add option to skip this */
#endif

    if (helpflg) {
	showhelp ();
	getout (NO, "");
    }

    if (dbgflg && (dbgfile = fopen (dbgname, "a")) == NULL) {   /* 3/21: was "w" */
	/*getout (NO, "Can't open debug file: %s", dbgname);*/
	fprintf(stdout, "Can't open debug file: %s\n", dbgname);
	fflush(stdout);
	exit(-1);
    }


#ifdef NOPE
/* #ifdef TERMCAP */
    /* 2/12/2020:  Give an early err msg if "termcap" is not found */
    struct stat xx_statbuf;
    char *xx_cp;

    if ((xx_cp = getenv ("TERMCAP"))) {
	if(stat(xx_cp, &xx_statbuf) == -1) {
	    /*getout (NO, "Can't find the file in the environment variable TERMCAP: %s", xx_buf);*/
	    fprintf(stdout, "Can't find the file \"%s\" specified in the TERMCAP environment variable.\n", xx_cp);
	    fflush(stdout);
	    exit(-1);
	}
    }
    else if (stat(TERMCAPFILE, &xx_statbuf) == -1) {
	fprintf(stdout, "Can't find the default file \"%s\" TERMCAPFILE variable, (localenv.h)\n", TERMCAPFILE);
	getout (NO, "Both the environment variable TERMCAP and TERMCAPFILE are not found", "");
    }
#endif

#ifdef LMCAUTO
    infoint0 ();
#endif /* LMCAUTO */

    startup ();

    if (replaying) Block {

	/* any key can interrupt a replay */
	/* intok = NO; hmm... */

#ifdef OUT
	if ((inputfile = open (inpfname, 0)) < 0)
	    getout (YES, "Can't open replay file %s.", inpfname);
#endif

	inputfile = STDIN;  /* fd 0 */
	/*
	 *   Reading replay file with stream routines makes e.t.c much cleaner
	 */
/*dbgpr("inpfname=(%s)\n", inpfname);*/
	if ((replay_fp = fopen (inpfname, "r")) == NULL) {
	    getout (YES, "Can't open replay file: %s", inpfname);
	    fflush(stdout);
	    exit(-1);
	}
	Block {
	    struct stat rbuf;
	    if (fstat(fileno(replay_fp), &rbuf) == -1) {
		dbgpr("stat failed on replay_fp\n");
	    }
	    replay_filesize = rbuf.st_size;
	    /*dbgpr("replay_filesize=(%d)\n", replay_filesize);*/
	}

	Block {
	    char buf[256];
	    short int a_ver;
	    int a_h, a_w;
	    char a_ichar;
	    char a_term[50];

	    fgets(buf, sizeof(buf), replay_fp);
	    sscanf (buf, "version=%hd ichar=%c term=%s h=%d w=%d",
		&a_ver, &a_ichar, a_term, &a_h, &a_w);
/****/
fprintf(stdout, "replayinfo: ver=%d ichar=%o term=%s h=%d w=%d\n",
    a_ver, a_ichar, a_term, a_h, a_w);
fflush(stdout);
/*****/
	    if (-a_ver != revision)
		getout (YES, "Replay file \"%s\" was made by revision %d of %s.",
		     inpfname, a_ver, progname);
	    if (strcmp(tname, a_term))
	    getout (YES, "\
Replay file \"%s\" was made by a different type of terminal '%s'.\n \
It needs to match '%s'.\n", inpfname, a_term, tname);

	    if (term.tt_width != a_w || term.tt_height != a_h ) {
		if (a_w > term.tt_width || a_h > term.tt_height)
		    getout (YES, "\
To replay file %s, you need a screensize at least %d w X %d h.\n\
Your current screensize is %d w X %d h.\n",
		    inpfname, a_w, a_h, term.tt_width, term.tt_height );
	    }
	    ichar = a_ichar;
	    term.tt_width = a_w;
	    term.tt_height = a_h;
	    initwindows (YES);
	}
    }
    else if (curarg < argc)
	ichar = NO_WINDOWS;         /* file args follow */
    else
	ichar = ALL_WINDOWS;        /* put up all old windows and files */


    if (recovering)
	printf ("\r\n\r\rRecovering from crash...");
    fflush (stdout);

    makescrfile (); /* must come before any calls to editfile () */

    if (!silent) {
	(*term.tt_home) ();
	(*term.tt_clear) ();
    }


#ifdef ENVIRON
#ifdef LMCVBELL
    if (getenv ("VBELL"))
	if (*term.tt_vbell != NULL) {
	    NoBell = NO;
	    VBell = YES;
	}
#endif /* LMCVBELL */
#endif /* ENVIRON */
    getstate (ichar);
#ifdef ASCIIKEYINFO
    fprintf(keyfile, "version=%d ichar=%c term=%s h=%d w=%d\n",
	-revision, ichar, tname, term.tt_height, term.tt_width);
#else
    putshort (revision, keyfile);
    putc (ichar, keyfile);          /* for next time */
    putshort ((short) termtype, keyfile);
/*-------------------------------------------------------*/
/*TESTING: place term height/width info in keystroke file*/
/*-------------------------------------------------------*/
    putshort ((short) term.tt_height, keyfile);
    putshort ((short) term.tt_width, keyfile);
#endif /* ASCIIKEYINFO */

 /* mesg (TELALL);  */

    infoinit ();

/*
dbgpr("main1, after infoinit, replaying=%d, recovering=%d\n",
    replaying, recovering);
*/

    if (!replaying && curarg < argc && *argv[curarg] != '\0') Block {
    /*  extern void keyedit ();  */
    /*  static void keyedit ();  */
	void keyedit ();
	Block {
	    Reg1 char *cp;
	    extern char *getenv ();
	    /*
	     *  Changed so a command line 'altfile' is used before any
	     *  environment "editalt" file.
	     */
	    if ((curarg + 1 < argc && *(cp = argv[curarg + 1]) != '\0')
		    || (cp = getenv ("editalt"))) {
		editfile (cp, (Ncols) 0, 0, 0, NO);
		keyedit (cp);
		/* do this so that editfile won't be cute about suppressing
		 * a putup on a file that is the same as the altfile
		 **/
		curfile = NULLFILE;
	    }
	}
	if (editfile (argv[curarg], (Ncols) 0, 0, 1, YES) <= 0)
	    eddeffile (YES);
	keyedit (argv[curarg]);
    }
    else if (!replaying || ichar != NO_WINDOWS) {
	putupwin ();
    }

#ifdef NCURSES
/*  dbgpr("e.c, end of main1(), calling initCurses()\n"); */
    initCurses();
#endif

    return;
}

#ifdef COMMENT
/*static void*/
void
keyedit (file)
    char   *file;
.
    Write out an edit command to the keys file.
#endif
/*static void*/
void
keyedit (file)
char *file;
{
    Reg1 char *cp;

    cp = append ("edit ", file);
    writekeys (CCCMD, cp, CCRETURN);
    sfree (cp);
}

#ifdef COMMENT
void
my_getprogname (arg0)
    char arg0[];
.
    Set the global character pointer 'progname' to point to the actual name
    that the editor was invoked with.
#endif
void
my_getprogname (arg0)
char arg0[];
{
    Reg1 char *cp;
    Reg2 Char lastc;

    lastc = Z;
    progname = cp = arg0;
    for (; *cp; cp++) {
	if (lastc == '/')
	    progname = cp;
	lastc = *cp;
    }
    cp = progname;

    if ((loginflg = (*progname == '-')))
	progname = LOGINNAME;
    return;
}

#ifdef COMMENT
void
checkargs ()
.
    Scan the arguments to the editor.  Set flags and options and check for
    errors in the option arguments.
#endif
void
checkargs ()
{
    dbgflg = NO;
    for (curarg = 1; curarg < numargs; curarg++) Block {
	Reg1 char *cp;
	Reg2 Flag opteqflg;
	Reg3 Short tmp;
	Reg4 char *opterr;
	curargchar = argarray[curarg];
	if (*curargchar != '-')
	    break;
	curargchar++;
	opteqflg = NO;
	for (cp = curargchar; *cp; cp++)
	    if (*cp == '=') {
		opteqflg = YES;
		*cp = 0;
		break;
	    }
	if (cp == curargchar)
	    goto unrecog;
	tmp = lookup (curargchar, opttable);
	if (opteqflg)
	    *cp++ = '=';
	if (tmp == -1)
	    goto unrecog;
	if (tmp == -2) {
	    opterr = "ambiguous";
	    goto error;
	}
	switch (opttable[tmp].val) {
	case OPTBULLETS:
	    optbullets = YES;
	    break;

	case OPTNOBULLETS:
	    optbullets = NO;
	    break;

	case OPTHELP:
	    helpflg = YES;
	    break;

	case OPTSTICK:
	    optstick = YES;
	    break;

	case OPTNOSTICK:
	    optstick = NO;
	    break;

#ifdef TERMCAP
	case OPTDTERMCAP:
	    optdtermcap = YES;
	    break;
#endif /* TERMCAP */

	case OPTSTATE:
	    if (opteqflg && *cp) {
		statefile = cp;
		state = YES;
	    }
	    break;

	case OPTDEBUG:
	    /* file for debug info */
	    if (!opteqflg || *cp == 0)
		getout (YES, "Must give debug file name");
	    if (dbgflg) {
		opterr = "repeated";
		goto error;
	    }
	    dbgflg = YES;
	    dbgname = cp;
	    break;

	case OPTINPLACE:
	    inplace = YES;
	    break;

	case OPTSILENT:
	    silent = YES;
	    break;

	case OPTNORECOVER:
	    norecover = YES;
	    break;

	case OPTNOTRACKS:
	    notracks = YES;
	    break;

	case OPTREPLAY:
	    replaying = YES;
	    if (opteqflg && *cp)
		replfname = cp;
	    break;

	case OPTKEYBOARD:
	    if (opteqflg && *cp)
		optkname = cp;
	    break;

#ifdef  KBFILE
	case OPTKBFILE:
	    if (opteqflg && *cp)
		optkbfile = cp;
	    break;
#endif /* KBFILE */

	case OPTTERMINAL:
	    if (opteqflg && *cp)
		opttname = cp;
	    break;

	case OPTPATTERN:
	    patmode = YES;
	    break;

	case OPTSTDKBD:
	    optkname = stdk;
	    break;

#ifdef  NOCMDCMD
	case OPTNOCMDCMD:
	    optnocmd = YES;
	    break;
#endif /* NOCMDCMD */

	case OPTREADONLY:               /* edit file in 'readonly' mode */
	    readonly = YES;
	    break;

	case OPTNOREADONLY:   /* no auto 'readonly' mode for '444' files */
	    noreadonly = YES;
	    break;

#ifdef  STARTUPFILE
	case OPTPROFILE:
	    if (opteqflg && *cp)
		optprofile = cp;
	    break;

	case OPTNOPROFILE:
	    noprofile = YES;
	    break;
#endif /* STARTUPFILE */

#ifdef RECORDING
	case OPTMACROFILE:
	    if (opteqflg && *cp)
		optmacrofile = cp;
	    break;

	case OPTNOMACROS:
	    optnomacros = YES;
	    break;
#endif

	case OPTCREATE:
	    autocreate = YES;
	    break;

#ifdef  FILELOCKING
	case OPTNOLOCKS:
	    nolocks = YES;
	    break;
#endif /* FILELOCKING */

	case OPTSEARCH:
	    if (opteqflg && *cp) {
		searchkey = salloc (strlen(cp)+1, YES);
		strcpy(searchkey, cp);
	    }
	    break;

	case OPTLITERAL:
	    if (uptabs || upblanks){
		opterr = "option cannot be used with -blanks or -tabs";
		goto error;
	    }
	    litmode = YES;
	    break;

	case OPTBLANKS:
	    if (uptabs || litmode){
		opterr = "option cannot be used with -literal or -tabs";
		goto error;
	    }
	    upblanks = YES;
	    break;

	case OPTNOSTRIP:
	    upnostrip = YES;
	    break;

#ifdef NCURSES_MOUSE
	case OPTSKIPMOUSE:
	    optskipmouse = YES;
	    break;
#endif

#ifdef NCURSES
	case OPTBGRGB:
	    if (!opteqflg || *cp == 0)
		getout (YES, "Usage:  -bg=r,g,b");
	    if (sscanf(cp, "%4d,%4d,%4d", &opt_bg_r, &opt_bg_g, &opt_bg_b) != 3)
		getout (YES, "Usage:  -bg=r,g,b");
	    bg_rgb_options = YES;
	    break;

	case OPTFGRGB:
	    if (!opteqflg || *cp == 0)
		getout (YES, "Usage:  -fg=r,g,b");
	    if (sscanf(cp, "%d,%d,%d", &opt_fg_r, &opt_fg_g, &opt_fg_b) != 3)
		getout (YES, "Usage:  -fg=r,g,b");
	    fg_rgb_options = YES;
	    break;

	case OPTSETAB:
	    if (!opteqflg || *cp == 0 || (sscanf(cp, "%d", &opt_setab) != 1))
		getout (YES, "Usage:  -setab=N");
	    if (opt_setab < 0 || opt_setab > 255)
		getout (YES, "setab value must be 0-255");
	    break;

	case OPTSETAF:
	    if (!opteqflg || *cp == 0 || (sscanf(cp, "%d", &opt_setaf) != 1))
		getout (YES, "Usage:  -setaf=N");
	    if (opt_setaf < 0 || opt_setaf > 255)
		getout (YES, "setaf value must be 0-255");
	    break;
#endif

	case OPTTABS:
	    if (upblanks || litmode){
		opterr = "option cannot be used with -blanks or -literal";
		goto error;
	    }
	    uptabs = YES;
	    break;


	default:
	unrecog:
	    opterr = "not recognized";
	error:
	    getout (YES, "%s option %s", argarray[curarg], opterr);
	}
    }
    return;
}

#ifdef COMMENT
void
startup ()
.
    Various initializations.
#endif

void
startup ()
{
    Reg2 char  *name;
    Reg3 Flag accessdot = YES;  /* can write in "." */
    char deftmpdir[100];

#ifdef HOSTTMPNAMES
#define HLEN 256        /* should use MAXNAMELEN in sys/param.h */
    char hostname[HLEN];
    /*
     *  we'll append login.hostname to the tmp files
     */
    hostname[0] = '.';
    hostname[1] = '\0';
/*  strcpy(hostname+1, (char *)mygethostname());*/
    gethostname(hostname+1,HLEN-2);

    Block {
	    register char *s;
	for (s = hostname+1; *s && *s != '.'; s++)
	    continue;
	if (*s == '.') *s = '\0';
    }
#endif /* HOSTTMPNAMES; */

    strncpy(deftmpdir, tmppath, sizeof(deftmpdir));

    Block {
	Reg1 Short i;
	for (i=0; i<MAXFILES ; i++) {
		names[i] = "";
/*
		oldnames[i] = "";
*/
	}
     }

#ifdef  SIGNALS
    Block {
	Reg1 Short  i;
	for (i = 1; i <= NSIG; ++i)
	    switch (i) {
	    case SIGINT:
		/* ignore so that replay questionnaire and exiting are
		 * not vulnerable
		 **/
		signal (i, SIG_IGN);
		break;

#ifdef SIGCHLD  /* 4bsd vmunix */
	    case SIGCHLD:
#endif
#ifdef SIGCLD   /* System V */
#ifndef SIGCHLD     /* 43 defines them both */
	    case SIGCLD:
#endif
#endif
#ifdef SIGTSTP  /* 4bsd vmunix */
	    case SIGTSTP:
	    case SIGCONT:
#endif
#ifdef SIGWINCH
	    case SIGWINCH:
/* begin reszie test */
		signal(i, resize_handler);
		break;
/* end resize test */
#endif /* SIGWINCH */
#define XWINDOWS    /*fornow*/
#ifdef XWINDOWS
#ifdef SIGIO
	    case SIGIO:
#endif
#endif /* XWINDOWS */
		/* leave at SIG_DFL */
		break;

	    default:
		if (signal (i, SIG_DFL) != SIG_IGN)
		    (void) signal (i, sig);
	    }
    }
#endif /* SIGNALS */

#ifdef PROFILE
    Block {
	extern Flag profiling;
	/* sample only 1/8 of all editor runs */
	if (strttime & 7)
	    profil ((char *) 0, 0, 0, 0);
	else
	    profiling = YES;
    }
#endif

    /*
     *  Do this before setitty so that cbreakflg can be cleared if
     *  it is inappropriate for the type of terminal.
     */
    gettermtype ();
    /* stty on input to CBREAK or RAW and ~ECHO ASAP to allow typeahead */
    setitty ();

    Block {
	extern time_t time ();
	time (&strttime);
    }

    /* stty on output for ~CRMOD and ~XTABS */
    setotty ();

    /* initialize terminal.  must be done before "starting..." message */
    /* so keys are initialized. */
    if (!silent) {
	d_put (VCCICL);
	windowsup = YES;
    }
    if (loginflg)
	printf ("%s is starting...", progname);
#ifdef  DEBUGDEF
    else
	printf ("terminal = %s,  keyboard = %s.\n\r", tname, kname);
    sleep (1);
#endif /* DEBUGDEF */

#ifdef VBSTDIO
    if ((outbuf = salloc (screensize, YES))) {
	setbuf (stdout, outbuf);
	stdout->_bufsiz = screensize;
    }
    else
	/* setbuf (stdout, _sobuf); */
	fatal ("Can't allocate screen buffer.\n");

#endif

#ifdef OUT
    blanks = salloc (term.tt_width, YES);
    fill (blanks, (Uint) term.tt_width, ' ');
#endif /* OUT */

    Block {
	Reg1 Short  i;
	static Ff_buf ffbufhdrs[NUMFFBUFS];
	static char ffbufs[NUMFFBUFS * FF_BSIZE];

	for (i = Z; i < NUMFFBUFS; i++) {
	    ffbufhdrs[i].fb_buf = &ffbufs[i * FF_BSIZE];
	    ff_use ((char *)&ffbufhdrs[i], 0);
	}
    }

    la_maxchans = MAXSTREAMS;   /* Essentially no limit.  Limit is dealt */
				/* with in editfile () */

    /* set up cline
     * start with size of first increment
     **/
    cline = salloc ((lcline = icline) + 1, YES);

    /* make the names of the working files */
#define PRIV (S_IREAD | S_IWRITE | S_IEXEC)
    Block {
	struct stat statbuf;

	if (stat(".",&statbuf) == -1 || access (".", 7) != 0)
	    accessdot = NO;
	if (accessdot && (userid != 0 || statbuf.st_uid == 0)) {
	    tmppath = ".e";     /* use current directory for e files */
#ifdef HOSTTMPNAMES
	    goto appendname;
#else
	    if (statbuf.st_uid != userid)
		goto appendname;
	    name = append ("", "");     /* so that name is alloced */
#endif /* HOSTTMPNAMES */
	}
	else {
	    /* maybe we should make tmppath "logindir/etmp/" here */
 appendname:
	    getmypath ();   /* also gets myname */
	    name = append (".", myname); /* we will append user name */
	}
    }

    Block {
	Reg1 char *c;
	c = append (tmpnstr, name);
	la_cfile = append (tmppath, c);
	sfree (c);
	c = append (rstr, name);
	rfile = append (tmppath, c);
	sfree (c);
#ifdef HOSTTMPNAMES
	c = la_cfile;
	la_cfile = append (la_cfile, hostname);
	sfree (c);
	c = rfile;
	rfile = append (rfile, hostname);
	sfree (c);
#endif /* HOSTTMPNAMES */

    }
    Block {
	Reg1 Short indv;
	indv = strlen (tmppath) + VRSCHAR;
	for (evrsn = '1'; ; evrsn++) {
	    if (evrsn > '9')
		getout (NO, "\n%s: No free temp file names left\n", progname);
	    la_cfile[indv] = evrsn;
	    rfile[indv] = evrsn;
#ifndef FILELOCKING
	    Block {
		Reg3 int i;
		if ((i = open (la_cfile, 0)) >= 0) {
		    /* Should use exclusive open here, but we don't have it */
		    /* exclusive => exists and no one has it exclusively open */
		    close (i);
		    dorecov (0);
		    crashed = YES;          /* ie, .ec1 exists */
		    break;
		}
	    }
#endif /* FILELOCKING */
	    /* if we get here, it's because either
	     *  (la_cfile exists and someone is using it)
	     *  or (it doesn't exist)
	     **/
	    if (access (la_cfile, 0) >= 0)
		/* exists and someone is using it */
		continue;
	    if (notracks && access (rfile, 0) >= 0)
		continue;
	    break;
	}
    }

    names[CHGFILE] = la_cfile;
    fileflags[CHGFILE] = INUSE;
    fileflags[NULLFILE] = INUSE;

    /* make the rest of the file names */
    bkeystr[VRSCHAR] = keystr[VRSCHAR] = evrsn;
    Block {
	Reg1 char *c;
	c = append (keystr, name);
	keytmp  = append (tmppath, c);
	sfree (c);
	c = append (bkeystr, name);
	inpfname = bkeytmp = append (tmppath, c);
	sfree (c);
#ifdef HOSTTMPNAMES
	c = keytmp;
	keytmp = append (keytmp, hostname);
	sfree (c);
	c = bkeytmp;
	inpfname = bkeytmp = append (bkeytmp, hostname);
	sfree (c);
#endif /* HOSTTMPNAMES */

    }
    sfree (name);

#ifndef FILELOCKING
    Block {
	struct stat statbuf;
	if (!crashed && stat (keytmp, &statbuf) != -1)
	    dorecov (1);
    }
#endif /* FILELOCKING */

#ifdef  FILELOCKING
    /*
     *  New crash detection:
     *
     *  In allowing the same uid to do multiple edits in the same dir,
     *  look through the current directory for any .ek[1-9] files.
     *  If one exists and is currently locked, another e session should be
     *  in progress.  If its not locked, should have a prior crash/aborted
     *  session.
     */

    Block {
	    Reg1 int indv;
#ifndef HOSTTMPNAMES
	    Reg2 int len;
#endif
	    DIR *dirp;
	    Reg4 int i;
#if SYSV || SOLARIS
	    struct dirent *dp;
#else
	    struct direct *dp;
#endif
	    char tdir[4096]; /* Guaranteed larger than deftmpdir, below */
#ifdef HOSTTMPNAMES
	    char tname[4096];

	sprintf(tname, ".%s%s", myname, hostname);
#else
	len = strlen (keytmp);
#endif
	indv = strlen (tmppath) + VRSCHAR;

	if (*tmppath == '/')
	    strcpy(tdir, deftmpdir);
	else
	    strcpy(tdir, ".");

	if ((dirp = opendir(tdir)) == NULL)
	    getout (NO, "\nCan't open %s\n", tdir);

	for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
#ifdef HOSTTMPNAMES
	    if ( !strncmp (dp->d_name, keytmp, 3)   /* .ek?.login.hostname */
		    && !strcmp(dp->d_name+4,tname)
		    && dp->d_name[indv] >= '1'
		    && dp->d_name[indv] <= '9'
		/*  && (i = open (dp->d_name, 0)) >= 0 ) {  */
		/* lockf() requires write permission */
		    && (i = open (dp->d_name, O_RDWR)) >= 0 ) {
/**
dbgpr("main1:  dp->d_name=(%s)\n", dp->d_name);
**/
#else
	    if (dp->d_namlen == len
		    && !strncmp (dp->d_name, keytmp, 3)
		    && dp->d_name[indv] >= '1'
		    && dp->d_name[indv] <= '9'
		/*  && (i = open (dp->d_name, 0)) >= 0 ) {  */
		    && (i = open (dp->d_name, O_RDWR)) >= 0 ) {
#endif /* HOSTTMPNAMES */
#ifdef FLOCK
		if (flock (i, LOCK_NB | LOCK_EX) == 0) {    /* not locked */
#else
		if (lockf (i, F_TLOCK, 0) == 0) {    /* not locked */
#endif
		    close(i);
		    evrsn = dp->d_name[indv];
		    bkeytmp[indv] = keytmp[indv] = la_cfile[indv] = evrsn;
		    rfile[indv] = evrsn;
		    dorecov (0);        /* 1 or 0 ? */
		    crashed = YES;
		    break;
		}
		close(i);
	    }
	}
	closedir(dirp);
    }
#endif /* FILELOCKING */
    if (!chosereplay)
	inpfname = replfname;
    mv (keytmp, bkeytmp);
    keysmoved = YES;
#ifdef SAVETMPFILES
    if (access(bkeytmp, 0) == 0)
	HaveBkeytmp = 1;
#endif /* SAVETMPFILES */
    if ((keyfile = fopen (keytmp, "w")) == NULL)
	getout (YES, "Can't create keystroke file (%s).", keytmp);

#ifdef  FILELOCKING
#ifdef  FLOCK
    if (flock (fileno (keyfile), LOCK_EX | LOCK_NB) == -1)
	getout (YES, "Can't lock (flock) keystroke file (%s).", keytmp);
#else
    if (lockf (fileno (keyfile), F_TLOCK, 0) == -1)
	getout (YES, "Can't lock (lockf) keystroke file (%s).", keytmp);
#endif /* FLOCK */
#endif /* FILELOCKING */

    chmod (keytmp, 0600);
    initwindows (NO);

    return;
}

#ifdef COMMENT
void
showhelp ()
.
    Do the help option.
#endif
void
showhelp ()
{
    fixtty ();
    printf ("\n\
syntax: %s [options]... [files]...\n\
options are:\n", progname);

    printf ("\
%c -blanks\n", upblanks == YES ? '*' : ' ');

    printf ("\
%c -bullets\n", optbullets == YES ? '*' : ' ');

    printf ("\
%c -create\n", autocreate == YES ? '*' : ' ');

    if (dbgflg)
	printf ("\
* -debug=%s\n", dbgname);

    printf ("\
%c -dtermcap\n", optdtermcap == YES ? '*' : ' ');

    printf ("\
%c -bullets\n", optbullets == YES ? '*' : ' ');

    printf ("\
* -help\n");

    printf ("\
%c -inplace\n", inplace ? '*' : ' ');

    printf ("\
%c -keyboard=%s (keyboard type)\n", optkname ? '*' : ' ',
	    kname ? kname : tname);

#ifdef  KBFILE
    printf ("\
%c -kbfile=%s (keyboard file)\n", optkbfile ? '*' : ' ',
	    kbfile ? kbfile : "(none)");
#endif /* KBFILE */

    printf ("\
%c -literal\n", litmode == YES ? '*' : ' ');

    printf ("\
%c -nobullets\n", optbullets == NO ? '*' : ' ');

#ifdef  NOCMDCMD
    printf ("\
%c -nocmdcmd\n", optnocmd ? '*' : ' ');
#endif /* NOCMDCMD */

#ifdef FILELOCKING
    printf ("\
%c -nolocks\n", nolocks == YES ? '*' : ' ');
#endif

#ifdef STARTUPFILE
    printf ("\
%c -noprofile\n", noprofile == YES ? '*' : ' ');
#endif

    printf ("\
%c -norecover\n", norecover ? '*' : ' ');

    printf ("\
%c -nostick\n", optstick == NO ? '*' : ' ');

    printf ("\
%c -notracks\n", notracks ? '*' : ' ');

#ifdef STARTUPFILE
    printf ("\
%c -profile\n", optprofile != NULL ? '*' : ' ');
#endif

    printf ("\
%c -readonly\n", readonly ? '*' : ' ');

#ifdef NCURSES_MOUSE
    printf ("\
%c -skipmouse\n", optskipmouse ? '*' : ' ');
#endif

    printf ("\
%c -regexp\n", patmode ? '*' : ' ');

    printf ("\
%c -replay=%s\n", replaying ? '*' : ' ', replaying ? inpfname : "filename");

#ifdef FILELOCKING
    printf ("\
%c -search\n", searchkey != NULL ? '*' : ' ');
#endif

    printf ("\
%c -silent\n", silent ? '*' : ' ');

    printf ("\
%c -state=%s\n", state ? '*' : ' ', state ? statefile : "");

    printf ("\
%c -tabs\n", uptabs == YES ? '*' : ' ');

    printf ("\
%c -terminal=%s (terminal type)\n\n", opttname ? '*' : ' ', tname);

    printf ("Color options for highlighting marked areas:\n");
    printf ("\
  -setab=N [0-255] set background color number (black=0, white=255)\n");

    printf ("\
  -setaf=N [0-255] set foreground color number\n");

    printf ("\
   or:\n");
    printf ("\
%c -bgcolor=r,g,b (background color)\n", ' ');

    printf ("\
%c -fgcolor=r,g,b (foreground color)\n\n", ' ');



    printf ("\
*  means option was in effect.");
    return;
}

#ifdef COMMENT
void
dorecov (type)
    int type;
.
    Ask the user how he wants to deal with a crashed or aborted session.
    Type is 0 for crashed, 1 for aborted.
    If the user opts for a recovery or replay, anything which was set by
    checkargs() must be set here to the way we want it for the replay.
#endif
#define RECOVERMSG  "recovermsg"
void
dorecov (type)
Reg3 int type;
{
    if (norecover)
	return;
    fixtty ();
    printf("\n"); fflush(stdout);
    for (;;) Block {
	char line[132];
	Block {
	    int tmp;
	    char recovmsg[256];

	    sprintf (recovmsg, "%s/%s", etcdir, RECOVERMSG);
	    tmp = dup (1);      /* so that filecopy can close it */
	    if (   (tmp = filecopy (recovmsg, 0, (char *)0, tmp, NO, 0)) == -1
		|| tmp == -3
	       )
		fatalpr ("Please notify the system administrators\n\
that the editor couldn't read file \"%s\".\n", recovmsg);
	}
	printf("\n\
Type the number of the option you want then hit <RETURN>: ");
	fflush (stdout);
	/*gets (line);*/
	fgets (line, sizeof line, stdin);
	char ch;
	    /* if a replay option is followed by a number (eg: 2 5),
	     * the 2nd number is used as the number of chars to halt replay
	     * before the end of the keystroke file.
	     */
	if (sscanf(line, "%c %d", &ch, &replay_stopcount) == 2) {
	    /* eg, 2 5 for option 2, stop 5 chars from end of keyfile */
	    if (replay_stopcount < 0) {
	       replay_stopcount = 0;
	    }
	  /*dbgpr("replay_stopcount set to %d\n", replay_stopcount);*/
	}

	if (feof (stdin))
	    getout (type, "");
	switch (*line) {
	case '1':
	    recovering = YES;
	    /*silent = YES;*/
	    if (0) {
	case '2':
		recovering = NO;
		silent = NO;
	    }
	    replaying = YES;
	    helpflg = NO;
	    notracks = NO;
	    chosereplay = YES;
	    break;
	case '3':
	    break;
	case '4':
	    getout (type, "");
	default:
	    continue;
	}
	break;
    }
    setitty ();
    setotty ();
fflush(stdout);
    return;
}

#ifdef COMMENT
void
gettermtype ()
.
    Get the terminal type.  If Version 7, get the TERM environment variable.
    Copy the appropriate terminal and keyboard structures from the tables
    in e.tt.c into the terminal structure to be used for the session.
#endif
void
gettermtype ()
{

    /* Get the selected terminal type */
    if (   !(tname = opttname)
#ifdef ENVIRON
	&& !(tname = getenv ("TERM"))
       )
	getout (YES, "No TERM environment variable or -terminal argument");
#else /* ENVIRON */
       )
	getout (YES, "No -terminal argument");
#endif /* ENVIRON */
    Block {
	Reg1 int ind;
	if ((ind = lookup (tname, termnames)) >= 0)
	    kbdtype = termnames[ind].val; /* assume same as term for now */
	else
	    kbdtype = 0;
	if (   ind >= 0 /* we have the name */
#ifdef  TERMCAP
	    && !optdtermcap /* we aren't forcing the use of termcap */
	    && tterm[termnames[ind].val] != tterm[0] /* not a termcap type */
#endif /* TERMCAP */
	    && tterm[termnames[ind].val] /* we have compiled-in code */
	   )
	    termtype = termnames[ind].val;
#ifdef  TERMCAP
	else {
	    char *str;

	    switch (getcap (tname)) {
	    default:
		termtype = 0; /* termcap type is 0 */
		break;
	    case -1:
		str = "known";
		goto badterm;
	    case -2:
		str = "usable";
	    badterm:
		getout (YES, "Un%s terminal type: \"%s\"", str, tname);
	    }
	}
#else /* TERMCAP */
	else
	    getout (YES, "Unknown terminal type: \"%s\"", tname);
#endif /* TERMCAP */
    }

    /* Get the selected keyboard type */
    if (   (kname = optkname)
#ifdef ENVIRON
	|| (kname = getenv ("EKBD"))
#endif
       ) Block {
	Reg1 int ind;
	if (   (ind = lookup (kname, termnames)) >= 0
	    && tkbd[termnames[ind].val] /* we have compiled-in code for it */
	   )
	    kbdtype = termnames[ind].val;
	else
	    getout (YES, "Unknown keyboard type: \"%s\"", kname);
    }
    else if (kbdtype)
	kname = tname;
    else
	kname = "standard";

    /* select the routines for the terminal type and keyboard type */
    my_move ((char *) tterm[termtype], (char *) &term, (Uint) sizeof (S_term));
    my_move ((char *) tkbd[kbdtype],   (char *) &kbd,  (Uint) sizeof (S_kbd));

    if (term.tt_width > MAXWIDTH)
	term.tt_width = MAXWIDTH;

#ifdef  KBFILE
    /** should integrate this stuff as a keyboard type */
    /** Don't document it until it is done that way */
    /* Get the keyboard file if specified */
    if (   (kbfile = optkbfile)
#ifdef ENVIRON
	|| (kbfile = getenv ("EKBFILE"))
#endif
       ) {
	extern int in_file();
	extern int nop ();

	getkbfile (kbfile);
	kbd.kb_inlex = in_file;
	kbd.kb_init  = nop;
	kbd.kb_end   = nop;
    }

#endif /* KBFILE */
    d_put (VCCINI);     /* initializes display image for d_write */
			/* and selects tt_height if selectable */

    /* initialize the keyboard */
    (*kbd.kb_init) ();

    Block {
	Reg1 int tmp;
	tmp = term.tt_naddr;
	tt_lt2 = term.tt_nleft  && 2 * term.tt_nleft  < tmp;
	tt_lt3 = term.tt_nleft  && 3 * term.tt_nleft  < tmp;
	tt_rt2 = term.tt_nright && 2 * term.tt_nright < tmp;
	tt_rt3 = term.tt_nright && 3 * term.tt_nright < tmp;
    }

    if (optbullets >= 0)
	borderbullets = optbullets;
    else if (borderbullets)
	borderbullets = term.tt_bullets;
    return;
}

#ifdef COMMENT
void
setitty ()
.
    Set the tty modes for the input tty.
#endif
void
setitty ()
{
#define BITS(start,yes,no) start  = ( (start| (yes) )&( ~(no) )  )
#ifdef SYSIII
    struct termio temp_termio;

#   ifdef CBREAK
	char ixon = cbreakflg;
#   else  /* CBREAK */
	char ixon = NO;
#   endif /* CBREAK; */
    if (ioctl(STDOUT, TCGETA, &in_termio) < 0)
	return;
    temp_termio = in_termio;
    temp_termio.c_cc[VMIN]=1;
    temp_termio.c_cc[VTIME]=0;
    BITS(temp_termio.c_iflag,
	 IGNPAR|ISTRIP,
	 IGNBRK|BRKINT|PARMRK|INPCK|INLCR|IGNCR|ICRNL|IUCLC|IXOFF
	);
    if(ixon) temp_termio.c_iflag  |= IXON;
    else     temp_termio.c_iflag  &= ~IXON;
    BITS(temp_termio.c_lflag,NOFLSH,ISIG|ICANON|ECHO);
    if(ioctl(STDOUT,TCSETAW,&temp_termio) >= 0) istyflg=YES;
    fcntlsave = fcntl(STDOUT,F_GETFL,0);
    return;
#undef BITS
#else /* SYSIII */


    if (ioctl (STDOUT, TIOCGETP, &instty) < 0)
	return;

#ifdef  CBREAK
#ifdef  TIOCGETC
    if (cbreakflg) Block {
	static struct tchars tchars = {
	    0xff,       /* char t_intrc;   / * interrupt */
	    0xff,       /* char t_quitc;   / * quit */
	    'Q' & 0x1f, /* char t_startc;  / * start output */
	    'S' & 0x1f, /* char t_stopc;   / * stop output */
	    0xff,       /* char t_eofc;    / * end-of-file */
	    0xff,       /* char t_brkc;    / * input delimiter (like nl) */
	};
	if (   ioctl (STDOUT, TIOCGETC, &spchars) < 0
	    || tchars.t_startc != spchars.t_startc
	    || tchars.t_stopc  != spchars.t_stopc
	    || ioctl (STDOUT, TIOCSETC, &tchars) < 0
	   )
	    cbreakflg = NO;
    }
#endif /* TIOCGETC */

#ifdef  TIOCGLTC
    if (cbreakflg) Block {
	static struct ltchars ltchars = {
	    0xff,       /* char t_suspc;   / * stop process signal */
	    0xff,       /* char t_dsuspc;  / * delayed stop process signal */
	    0xff,       /* char t_rprntc;  / * reprint line */
	    0xff,       /* char t_flushc;  / * flush output (toggles) */
	    0xff,       /* char t_werasc;  / * word erase */
	    0xff,       /* char t_lnextc;  / * literal next character */
	};
	if (   ioctl (STDOUT, TIOCGLTC, &lspchars) < 0
	    || ioctl (STDOUT, TIOCSLTC, &ltchars) < 0
	   ) {
	    (void) ioctl (STDOUT, TIOCSETC, &spchars);
	    cbreakflg = NO;
	}
    }
#endif /* TIOCGLTC */
#endif /* CBREAK */

    Block {
	Reg1 int tmpflags;
	tmpflags = instty.sg_flags;
#ifdef  CBREAK
	if (cbreakflg)
	    instty.sg_flags = CBREAK | (instty.sg_flags & ~(ECHO | CRMOD));
	else
#endif /* CBREAK */
	    instty.sg_flags = RAW | (instty.sg_flags & ~(ECHO | CRMOD));
	if (ioctl (STDOUT, TIOCSETP, &instty) >= 0)
	    istyflg = YES;
	instty.sg_flags = tmpflags;             /* all set up for cleanup */
    }
    return;
#endif /* SYSIII */
}

#ifdef COMMENT
void
setotty ()
.
    Set the tty modes for the output tty.
#endif
void
setotty ()
{
#ifdef SYSIII
    if (ioctl (STDOUT, TCGETA, &out_termio) < 0)
#else /* SYSIII */
    if (ioctl (STDOUT, TIOCGETP, &outstty) < 0)
#endif /* SYSIII */
	fast = YES;
    else Block {
	Reg1 int i;
#ifdef MESG_NO
	struct stat statbuf;
#endif /* MESG_NO */

#ifdef SYSIII

#ifdef COMMENT
The define of speed below and the comparison, further below, of the
result with B4800 assume a monotinicity of the Bs in the speeds they
represent which is true but undocumented for System 3.
In fact the system 3 Bs are identical to the version 7 Bs.
#endif /* COMMENT */
#define SPEED ((out_termio.c_cflag)&CBAUD)
	i = out_termio.c_oflag;
	out_termio.c_oflag &= ~(OLCUC|ONLCR|OCRNL|ONOCR|ONLRET);
	if( (out_termio.c_oflag & TABDLY) == TAB3)
	    out_termio.c_oflag &= ~TABDLY;
	if(ioctl(STDOUT,TCSETA,&out_termio) >= 0) {
	    ostyflg = YES;
	    out_termio.c_oflag = i;  /* all set up for cleanup */
	}
#else /* SYSIII */
#define SPEED (outstty.sg_ospeed)
	i = outstty.sg_flags;
	outstty.sg_flags &= ~CRMOD;
	if ((outstty.sg_flags & TBDELAY) == XTABS)
	    outstty.sg_flags &= ~TBDELAY;
	if (ioctl (STDOUT, TIOCSETP, &outstty) >= 0) {
	    ostyflg = YES;
	    outstty.sg_flags = i;             /* all set up for cleanup */
	}
#endif /* SYSIII */

#ifdef MESG_NO
	fstat (STDOUT, &statbuf);        /* save old tty message status and */
	oldttmode = statbuf.st_mode;
#ifdef TTYNAME
	if ((ttynstr = ttyname (STDOUT)) != NULL)
#endif /* TTYNAME */
#ifdef TTYN
	if ((ttynstr[strlen (ttynstr) - 1] = ttyn (STDOUT)) != NULL)
#endif /* TTYN */
	    chmod (ttynstr, 0600);        /* turn off messages */
#endif /* MESG_NO */
	fast = (ospeed = SPEED) >= B4800;
    }
    /* no border bullets if speed is slow */
    if (!fast && optbullets == -1)
	borderbullets = NO;
    return;
#undef SPEED
}

#ifdef COMMENT
void
makescrfile ()
.
    Initialize the #o and #c pseudo-files.
    Set up the qbuffers (pick, close, etc.).
#endif
void
makescrfile ()
{
    Reg1 int j;

    for (j = FIRSTFILE; j < FIRSTFILE + NTMPFILES; j++) {
	names[j] = append (tmpnames[j - FIRSTFILE], ""); /* must be freeable*/
	/*  the firstlas is used for appending lines, and a separate stream
	 *  is cloned for the lastlook wksp so that seeks on it won't disturb
	 *  the seek position of firstlas[j]
	 **/
	if (!la_open ("", "nt", &fnlas[j], (La_bytepos) 0,
		(Ff_stream *) 0, 0, 0))
	    getout (YES, "can't open %s", names[j]);
	(void) la_clone (&fnlas[j], &lastlook[j].las);
	lastlook[j].wfile = j;
	fileflags[j] = INUSE | CANMODIFY;
    }

    for (j = 0; j < NQBUFS; j++)
	(void) la_clone (&fnlas[qtmpfn[j]], &qbuf[j].buflas);
    return;
}

#ifdef COMMENT
void
getstate (ichar)
    Char ichar;
.
    Set up things as per the state file.
    See State.fmt for a description of the state file format.
    The 'ichar' argument is still not totally cleaned up from the way
    this was originally written.
    If ichar ==
      ALL_WINDOWS use all windows and files from state file.
      ONE_WINDOW  set up one full-size window and dont put up any files.
#endif
void
getstate (ichar)
Char ichar;
{
    Small winnum;
    Reg1 FILE *gbuf;
    char *x;

    x = state ? statefile : rfile;
    if (   ichar == NO_WINDOWS
	|| notracks
	|| (gbuf = fopen (x, "r")) == NULL
       ) {
	makestate (gbuf == NULL);
	return;
    }

    Block {
	Reg2 short i;
	if ( (i = getshort (gbuf)) != revision) {
	    if (i >= 0 || feoferr (gbuf))
		goto badstart;
	    if (i != -1) {          /* always accept -1 */
		/*
		 *  If not 'recovering' and not 'replaying', get rid of
		 *  the 0-length .ek1 file just created so next time we
		 *  don't get the bogus 'crashed' msg.
		 */
		if (!recovering && !replaying && keyfile != NULL) {
		    fclose (keyfile);
		    unlink (keytmp);
		}
		getout (YES,
			"Startup file: \"%s\" was made by revision %d of %s.\n\
Delete it or give a filename after the %s command.",
			rfile, -i, progname, progname);
	    }
	}
    }

    /* terminal type */
    Block {
	Reg2 Small nletters;
	nletters = getshort (gbuf);
	if (feoferr (gbuf))
	    goto badstart;
	getskip (nletters, gbuf);
    }

    Block {
	Reg2 Slines nlin;
	Reg3 Scols ncol;
	nlin = getc (gbuf) & 0377;
	ncol = getc (gbuf) & 0377;
	if (nlin != term.tt_height || ncol != term.tt_width) {
	    if (nlin > term.tt_height || ncol > term.tt_width) {
		/*
		 *  If not 'recovering' and not 'replaying', get rid of
		 *  the 0-length .ek1 file just created so next time we
		 *  don't get the bogus 'crashed' msg.
		 */
		if (!recovering && !replaying && keyfile != NULL) {
		    fclose (keyfile);
		    unlink (keytmp);
		}
		getout (YES, "\
Startup file: \"%s\" was made for a terminal with a different screen size. \n\
(%d h X %d w).  Delete it or give a filename after the %s command.",
		    rfile, nlin, ncol, progname);
	    }
	    /*
	     *  Can readjust screensize to statefile parameters.
	     */
	    term.tt_width = ncol;
	    term.tt_height = nlin;
	    initwindows (YES);
	}
    }

    /* sttime     = */ getlong (gbuf);

    if ((ntabs = getshort (gbuf))) Block {
	Reg2 int ind;
	if (feoferr (gbuf))
	    goto badstart;
	stabs = max (ntabs, NTABS);
	tabs = (ANcols *) salloc (stabs * sizeof *tabs, YES);
	for (ind = 0; ind < ntabs; ind++)
	    tabs[ind] = (ANcols) getshort (gbuf);
	if (feoferr (gbuf))
	    goto badstart;
    }

    linewidth = getshort (gbuf);

    Block {
	Reg2 Short i;
	if ((i = getshort (gbuf))) {
	    if (feoferr (gbuf))
		goto badstart;
	    searchkey = salloc (i, YES);
	    fread (searchkey, 1, i, gbuf);
	    if (feoferr (gbuf))
		goto badstart;
	}
    }

    insmode = getc (gbuf);

    if (getc(gbuf) || patmode)  patmode = YES;  /* Added Purdue CS 10/8/82 MAB */
    if (getc(gbuf) || litmode)  litmode = YES;  /* Added Purdue CS 2/8/83 MAB */
    if (getc(gbuf) || uptabs)   uptabs = YES;   /* Added Purdue CS 2/8/83 MAB */
    if (getc(gbuf) || upblanks) upblanks = YES; /* Added Purdue CS 2/8/83 MAB */
    if (getc(gbuf) || upnostrip)upnostrip = YES;/* Added Purdue CS 2/8/83 MAB */

    if (getc (gbuf)) {  /* curmark */
	getskip (sizeof (long)
		 + sizeof (short)
		 + sizeof (char)
		 + sizeof (short),
		 gbuf);
    }

#ifdef LMCAUTO
    autofill = getc (gbuf);
    autolmarg = getshort (gbuf);
    info (inf_auto, 2, autofill ? "WP" : "  ");
#else /* LMCAUTO */
    (void) getc (gbuf);
    (void) getshort (gbuf);
#endif /* LMCAUTO */

    nwinlist = getc (gbuf);
    if (ferror(gbuf) || nwinlist > MAXWINLIST)
	goto badstart;
    winnum = getc (gbuf);
    if (ichar != ALL_WINDOWS) Block {
	Reg3 Short  n;
	/* skip over windows until we get to the right window */
	for (n = Z; n < winnum; n++) {
	    getskip (sizeof (char)
		     + sizeof (char)
		     + sizeof (short)
		     + sizeof (char)
		     + sizeof (short)
		     + sizeof (char)
		     , gbuf);
	    Block {
		Reg2 Short i;
		if ((i = getshort (gbuf))) {
		    getskip (i, gbuf);
		    getskip (sizeof (long)
			     + sizeof (short)
			     + sizeof (char)
			     + sizeof (short),
			     gbuf);
		}
	    }
	    getskip (getshort (gbuf), gbuf);
	    getskip (sizeof (long)
		     + sizeof (short)
		     + sizeof (char)
		     + sizeof (short),
		     gbuf);
	    if (feoferr (gbuf))
		goto badstart;
	}
	winnum = 0;
	nwinlist = 1;
    }

    /* set up the windows */
    Block {
	Reg7 Small n;
	for (n = Z; n < nwinlist; n++) Block {
	    Reg6 Small gf;
	    Reg5 S_window *window;
	    window = winlist[n] = (S_window *) salloc (SWINDOW, YES);
	    window->prevwin = getc (gbuf);
	    if (ichar != ALL_WINDOWS)
		window->prevwin = 0;
	    Block {
		Reg2 Scols   lmarg;
		Reg3 Scols   rmarg;
		Reg4 Slines  tmarg;
		Slines  bmarg;
		AFlag   winflgs;

		tmarg = getc (gbuf);
		lmarg = getshort (gbuf);
		bmarg = getc (gbuf);
		rmarg = getshort (gbuf);
		winflgs = getc (gbuf);
		if (ichar != ALL_WINDOWS) {
		    tmarg = 0;
		    lmarg = 0;
		    bmarg = term.tt_height - 1 - NPARAMLINES;
		    rmarg = term.tt_width - 1;
		    winflgs = 0;
		}
		defplline = getshort (gbuf);
		defmiline = getshort (gbuf);
		defplpage = getshort (gbuf);
		defmipage = getshort (gbuf);
		deflwin = getshort (gbuf);
		defrwin = getshort (gbuf);
		setupwindow (window, lmarg, tmarg, rmarg, bmarg, winflgs, 1);
	    }
	    if (n != winnum)
		drawborders (winlist[n], WIN_INACTIVE);
	    switchwindow (window);
	    gf = 0;
	    Block {
		Reg3 Small nletters;
		if ((nletters = getshort (gbuf))) {   /* there is an alternate file */
		    char *fname;
		    if (feoferr (gbuf))
			goto badstart;
		    fname = salloc (nletters, YES);
		    fread (fname, 1, nletters, gbuf);
		    Block {
			Reg2 Nlines  lin;
			Reg4 Ncols   col;
			Slines tmplin;
			Scols tmpcol;
			lin = getlong  (gbuf);
			col = getshort (gbuf);
			tmplin = getc (gbuf);
			tmpcol = getshort (gbuf);
			if (ichar != ONE_FILE) {
			    if (editfile (fname, col, lin, 0, NO) == 1)
				gf = 1;
			    /* this sets them up to get copied into
			     * curwksp->ccol & clin */
			    poscursor (curwksp->ccol = tmpcol,
				       curwksp->clin = tmplin);
			}
			else
			    poscursor (0, 0);
		    }
		    /*  do this so that editfile won't be cute about
		     *  suppressing a putup on a file that is the same
		     *  as the altfile
		     **/
		    curfile = NULLFILE;
		}
	    }
	    Block {
		Reg4 char *fname;
		Block {
		    Reg2 Small nletters;
		    nletters = getshort (gbuf);
		    if (feoferr (gbuf))
			goto badstart;
		    fname = salloc (nletters, YES);
		    fread (fname, 1, nletters, gbuf);
		}
		Block {
		    Reg2 Nlines  lin;
		    Reg3 Ncols   col;
		    Slines tmplin;
		    Scols tmpcol;
		    lin = getlong  (gbuf);
		    col = getshort (gbuf);
		    tmplin = getc (gbuf);
		    tmpcol = getshort (gbuf);
		    if (feoferr (gbuf))
			goto badstart;
		    if (n != winnum)
			chgborders = 2;
		    if (editfile (fname, col, lin, 0, n == winnum ? NO : YES)
			== 1) {
			gf = 2;
			poscursor (curwksp->ccol = tmpcol,
				   curwksp->clin = tmplin);
		    }
		    chgborders = 1;
		}
	    }
	    if (gf < 2) {
		if (n != winnum)
		    chgborders = 2;
		if (gf == 0) {
		    eddeffile (n == winnum ? NO : YES);
		    curwksp->ccol = curwksp->clin = 0;
		}
		else if (n != winnum)
		    putupwin ();
		chgborders = 1;
	    }
	}
    }

    if ( feoferr (gbuf))
badstart:
	getout (YES, "Bad state file: \"%s\"\nDelete it and try again.",
		 rfile);

    drawborders (winlist[winnum], WIN_ACTIVE);
    switchwindow (winlist[winnum]);
    infotrack (winlist[winnum]->winflgs & TRACKSET);
    poscursor (curwksp->ccol, curwksp->clin);
    fclose (gbuf);
    return;
}

#ifdef COMMENT
void
getskip (num, file)
    Reg2 int num;
    Reg1 FILE *file;
.
    Skip 'num' bytes in 'file'.  'Num' must be > 0.
#endif
void
getskip (num, file)
Reg1 int num;
Reg2 FILE *file;
{
    do {
	getc (file);
    } while (--num);
    return;
}

#ifdef COMMENT
void
makestate (nofileflg)
    Flag nofileflg;
.
    Make an initial state without reference to the state file.
    If 'nofileflg' != 0, edit the 'scratch' file.
#endif
void
makestate (nofileflg)
Flag nofileflg;
{
    Reg1 S_window *window;

    nwinlist = 1;
    window = winlist[0] = (S_window *) salloc (SWINDOW, YES);

    stabs = NTABS;
    tabs = (ANcols *) salloc (stabs * sizeof *tabs,YES);
    ntabs = 0;

    setupwindow (winlist[0], 0, 0,
		 term.tt_width - 1, term.tt_height - 1 - NPARAMLINES, 0, 1);
    switchwindow (window);              /* switched this and next line */
    tabevery ((Ncols) TABCOL, (Ncols) 0, (Ncols) (((NTABS / 2) - 1) * TABCOL),
	      YES);
    drawborders (window, WIN_ACTIVE);
    poscursor (0, 0);
    if (nofileflg) {
	edscrfile (NO);
	poscursor (0, 0);
    }
    return;
}

#ifdef LMCAUTO
#ifdef COMMENT
void
infoint0 ()
.
    Set up initial parameters for the info line.
#endif
void
infoint0 ()
{
    Reg1 Scols col;

    inf_insert = 0; col = 7;    /* "INSERT" */
#ifdef RECORDING
    inf_record = col; col += 4;    /* "REC" */
#endif
    inf_track = col; col += 4;  /* "TRK" */
    inf_range = col; col += 7;  /* "=RANGE" */
    inf_pat = col; col += 3;    /* "RE" */
#ifdef LMCAUTO
    inf_auto = col; col += 3;   /* "WP" */
#endif
    inf_tab = col; col += 4;    /* "TAB"/"LIT"/"BL" Added 2/8/83 MAB */
    inf_str = col; col += 4;    /* "NOS" Added 2/8/83 MAB */
    inf_mark = col; col += 5;   /* "MARK" */
    inf_area = col; col += 7;   /* "30x16" etc. */

    inf_at = col; col += 3;     /* "At"         */
    inf_line = col; col += 7;   /* line         */
    inf_in = col; col += 3;     /* "in"         */
    inf_file = col;             /* filename     */

}

#ifdef COMMENT
void
infoinit ()
.
    Set up the displays on the info line.
#endif
void
infoinit ()
{
    if (autofill)
	info (inf_auto, 2, "WP");
#else /* LMCAUTO */
#ifdef COMMENT
void
infoinit ()
.
    Set up the displays on the info line.
#endif
void
infoinit ()
{
    Reg1 Scols col;

    inf_insert = 0; col = 4;    /* "INS" */

#ifdef RECORDING
    inf_record = col; col += 4;    /* "REC" */
#endif

    inf_track = col; col += 6;  /* "TRACK" */

    inf_range = col; col += 7;  /* "=RANGE" */

    inf_pat = col; col += 3;    /* "RE" Added 10/18/82 MAB */
/*  inf_auto = col; col += 3;   / * "WP" */
    inf_tab = col; col += 4;    /* "TAB"/"LIT"/"BL" Added 2/8/83 MAB */

    inf_str = col; col += 4;    /* "NOS" Added 2/8/83 MAB */

    inf_mark = col; col += 5;   /* "MARK" */

    inf_area = col; col += 7;   /* "30x16" etc. */

    inf_at = col; col += 3;     /* "At"         */
    inf_line = col; col += 7;   /* line         */
    inf_in = col; col += 3;     /* "in"         */
    inf_file = col;             /* filename     */
#endif /* LMCAUTO */

    info (inf_at, 2, "At");
    info (inf_in, 2, "in");
    infoline = -1;
    infofile = NULLFILE;
    if (insmode) {
	insmode = NO;
	tglinsmode ();
    }
    if (patmode) {
	patmode = NO;
	tglpatmode ();
    }
    if (uptabs) {               /* Added Purdue CS 2/8/83 MAB */
	uptabs = NO;
	tgltabmode ();
    }
    if (upblanks){              /* Added Purdue CS 2/8/83 MAB */
	upblanks = NO;
	tglblamode ();
    }
    if (litmode) {              /* Added Purdue CS 2/8/83 MAB */
	litmode = NO;
	tgllitmode ();
    }
    if (upnostrip) {            /* Added Purdue CS 2/8/83 MAB */
	upnostrip = NO;
	tglstrmode ();
    }
}

#ifdef COMMENT
void
getout (filclean, str, a1,a2,a3,a4,a5,a6)
    Flag filclean;
    char *str;
.
    This is called to exit from the editor for conditions that arise before
    entering mainloop().
#endif
/* VARARGS2 */
#ifdef OUT
_Noreturn
void
getout (filclean, str, a1,a2,a3,a4,a5,a6)
Flag filclean;
char   *str, *a1;
{
    extern char verstr[];

    fixtty ();
    if (windowsup)
	screenexit (YES);
    if (filclean && !crashed)
	cleanup (YES, NO);
    if (keysmoved)
	mv (bkeytmp, keytmp);
    d_put (0);
    if (*str)
/*      dopr (stdout, &str);    */
	/* fprintf (stdout, str, a1,a2,a3,a4,a5,a6); */
	fprintf (stdout, str, a1);
	fprintf (stdout, "This is screwed up: %s.\n", a1);
	fatal (FATALSIG, "Bombing out: %d\n", 1);
    printf ("\nThis is %s revision %d\n%s", progname, -revision, verstr);
#ifdef PROFILE
    monexit (-1);
#else
    exit (-1);
#endif /* PROFILE */
    /* NOTREACHED */
}
#endif /* OUT */

/* This is the real version of getout ().  We had to change it to the
 * new "varargs" version because "e" went into a crash loop when using the
 * old way of just loading up extra arguments in the argument list.
 */
/* VARARGS2 */
_Noreturn void getout (Flag filclean, char *str, ...)
{
    va_list ap;
    va_start (ap, str);

    extern char verstr[];

    fixtty ();
    if (windowsup)
	screenexit (YES);
    if (filclean && !crashed)
	cleanup (YES, NO);
    if (keysmoved)
	mv (bkeytmp, keytmp);
    d_put (0);
    if (*str)
/*      dopr (stdout, &str);    */
	vfprintf (stdout, str, ap);
    printf ("\nThis is %s revision %d\n%s", progname, -revision, verstr);
#ifdef PROFILE
    monexit (-1);
#else
    exit (-1);
#endif
    va_end (ap);        /* For form's sake */
    /* NOTREACHED */
}


#ifdef COMMENT
void
initwindows (resizing)
    Flag resizing;
.
    This is called to initialize (resizing=FALSE) various windows on
    startup, or to resize them (resizing=TRUE) to accommodate a
    replay from a session made with different screen sizes.
#endif
void
initwindows (resizing)
    Flag resizing;
{
    extern Uchar *image;

    if (resizing) {
	sfree (image);
	image = (Uchar *)NULL;
	d_init(YES,NO);
	if (wholescreen.wksp)
	    sfree (wholescreen.wksp);
	if (enterwin.wksp)
	    sfree (enterwin.wksp);
	if (infowin.wksp)
	    sfree (infowin.wksp);
	if (blanks)
	    sfree (blanks);
    }

    blanks = salloc (term.tt_width, YES);
    fill (blanks, (Uint) term.tt_width, ' ');

    /* Set up utility windows.
     * They have no margins, and they are not on winlist.
     **/
    /* wholescreen window.  general utility */
    setupwindow (&wholescreen, 0, 0,
		 term.tt_width - 1, term.tt_height - 1, 0, NO);
    /* parameter entry window */
    setupwindow (&enterwin, 0,
		 term.tt_height - NENTERLINES - NINFOLINES,
		 term.tt_width - 1,
		 term.tt_height - 1 - NINFOLINES, 0, NO);
    enterwin.redit = term.tt_width - 1;
    /* info display window. */
    setupwindow (&infowin, 0,
		 term.tt_height - NINFOLINES,
		 term.tt_width - 1,
		 term.tt_height - 1, 0, NO);

    curwin = &wholescreen;
    return;
}


/*
 * catch a resize window signal, let user
 * know resizing is not supported (yet)
 */
void
resize_handler (int sig) {

    signal(SIGWINCH, SIG_IGN);

    struct winsize winsize;
    int h, w;
    if (ioctl(0, TIOCGWINSZ, (char *) &winsize) == 0) {
	h = winsize.ws_row;
	w = winsize.ws_col;
     /**/ dbgpr("got SIGWINCH sig=%d, h=%d w=%d LINES=%d, COLS=%d\n", sig, h, w, LINES, COLS); /**/
    }

    /* if the window is smaller, advice how best to recover */
    char buf[128] = "";
    if (h < term.tt_height || w < term.tt_width) {
	snprintf(buf, 80, "Enlarge to at least %d x %d, then <cmd>redraw to recover.",
	   term.tt_height, term.tt_width);
    }
    if (h != term.tt_height || w != term.tt_width) {
	mesg(ERRALL + 2, "Window size changes are not supported. ",  buf);
	fflush(stdout);
    }

    signal(SIGWINCH, resize_handler);
}

