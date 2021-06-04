#ifdef COMMENT
--------
file e.u.c
    use and editfile routines.
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include "e.h"
#include "e.m.h"
#include "e.cm.h"
#include <sys/stat.h>
#include <pwd.h>

extern void doedit ();
extern char *getmypath ();
extern char *ExpandName ();
extern int dircheck ();
extern int dirncheck ();
extern int filetype ();
extern void GetLine ();
extern Flag readonly;
extern Flag noreadonly;
extern Flag autocreate;

#ifdef COMMENT
Cmdret
edit ()
.
    Do the "edit" command.
#endif
Cmdret
edit ()
{
    Reg1 char *eol;

    if (opstr[0] == '\0') {
	switchfile ();
	return CROK;
    }
    if (*nxtop)
	return CRTOOMANYARGS;

    eol = &opstr[strlen (opstr)];
    *eol = '\n';
    opstr[dechars (opstr) - 1] = '\0';
    editfile (opstr, (Ncols) -1, (Nlines) -1, 1, YES);
    return CROK;
}

#ifdef COMMENT
Small
editfile (file, col, line, mkopt, puflg)
    char   *file;
    Ncols   col;
    Nlines  line;
    Small   mkopt;
    Flag    puflg;
.
    Installs 'file' as edited file in current window, with window upper-left
      at ['line', 'col'].  Use lastlook if -1.
    If 'file' is not there
	if 'mkopt' == 2 || replaying || recovering, create the file.
	else if 'mkopt' == 1, give the user the option of making it
	else return
    Returns
      -1 if user does not want to make one,
       0 if error condition,
       1 if successfull.
    Writes screen (calls putupwin ()) if puflg is YES.
