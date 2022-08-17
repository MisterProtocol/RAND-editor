La_stream *la_open(char*, char*, La_stream *, long ,Ff_stream *, int, int);
La_stream *la_ffopen (Ff_stream *, La_stream *, long);
La_stream *la_clone (La_stream *, La_stream *);
int la_verify (La_stream *);
La_stream *la_newstream (La_stream *);
void la_makestream (La_stream *, La_file *);
La_linepos la_parse (Ff_stream *, long, La_fsd **, La_fsd**, La_file *, La_bytepos, char *);
void la_freefsd (La_fsd *);
La_linepos la_align (La_stream *, La_stream *);
La_linepos la_blank (La_stream *, La_linepos);
La_bytepos la_bsize (La_stream *);
La_linesize la_clseek (La_stream *, La_linesize);
void la_fsddump (La_fsd*, La_fsd*, La_flag, char *);
void la_sdump (La_stream *, char *);
void la_fdump (La_file *, char *);
void la_schaindump (char *);
void la_fschaindump (La_stream *, char *);
int la_error (void);
int la_freplace (char *, La_stream *);
La_linepos la_lcount (La_stream *, La_linepos, La_linepos, int);
La_linepos la_lcopy (La_stream *, La_stream *, La_linepos);
La_linepos la_lflush (La_stream *, La_linepos, La_linepos, int, int, unsigned int);
void la_lfalarm (void);
La_linesize la_lget (La_stream *, char *, int);
La_linesize la_lpnt (La_stream *, char **);
La_linepos la_lreplace (La_stream *, char *, int, La_linepos *, La_stream *);
La_linepos la_ldelete (La_stream *, La_linepos, La_stream *);
La_linepos la_lcollect (int, char *, int);
int la_tcollect (long);
La_linepos la_lrcollect (La_stream *, La_linepos *, La_stream *);
La_linepos la_linsert (La_stream *, char *, int);
La_flag la_zbreak (La_stream *);
La_flag la_break (La_stream *, int, La_fsd **, La_fsd **, La_linepos *
#ifdef LA_BP
    , La_bytepos *
#endif /* LA_BP */
    );
La_linepos la_close (La_stream *);
void la_link (La_stream *, La_fsd *, La_fsd *, La_linepos
#ifdef LA_BP
    , La_bytepos
#endif /* LA_BP */
    );
La_linesize la_lrsize (La_stream *);
La_linepos la_lseek (La_stream *, La_linepos, int);
La_linesize la_lwsize (La_stream *);
int la_int(void);
La_stream * la_other (La_stream *);
void la_lflalarm(int);


#ifdef DBG_DEBUG
void dbgpr_la_fsddump (La_fsd*, La_fsd*, La_flag, char *);
void dbgpr_la_sdump (La_stream *, char *);
void dbgpr_la_fdump (La_file *, char *);
void dbgpr_la_schaindump (char *);
void dbgpr_la_fschaindump (La_stream *, char *);
#endif
