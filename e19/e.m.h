/*
 * file e.m.h - some specialized include stuff
 **/

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

extern char *cmdname;   /* the full name of the command given */
extern char *cmdopstr;  /* command operand string - points to the rest of
			 * paramv after the command */
extern char *opstr;     /* first word in cmdopstr from a call to getword () */
extern char *nxtop;     /* next word after opstr - use for further getwords */

/* commands: <arg> command <ret> */
#define CMD_MARK        0
#define CMDCHWINDOW     1
#define CMDEXIT         2
#define CMDLOGOUT       3
#define CMD_COMMAND     4
#define CMDCENTER       5
#define CMDMARK         6
#define CMDWINDOW       7
#define CMD_WINDOW      8
#define CMDRUN          9
#define CMDSAVE         10
#define CMDSTOP         11
#define CMDFILL         12
#define CMDJUST         13
#define CMDEDIT         14
#define CMDREDRAW       15
#define CMDGOTO         16
#define CMDSPLIT        17
#define CMD_JOIN        17
#define CMDJOIN         18
#define CMD_SPLIT       18
#define CMDPRINT        19
#define CMDCOMMAND      20
#define CMD_PICK        26 /************************************/
#define CMD_CLOSE       27 /*  these three must be consecutive */
#define CMD_ERASE       28 /************************************/
#define CMDTAB          29
#define CMD_TAB         30
#define CMDTABS         31
#define CMD_TABS        32
#define CMDTABFILE      33
#define CMD_TABFILE     34
#define CMDFEED         35
#define CMDHELP         36
#define CMDDELETE       37
#define CMDCLEAR        38
#define CMDNAME         39
#define CMDSHELL        40
#define CMDCALL         41
#define CMDMACRO        42
#define CMD_MACRO       43
#define CMDENDM         44
#define CMDSEARCH       45
#define CMD_SEARCH      46
#define CMDREPLACE      47
#define CMD_REPLACE     48
#define CMDUPDATE       49
#define CMD_UPDATE      50
#define CMDCOVER        51 /***********************************/
#define CMDOVERLAY      52 /*  these six must be consecutive  */
#define CMDUNDERLAY     53 /*                                 */
#define CMDBLOT         54 /*                                 */
#define CMD_BLOT        55 /*                                 */
#define CMDINSERT       56 /***********************************/
#define CMDUNDO         57
#ifdef DEBUGALLOC
#define CMDVERALLOC     58
#endif
#define CMDTRACK        59
#define CMD_TRACK       60
#define CMDRANGE        61
#define CMD_RANGE       62
#define CMDQRANGE       63
#define CMDSET          64
#define CMDQSET         65
#define CMDPATTERN      66
#define CMD_PATTERN     67
#ifdef LMCAUTO
#define CMDAUTO         68
#define CMD_AUTO        69
#endif
#ifdef LMCDWORD
#define CMDDWORD        70
#define CMD_DWORD       71
#endif /* LMCDWORD */
#define CMD_DIFF        72
#define CMDDIFF         73
#define CMDSTATS        77
#define CMDPICK         100  /************************************/
#define CMDCLOSE        101  /*  these seven must be consecutive */
#define CMDERASE        102  /*                                  */
#define CMDOPEN         103  /*                                  */
#define CMDBOX          104  /*                                  */
#ifdef LMCCASE
#define CMDCAPS         105  /*                                  */
#define CMDCCASE        106  /************************************/
#endif
#ifdef RECORDING
#define CMDRECORD       107
#define CMDPLAY         108
#define CMDSTORE        109
#define CMDUNDEFINE     110
#define CMDQMACROS      111
#endif /* RECORDING */

#ifdef NCURSES
#define CMDMOUSEONOFF   112
#endif

/* these came from the top of e.m.c */
/*extern*/
struct loopflags {
    AFlag clear;        /* clear enterwin  */
    AFlag hold;         /* hold cmd line until next key input */
    AFlag beep;         /* beep on clearing cmd line */
    AFlag flash;        /* bullet at cursor position is to be temporarily */
    AFlag bullet;       /* redo all border bullets */
 };
extern
struct loopflags loopflags;

extern char   *copy ();

/**/

struct markenv
{
    Nlines  mrkwinlin;
    ANcols  mrkwincol;
    ASlines mrklin;
    Scols   mrkcol;
};
extern
struct markenv *curmark,
	       *prevmark;

extern
Nlines  marklines;      /* display of num of lines marked */
extern
Ncols   markcols;       /* display of num of columns marked */

extern
char    mklinstr [],   /* string of display of num of lines marked */
	mkcolstr [];   /* string of display of num of lines marked */
extern
Small   infoarealen;    /* len of string of marked area display */

