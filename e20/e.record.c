#ifdef COMMENT
--------
file e.record.c
    define/play recording
    store/play macros
#endif

#include "e.h"
#include "e.cm.h"
#include "e.inf.h"
#include "e.m.h"
#include "e.tt.h"
#include "e.h.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef RECORDING

extern int fileno();
extern char *optmacrofile;      /* TODO:  move to e.h */

Flag recording, playing, recording_defined, macroing;
Flag play_silent;
int rec_size, rec_len, play_count, play_len, lastcmd_len;
int n_macrosdefined = 0;
Uchar *rec_p, *rec_text, *play_ptr;

#define NMACROS 100
struct macros {
    char *name;
    int len;
    Uchar *text;
};
static struct macros macros[NMACROS];
struct macros *FindMacro(char *);
struct macros *NextFreeMacro(void);

#ifdef COMMENT
Cmdret
SetRecording(cmd)
.
    Very simple recording implementation (based somewhat on LMCs
    macro implementation).  The arg 'cmd' is either CMDRECORD or CCRECORD
    depending on whether a function key got us here, or the CMD line.
#endif

Cmdret
SetRecording(cmd)
Short cmd;
{
    if (recording) {        /* a toggle */
	recording = NO;
	info (inf_record, 3, "");
	/*
	 * if the recording is ended via the CMD line, remove the
	 * text "CCCMDrec[ord]" from rec_text (we simply decrement
	 * rec_len).
	 */
	if (cmd == CMDRECORD && lastcmd_len)
	    rec_len = lastcmd_len;
	lastcmd_len = 0;
	rec_p = rec_text;
	return CROK;
    }
    if (!macroing && rec_size)
	sfree (rec_text);
    else
	macroing = 0;
    rec_text = (Uchar *) salloc (512, NO);
    rec_size = 512;
    rec_p = rec_text;
    rec_len = 0;
    lastcmd_len = 0;
    recording = YES;
    info (inf_record, 3, "REC");
    return (CROK);
}


void
RecordChar(c)
unsigned Short c;
{
    if (c == CCRECORD)  /* silently, we're about to cancel a recording */
	return;

    if (c == CCPLAY) {
	mesg (ERRALL+1, "PLAY not allowed while recording.");
	return;
    }

    if (c == CCCMD)             /* So "CMD: record" can be stripped off */
	lastcmd_len = rec_len;  /* when recording is cancelled this way */

#ifdef NCURSES
    if (c == CCMOUSE) {
	/*  Saving/replaying a click could be done (like we do replaying
	 *  a crashed session), but do we need to track the window the click
	 *  occurred in and make sure it was replayed in the same window...???
	 *
	 *  Punt for now....
	 */
	mesg (ERRALL+1, "Mouse clicks not allowed while recording.");
	fflush(stdout);
	sleep(2);   /* otherwise the mesg is unseen by the mouse up event */
	return;
    }
/*dbgpr("RecordChar: add c=(%o)(%c)\n", c, c);*/
#endif

    *rec_p++ = c;
    rec_len++;

    if (rec_len >= rec_size) {  /* full, assume more to come */
	  Uchar *cp;
	cp = (Uchar *)salloc(rec_size+512, NO);
	my_move ((char *) rec_text, (char *) cp, (ulong)rec_size);
	rec_p = cp + rec_size;
	rec_size += 512;
	sfree (rec_text);
	rec_text = cp;
    }

    return;
}

Cmdret
UnSetRecording()
{
    recording = NO;
    info (inf_record, 3, "");
    if (!macroing)
	sfree (rec_text);
    rec_len = 0;
    rec_p = rec_text = (Uchar *)NULL;
    return CROK;
}

Cmdret
PlayRecording(n)
    int n;
{
    if (recording) {
	mesg (ERRALL+1, "PLAY not allowed while recording.");
	return CROK;
    }

    if (rec_len == 0) {
	mesg (ERRALL+1, "No recording defined.");
	return CROK;
    }

    if (n <= 0) n = 1;
    play_count = n;
    playing = YES;
    play_len = 0;
    play_ptr = rec_text;

    if (play_silent)
	silent++;

    return CROK;
}

Uchar *
PlayChar(peekflg)
Flag peekflg;
{
    Uchar *cp;

    if (peekflg != WAIT_KEY)
	return play_ptr;

    cp = play_ptr;
    play_ptr++;
    if (++play_len >= rec_len) {
	     /* play_count is the number of times to replay
	      * the recording
	      */
	if (--play_count <= 0)
	    playing = 0;
	else {
	    play_len = 0;
	    play_ptr = rec_text;
	}
    }
    return cp;
}