#endif
Small
editfile (file, col, line, mkopt, puflg)
char   *file;
Ncols   col;
Nlines  line;
Small   mkopt;
Flag    puflg;
{
    int retval;
    static Flag toomany = NO;
    Short   j;
    Flag    dwriteable,         /* directory is writeable */
	    fwriteable,         /* file is writeable */
	    isdirectory;        /* is a directory - don's set CANMODIFY */
    Flag    hasmultlinks = NO;
    La_stream  tlas;
    La_stream *nlas;
    char *dir = NULL;
    register Fn newfn;
    int nlink;
    Flag    nameexpanded = NO;  /* "~/file"  or "~user/file" */
#ifdef SYMLINKS
    Flag    wassymlink = NO;
    char    *origfile;
#endif /* SYMLINKS */
    static Flag noaskonrecover = YES;

    fwriteable = 0;

    if (file == NULL || *file == 0)     /* If null filename */
	Goret (-1);                     /* then can't do too much */

    /*  That the next statement is here is sort of a kludge, so that
     *  when editfile() is called before mainloop, only the last file
     *  edited will be able to set the hold.
     **/
    loopflags.hold = NO;

    /*
     *  Expand "~" to a login directory
     */
    if (*file == '~') {
	file = ExpandName (file);
	nameexpanded = YES;
	if (file == NULL) {
	    retval = 0;
	    goto ret;
	}
    }

#ifdef SYMLINKS
    /*
     *  Deal with symbolic links
     */
    Block {
	    register char *cp;
	    char *ReadSymLink();
	cp = ReadSymLink (file);
	if (cp == NULL) {
	    retval = 0;
	    goto ret;
	}
	else if (cp != file) {
	    origfile = file;
	    file = cp;
	    wassymlink = YES;
	}
    }
#endif /* SYMLINKS */

    /* dbgpr ("Edit %s\n", file);  **/
    if ((newfn = hvname (file)) != -1) {
	/* we have it active */
	if (newfn == curfile)
	    puflg = NO;
	/* dbgpr ("Have %s as %d\n", file, newfn);  **/
    }
    else if (   (newfn = hvoldname (file)) != -1
	     || (newfn = hvdelname (file)) != -1
	    ) {
	/* has been renamed or deleted */
	/* its directory must be writeable */
	dwriteable = YES;
	goto asknew;
    }
    else {
	/* don't have it as an old name*/
	if (toomany || nopens >= MAXOPENS) {
	    mesg (ERRALL + 1, "Too many files -- Editor limit!");
	    Goret (0);
	}
	/* find the directory */
	if ((j = dircheck (file, &dir, (char **) 0, NO, YES)) == -1)
	    Goret (0);

/* 3/7/84 changed to handle null directory */
	if (dir == NULL || *dir == 0)
	    dwriteable = access (".", 2) >= 0; 
	else
	    dwriteable = access (dir, 2) >= 0;

	if ((j = filetype (file)) != -1) {
	    /* file already exists */
	    Fn tfn;
	    fwriteable = access (file, 2) >= 0;
	    if (j != S_IFREG &&  j != S_IFDIR ) {
		mesg (ERRALL + 1, "Can only edit files");
		Goret (0);
	    }
	    if (access (file, 4) < 0) {
		mesg (ERRALL + 1, "File read protected.");
		Goret (0);
	    }
	    isdirectory = j == S_IFDIR;
	    if ((newfn = getnxfn ()) >= MAXFILES - 1)
		toomany = YES;      /* we'll get you next time */
	    intok = YES;
	    if ((nlas = la_open (file, "", &tlas, (La_bytepos) 0,
		    (Ff_stream *) 0, 0, fwriteable)) == NULL) {
		intok = NO;
		if (la_errno == LA_INT)
		    mesg (ERRALL + 1, "Interrupted");
		else
		    mesg (ERRALL + 1, "Can't open the file");
		Goret (0);
	    }
	    intok = NO;
#ifdef  DEBUGDEF
{char xyz[20]; sprintf (xyz,", length is %d.", la_lsize(nlas));
mesg(TELALL+3, "File ", file, xyz);}
#endif /* DEBUGDEF */
	    if (nlas->la_file == la_chglas->la_file) {
		mesg (ERRALL + 1, "Can't edit the 'changes' file");
		(void) la_close (nlas);
		Goret (0);
	    }
	    Block {
		La_file   *tlaf;
		tlaf = nlas->la_file;
		/* is this another name for one we already have open? */
		/* dbgpr ("Looking up %s\n", file);  **/
		for (tfn = FIRSTFILE + NTMPFILES; tfn < MAXFILES; tfn++)
		    if (   (fileflags[tfn] & (INUSE | NEW)) == INUSE
			&& tlaf == fnlas[tfn].la_file
		       ) {
			/* dbgpr ("Same as %s\n", names[tfn]);  **/
			/* yes it is */
			/* if it is DELETED, then we go and make a NEW one */
			if (fileflags[tfn] & DELETED)
			    break;
			/* deallocate newfn */
			fileflags[newfn] = 0;
			newfn = tfn;
			(void) la_close (nlas);
			toomany = NO;
			mesg (TELALL + 3, "EDIT: ", names[tfn], " (linked)");
			d_put (0);
			loopflags.hold = YES;
			goto editit;
		    }
	    }
	    nopens++;
	    mesg (TELSTRT + 2, "EDIT: ", file);
	    d_put (0);
	    nlink = fmultlinks (la_chan (nlas));
	    if (isdirectory) {
		mesg (1, "  (is a directory)");
		loopflags.hold = YES;
	    }
	    else if (1 < nlink) Block {
		    char numstr[10];
		hasmultlinks = YES;
		sprintf (numstr, "%d", nlink);
		mesg (3, "  (has ", numstr, " links)");
		loopflags.hold = YES;
	    }
#ifdef SYMLINKS
	    if (wassymlink) {
		mesg (3, " [symbolic link from ", origfile, "]" );
		loopflags.hold = YES;
		if (nameexpanded)
		    sfree (origfile);
		fileflags[newfn] |= SYMLINK;
	    }
#endif /* SYMLINKS */
	    if (dwriteable)
		fileflags[newfn] |= DWRITEABLE;
	    if (fwriteable)
		fileflags[newfn] |= FWRITEABLE;
	    if (dwriteable && !isdirectory) {
		/*
		 *  Changed to only set CANMODIFY if file is writeable
		 *  [really if access(file,2) succeeds]
		 */
		if ((!readonly && fwriteable) || noreadonly) {
		    fileflags[newfn] |= CANMODIFY;
		    if (inplace && hasmultlinks)
			fileflags[newfn] |= INPLACE;
		}
		else {
		    mesg (1, " [Readonly]");
		    loopflags.hold = YES;
		}
	    }
	    mesg (TELDONE);
	    d_put (0);
	    fileflags[newfn] |= UPDATE;
	}
	else
 asknew:
	    if (mkopt == 2 || ((noaskonrecover && replaying) || recovering)) {
		noaskonrecover = NO;
		goto createit;
	    }
	else if (mkopt == 1) {
		int ntimes = 0;
#ifdef  STARTUPFILE
		Flag sav_dotprofile;
#endif /* STARTUPFILE */
	    if (autocreate)
		goto createit;
	    do {
		mesg ((TELSTRT|TELCLR) + 3,
		     "Do you want to create ", file, "? (y): ");
		keyused = YES;
#ifdef  STARTUPFILE
		/*
		 * turnoff dot_profile flag so input comes from the terminal
		 */
		sav_dotprofile = dot_profile;
		dot_profile = NO;
#endif /* STARTUPFILE */
		getkey (WAIT_KEY);
#ifdef  STARTUPFILE
		dot_profile = sav_dotprofile;
#endif /* STARTUPFILE */
		keyused = YES;
		mesg (TELDONE);
		ntimes++;
	    } while (!index("\nyYNn\r", key));
		/*
		 *  so "x<return>" isn't treated as "<ret>"
		 */
	    if (ntimes > 1 && (key == '\n' || key == '\r'))
		Goret (-1);
	    if (key != 'y' && key != 'Y' && key != '\n' && key != '\r')
		Goret (-1);
 createit:
	    if (!dwriteable) {
		/* tell 'em */
		dirncheck (dir, YES, YES);
		Goret (0);
	    }
	    /* ok to create file, so do it */
	    if ((newfn = getnxfn ()) >= MAXFILES - 1)
		toomany = YES;  /* we'll get you next time */
	    fileflags[newfn] |= UPDATE | DWRITEABLE | FWRITEABLE
			     | CANMODIFY | NEW;
	    /* a NEW file can NOT be INPLACE */
	    nlas = la_open (file, "n", &tlas, (La_bytepos)0, (Ff_stream *)0, 0, 1);
	}
	else
	    Goret (-1);
	names[newfn] = append (file, "");
	/* dbgpr ("%s: las=%x\n", file, nlas);  **/
	if (   !la_clone (nlas, &fnlas[newfn])
	    || !la_clone (nlas, &lastlook[newfn].las)
	   ) {
	    fatal (FATALBUG, "La_clone failed in editfile()");
	    /* NOTREACHED */
	}
	(void) la_close (nlas);
    /*  elasdump (&fnlas[newfn], "open");         **/
    }

editit:
    doedit (newfn, line, col);
    curwin->winflgs &= ~TRACKSET;
    infotrack (NO);
    if (puflg) {
	if (newfn == OLDLFILE)
	    GetLine (-1L);   /* so that the last line modified will show up */
	putupwin ();
    }
    limitcursor ();
    poscursor (curwksp->ccol, curwksp->clin);
    retval = 1;
ret:
#ifdef SYMLINKS
    if (wassymlink) {
	sfree (file);
	file = NULL;
    }
#endif /* SYMLINKS */
    if (nameexpanded && file != NULL)
	sfree (file);
    if (dir != NULL)
	sfree (dir);
    return retval;
}

