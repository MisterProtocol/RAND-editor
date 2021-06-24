#include <stdio.h>

extern void exit();

#define CCLAST         0236
#define CMD 0

#ifndef SHOWKEYS_INPLACE
#define STANDALONE
#endif /* SHOWKEYS_INPLACE */

#ifdef STANDALONE
#include <sys/types.h>
#include <sys/stat.h>
void exit();
long ftell();
void showkeys();
int fstat();
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
	fprintf(stderr, "can't open $argv[1]\n");
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
		{"<wleft>"},
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

	/*struct keymap hi_keys[] = { */    /* 0200 - 0236 */
	keymap hi_keys[] = {     /* 0200 - 0236 */
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
		{"<mouse>"}
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
		if( c == CMD ) {
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

		if( c == 0236 ) { /* CCMOUSE */
		    fscanf(fp, "%3d%3d", &y, &x);
		    printf("<CCMOUSE(%d,%d)>", y,x);
		}
		else {
		  printf("%s", hi_keys[c - 0200].keysym);
		}

	    }
	    else
		fprintf( stderr, "key %o unrecognized\n", c );
	}
	fflush(stdout);

	return;
}
