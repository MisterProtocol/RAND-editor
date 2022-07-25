#ifdef COMMENT
--------
file e.sv.c
    file saving routines
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#include "e.h"
#include "e.m.h"
#include "e.cm.h"


#define UMASK       /* hack, 2.5.11, use umask when creating new files in e.sv.c */
#include <sys/stat.h>   /* added for UMASK */
#include <sys/types.h>  /* added for UMASK */

extern int fgetpriv ();
extern int la_freplace (char *, La_stream *);

#ifdef COMMENT
Cmdret
save ()
.
    Do the "save" command.
#endif
Cmdret
save ()
{
    if (curmark)
	return NOMARKERR;
    if (opstr[0] == '\0')
	return CRNEEDARG;
    if (*nxtop)
	return CRTOOMANYARGS;


/*  savefile (opstr, curfile, NO); */
    savefile (opstr, curfile, NO, NO);
    return CROK;
}

#ifdef COMMENT
Flag
savefile (filename, fn, svinplace, rmbak)
    char *filename;
    Fn    fn;
    Flag  svinplace;         /* enable inplace save */
    Flag  rmbak;
.
  There are 2 cases to deal with.
    1. Doing a save as part of exit sequence
       a. Only remove the backup file
       b. Do the saving
    2. Doing a "save" command of fn to filename
  If filename is a null pointer, then we are doing case 1, else case 2
.
    savefile (filename, fn, svinplace, rmbak)
	if filename == NULL && rmbak != 0
	    remove backup file if any
	    return
	if filename != NULL
	    if filename is open for editing (this includes the case where
	      the filename is the name of the file)
		disallow the save
		return NO;
	else filename == NULL, meaning save to this file,
	    if not EDITED
		return YES
	if svinplace and file has multiple links
	    Copy original to backup
	    if filename == NULL
		Do an la_freplace to the backup filename
	    la_lflush the file to the original file
	else
	    write out file number fn to a temp file called ,esvXXXXXX
		where XXXXX is unique.
	    rename the original file to the backup name
	    rename the temp file to the original name
	if filename == 0
	    clear EDITED and CANMODIFY
	return YES;
.
	normal return is YES; error is NO;
