/* declarations of file name strings */

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

extern
char   *tmppath,
       *ttynstr,
	scratch[];

#define VRSCHAR 1   /* index of the version number into the following names */
extern
char    tmpnstr[],                /* they are initialized in e.x         */
	keystr[],
	bkeystr[],
	rstr[];

extern
char   *keytmp,
       *bkeytmp,
       *rfile,          /* strt file name and backup name */
       *brfile,
       *inpfname;
