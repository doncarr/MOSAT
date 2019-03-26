/***************************************************************************
Copyright (c) 2002,2003,2004 Donald Wayne Carr
                                                                                                                             
Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:
                                                                                                                             
The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.
                                                                                                                             
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include <errno.h>
#include <math.h>

#include "rtcommon.h"
#include "spd_display.h"
#include "spd_comm.h"
#include "ap_config.h"

#include "odo_mode.h"

static int last_col;


/*********************************************************************/

void odo_redraw_all(void)
{
  erase();
  box(stdscr, 0, 0);
  refresh();
}

/*******************************************************************/

int xodo_wait_key(const char *msg)
{
  int mych;
  mvprintw(24, 2,"%-36s", msg); 
  refresh();
  while (-1 != getch());
  
  while (1)
  {
    mych = getch();
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(10000);
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
  }
  mvprintw(24,2,"got %c      ", mych); 
  refresh();
  mvprintw(15,2,"%-36s", ""); 
  refresh();
  return mych;
}

/*******************************************************************/

int odo_wait_key(const char *msg)
{
  int mych;
  mvprintw(15,2,"%-36s", msg); 
  refresh();
  while (-1 != getch());
  
  while (1)
  {
    mych = getch();
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    mvprintw(15,2,"%-36s", " "); 
    refresh();
    mych = getch();
    //if ((mych ==  'q') || (mych == 'Q')) {endwin(); exit(0);}
    if (mych != -1)
    {
      break;
    } 
    usleep(400000);
    mvprintw(15,2, msg); 
    refresh();
  }
  mvprintw(15,2,"%-36s", ""); 
  refresh();
  return mych;
}

/*********************************************************************/

void odo_init_screen()
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

  short fg, bg;
  pair_content(COLOR_PAIR(0), &fg, &bg);
                                                                                
  init_pair(1, COLOR_RED, bg);
  init_pair(2, COLOR_GREEN, bg);
  init_pair(3, COLOR_BLACK, COLOR_YELLOW);
  init_pair(4, COLOR_BLUE, bg);
  init_pair(5, COLOR_MAGENTA, bg);
  init_pair(6, COLOR_CYAN, bg);
  init_pair(7, COLOR_BLACK, COLOR_WHITE);
  timeout(0);
  last_col = COLS;
  odo_redraw_all();
  refresh();
};

/**********************************************************************/

void odo_print_current(double speed, double distance, long total_count, long current_count)
{ 
  if (last_col != COLS)
  {
    erase();
    odo_redraw_all();
    last_col = COLS;
  }

  mvprintw(1,2,"Speed: %12.1lf", speed);
  mvprintw(2,2,"Dist.: %12.1lf", distance);
  mvprintw(3,2,"Count: %10d", total_count);
  mvprintw(4,2,"Count: %10d", current_count);
  refresh();
}

/*********************************************************************/

int odo_endwin(void)
{
  return endwin();
}

/*********************************************************************/

int odo_getch(void)
{
  return  getch();
}

/*********************************************************************/