Cmdret
DoMacro(name, arg)
char *name;
char *arg;
{
    char tmp[50];
    Reg2 struct macros *mp;

    sprintf(tmp, "DoMacro: |%s|", name+1);
    if(DebugVal == 2)
	info(0, strlen(tmp), tmp);

    if ((mp = FindMacro(name+1)) == (struct macros *)NULL) {
	if (rec_len) {
	    rec_len = 0;
	    if (!macroing)
		sfree (rec_text);
	    else
		macroing = 0;
	    rec_text = (Uchar *)NULL;
	}
	mesg (ERRALL+1, "Macro not defined.");
	return CROK;        /* TODO err msg */
    }

    rec_len = mp->len;
    rec_p = rec_text = mp->text;
    macroing++;     /* set so rec_text is not free'd */

    PlayRecording( arg ? atoi(arg) : 1 );

    return CROK;
}


Cmdret
UndefMacro(macname)
Reg1 char *macname;
{
    Reg3 struct macros *mp, *last;

    if ((mp = FindMacro(macname)) == (struct macros *)NULL) {
	    char tmp[50];
	sprintf( tmp, "Macro %s is not defined.", macname );
	mesg (ERRALL+1, tmp );
	return CROK;
    }

    sfree(mp->name);
    sfree(mp->text);
    last = mp;

    /*
     * move the last entry here to fill the void.
     */
    for( last++; last->name && last != &macros[NMACROS]; last++ ) {
	continue;
    }
    if( !last->name )
	last--;
    mp->name = last->name;
    mp->len = last->len;
    mp->text = last->text;
    last->name = (char *)NULL;
    n_macrosdefined--;

    return CROK;
}

#ifdef COMMENT
StoreMacro(macname)
.
    Save the "recording" in a named macro.
#endif /* COMMENT */

Cmdret
StoreMacro(macname)
char *macname;
{
    Reg3 struct macros *mp;
/*  Reg4 int i; */

#ifdef HMMMM
    if (!rec_len) {
	mesg (ERRALL+1, "Nothing recorded to store.");
	return CROK;
    }
#endif

    if ((mp = FindMacro(macname)) == (struct macros *)NULL) {
	if ((mp = NextFreeMacro()) == (struct macros *)NULL) {
	     mesg (ERRALL+1, "Too many macros");
	     return CROK;
	}
    }
    else {
	sfree(mp->name);
	sfree(mp->text);
    }

    mp->name = salloc ((Ncols)strlen(macname)+1, NO);
    strcpy(mp->name, macname);
    mp->len = rec_len;
    mp->text = (Uchar *)salloc(rec_len, NO);
    my_move((char *)rec_text, (char *)mp->text, (ulong) rec_len);
    n_macrosdefined++;

    return CROK;
}

/*
 *  Returns ptr to name in  macro table (or null).
 */

struct macros *
FindMacro(mname)
char *mname;
{
    Reg1 struct macros *mp;
    register size_t /*int*/ len = strlen(mname);

    for (mp = &macros[0]; mp != &macros[NMACROS]; mp++) {
	if (!mp->name)
	    break;
	if (!strncmp(mname, mp->name, len))
	    return (mp);
    }

    return((struct macros *)NULL);
}


/*
 *  Returns ptr to next free entry in macro table.
 */
struct macros *
NextFreeMacro()
{
    Reg2 struct macros *mp;

    for (mp = &macros[0]; mp != &macros[NMACROS]; mp++)
	if (!mp->name)
	    return (mp);

    return((struct macros *)NULL);
}


#define MACROFILE ".e_macros"

