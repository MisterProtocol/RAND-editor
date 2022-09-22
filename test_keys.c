#include <ncurses.h>

int main() {
  initscr();
  clear();
  noecho();
  cbreak();
  keypad(stdscr, true);

  printw("Press 'q' to quit.\n");
  refresh();

  int ch;
  int n = 1;
  char tmp[10];
  for (ch = getch(); ch != 'q'; ch = getch()) {

    mvprintw(n, 0, "%d", ch);
    mvprintw(n++, 10, keyname(ch));
    if (n > 30) n = 1;
    clrtoeol();
    clrtobot();
    refresh();
  }

  endwin();
}
