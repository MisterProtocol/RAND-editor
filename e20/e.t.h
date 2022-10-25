/*
 * file e.t.h - header file that is terminal dependent
 *
 **/

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#ifdef OUT
/* margin characters and others */
#define FIRSTSPCL  127
#define ESCCHAR    127  /* escape character */
#define BULCHAR    128          /* bullet character */
#define FIRSTMCH   129
#define LMCH       129  /* left */
#define RMCH       130  /* right */
#define MLMCH      131  /* more left */
#define MRMCH      132  /* more right */
#define TMCH       133  /* top */
#define BMCH       134  /* bottom */
#define TLCMCH     135  /* top left corner */
#define TRCMCH     136  /* top right corner */
#define BLCMCH     137  /* bottom left corner */
#define BRCMCH     138  /* bottom right corner */
#define TTMCH      139  /* top tab */
#define BTMCH      140  /* bottom tab */
#define LASTGRAF   140          /* FIRSTSPCL - LASTGRAF are graphics */
#define ELMCH      141  /* empty left */
#define INMCH      142  /* inactive */
#define LASTSPCL   142

#define NSPCHR     LASTSPCL-FIRSTSPCL+1 /* number of special characters */
#define NMCH       LASTSPCL-FIRSTMCH+1  /* number of margin characters */
#endif /* OUT */

/* margin characters and others */
#define FIRSTSPCL  (Uchar)127
#define ESCCHAR    (Uchar)127  /* escape character */
#define BULCHAR    (Uchar)128  /* bullet character */
#define FIRSTMCH   (Uchar)129
#define LMCH       (Uchar)129  /* left */
#define RMCH       (Uchar)130  /* right */
#define MLMCH      (Uchar)131  /* more left */
#define MRMCH      (Uchar)132  /* more right */
#define TMCH       (Uchar)133  /* top */
#define BMCH       (Uchar)134  /* bottom */
#define TLCMCH     (Uchar)135  /* top left corner */
#define TRCMCH     (Uchar)136  /* top right corner */
#define BLCMCH     (Uchar)137  /* bottom left corner */
#define BRCMCH     (Uchar)138  /* bottom right corner */
#define TTMCH      (Uchar)139  /* top tab */
#define BTMCH      (Uchar)140  /* bottom tab */
#define LASTGRAF   (Uchar)140          /* FIRSTSPCL - LASTGRAF are graphics */
#define ELMCH      (Uchar)141  /* empty left */
#define INMCH      (Uchar)142  /* inactive */
#define LASTSPCL   (Uchar)142

#ifdef OUT
/* these two are not used */
#define NSPCHR     142-127+1  /* number of special characters */
#define NMCH       142-129+1  /* number of margin characters */
#endif
