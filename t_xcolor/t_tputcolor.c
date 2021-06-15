#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <term.h>
#include <ctype.h>

int
pch(int ch) { putchar(ch); return (0); }


/* try the
chgat() functions
*/

void exit();
void sleep();

int
main() {

    int i;
    int win_h;

    initscr();
    clear();
    noecho();
    cbreak();


    char *setab = tigetstr("setab");   /* set ansi background color # */
    char *sgr0 = tigetstr("sgr0");     /* cancel graphics */

    win_h = (LINES - 4) / 2;

    keypad(stdscr, TRUE);

    for (i = 0; i <= 255; i++ ) {
	tputs(tparm(setab,i), 1, pch);
	/*printf("Why didn't Bach sign his manuscripts? i=%d\r\n", i);*/
	printf("Why didn't Bach sign his manuscripts? ");
	tputs(sgr0,1,pch);
	printf(" i=%d\r\n",i);
	printf("\r\n");
	fflush(stdout);

	/*   for some reason, outputting the color changes w/o any delay
	 *   displays only the last color value for all lines (likely a curses optimization).
	 *   Any delay seems to override the optimization
	 */

/*        sleep(1);*/
	timeout(200);
	wgetch(stdscr);
	timeout(-1);

	if (i && (i % win_h) == 0) {
	    wprintw(stdscr, "q/quit, any key to continue: ");
	    if(wgetch(stdscr) == 'q') {
		endwin();
		exit(1);
	    }
	    wclear(stdscr);
	    wmove(stdscr, 1, 0);
	}
    }

    timeout(-1);
    wprintw(stdscr, "hit any key to exit: ");
    wgetch(stdscr);

    endwin();
    exit(0);
}