#ifdef COMMENT
void
doedit (fn, line, col)
    Fn      fn;
    Nlines  line;
    Ncols   col;
.
    Now that we have determined that everything is OK, go ahead and
    install the file as element 'fn' in the list of files we are editing.
    If 'line' != -1 make it the line number of the current workspace;
    Similarly for 'col'.
#endif
void
doedit (fn, line, col)
Fn fn;
Nlines  line;
Ncols   col;
{
    register S_wksp *lwksp;
    register S_wksp *cwksp;

    /* Some replays result in this error condition (5/2021) */
    if (curwin == &enterwin) {
	chgwindow(-1);
	mesg (ERRALL + 1, "An error has been detected, please exit and restart your session.");
    }

    if (curfile == deffn) {
	/* insure that the new altwksp will be null */
	releasewk (curwin->wksp);
	curwin->wksp->wfile = NULLFILE;
    }
    releasewk (curwin->altwksp);
    exchgwksp (NO);
    lwksp = &lastlook[fn];
    if (line != -1)
	lwksp->wlin = line;       /* line no of upper-left of screen */
    if (col != -1)
	lwksp->wcol = col;        /* col no of column 0 of screen */
    cwksp = curwksp;
    cwksp->wlin = lwksp->wlin;
    cwksp->wcol = lwksp->wcol;
    cwksp->ccol = lwksp->ccol;
    cwksp->clin = lwksp->clin;

    (void) la_clone (&fnlas[fn], curlas = &cwksp->las);
    cwksp->wfile = curfile = fn;
    return;
}

