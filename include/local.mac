
/* system environment stuff for unix 3.0 */

#define index strchr            /* */
#define rindex strrchr          /* */
/* fine NOIOCTL_H               / * there is no ioctl.h */
/* fine RDNDELAY                / * read call has NDELAY capability */
/* fine EMPTY                   / * can implement empty(fd) subroutine call */
#define LINKS                   /* file system has links */
#define CANFORK                 /* system has fork() */
/* fine VFORK                   / * system has vfork() */
#define ABORT_SIG SIGILL        /* which signal does abort() use */
/* fine SIGCHLD SIGCLD             */
#define SIGARG                  /* signal catch routine has sig num as arg */
#define SIG_INCL <signal.h>
#define SGTT_INCL <sgtty.h>
#define TTYNAME                 /* use ttyname function */
/* define TTYN                  / * use ttyn function */
#define SHORTUID                /* uid is a short, not a char (v7+) */
#define ENVIRON                 /* getenv() is implemented */
/* fine VAX_MONITOR             / * use monitor() routine for vax */
#define SIGNALS                 /* system has óignals */

#define SYSSELECT               /* system select() */
#define FDSET                   /* select() newer versions using fd_set */
#define NDIR                    /* enables long filenames */
#define RENAME                  /* rename sys call available */
#define SYMLINKS                /* symlink sys call available */
#ifndef __STDC_VERSION__
#define _Noreturn __attribute__((noreturn))
#else
#if __STDC_VERSION__ < 201112L
#define _Noreturn __attribute__((noreturn))
#endif
#endif
#define TERMCAPFILE	"/usr/local/etc/termcap"
#define NCURSES
