
#include <stdio.h>
#include <ncurses.h>

#include "d2x24.h"

/*********************************************************/

void d2x24_init_screen(void)
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
  //spd_redraw_all();
  refresh();
};

static bool d2x24_curses_init = false;

int d2x24_printf(int line, const char *fmt, ...)
{
  if (!d2x24_curses_init)
  {
    d2x24_init_screen();
    d2x24_curses_init = true;
  }
  char myline[25];
  va_list arg_ptr;
  va_start(arg_ptr, fmt);
  int n = vsnprintf(myline, sizeof(myline), fmt, arg_ptr);
  va_end(arg_ptr);
  if (line != 0) line = 1; // Make sure line is 0 or 1
  mvprintw(line,0,"%-24s", myline);
  refresh();
  return n;
}

/*********************************************************/

int d2x24_end_screen()
{
  return endwin();
}

/*********************************************************/

int d2x24_getch(void)
{
  return getch();
}

/*********************************************************/

int d2x24_beep(void)
{
  return beep();
}

/*********************************************************/

