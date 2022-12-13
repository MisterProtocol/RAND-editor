#include <stdio.h>
#include <string.h>
extern void exit();

#define CCLAST         0253
#define CMD0 0
#define CMD 1

#ifndef INPLACE
#define STANDALONE
#endif /* INPLACE */

void showkeys(FILE *, long);
void doResize(FILE *);

#ifdef STANDALONE
#include <sys/types.h>
#include <sys/stat.h>
extern void exit(int);
long ftell();
int fstat();
int fileno(FILE *);
extern void dbgpr();

int
main(argc, argv)
int argc;
char **argv;
{
    FILE *fp;
    struct stat statbuf;

    if (argc != 2) {
	fprintf(stderr, "Usage:  e_showkeys keyfile\n");
	exit(1);
    }

    if ((fp = fopen(argv[1], "r")) == NULL) {
	fprintf(stderr, "can't pen $argv[1]\n");
	exit(1);
    }
    if (fstat(fileno(fp), &statbuf) == -1) {
	fprintf(stderr, "fstat failed\n");
	exit(1);
    }

    showkeys (fp, statbuf.st_size);
    exit(0);
}
#endif /* STANDALONE */


void
showkeys(FILE *fp, long filesize)
{
	typedef struct keymap {
	    char *keysym;
	} keymap;

	/*struct keymap low_keys[] = {*/
	keymap low_keys[] = {
		{"<cmd>"},
		{"<cmd>"},      // was <wleft>, moved to 0252
		{"<edit>"},
		{"<int>"},
		{"<open>"},
		{"<-sch>"},
		{"<close>"},
		{"<mark>"},
		{"<left>"},
		{"<tab>"},
		{"<down>"},
		{"<home>"},
		{"<pick>"},
		{"<ret>"},
		{"<up>"},
		{"<ins>"},
		{"<repl>"},
		{"<-page>"},
		{"<+sch>"},
		{"<wright>"},
		{"<+line>"},
		{"<dchar>"},
		{"<+word>"},
		{"<-line>"},
		{"<-word>"},
		{"<+page>"},
		{"<chwin>"},
		{"<srtab>"},
		{"<cchar>"},
		{"<-tab>"},
		{"<bksp>"},
		{"<right>"}
	};

	/*struct keymap hi_keys[] = { */    /* 0200 - 0253 */
	keymap hi_keys[] = {     /* 0200 - 0253 */
		{"<stop>"},
		{"<erase>"},
		{"<undef>"},
		{"<split>"},
		{"<join>"},
		{"<exit>"},
		{"<abort>"},
		{"<redraw>"},
		{"<clrtabs>"},
		{"<center>"},
		{"<fill>"},
		{"<just>"},
		{"<clear>"},
		{"<track>"},
		{"<box>"},
		{"<stopx>"},
		{"<quit>"},
		{"<cover>"},
		{"<overlay>"},
		{"<blot>"},
		{"<help>"},
		{"<ccase>"},
		{"<caps>"},
		{"<autofill>"},
		{"<range>"},
		{"<null>"},
		{"<dword>"},
		{"<unass>"},
		{"<record>"},
		{"<play>"},
		{"<mouse>"},   // 0236
		{"<mouseonoff>"},
		{"<brace>"},  // 0240
		{"<put>"},
		{"<unmark>"},
		{"<regexonoff>"},
		{"<+win>"},
		{"<-win>"},   // 0245
		{"<-close>"},
		{"<-erase>"}, // 0247
		{"<-rec>"},   // 0250
		{"<tag>"},
		{"<wleft"},   // 252
		{"<resize>"}, // 253
	};

	register int c;
	int y, x;
	long pos;

/* test */
#ifdef OUT
int i;
for(i = 0; i < (CCLAST - 0200); i++) {
  printf("%d=%s\n",i,hi_keys[i]);
}
exit(1);
#endif

#ifdef STANDALONE
	/*
	 * skip up to first \r (or \n with new ascii format for 1st line)
	 */
	do
	    c = fgetc( fp );
	while( c != '\r' && c != '\n' && c != EOF );

	if( c == EOF ) {
	    fprintf( stderr, "unexpected end of file!\n" );
	    exit(1);
	}
#endif /* STANDALONE */

	while(( c = fgetc( fp )) != EOF ) {
/*dbgpr("showkeys: c=%o\n", c);*/
	    c &= 0377;
	    if( c < 040 ) {
		if( (c == CMD) || (c == CMD0) ) {
		    pos = filesize - (ftell(fp) - 1);
		    printf("\n----------\nSelect replay option: \"2 %ld\" to stop " \
		      "replay before the following:\n", pos);
		}
		printf("%s", low_keys[c].keysym );
		if( c == '\r' )
		    putchar('\n');
	    }
	    else if( c < 0200 )
		putchar(c);
	    else if( c <= CCLAST ) {

		/*fprintf(stderr, "c=(%o)idx=(%d)\n", c, c-0200);*/

		switch (c) {
		    case 0236:  // CCMOUSE
			fscanf(fp, "%3d%3d", &y, &x);
			printf("<CCMOUSE(%d,%d)>", y,x);
			break;

		    case 0253:  // CCRESIZE
			pos = filesize - (ftell(fp) - 1);
			printf("\n----------\nSelect replay option: \"2 %ld\" to stop " \
			  "replay before the following:\n", pos);
			printf("<CCRESIZE>");
			doResize(fp);
			break;

		    default:
			printf("%s", hi_keys[c - 0200].keysym);
			break;
		}
	    }
	    else
		fprintf( stderr, "key %o unrecognized\n", c );
	}
	fflush(stdout);

	return;
}