#ifdef COMMENT
Fn
getnxfn ()
.
    Return the next file number that is not INUSE.
#endif
Fn
getnxfn ()
{
    register Fn fn;

    for (fn = FIRSTFILE + NTMPFILES; fileflags[fn] & INUSE; )
	if (++fn >= MAXFILES)
	    fatal (FATALBUG, "Too many files");

    fileflags[fn] = INUSE;
    return fn;
}



#ifdef COMMENT
char *
ReadSymLink (file)
char *file
.
    For symbolic links, get the referenced name of the file to update.
    Treated as if user had edited that file directly.
    If the referenced name is not rooted, construct the appropriate
    relative path.

#endif /* COMMENT */

#ifdef SYMLINKS
char *
ReadSymLink (file)
char *file;
{
    Reg1 int nc;
    Reg2 char *cp;
    char linkname[300];
    struct stat sb;
    char *rindex();


    for (;;) {
	if (lstat (file, &sb) < 0)
	    break;
	if ((sb.st_mode&S_IFMT) != S_IFLNK)         /* not a symbolic link */
	    break;

	if ((nc = readlink (file, linkname, sizeof(linkname))) == -1) {
	    mesg (ERRALL + 2, "Can't read symbolic link ", file);
	    return NULL;
	}
	linkname[nc] = '\0';    /* documentation says readlink doesn't */
				/*  null terminate */

	if (*linkname == '/' || ((cp = rindex (file, '/')) == NULL))
	    file = append (linkname, "");
	else {                  /* have a relative pathname */
		char tmp[400];
	    strcpy (tmp, file);
	    tmp[(cp - file) + 1] = '\0';    /* after last '/' */
	    file = append (tmp, linkname);
	}
    }

    return (file);
}
#endif /* SYMLINKS */


#ifdef COMMENT
char *
ExpandName (file)
char *file
.
    Convert a leading "~" into the appropriate home directory.
    (Assumes caller verifies the name begins with ~)

#endif /* COMMENT */

char *
ExpandName (file)
char *file;
{
    Reg1 char *end;
    Reg2 char *beg;
    char tmp[150];
    struct passwd *pw, *getpwnam();

    beg = file+1;         /* skip ~ */

    if (*beg == '/') {           /* "~/file" */
	strcpy (tmp, getmypath());
	strcat (tmp, beg);
    }
    else {                      /* ~user/file */
	for (beg = tmp, end = file+1; *end && *end != '/'; )
	    *beg++ = *end++;
	*beg = '\0';
	if ((pw = getpwnam(tmp)) == NULL) {
	    mesg (ERRALL+2, "No such user: ", tmp);
	    return NULL;
	}
	strcpy (tmp, pw->pw_dir);
	if (*end)
	    strcat (tmp, end);   /* append rest of the name */
    }

    return (append(tmp, ""));
}

