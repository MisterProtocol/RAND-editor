/*
 * file e.sg.h: sgtty stuff
 **/

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif


#ifdef SYSIII
#include <termio.h>
#else
#include <sgtty.h>
#include <sys/ioctl.h>
extern struct sgttyb instty, outstty;
#endif



#ifdef SYSIII
extern
struct termio in_termio,
	      out_termio;
extern int fcntlsave;
#endif /* SYSIII */

#ifdef  CBREAK
#ifdef  TIOCGETC
extern
struct tchars spchars;
#endif /* TIOCGETC */
#ifdef  TIOCGLTC
extern
struct ltchars lspchars;
#endif /* TIOCGLTC */
extern
Flag cbreakflg;
#endif /* CBREAK */

extern
Flag istyflg,
     ostyflg;

extern
unsigned Short oldttmode;