/*
 * The fp offset is 1 char past CCRESIZE (0235)
 */
void
doResize(FILE *fp)
{
    int w, h;
    int nwin, wnum;
    int tm, bm, lm, rm;
    int tt, bt, lt, rt;
    int te, be, le, re;
    int clin, ccol, wlin, wcol;

    __attribute__((unused))int rc;

    char buf[256], *s;
    int i;

    fgets(buf, (int) sizeof(buf), fp);
    //buf[strlen(buf)-1] = '\0';

//for(i=0; i<8; i++)
//printf("[%d]=(%o),", i, (unsigned char)buf[i]);
//printf("\n");

    s = buf;
    rc = sscanf(s, " h=%d w=%d nwin=%d\n", &h, &w, &nwin);

#ifndef INPLACE
//      printf("-----\nbuf=%s", s);
//      printf("rc=%d from sscanf\n", rc);
#endif
    printf("h=%d w=%d nwin=%d\n", h, w, nwin);


#ifdef INPLACE  // ie, option "2 ?" while doing a replay
    return;
#endif

    for (i=0; i<nwin; i++) {
	fgets(buf, (int) sizeof(buf), fp);
	if(feof(fp)) break;

	s = buf;
	//printf("buf=%s", s);

	sscanf(s, "win %d", &wnum);
	s += wnum > 9 ? 6 : 5;

	//printf("buf=%s", s);

	sscanf(s, " tm=%d bm=%d lm=%d rm=%d tt=%d bt=%d lt=%d rt=%d \
		te=%d be=%d le=%d re=%d \
		clin=%d ccol=%d wlin=%d wcol=%d\n",
	    &tm, &bm, &lm, &rm, &tt, &bt, &lt, &rt, &te, &be, &le, &re,
	    &clin, &ccol, &wlin, &wcol);

	printf("win %d tm=%d bm=%d lm=%d tm=%d ", wnum, tm, bm, lm, rm);
	printf("tt=%d bt=%d lt=%d rt=%d ", tt, bt, lt, rt);
	printf("te=%d be=%d le=%d re=%d ", te, be, le, re);
	printf("clin=%d ccol=%d wlin=%d wcol=%d\n", clin, ccol, wlin, wcol);
     }

    return;
}
