#ifdef COMMENT
--------
file e._profile.c
    routines dealing with ".e_profile" file
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#include "e.h"
#include "e.m.h"
#include <sys/stat.h>
#include "e.tt.h"

#ifdef  STARTUPFILE

#define PROFILENAME     ".e_profile"

Flag    dot_profile;            /* set when .e_profile file exists */
FILE    *fp_profile;

char    *index();
char    *getenv();

extern  char **argarray;        /* used in checkargs() in e.c */
extern  Small numargs;          /*  "           "        "    */
extern  struct loopflags loopflags;
extern  Flag entering;
extern void chk_profile (char *);
extern int get_profilekey (Flag);

int SafeCurDirProfile(void);
void getopt_startup(void);


S_looktbl keysyms[] = {
    {"+line",   CCPLLINE},
    {"+page",   CCPLPAGE},
    {"+sch",    CCPLSRCH},
    {"+tab",    CCTAB},
    {"+word",   CCRWORD},
    {"-line",   CCMILINE},
    {"-page",   CCMIPAGE},
    {"-sch",    CCMISRCH},
    {"-tab",    CCBACKTAB},
    {"-word",   CCLWORD},
    {"bksp",    CCBACKSPACE},
    {"cchar",   CCCTRLQUOTE},
    {"chwin",   CCCHWINDOW},
    {"close",   CCCLOSE},
    {"cmd",     CCCMD},
    {"dchar",   CCDELCH},
    {"down",    CCMOVEDOWN},
    {"dword",   CCDWORD},
    {"edit",    CCSETFILE},
    {"erase",   CCERASE},
    {"esc",     CCINSMODE},
    {"home",    CCHOME},
    {"insmd",   CCINSMODE},
    {"int",     CCINT},
    {"join",    CCJOIN},
    {"left",    CCMOVELEFT},
    {"mark",    CCMARK},
    {"mouse",   CCMOUSEONOFF},
    {"open",    CCOPEN},
    {"pick",    CCPICK},
#ifdef RECORDING
    {"play",    CCPLAY},
#endif
#ifdef RECORDING
    {"record",  CCRECORD},
#endif
    {"redraw",  CCREDRAW},      /* added for KEY_F ekbfile support */
    {"replace", CCREPLACE},
    {"ret",     CCRETURN},
    {"right",   CCMOVERIGHT},
    {"split",   CCSPLIT},
    {"srtab",   CCTABS},
    {"tab",     CCTAB},
    {"undef",   CCUNAS1},
    {"up",      CCMOVEUP},
    {"wleft",   CCLWINDOW},
    {"wright",  CCRWINDOW},
    {0,         0}
 };





/*
 *  chk_profile - check if a .e_profile file exists: use environment
 *      EPROFILE, or "./.e_profile" (if safe), otherwise "~/.e_profile".
 */

void
chk_profile( profilename )
  char *profilename;
{
	char tmp[200];
	char *startupfile;
	int SafeCurDirProfile ();
	void getopt_startup ();

	if( profilename == (char *)NULL ) {
	    /*
	     *  if EPROFILE20 or EPROFILE is set in the environment, use it.
	     */
	    if(( startupfile = getenv( "EPROFILE20" )) == NULL ) {
		if(( startupfile = getenv( "EPROFILE" )) == NULL ) {
		    if( SafeCurDirProfile())
			strcpy( tmp, PROFILENAME );
		    else
			sprintf( tmp, "%s/%s", getmypath(), PROFILENAME );
		    startupfile = tmp;
		}
	    }
	}
	else
	    startupfile = profilename;

	fp_profile = fopen( startupfile, "r" );

	if( fp_profile != NULL ) {
	    dot_profile = YES;
	    getopt_startup();
	}

	return;
}

/*
 *  SafeCurDirProfile -- if ./.e_profile exists, use it if
 *      - we don't have root privileges
 *      - we own the file
 *      - it's not writable by the world
 */

int
SafeCurDirProfile()
{
	unsigned int euid;
	struct stat sb;

	if( userid == 0 || (euid = geteuid()) == 0 )
	    return 0;

	if( stat( PROFILENAME, &sb ) < 0 )
	    return 0;

	if( userid != sb.st_uid && euid != sb.st_uid )
	    return 0;

	if( (sb.st_mode & 07) & 02 )
	    return 0;

	return 1;
}

/*
 *  getopt_startup - if the first line of the .e_profile file contains
 *      command line options, set up to use checkargs() to
 *      process the options.
 */

