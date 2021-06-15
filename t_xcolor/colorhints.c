#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <term.h>
#include <ctype.h>

int
pch(int ch) { putchar(ch); return (0); }

void exit();
void sleep();

int
main() {

    int i;
    int win_h;
    int bgpairs[] = {11,36,87,117,188,193,222,229,255};
    int fgpairs[] = {4,9,35,42,72,87,124,141,180,250};

    initscr();
    clear();
    noecho();
    cbreak();


    char *setab = tigetstr("setab");   /* set ansi background color # */
    char *sgr0 = tigetstr("sgr0");     /* cancel graphics */

    win_h = (LINES - 4) / 2;

    keypad(stdscr, TRUE);

    wrefresh(stdscr);

    printf("Background examples:\r\n");

    for (i = 0; i < sizeof(bgpairs) / sizeof(bgpairs[0]); i++ ) {
	tputs(tparm(setab,bgpairs[i]), 1, pch);
    /*  printf("Why didn't Bach sign his manuscripts? -setab=%d\r\n", bgpairs[i]); */
	printf("Why didn't Bach sign his manuscripts? ");
	tputs(sgr0,1,pch);
	printf(" -setab=%d\r\n", bgpairs[i]);
	printf("\r\n");
	fflush(stdout);

	timeout(200);
	wgetch(stdscr);
	timeout(-1);
    }

    printf("Foreground examples:\r\n");
    for (i = 0; i < sizeof(fgpairs) / sizeof(fgpairs[0]); i++ ) {
	tputs(tparm(setab,fgpairs[i]), 1, pch);
    /*  printf("Why didn't Bach sign his manuscripts? -setaf=%d\r\n", fgpairs[i]); */
	printf("Why didn't Bach sign his manuscripts? ");
	tputs(sgr0,1,pch);
	printf(" -setaf=%d\r\n", fgpairs[i]);
	printf("\r\n");
	fflush(stdout);

	timeout(200);
	wgetch(stdscr);
	timeout(-1);
    }

    timeout(-1);
    wprintw(stdscr, "hit any key to exit: ");
    wgetch(stdscr);

    endwin();
    exit(0);
}
