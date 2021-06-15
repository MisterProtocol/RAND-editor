#include <string.h>
#include <stdio.h>
#include <ncurses.h>
#include <ctype.h>

#include "nxcolors.h"

#define CTRL(x) ((x) & 0x1f)

/* try the
chgat() functions
*/

void exit();
void sleep();

int
main()
{
    int i;
    int win_h;

    initscr();
    clear();
    noecho();
    cbreak();

    win_h = (LINES - 4) / 2;

    keypad(stdscr, TRUE);
    start_color();

    int r1, r2;
    NCURSES_COLOR_T fg = COLOR_WHITE;  /* 7 */

    init_pair(9, COLOR_BLACK, COLOR_WHITE);  /* pair number, fg, bg */
    color_set(9, NULL);
    wbkgd(stdscr, COLOR_PAIR(9));

    XRGB *rgb;

    for (i = 0; i <= 255; i++ ) {
      /*rgb = &x_rgb[i];*/
	rgb = &mac_rgb[i];

	r1 = init_color(35, rgb->r, rgb->g, rgb->b);  /* 35 picked at random */
	r2 = init_pair(6, fg, 35);
	color_set(6, NULL);

	/* wcolor_set(stdscr, 6, NULL); */
	wprintw(stdscr, "Hello world COLORS=%d COLOR_PAIRS=%d(r1=%d,r2=%d), n=(%d)rgb=(%d,%d,%d)\n",
	   COLORS, COLOR_PAIRS, r1, r2, i, rgb->r, rgb->g, rgb->b);
	wattrset(stdscr, A_NORMAL);    /* same as 0 */
	wrefresh(stdscr);

	wprintw(stdscr, "\n");


	/*   for some reason, outputting the color changes w/o any delay
	 *   displays only the last color value for all lines (likely a curses optimization).
	 *   Any delay seems to override the optimization
	 */

	/*sleep(1); */  /* too long */
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