void
getopt_startup()
{
	register char *beg, *end;
	char buf[500];
	char *arg_opt[3];
    /*  int get_profilekey (); */
	/*void checkargs ();*/

	do {
	    fgets( buf, 200, fp_profile );
	    if( feof( fp_profile ))
		break;
	}
	while ( buf[0] == '#' || buf[0] == '\n' );
	if( feof( fp_profile )) {
	    /*
	     *  empty file
	     */
	    fclose( fp_profile );
	    dot_profile = NO;
	    return;
	}

	/*
	 *  if file begins with "opt", process options; otherise
	 *  reset file pointer.
	 */

	if( strncmp( buf, "opt", 3 )) {
	    rewind( fp_profile );
	    get_profilekey( YES );
	    return;
	}

	beg = index( buf, ':' );
	if( !beg ) {
	    mesg (ERRALL+1, ".e_profile file option format error");
	    return;
	}

	arg_opt[2] = NULL;

	/*
	 * skip white space, locate 1st arg
	 */
	while( *beg == ' ' || *beg == '\t' )
	    beg++;

	while( *beg && *beg != '\n' ) {
	    /*
	     *  locate end of arg and null terminate
	     */
	    end = beg + 1;
	    while( *end != ' ' && *end != '\t' && *end != '\n' )
		end++;
	    *end = '\0';
	    arg_opt[1] = beg;
	    /*
	     *  init nxt 2 globals so checkargs() processes exactly 1 arg
	     */
	    numargs = 2;
	    argarray = arg_opt;
	    checkargs();
	    /*
	     *  find nxt arg
	     */
	    beg = end + 1;
	    while( *beg == ' ' || *beg == '\t' )
		beg++;
	}

	if( feof( fp_profile )) {
	    dot_profile = NO;
	    fclose( fp_profile );
	}
	else
	    get_profilekey( YES );

	return;
}


/*
 *  get_profilekey - return the next "key" from the .e_profile file
 *      (stops reading on any errors)
 */

int
get_profilekey( init )
  Flag init;
{
	static char xline[500];
	static char *nxtc;
	register int skey;
	char buf[50];
	static int repeat_key, repeat;
	extern void RecordChar ();

	if( init ) {
	    keyused = YES;      /* ??? */
	    do {
		if( fgets( xline, 200, fp_profile ) == NULL )
		    break;
	    }
	    while ( xline[0] == '#' || xline[0] == '\n' );
	    if( feof( fp_profile )) {
		fclose( fp_profile );
		dot_profile = NO;
		if( silent && !recovering )
		    silent = NO;
	    }
	    else {
		nxtc = xline;
		xline[strlen(xline)-1] = '\0';        /* zap newline */
	    }
	    return(0);
	}

	if( repeat ) {
	    repeat--;
	    skey = repeat_key;
	    goto ret;
	}

	if( !*nxtc ) {     /* end of line? */
	    do {
		if( fgets( xline, 200, fp_profile ) == NULL )
		    break;
	    }
	    while ( xline[0] == '#' || xline[0] == '\n' );
	    if( feof( fp_profile )) {
err:            fclose( fp_profile );
		dot_profile = NO;
		flushkeys();
		if (silent) {
		    void multchar ();

		    silent = NO;
		    (*term.tt_ini1) (); /* not d_put(VCCICL) because fresh() */
					/* follows */
		    /*
		     *  this next nonsense rewrites the original text
		     *  (on the first line) that was scribbled on by
		     *  by the silent replay -- probably by param()...
		     */
		    savecurs();
		    poscursor(0,0);
		    multchar( ' ', curwin->rtext );
		    restcurs();
		    putupwin();
		    windowsup = YES;
		    fresh ();
		}
		return( CCNULL );
	    }
	    xline[strlen(xline)-1] = '\0';
	    nxtc = xline;
	}

	if( *nxtc == '\\' ) {           /* "\<" returns "<" */
	    nxtc++;
	    if( *nxtc == '<' ) {
		skey = '<';
		nxtc++;
	    }
	    else
		skey = '\\';
	}
	else if( *nxtc == '<' ) {    /* beginning of a key, eg: <cmd> */
		register int indx;
		char *s;
	    s = buf;
	    nxtc++;
	    repeat = 0;
	    if( *nxtc >= '0' && *nxtc <= '9' ) {        /* eg: <#key>  */
		do {
		    repeat = repeat * 10 + *nxtc - '0';
		    nxtc++;
		}
		while( *nxtc >= '0' && *nxtc <= '9' );
	    }
	    while( *nxtc && *nxtc != '>' )
		*s++ = *nxtc++;
	    if( !*nxtc ) {
		mesg( 2, "[.e_profile] missing '>' in: ", xline );
		fflush( stdout );
		sleep(2);       /* errmsg disappears too fast */
		goto err;
	    }
	    nxtc++;
	    *s = '\0';
	    indx = lookup( buf, keysyms );
/***
dbgpr( "looking for: %s, indx=%d\n", buf, indx );
 ***/
	    if (indx == -1 || indx == -2) {
		mesg( 4, "[.e_profile] bad key <", buf, "> in: ", xline );
		fflush( stdout );
		sleep(2);
		goto err;
	    }
	    skey = keysyms[indx].val;
	    if( repeat ) {
		repeat--;
		repeat_key = skey;
	    }
	}
	else
	    skey = *nxtc++;

/***
dbgpr( "get_profilekey: returning %o, nxtc=%o, xline=%o\n", skey, nxtc, xline );
 ***/

	/*
	 *  update keystroke file so recoveries work properly
	 */
ret:    putc( skey, keyfile );

#ifdef RECORDING
	if( recording )
	    RecordChar( (Uint) skey );
#endif /* RECORDING */

	return( skey );
}

#endif /* STARTUPFILE */
