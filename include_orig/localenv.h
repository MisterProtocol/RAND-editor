
/* system environment stuff for unix 3.0 */

#define index strchr            /* */
#define rindex strrchr          /* */
/* fine NOIOCTL_H               /* there is no ioctl.h */
/* fine RDNDELAY                /* read call has NDELAY capability */
/* fine EMPTY                   /* can implement empty(fd) subroutine call */
#define LINKS                   /* file system has links */
#define CANFORK                 /* system has fork() */
/* fine VFORK                   /* system has vfork() */
#define ABORT_SIG SIGILL        /* which signal does abort() use */
/* fine SIGCHLD SIGCLD             */
#define SIGARG                  /* signal catch routine has sig num as arg */
#define SIG_INCL <signal.h>
#define SGTT_INCL <sgtty.h>
#define TTYNAME                 /* use ttyname function */
/* define TTYN                  /* use ttyn function */
#define SHORTUID                /* uid is a short, not a char (v7+) */
#define ENVIRON                 /* getenv() is implemented */
/* fine VAX_MONITOR             /* use monitor() routine for vax */
#define SIGNALS                 /* system has �ignals */

#define SYSSELECT
#define NDIR                    /* enables long filenames */