#endif
/* VARARGS 3 */
Flag
savefile (filename, fn, svinplace, rmbak)
char   *filename;
Fn      fn;
Flag svinplace;         /* enable inplace save */
Flag rmbak;
{
    char   *origfile,       /* origfile to be saved                      */
	   *dirname,        /* the directory for the save */
	   *filepart,       /* filename part of pathname */
	   *bakfile;
    register Short  tmp;
    register Short  j;
    Flag hasmultlinks;


 /* putline (YES); */
    putline ();
    bakfile = NULL;
    if (filename == NULL)
	origfile = names[fn];
    else {
	if (   hvname (filename) != -1
	    || hvdelname (filename) != -1
	    || hvoldname (filename) != -1
	   ) {
	    mesg (ERRALL + 1, "Can't save to a file we are using");
	    return NO;
	}
	else if (   (j = filetype (origfile = filename)) != -1
		 && j != S_IFREG
		) {
	    mesg (ERRALL + 1, "Can only save to files");
	    return NO;
	}
    }

/**  dbgpr ("savefile: filename=(%s) origfile=(%s)\n", filename, origfile); **/

    /* get the directory name and the file part of the name */
    if (dircheck (origfile, &dirname, &filepart, YES, YES) == -1) {
	sfree (dirname);
	return NO;
    }

    /* make the backup name */
    if (prebak[0] != '\0') Block {
	char *cp;
	cp = append (prebak, filepart);
	bakfile = append (dirname, cp);
	sfree (cp);
    }
    if (postbak[0] != '\0')
	bakfile = append (origfile, postbak);

    if (filename == NULL && rmbak) {
	unlink (bakfile);
	sfree (dirname);
	sfree (bakfile);
	return YES;
    }

    /* if this is a saveall save, and the file was renamed,
     * or the filename is the same as the old name of another renamed file,
     * then we must do the renaming now
     **/
    if (filename == NULL) Block {
	Fn tmp;
	if (   (tmp = hvoldname (origfile)) != -1
	    && !svrename (tmp)
	   ) {
 err2:      sfree (dirname);
	    sfree (bakfile);
	    return NO;
	}
	if (   (fileflags[tmp = fn] & RENAMED)
	    && !svrename (tmp)
	   )
	    goto err2;
    }

    hasmultlinks = multlinks (origfile) > 1;
    if (svinplace) Block {
	Fd origfd;
	int origpriv;
	mesg (TELALL + 3, "SAVE: ", origfile,
	      hasmultlinks ? " (preserving link)" : " (inplace)");
	d_put (0);
	/* copy orig to backup */
	sfree (dirname);
	unlink (bakfile);            /* get rid of any old backup    */
	if ((origfd = open (origfile, 2)) == -1) {
 err1:      mesg (ERRALL + 3, "Unable to copy ", origfile, " to backup");
	    sfree (bakfile);
	    return NO;
	}
	origpriv = fgetpriv (origfd);
	if ((j = filecopy ((char *) 0, origfd, bakfile, 0,
		     YES, fgetpriv (la_chan (&fnlas[fn]))))
	    == -2) {
	    mesg (ERRALL + 1, "Unable to create Backup");
	    goto err1;
	}
	else if (j < 0)
	    goto err1;

	if (   filename == NULL
	    && !la_freplace (bakfile, &fnlas[fn])
	   )
	    goto err1;
	sfree (bakfile);

	/* I wish that I could write to the file and then truncate it to the
	 * written length, rather than doing a creat on it and releasing
	 * all those blocks to the free list possibly to be gobbled up
	 * by someone else, leaving me stranded.
	 * But current unix can't truncate except by doing a creat.
	 **/
	if ((origfd = creat (origfile, (mode_t)origpriv)) == -1) {
	    mesg (ERRALL + 2, "Unable to create ", origfile);
	    return NO;
	}
	if (la_lflush (&fnlas[fn], (La_linepos) 0,
		       la_lsize (&fnlas[fn]), origfd, NO, 0)
	    != la_lsize (&fnlas[fn])
	   ) {
	    mesg (ERRALL + 2, "Error to writing to ", origfile);
	    close (origfd);
	    return NO;
	}
	close (origfd);
    }
    else

    Block {  /* not inplace */
	Fd tempfd;
	struct stat stbuf;
	char *mktemp ();
	char *tempfile;
#ifdef USE_MKSTEMP
	char tmp_template[200];
#endif
	Nlines ltmp;
	/*
	 * Leaving this in here in case we need it again.
	 * If the editor goes into a loop writing the save file
	 * and fills the file system, the following code will
	 * stop it short and give a core dump.
	 */
	/*
	struct rlimit rlp;
	getrlimit (RLIMIT_FSIZE, &rlp);
	rlp.rlim_cur = 50*1024*1024;
	setrlimit (RLIMIT_FSIZE, &rlp); */
	mesg (TELALL + 3, "SAVE: ", origfile,
	      hasmultlinks ? " (breaking link)" : "");
	d_put (0);
#ifdef USE_MKSTEMP
	tempfile = &tmp_template[0];
	strncpy(tempfile, (append (dirname, ",esvXXXXXX")), sizeof tmp_template);
	sfree (dirname);
	if ((tempfd = mkstemp(tempfile)) == -1) {
 retno:   return NO;
	}
	/*   dbgpr ("e.sv.c: tempfd=(%d) tempfile=(%s)\n", tempfd, tempfile);  **/

#else  /* UNSAFE */
	tempfile = mktemp (append (dirname, ",esvXXXXXX"));
	sfree (dirname);
	if ((tempfd = creat (tempfile, groupid == 1 ? 0644 : 0664)) == -1) {
	    mesg (ERRALL + 1, "Unable to create tmp file!");
 retno:     sfree (tempfile);
	    return NO;               /* error */
	}
#endif
	dbgpr ("fn = %d, size = %d, tempfd = %d\n", fn, la_lsize (&fnlas[fn]), tempfd);
	/* fatal (FATALBUG, "BYE!"); */
	if ((ltmp = la_lflush (&fnlas[fn], (La_linepos) 0,
		       la_lsize (&fnlas[fn]), tempfd, NO))
	    != la_lsize (&fnlas[fn])
	   ) {
	 /* dbgpr ("flush %s %d of %d lines\n", tempfile, ltmp,
		   la_lsize (&fnlas[fn]));       **/
	    mesg (ERRALL + 3, "Error saving ", origfile, " to disk");
	    unlink (tempfile);
	    close (tempfd);
	    goto retno;              /* error */
	}
	close (tempfd);

	/* if the name we just saved to is also in use as a deleted name,
	 * we have to clear the INUSE and DELETED flags for that fn so our
	 * file won't get deleted later as part of the saveall sequence
	 **/
	if ((tmp = hvdelname (origfile)) != -1) {
	    unlink (origfile);
	    fileflags[tmp] &= ~(INUSE | DELETED);
	}

	tmp = mv (origfile, bakfile);
	sfree (bakfile);
	tmp = mv (tempfile, origfile);

	if (!(fileflags[fn] & NEW)) {
	    if (userid == 0) {
		fstat (la_chan (&fnlas[fn]), &stbuf);
		chown (origfile, stbuf.st_uid, stbuf.st_gid);
		chmod (origfile, (int) stbuf.st_mode & 0777);
	    }
	    else
		chmod (origfile, (mode_t)fgetpriv (la_chan (&fnlas[fn])));
	}
	else {
#ifdef xxx_UMASK
/* this is not working right */
	    mode_t mask = umask (0);
	    umask (mask);
	    chmod(origfile, mask);
#endif
	  if (userid != 0 )
/**/          chmod(origfile, 0664);  /*10.27.2010: tmp hack */
	}

#ifndef USE_MKSTEMP
	sfree (tempfile);
#endif
    }

    if (filename == NULL) {
	/* see to it that nothing more will happen to this file */
	fileflags[fn] &= ~(CANMODIFY | NEW | DWRITEABLE);
	la_unmodify (&fnlas[fn]);
    }
    return YES;
}

#ifdef COMMENT
Flag
svrename (fn)
    Fn fn;
.
    Do the actual renaming of the file on disk.
    Called as part of the exit saving sequence.
    If all went OK return YES, else NO.
#endif
Flag
svrename (fn)
Reg1 Fn fn;
{
    Block {
	Reg2 char *old;
	Reg3 char *new;
	old = oldnames[fn];
	new = names[fn];
	mesg (TELALL + 4, "RENAME: ", old, " to ", new);
	if (!mv (old, new)) {
	    mesg (ERRALL + 4, "Can't rename ", old, " to ", new);
	    return NO;
	}
    }
    fileflags[fn] &= ~RENAMED;

    /* if the name we just renamed to is also in use as a deleted name,
     * we have to clear the INUSE and DELETED flags for that fn so our
     * file won't get deleted later as part of the saveall sequence
     **/
    Block {
	Reg2 Fn tmp;
	if ((tmp = hvdelname (names[fn])) != -1)
	    fileflags[tmp] &= ~(INUSE | DELETED);
    }
    return YES;
}