void
ReadMacroFile()
{

    FILE *fp;
    char *macfile;
    char buf[8*BUFSIZ];
/*  char mbuf[4*BUFSIZ], *t; */
    char name[200], *index();
    char tmp[500];
    struct macros *mp;

    macfile = tmp;

    /*
     *  1st choice:   command line
     *  2nd:          EMACROFILE environment variable
     *  3rd:          ~/.e_macros
     */
    if( optmacrofile && *optmacrofile )
	strcpy( tmp, optmacrofile );
    else if(( macfile = (char *)getenv( "EMACROFILE" )) == NULL )
	sprintf( tmp, "%s/%s", getmypath(), MACROFILE );

    if(( fp = fopen( macfile, "r" )) == NULL )
	return;

    mp = &macros[0];

    fgets(buf, sizeof buf, fp);
    if( strncmp( buf, "# DO NOT EDIT", 13 )) {
	mesg( ERRALL+1, "Bad format in ~/.e_macro file" );
	sleep(3);   /* time to read the error message */
	return;
    }

    /*
     * format:  name len DATAname len DATA...
     */
    while( !feof( fp )) {
	fscanf( fp, "%s %d ", name, &mp->len );
	mp->name = salloc((Ncols)strlen(name) + 1, NO);
	strcpy( mp->name, name );

	mp->text = (Uchar *)salloc(mp->len, NO);
	if( fread( mp->text, (size_t)sizeof(Uchar), (size_t) mp->len, fp ) != (size_t) mp->len ) {
	    mesg( ERRALL+1, "Bad return reading ~/.e_macros" );
	    sleep(3);
	}
	mp++;
	if( ++n_macrosdefined >= NMACROS ) {
	    mesg( ERRALL+1, "Bad format in ~/.e_macro file" );
	    break;
	}
    }

    fclose(fp);
    return;
}

/*
 * Save defined macros in ~/.e_macros.
 *
 *      format(yuk):  "name len DATAname len DATAname len DATA..."
 */

void
SaveMacros()
{
    char *macfile;
    char tmp[200], savname[200];
    struct macros *mp;
    FILE *fp;

    if( userid == 0 || geteuid() == 0 ) {
	mesg( ERRALL+1, "Sorry, not allowed for root." );
	return;
    }

    if( !n_macrosdefined ) {
	mesg( ERRALL+1, "No macros are defined." );
	return;
    }

    /*
     *  if EMACROFILE is set in the environment, use it.
     */
    if(( macfile = (char *)getenv( "EMACROFILE" )) == NULL ) {
	sprintf( tmp, "%s/%s", getmypath(), MACROFILE );
	macfile = tmp;
    }

    /* save old .e_macros */
    sprintf( savname, "%s/,%s", getmypath(), MACROFILE );
    mv(macfile, savname);

    if(( fp = fopen( macfile, "w" )) == NULL ) {
	sprintf( tmp, "Can't open %s for writing.", macfile );
	mesg( ERRALL+1, tmp );
	return;
    }

    fprintf( fp, "# DO NOT EDIT, THIS FILE WAS AUTOMATICALLY GENERATED BY E\n" );
    for( mp = &macros[0]; mp->name; mp++ ) {
	fprintf( fp, "%s %d ", mp->name, mp->len );
	fwrite( mp->text, (size_t)sizeof(Uchar), (size_t)mp->len, fp );
    }
    fchmod(fileno(fp), 0444);       /* help prevent accidental changes */
    fclose(fp);                     /* while browsing in E */

    return;
}



/*
 *    CMD: ?macros       -> list of macro names
 *    CMD: ?macros name  -> show keystrokes for macro 'name'
 */

Cmdret
ShowMacros(opt)
char *opt;
{
    unsigned Short rc;
    struct macros *mp;
    register int i, c;

    savecurs();
    while (1) {
	    int seenany = 0;
	MCLEAR;
	MHOME;
	printf ("\n\rPress RETURN to continue editing.\r\n");
	printf ("\n\rNAME");
	if (opt && *opt)
	    printf ("\tKEYSTROKES");

	for (mp = &macros[0]; mp != &macros[NMACROS]; mp++) {
	    if (!mp->name || !mp->len)
		break;
	    if (opt && *opt && strcmp(opt, mp->name))
		continue;
	    printf ("\n\r%s\t", mp->name);
	    if (!opt || !*opt)  /* just print names */
		continue;
	    seenany = 1;
	    for (i = 0; i < mp->len; i++) {
		c = mp->text[i];
		if (c >= 040 && c < 0177)
		    putchar (c);
		else
		    printf ("<%s>", keycaps[c < 040 ? c : c - 0177 + 040]);
	    }
	    printf ("\n\r");
	}
	if (!seenany && opt && *opt)
	    printf ("\n\r\n\rMacro \"%s\" is not defined.\r\n", opt);

	keyused = YES;
#ifdef NCURSES
	rc = mGetkey (WAIT_KEY, NULL);
#else
	rc = getkey (WAIT_KEY, NULL);
#endif
	if (rc == CCRETURN ) break;
    }

    restcurs();
    mesg(TELALL+1, " ");
    fflush(stdout);
    poscursor(cursorcol, cursorline+1);

    return CROK ;
}

#endif /* RECORDING */
