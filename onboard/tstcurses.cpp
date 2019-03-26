#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

int main(int argc, char *argv[])
{
  initscr();
  start_color();
  use_default_colors();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  curs_set(0);
  timeout(0);

  box(stdscr, 0, 0);

  mvhline(2, 0, 0, COLS);
  mvhline(11, 0, 0, COLS);
  attron(A_ALTCHARSET);
  mvprintw(2,0,"%c", ACS_LTEE);
  mvprintw(2,COLS-1,"%c", ACS_RTEE);
  mvprintw(11,0,"%c", ACS_LTEE);
  mvprintw(11,COLS-1,"%c", ACS_RTEE);
  attroff(A_ALTCHARSET);
  mvhline(9, 2, 0, 4);
  mvhline(9, COLS-7, 0, 4);

  mvprintw(5,5,"hello world ");
  mvprintw(12,20,"goodby world");
  refresh();

  sleep(2);
  getch();
  //last_col = COLS;
  endwin();
}
