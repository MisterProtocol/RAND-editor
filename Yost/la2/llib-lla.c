#include <lalocal.h>

/* LINTLIBRARY */

int         la_maxchans;
La_linesize la_maxline;
La_stream * la_chglas;
Ff_stream * la_chgffs;
La_flag     la_chgopen;
int         la_chans;
La_stream * la_firststream;
La_stream * la_laststream;
int         la_errno;

/* VARARGS1 */
void        la_abort (str) char *str; { if (str) ;}
La_linepos  la_align (olas, nlas) La_stream *olas; La_stream *nlas;
	     { if (olas && nlas) ; return 0;}
La_linepos  la_blank (plas, nlines) La_stream *plas; La_linepos nlines;
	     { if (plas && nlines) ; return 0; }
#ifndef LA_BP
La_bytepos  la_bsize (plas) La_stream *plas; { if (plas); return 0;}
#endif
La_stream * la_clone (oldlas, newlas) La_stream *oldlas; La_stream *newlas;
	     {if (oldlas && newlas); return 0;}
La_linepos  la_close (plas) La_stream *plas; { if (plas); return 0;}
La_linesize la_clseek (plas, nchars) La_stream *plas; La_linesize nchars;
	     { if (plas && nchars) ; return 0; }
int         la_error () {return 0;}
int         la_freplace (filename, plas) char *filename; La_stream *plas;
	     { if (filename && plas); return 0;}
La_linepos  la_lcollect (state, buf, nbytes) int state; char *buf; int nbytes;
	     { if (state && buf && nbytes); return 0;}
La_linepos  la_lcopy (slas, dlas, nlines) La_stream *slas; La_stream *dlas;
	     La_linepos nlines; { if (slas && dlas && nlines); return 0;}
La_linepos  la_lcount (plas, start, number, mode) La_stream *plas;
	     La_linepos start; La_linepos number; int mode;
	     { return 0; }
La_linepos  la_ldelete (plas, nlines, dlas) La_stream *plas; La_stream
	     *dlas; La_linepos nlines; { if (plas && dlas && nlines); return 0;}
/* VARARGS5 */
La_linepos  la_lflush (plas, start, nlines, chan, oktoint, timeout)
	     La_stream *plas; La_linepos start, nlines; int chan;
	     int oktoint; int timeout;
	     { if (plas && start && nlines && chan && oktoint && timeout) ;
	     return 0;}
La_linesize la_lget (plas, buf, nchars) La_stream *plas; char *buf; int nchars;
	     { if (plas && buf && nchars); return 0;}
La_linepos  la_linsert (plas, buf, nchars) La_stream *plas; char *buf;
	     int nchars; { if (plas && buf && nchars); return 0;}
La_linesize la_lpnt (plas, buf) La_stream *plas; char **buf;
	     { if (plas && buf); return 0;}
/* VARARGS2 */
La_linepos  la_lrcollect (plas, nlines, dlas) La_stream *plas, *dlas;
	     La_linepos *nlines; { if (plas && nlines && dlas); return 0;}
La_linepos  la_lreplace (plas, buf, nchars, nlines, dlas) La_stream *plas;
	     char *buf; int nchars; La_linepos *nlines; La_stream *dlas;
	     { if (plas && buf && nchars && nlines && dlas); return 0;}
La_linesize la_lrsize (plas) La_stream *plas; {if (plas); return 0;}
La_linepos  la_lseek (plas, nlines, type) La_stream *plas; La_linepos
	     nlines; int type;
	     {if (plas && nlines && type); return 0;}
/* VARARGS3 */
La_stream * la_open (filename, modestr, lastream, offset, ffstream, chan)
	     char *filename; char *modestr; long offset; La_stream *lastream;
	     Ff_stream *ffstream; int chan;
	     {if (filename && modestr && lastream && offset && ffstream &&
	     chan); return 0;}
La_stream * la_other (plas) register La_stream *plas;
	     {if (plas); return 0;}
int         la_tcollect (pos) long pos; { return 0; }
La_linesize la_wrsize (plas) La_stream *plas; {if (plas); return 0;}

int         _labreak (plas, copy, ffsd, lfsd, pnlines, nbytes ) La_stream *plas; int
	     copy; La_fsd **ffsd; La_fsd **lfsd; La_linepos *pnlines; La_bytepos
	     *nbytes; { if (plas && copy && ffsd && lfsd && pnlines && nbytes);
	     return 0;}
int         _lazbreak (plas) La_stream *plas; { if (plas); return 0;}
int         _lalink (plas, ffsd, lfsd, nl , nchars ) La_stream *plas; La_fsd *ffsd,
	     *lfsd; La_linepos nl; La_bytepos nchars;
	     {if (plas && ffsd && lfsd && nl && nchars); return;}
